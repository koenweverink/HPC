#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <mpi.h>
#include "beehive.h"

#define ROWS 20
#define COLS 20
#define GENERATIONS 10

// Function prototypes
int **allocate_grid(int rows, int cols);
void free_grid(int **grid);
void initialize_grid(int **grid, int rows, int cols);
void load_pattern(int **grid, int start_row, int start_col, const uint8_t pattern[][BEEHIVE_WIDTH], int pattern_height, int pattern_width);
void communicate_halos(int **local_grid, int local_rows, int cols, int rank, int num_processes, MPI_Comm comm);
void simulate_local(int **local_grid, int **next_local_grid, int local_rows, int cols);
int count_population(int **grid, int rows, int cols);
void print_grid(int **grid, int rows, int cols);

int main(int argc, char **argv) {
    MPI_Init(&argc, &argv);

    int rank, num_processes;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &num_processes);

    if (ROWS % num_processes != 0) {
        if (rank == 0) {
            fprintf(stderr, "Error: Number of processes must evenly divide the number of rows.\n");
        }
        MPI_Abort(MPI_COMM_WORLD, 1);
    }

    int internal_rows = ROWS / num_processes;
    int local_rows = internal_rows + 2;  // Include halo rows

    // Allocate grids
    int **global_grid = NULL;
    if (rank == 0) {
        global_grid = allocate_grid(ROWS, COLS);
        initialize_grid(global_grid, ROWS, COLS);
        load_pattern(global_grid, 10, 10, beehive, BEEHIVE_HEIGHT, BEEHIVE_WIDTH);
    }

    int **local_grid = allocate_grid(local_rows, COLS);
    int **next_local_grid = allocate_grid(local_rows, COLS);

    // Scatter rows to processes (scatter internal rows only)
    MPI_Scatter(rank == 0 ? &global_grid[0][0] : NULL, internal_rows * COLS, MPI_INT,
                &local_grid[1][0], internal_rows * COLS, MPI_INT, 0, MPI_COMM_WORLD);

    for (int gen = 0; gen < GENERATIONS; gen++) {
        // Communicate halos
        communicate_halos(local_grid, local_rows, COLS, rank, num_processes, MPI_COMM_WORLD);

        // Simulate locally
        simulate_local(local_grid, next_local_grid, local_rows, COLS);

        // Swap grids
        int **temp = local_grid;
        local_grid = next_local_grid;
        next_local_grid = temp;

        // Gather results back (exclude halos)
        MPI_Gather(&local_grid[1][0], internal_rows * COLS, MPI_INT,
                   rank == 0 ? &global_grid[0][0] : NULL, internal_rows * COLS, MPI_INT, 0, MPI_COMM_WORLD);

        // Validate on rank 0
        if (rank == 0) {
            printf("Generation %d:\n", gen);
            print_grid(global_grid, ROWS, COLS);

            int beehive_population = 0;
            for (int i = 10; i < 10 + BEEHIVE_HEIGHT; i++) {
                for (int j = 10; j < 10 + BEEHIVE_WIDTH; j++) {
                    if (i < ROWS && j < COLS) {
                        beehive_population += global_grid[i][j];
                    }
                }
            }
            if (beehive_population != 6) {
                fprintf(stderr, "Error: Beehive population mismatch in generation %d. Population: %d\n", gen, beehive_population);
            } else {
                printf("Beehive population is correct (%d) in generation %d.\n", beehive_population, gen);
            }

            fflush(stdout); // Ensure output is flushed
        }
    }

    // Free memory
    free_grid(local_grid);
    free_grid(next_local_grid);

    if (rank == 0) {
        free_grid(global_grid);
    }

    MPI_Finalize();
    return 0;
}

// Allocate a grid
int **allocate_grid(int rows, int cols) {
    int **grid = malloc(rows * sizeof(int *));
    grid[0] = malloc(rows * cols * sizeof(int)); // Contiguous memory allocation
    for (int i = 1; i < rows; i++) {
        grid[i] = grid[0] + i * cols;
    }
    return grid;
}

// Free a grid
void free_grid(int **grid) {
    free(grid[0]); // Free the contiguous block
    free(grid);
}

// Initialize a grid
void initialize_grid(int **grid, int rows, int cols) {
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            grid[i][j] = 0;
        }
    }
}

// Load a pattern
void load_pattern(int **grid, int start_row, int start_col, const uint8_t pattern[][BEEHIVE_WIDTH], int pattern_height, int pattern_width) {
    for (int i = 0; i < pattern_height; i++) {
        for (int j = 0; j < pattern_width; j++) {
            grid[start_row + i][start_col + j] = pattern[i][j];
        }
    }
}

// Communicate halos
void communicate_halos(int **local_grid, int local_rows, int cols, int rank, int num_processes, MPI_Comm comm) {
    MPI_Status status;

    int *recv_top = malloc(cols * sizeof(int));
    int *recv_bottom = malloc(cols * sizeof(int));

    // Exchange halos
    if (rank > 0) { // Top neighbor
        MPI_Sendrecv(local_grid[1], cols, MPI_INT, rank - 1, 0,
                     recv_top, cols, MPI_INT, rank - 1, 0, comm, &status);
    }
    if (rank < num_processes - 1) { // Bottom neighbor
        MPI_Sendrecv(local_grid[local_rows - 2], cols, MPI_INT, rank + 1, 0,
                     recv_bottom, cols, MPI_INT, rank + 1, 0, comm, &status);
    }

    if (rank > 0) {
        for (int j = 0; j < cols; j++) {
            local_grid[0][j] = recv_top[j];
        }
    }
    if (rank < num_processes - 1) {
        for (int j = 0; j < cols; j++) {
            local_grid[local_rows - 1][j] = recv_bottom[j];
        }
    }

    free(recv_top);
    free(recv_bottom);
}

// Simulate one step locally
void simulate_local(int **local_grid, int **next_local_grid, int local_rows, int cols) {
    for (int i = 1; i < local_rows - 1; i++) {
        for (int j = 0; j < cols; j++) {
            int alive_neighbors = 0;
            for (int x = -1; x <= 1; x++) {
                for (int y = -1; y <= 1; y++) {
                    if (x == 0 && y == 0) continue;
                    int ni = i + x, nj = j + y;
                    if (nj >= 0 && nj < cols) {
                        alive_neighbors += local_grid[ni][nj];
                    }
                }
            }
            if (local_grid[i][j] == 1) {
                next_local_grid[i][j] = (alive_neighbors == 2 || alive_neighbors == 3) ? 1 : 0;
            } else {
                next_local_grid[i][j] = (alive_neighbors == 3) ? 1 : 0;
            }
        }
    }
}

// Count population
int count_population(int **grid, int rows, int cols) {
    int population = 0;
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            population += grid[i][j];
        }
    }
    return population;
}

// Print grid
void print_grid(int **grid, int rows, int cols) {
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            printf("%d ", grid[i][j]);
        }
        printf("\n");
    }
    printf("\n");
}