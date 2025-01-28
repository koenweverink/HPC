#include <stdio.h>
#include <stdlib.h>
#include "glider.h"  // Include the glider pattern

#define ROWS 10
#define COLS 10
#define GENERATIONS 5

// Function prototypes
void initialize_grid(int grid[ROWS][COLS]);
void load_pattern(int grid[ROWS][COLS], int start_row, int start_col, const uint8_t pattern[][GLIDER_WIDTH], int pattern_rows, int pattern_cols);
void simulate(int grid[ROWS][COLS], int next_grid[ROWS][COLS]);
int count_alive_neighbors(int grid[ROWS][COLS], int row, int col);
void print_grid(int grid[ROWS][COLS]);

int main() {
    int grid[ROWS][COLS];       // Current grid
    int next_grid[ROWS][COLS];  // Next grid (temporary storage)

    // Initialize the grid with all dead cells
    initialize_grid(grid);

    // Load the glider pattern
    load_pattern(grid, 1, 1, glider, GLIDER_HEIGHT, GLIDER_WIDTH);

    // Print the initial grid
    printf("Initial Grid:\n");
    print_grid(grid);

    // Simulate the Game of Life for a few generations
    for (int generation = 1; generation <= GENERATIONS; generation++) {
        simulate(grid, next_grid);

        // Copy next_grid into grid
        for (int i = 0; i < ROWS; i++) {
            for (int j = 0; j < COLS; j++) {
                grid[i][j] = next_grid[i][j];
            }
        }

        // Print the grid for the current generation
        printf("Generation %d:\n", generation);
        print_grid(grid);
    }

    return 0;
}

// Initialize the grid with all cells set to dead (0)
void initialize_grid(int grid[ROWS][COLS]) {
    for (int i = 0; i < ROWS; i++) {
        for (int j = 0; j < COLS; j++) {
            grid[i][j] = 0;
        }
    }
}

// Load a pattern into the grid
void load_pattern(int grid[ROWS][COLS], int start_row, int start_col, const uint8_t pattern[][GLIDER_WIDTH], int pattern_rows, int pattern_cols) {
    for (int i = 0; i < pattern_rows; i++) {
        for (int j = 0; j < pattern_cols; j++) {
            grid[start_row + i][start_col + j] = pattern[i][j];
        }
    }
}

// Simulate one generation of the Game of Life
void simulate(int grid[ROWS][COLS], int next_grid[ROWS][COLS]) {
    for (int i = 1; i < ROWS - 1; i++) {
        for (int j = 1; j < COLS - 1; j++) {
            int alive_neighbors = count_alive_neighbors(grid, i, j);

            // Apply the rules of the Game of Life
            if (grid[i][j] == 1) {
                // Cell survives if it has 2 or 3 alive neighbors
                next_grid[i][j] = (alive_neighbors == 2 || alive_neighbors == 3) ? 1 : 0;
            } else {
                // Cell is born if it has exactly 3 alive neighbors
                next_grid[i][j] = (alive_neighbors == 3) ? 1 : 0;
            }
        }
    }

    // Ensure borders remain dead
    for (int i = 0; i < ROWS; i++) {
        next_grid[i][0] = 0;           // Left border
        next_grid[i][COLS - 1] = 0;    // Right border
    }
    for (int j = 0; j < COLS; j++) {
        next_grid[0][j] = 0;           // Top border
        next_grid[ROWS - 1][j] = 0;    // Bottom border
    }
}

// Count the number of alive neighbors of a cell
int count_alive_neighbors(int grid[ROWS][COLS], int row, int col) {
    int count = 0;
    for (int i = -1; i <= 1; i++) {
        for (int j = -1; j <= 1; j++) {
            if (i != 0 || j != 0) {  // Exclude the cell itself
                count += grid[row + i][col + j];
            }
        }
    }
    return count;
}

// Print the grid
void print_grid(int grid[ROWS][COLS]) {
    for (int i = 0; i < ROWS; i++) {
        for (int j = 0; j < COLS; j++) {
            printf("%c ", grid[i][j] ? 'O' : '.');  // 'O' for alive, '.' for dead
        }
        printf("\n");
    }
    printf("\n");
}
