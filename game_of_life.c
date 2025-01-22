#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "glider.h"  // Include the glider pattern header file

#define ROWS 3002
#define COLS 3002
#define GENERATIONS 5000

// Function prototypes
void initialize_grid(int **grid);
void load_pattern(int **grid, int start_row, int start_col, const uint8_t pattern[][GLIDER_WIDTH], int pattern_rows, int pattern_cols);
void simulate(int **grid, int **next_grid);
int count_alive_neighbors(int **grid, int row, int col);
void free_grid(int **grid);
void print_small_grid(int **grid, int rows, int cols);

int main() {
    // Allocate memory for the grids
    int **grid = malloc(ROWS * sizeof(int *));
    int **next_grid = malloc(ROWS * sizeof(int *));
    for (int i = 0; i < ROWS; i++) {
        grid[i] = malloc(COLS * sizeof(int));
        next_grid[i] = malloc(COLS * sizeof(int));
    }

    // Initialize the grid
    initialize_grid(grid);

    // Load the glider pattern into the center of the grid
    load_pattern(grid, 1500, 1500, glider, GLIDER_HEIGHT, GLIDER_WIDTH);

    // Print a small section of the grid to verify the pattern
    printf("Initial Grid (center region):\n");
    print_small_grid(grid, 10, 10);

    // Simulate the Game of Life for a set number of generations
    for (int generation = 1; generation <= GENERATIONS; generation++) {
        simulate(grid, next_grid);

        // Swap the grids
        int **temp = grid;
        grid = next_grid;
        next_grid = temp;

        // Print progress for debugging
        if (generation % 1000 == 0) {
            printf("Generation %d (center region):\n", generation);
            print_small_grid(grid, 10, 10);
        }
    }

    // Free allocated memory
    free_grid(grid);
    free_grid(next_grid);

    return 0;
}

// Initialize the grid with all dead cells
void initialize_grid(int **grid) {
    for (int i = 0; i < ROWS; i++) {
        for (int j = 0; j < COLS; j++) {
            grid[i][j] = 0;
        }
    }
}

// Load a pattern into the grid
void load_pattern(int **grid, int start_row, int start_col, const uint8_t pattern[][GLIDER_WIDTH], int pattern_rows, int pattern_cols) {
    for (int i = 0; i < pattern_rows; i++) {
        for (int j = 0; j < pattern_cols; j++) {
            grid[start_row + i][start_col + j] = pattern[i][j];
        }
    }
}

// Simulate one generation of the Game of Life
void simulate(int **grid, int **next_grid) {
    for (int i = 1; i < ROWS - 1; i++) {
        for (int j = 1; j < COLS - 1; j++) {
            int alive_neighbors = count_alive_neighbors(grid, i, j);

            // Apply the rules of the Game of Life
            if (grid[i][j] == 1) {
                next_grid[i][j] = (alive_neighbors == 2 || alive_neighbors == 3) ? 1 : 0;
            } else {
                next_grid[i][j] = (alive_neighbors == 3) ? 1 : 0;
            }
        }
    }

    // Ensure borders remain dead
    for (int i = 0; i < ROWS; i++) {
        next_grid[i][0] = 0;
        next_grid[i][COLS - 1] = 0;
    }
    for (int j = 0; j < COLS; j++) {
        next_grid[0][j] = 0;
        next_grid[ROWS - 1][j] = 0;
    }
}

// Count the number of alive neighbors of a cell
int count_alive_neighbors(int **grid, int row, int col) {
    int count = 0;
    for (int i = -1; i <= 1; i++) {
        for (int j = -1; j <= 1; j++) {
            if (i != 0 || j != 0) {
                count += grid[row + i][col + j];
            }
        }
    }
    return count;
}

// Free the memory for a grid
void free_grid(int **grid) {
    for (int i = 0; i < ROWS; i++) {
        free(grid[i]);
    }
    free(grid);
}

// Print a small section of the grid (for debugging)
void print_small_grid(int **grid, int rows, int cols) {
    for (int i = 1500; i < 1500 + rows; i++) {
        for (int j = 1500; j < 1500 + cols; j++) {
            printf("%c ", grid[i][j] ? 'O' : '.');
        }
        printf("\n");
    }
    printf("\n");
}
