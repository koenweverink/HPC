#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <omp.h>
#include "grower.h" // Include the provided grower.h

#define GRID_SIZE 3000
#define ITERATIONS 5000

// Function prototypes
void initialize_grid(uint8_t grid[GRID_SIZE][GRID_SIZE]);
void copy_grid(uint8_t dest[GRID_SIZE][GRID_SIZE], uint8_t src[GRID_SIZE][GRID_SIZE]);
int count_neighbors(uint8_t grid[GRID_SIZE][GRID_SIZE], int x, int y);

int main() {
    // Allocate the grids
    uint8_t (*grid)[GRID_SIZE] = malloc(GRID_SIZE * GRID_SIZE * sizeof(uint8_t));
    uint8_t (*new_grid)[GRID_SIZE] = malloc(GRID_SIZE * GRID_SIZE * sizeof(uint8_t));

    if (grid == NULL || new_grid == NULL) {
        fprintf(stderr, "Memory allocation failed.\n");
        return EXIT_FAILURE;
    }

    // Initialize the grid using grower.h
    initialize_grid(grid);

    for (int iter = 0; iter < ITERATIONS; iter++) {
        // Update the grid in parallel
        #pragma omp parallel for
        for (int i = 0; i < GRID_SIZE; i++) {
            for (int j = 0; j < GRID_SIZE; j++) {
                int neighbors = count_neighbors(grid, i, j);
                if (grid[i][j] == 1) {
                    new_grid[i][j] = (neighbors == 2 || neighbors == 3) ? 1 : 0;
                } else {
                    new_grid[i][j] = (neighbors == 3) ? 1 : 0;
                }
            }
        }

        // Copy the new grid into the current grid
        #pragma omp parallel for
        for (int i = 0; i < GRID_SIZE; i++) {
            for (int j = 0; j < GRID_SIZE; j++) {
                grid[i][j] = new_grid[i][j];
            }
        }

    //     // Calculate population (number of alive cells) in parallel
    //     int total_population = 0;
    //     #pragma omp parallel for reduction(+ : total_population)
    //     for (int i = 0; i < GRID_SIZE; i++) {
    //         for (int j = 0; j < GRID_SIZE; j++) {
    //             total_population += grid[i][j];
    //         }
    //     }

    //     printf("Iteration %d: Population = %d\n", iter + 1, total_population);
    }

    // Final population
    int final_population = 0;
    #pragma omp parallel for reduction(+ : final_population)
    for (int i = 0; i < GRID_SIZE; i++) {
        for (int j = 0; j < GRID_SIZE; j++) {
            final_population += grid[i][j];
        }
    }
    printf("Final population: %d\n", final_population);

    free(grid);
    free(new_grid);
    return EXIT_SUCCESS;
}

void initialize_grid(uint8_t grid[GRID_SIZE][GRID_SIZE]) {
    // Set all cells to 0
    for (int i = 0; i < GRID_SIZE; i++) {
        for (int j = 0; j < GRID_SIZE; j++) {
            grid[i][j] = 0;
        }
    }

    // Offset for placing the grower pattern in the middle of the grid
    int offset_x = (GRID_SIZE - GROWER_HEIGHT) / 2;
    int offset_y = (GRID_SIZE - GROWER_WIDTH) / 2;

    // Copy the grower pattern into the grid
    for (int i = 0; i < GROWER_HEIGHT; i++) {
        for (int j = 0; j < GROWER_WIDTH; j++) {
            grid[offset_x + i][offset_y + j] = grower[i][j];
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
