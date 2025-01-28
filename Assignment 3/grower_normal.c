#include <stdio.h>
#include "grower.h"

#define BIG_GRID_HEIGHT (3000)
#define BIG_GRID_WIDTH (3000)
#define INNER_OFFSET (1500)  // Place grower at (1500, 1500) in big grid

uint8_t bigGrid[BIG_GRID_HEIGHT][BIG_GRID_WIDTH] = {0};

// Initialize the bigGrid with grower's data
void initBigGrid() {
    for (int i = 0; i < GROWER_HEIGHT; i++) {
        for (int j = 0; j < GROWER_WIDTH; j++) {
            bigGrid[INNER_OFFSET + i][INNER_OFFSET + j] = grower[i][j];
        }
    }
}

// Function to count live neighbors of a cell, within the bounds of grower's influence
int countNeighbors(int row, int col) {
    int count = 0;
    for (int i = -1; i <= 1; i++) {
        for (int j = -1; j <= 1; j++) {
            if (i == 0 && j == 0) continue;  // Skip the cell itself
            int newRow = row + i;
            int newCol = col + j;
            
            // Check if the neighbor is within the area influenced by grower
            if (newRow >= INNER_OFFSET && newRow < INNER_OFFSET + GROWER_HEIGHT && 
                newCol >= INNER_OFFSET && newCol < INNER_OFFSET + GROWER_WIDTH) {
                count += bigGrid[newRow][newCol];
            }
        }
    }
    return count;
}

// Function to update the grid according to Game of Life rules, only for the area of grower
void updateGrid() {
    uint8_t newGrid[GROWER_HEIGHT][GROWER_WIDTH] = {0};
    
    for (int i = 0; i < GROWER_HEIGHT; i++) {
        for (int j = 0; j < GROWER_WIDTH; j++) {
            int liveNeighbors = countNeighbors(INNER_OFFSET + i, INNER_OFFSET + j);
            if (bigGrid[INNER_OFFSET + i][INNER_OFFSET + j] == 1) {  // If the cell is alive
                if (liveNeighbors == 2 || liveNeighbors == 3) {
                    newGrid[i][j] = 1;
                }
            } else {  // If the cell is dead
                if (liveNeighbors == 3) {
                    newGrid[i][j] = 1;
                }
            }
        }
    }
    
    // Copy the new grid back to the big grid, only updating grower's area
    for (int i = 0; i < GROWER_HEIGHT; i++) {
        for (int j = 0; j < GROWER_WIDTH; j++) {
            bigGrid[INNER_OFFSET + i][INNER_OFFSET + j] = newGrid[i][j];
        }
    }
}

// Function to count alive cells within grower's area
int countAliveCells() {
    int aliveCount = 0;
    for (int i = 0; i < GROWER_HEIGHT; i++) {
        for (int j = 0; j < GROWER_WIDTH; j++) {
            aliveCount += bigGrid[INNER_OFFSET + i][INNER_OFFSET + j];
        }
    }
    return aliveCount;
}

int main() {
    int generations = 11;  // Number of generations to simulate

    initBigGrid();  // Initialize the big grid with grower's data

    for (int gen = 0; gen < generations; gen++) {
        printf("Generation %d: Population = %d\n", gen, countAliveCells());
        updateGrid();
        if (countAliveCells() == 0) {
            printf("Simulation ended at generation %d with no live cells.\n", gen);
            break;
        }
    }

    return 0;
}