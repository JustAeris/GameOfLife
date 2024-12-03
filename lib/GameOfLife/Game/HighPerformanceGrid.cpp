#include "HighPerformanceGrid.h"
#include <algorithm>
#include <future>
#include <iostream>


namespace GameOfLife::Game {
    /**
     * Base constructor
     * @param rows No of rows
     * @param cols No of columns
     */
    HighPerformanceGrid::HighPerformanceGrid(const int rows, const int cols) : rows(rows), cols(cols) {
        // Initialize the grid with dead cells
        cells.reserve(rows);
        cells.resize(rows, std::vector<bool>(cols));
        next = cells;

        // If the grid is too large, use multithreading
        isMultiThreaded = rows * cols > 90000 || rows >= 500 || cols >= 500;
    }

    /**
     * Constructor with cells
     * @param cells Cells
     * @param rows No of rows
     * @param cols No of columns
     */
    HighPerformanceGrid::HighPerformanceGrid(const std::vector<std::vector<bool>> &cells, const int rows, const int cols) : rows(rows),
        cols(cols) {
        // Initialize the grid by copying the cells
        this->cells = cells;
        next = cells;

        // If the grid is too large, use multithreading
        isMultiThreaded = rows * cols > 90000 || rows >= 600 || cols >= 600;
    }

    /**
     * Print the grid
     */
    void HighPerformanceGrid::print() const {
        print(0, 0, rows, cols);
    }

    /**
     * Print the grid from a specific range
     * @param fromRow Start row
     * @param fromCol Start column
     * @param toRow End row
     * @param toCol End column
     */
    void HighPerformanceGrid::print(const int fromRow, const int fromCol, const int toRow, const int toCol) const {
        for (int i = fromRow; i < toRow; i++) {
            for (int j = fromCol; j < toCol; j++) {
                std::cout << (cells[i][j] ? "X" : ".");
                std::cout << " ";
            }
            std::cout << std::endl;
        }
        std::cout << std::endl;
    }

    std::vector<std::vector<bool>> HighPerformanceGrid::toBooleanGrid() const {
        return cells;
    }

    /**
     * Set cell alive
     * @param row Row
     * @param col Column
     * @param alive Alive
     */
    void HighPerformanceGrid::setAlive(const int row, const int col, const bool alive) {
        cells[row][col] = alive;
    }

    /**
     * Toggle cell
     * @param row Row
     * @param col Column
     */
    void HighPerformanceGrid::toggle(const int row, const int col) {
        cells[row][col] = !cells[row][col];
    }

    /**
     * Check if cell is alive
     * @param row Row
     * @param col Column
     * @return Alive
     */
    bool HighPerformanceGrid::isAlive(const int row, const int col) const {
        return cells[row][col];
    }

    /**
     * Count living neighbors
     * @param row Row
     * @param col Column
     * @param wrap Wrap, if true, the grid is considered to be a torus
     * @return Living neighbors
     */
    [[nodiscard]]
    int HighPerformanceGrid::countNeighbors(const int row, const int col, const bool wrap) const {
        int count = 0;
        // Define the range of neighbors
        const int rowStart = row - 1;
        const int rowEnd = row + 1;
        const int colStart = col - 1;
        const int colEnd = col + 1;

        // Count the living neighbors
        for (int i = rowStart; i <= rowEnd; i++) {
            for (int j = colStart; j <= colEnd; j++) {
                if (i == row && j == col) {
                    continue;
                }
                // If the grid is a torus, wrap the neighbors
                int wrappedRow = wrap ? (i + rows) % rows : i;
                int wrappedCol = wrap ? (j + cols) % cols : j;
                if (wrappedRow >= 0 && wrappedRow < rows && wrappedCol >= 0 && wrappedCol < cols) {
                    count += cells[wrappedRow][wrappedCol];
                }
            }
        }

        return count;
    }

    /**
     * Step the grid
     */
    void HighPerformanceGrid::step() {
        step(false);
    }

    /**
     * Step the grid
     * @param wrap Wrap, if true, the grid is considered to be a torus
     */
    void HighPerformanceGrid::step(const bool wrap) {
        if (isMultiThreaded) {
            multiThreadedStep(wrap);
        } else {
            sequentialStep(wrap);
        }
    }

    /**
     * Sequential step
     * @param wrap Wrap, if true, the grid is considered to be a torus
     */
    void HighPerformanceGrid::sequentialStep(const bool wrap) {
        // Compute the next state sequentially for each cell
        for (int i = 0; i < rows; i++) {
            for (int j = 0; j < cols; j++) {
                int aliveNeighbours = countNeighbors(i, j, wrap);
                next[i][j] = cells[i][j] ? aliveNeighbours == 2 || aliveNeighbours == 3 : aliveNeighbours == 3;
            }
        }

        // Swap the grids
        std::swap(cells, next);
    }

    /**
     * Multithreaded step
     * @param wrap Wrap, if true, the grid is considered to be a torus
     */
    void HighPerformanceGrid::multiThreadedStep(const bool wrap) {
        // Lambda function to compute the next state
        auto computeNextState = [this, wrap](int startRow, int endRow) {
            for (int i = startRow; i < endRow; i++) {
                for (int j = 0; j < cols; j++) {
                    int aliveNeighbours = countNeighbors(i, j, wrap);
                    next[i][j] = cells[i][j] ? aliveNeighbours == 2 || aliveNeighbours == 3 : aliveNeighbours == 3;
                }
            }
        };

        // Compute the next state using multiple threads
        const int numThreads = static_cast<int>(std::thread::hardware_concurrency());
        const int rowsPerThread = rows / numThreads;
        std::vector<std::future<void>> futures;

        // Start the threads
        for (int t = 0; t < numThreads; t++) {
            int startRow = t * rowsPerThread;
            int endRow = (t == numThreads - 1) ? rows : startRow + rowsPerThread;
            futures.push_back(std::async(std::launch::async, computeNextState, startRow, endRow));
        }

        // Wait for the threads to finish
        for (auto &f : futures) {
            f.get();
        }

        // Swap the grids
        std::swap(cells, next);
    }

    /**
     * Randomize the grid, setting each cell to be alive with a 50% chance
     */
    void HighPerformanceGrid::randomize() {
        for (int i = 0; i < rows; i++) {
            for (int j = 0; j < cols; j++) {
                cells[i][j] = rand() % 2 == 0;
            }
        }
    }

    /**
     * Clear the grid
     */
    void HighPerformanceGrid::clear() {
        // Clear the grid by replacing all cells with new dead cells
        cells = std::vector(rows, std::vector<bool>(cols));
    }
}
