#include "BaseGrid.h"

#include <stdexcept>
#include <unordered_set>
#include <vector>

#include "HashFunction.h"

namespace GameOfLife::Game {
    template<typename TGrid, typename T>
    void BaseGrid<TGrid, T>::move(std::vector<std::vector<T>> &grid,
        std::unordered_set<std::pair<int, int>, HashFunction> &livingCells,
        std::unordered_set<std::pair<int, int>, HashFunction> &changedCells,
        int fromRow, int fromCol, int numRows, int numCols, int toRow, int toCol) {
        // Extract the submatrix
        std::vector submatrix(numRows, std::vector<T>(numCols));
        for (int i = 0; i < numRows; ++i) {
            for (int j = 0; j < numCols; ++j) {
                submatrix[i][j] = grid[fromRow + i][fromCol + j];
            }
        }

        // Clear the original location
        for (int i = 0; i < numRows; ++i) {
            for (int j = 0; j < numCols; ++j) {
                grid[fromRow + i][fromCol + j] = false; // Assuming false is the default value
            }
        }

        // Insert the submatrix at the new location and update living and changed cells
        livingCells.clear();
        changedCells.clear();
        for (int i = 0; i < numRows; ++i) {
            for (int j = 0; j < numCols; ++j) {
                grid[toRow + i][toCol + j] = submatrix[i][j];
                changedCells.insert(std::make_pair(toRow + i, toCol + j));
                if (submatrix[i][j]) {
                    livingCells.insert(std::make_pair(toRow + i, toCol + j));
                }
            }
        }
    }

    template<typename TGrid, typename T>
    void BaseGrid<TGrid, T>::resize(std::vector<std::vector<T>> &grid, std::vector<std::vector<T>> &next,
        std::unordered_set<std::pair<int, int>, HashFunction> &livingCells,
        int addNorth, int addEast, int addSouth, int addWest, int rows, int cols, int maxRows, int maxCols) {
        // Argument check
        if (addNorth < 0 || addEast < 0 || addSouth < 0 || addWest < 0) {
            throw std::invalid_argument("The number of rows and columns to add must be positive.");
        }

        // Check against the maximum size
        if (rows + addNorth + addSouth > maxRows || cols + addEast + addWest > maxCols) {
            return;
        }

        if (addEast > 0) {
            for (int i = 0; i < rows; ++i) {
                grid[i].resize(cols + addEast);
            }
            cols += addEast;
        }
        if (addSouth > 0) {
            grid.resize(rows + addSouth, std::vector<T>(cols));
            rows += addSouth;
        }

        if (addNorth > 0) {
            // Insert new rows at the top
            grid.insert(grid.begin(), addNorth, std::vector<T>(cols));
            rows += addNorth;
        }

        if (addWest > 0) {
            // Insert new columns at the left
            for (int i = 0; i < rows; ++i) {
                grid[i].insert(grid[i].begin(), addWest, T());
            }
            cols += addWest;
        }

        // Update the living cells set
        std::unordered_set<std::pair<int, int>, HashFunction> newLivingCells;
        for (const auto &cell : livingCells) {
            int newRow = cell.first + addNorth;
            int newCol = cell.second + addWest;
            if (newRow >= 0 && newRow < rows && newCol >= 0 && newCol < cols) {
                newLivingCells.insert({newRow, newCol});
            }
        }
        livingCells = newLivingCells;

        // Resize the next generation
        next = grid;
    }

    template<typename TGrid, typename T>
    void BaseGrid<TGrid, T>::insert(std::vector<std::vector<T>> &grid,
        std::unordered_set<std::pair<int, int>, HashFunction> &livingCells,
        std::unordered_set<std::pair<int, int>, HashFunction> &changedCells,
        const std::vector<std::vector<T>> &cells,
        int row, int col, int rows, int cols, int maxRows, int maxCols, bool hollow) {
        // Argument check
        if (row < 0 || row >= rows || col < 0 || col >= cols) {
            throw std::invalid_argument("The row and column must be within the grid.");
        }

        // Check against the maximum size
        if (row + cells.size() > maxRows || col + cells[0].size() > maxCols) {
            return;
        }

        // Insert the cells
        for (int i = 0; i < cells.size(); ++i) {
            for (int j = 0; j < cells[i].size(); ++j) {
                if (!hollow || cells[i][j]) {
                    grid[row + i][col + j] = cells[i][j];
                    changedCells.insert({row + i, col + j});
                }
            }
        }

        // Update the living cells set
        for (int i = 0; i < cells.size(); ++i) {
            for (int j = 0; j < cells[i].size(); ++j) {
                if (cells[i][j]) {
                    livingCells.insert({row + i, col + j});
                }
            }
        }
    }
}
