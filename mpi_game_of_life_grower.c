#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <mpi.h>
#include <omp.h>
#include "grower.h"

#define GRID_SIZE 3000
#define ITERATIONS 5000

// Function prototypes
void initialize_grid(uint8_t grid[GRID_SIZE][GRID_SIZE]);
void copy_grid(uint8_t dest[GRID_SIZE][GRID_SIZE], uint8_t src[GRID_SIZE][GRID_SIZE]);
void update_grid(uint8_t grid[GRID_SIZE][GRID_SIZE], uint8_t new_grid[GRID_SIZE][GRID_SIZE], int start_row, int end_row);
int count_neighbors(uint8_t grid[GRID_SIZE][GRID_SIZE], int x, int y);

int main(int argc, char** argv) {
    int rank, size;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (GRID_SIZE % size != 0) {
        if (rank == 0) {
            fprintf(stderr, "Grid size must be divisible by the number of processes.\n");
        }
        MPI_Finalize();
        return EXIT_FAILURE;
    }

    int rows_per_process = GRID_SIZE / size;
    uint8_t(*grid)[GRID_SIZE] = malloc(GRID_SIZE * GRID_SIZE * sizeof(uint8_t));
    uint8_t(*new_grid)[GRID_SIZE] = malloc(GRID_SIZE * GRID_SIZE * sizeof(uint8_t));

    if (grid == NULL || new_grid == NULL) {
        fprintf(stderr, "Memory allocation failed.\n");
        MPI_Finalize();
        return EXIT_FAILURE;
    }

    if (rank == 0) {
        initialize_grid(grid);
    }

    MPI_Bcast(grid, GRID_SIZE * GRID_SIZE, MPI_UINT8_T, 0, MPI_COMM_WORLD);

    for (int iter = 1; iter < ITERATIONS; iter++) {
        int start_row = rank * rows_per_process;
        int end_row = start_row + rows_per_process;

        #pragma omp parallel for
        for (int i = start_row; i < end_row; i++) {
            for (int j = 0; j < GRID_SIZE; j++) {
                int neighbors = count_neighbors(grid, i, j);
                if (grid[i][j] == 1) {
                    new_grid[i][j] = (neighbors == 2 || neighbors == 3) ? 1 : 0;
                } else {
                    new_grid[i][j] = (neighbors == 3) ? 1 : 0;
                }
            }
        }

        MPI_Allgather(MPI_IN_PLACE, rows_per_process * GRID_SIZE, MPI_UINT8_T, new_grid, rows_per_process * GRID_SIZE, MPI_UINT8_T, MPI_COMM_WORLD);
        copy_grid(grid, new_grid);

        int local_population = 0;
        for (int i = start_row; i < end_row; i++) {
            for (int j = 0; j < GRID_SIZE; j++) {
                local_population += grid[i][j];
            }
        }

        int total_population;
        MPI_Reduce(&local_population, &total_population, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);

        if (rank == 0) {
            printf("Iteration %d: Population = %d\n", iter, total_population);
        }
    }

    if (rank == 0) {
        int population = 0;
        for (int i = 0; i < GRID_SIZE; i++) {
            for (int j = 0; j < GRID_SIZE; j++) {
                population += grid[i][j];
            }
        }
        printf("Final population: %d\n", population);
    }

    free(grid);
    free(new_grid);
    MPI_Finalize();
    return EXIT_SUCCESS;
}

void initialize_grid(uint8_t grid[GRID_SIZE][GRID_SIZE]) {
    for (int i = 0; i < GRID_SIZE; i++) {
        for (int j = 0; j < GRID_SIZE; j++) {
            grid[i][j] = 0;
        }
    }

    int offset_x = 1500;
    int offset_y = 1500;

    for (int i = 0; i < GROWER_HEIGHT; i++) {
        for (int j = 0; j < GROWER_WIDTH; j++) {
            grid[offset_x + i][offset_y + j] = grower[i][j];
        }
    }
}

void copy_grid(uint8_t dest[GRID_SIZE][GRID_SIZE], uint8_t src[GRID_SIZE][GRID_SIZE]) {
    for (int i = 0; i < GRID_SIZE; i++) {
        for (int j = 0; j < GRID_SIZE; j++) {
            dest[i][j] = src[i][j];
        }
    }
}

int count_neighbors(uint8_t grid[GRID_SIZE][GRID_SIZE], int x, int y) {
    int count = 0;
    for (int i = -1; i <= 1; i++) {
        for (int j = -1; j <= 1; j++) {
            if (i == 0 && j == 0) continue;

            int nx = x + i;
            int ny = y + j;

            if (nx >= 0 && nx < GRID_SIZE && ny >= 0 && ny < GRID_SIZE) {
                count += grid[nx][ny];
            }
        }
    }
    return count;
}
