#include "ExtendedGrid.h"
#include <iostream>
#include <mutex>
#include <sstream>
#include <thread>


namespace GameOfLife::Game {
    ExtendedGrid::ExtendedGrid(const int rows, const int cols, const int maxRows, const int maxCols, const bool isDynamic) :
    rows(rows), cols(cols), maxRows(maxRows), maxCols(maxCols), isDynamic(isDynamic) {
        cells.resize(rows, std::vector<Cell>(cols));
        next = cells;
    }

    ExtendedGrid::ExtendedGrid(const std::vector<std::vector<Cell>> &cells, const int rows, const int cols, const int maxRows, const int maxCols, const bool isDynamic) :
    rows(rows), cols(cols), maxRows(maxRows), maxCols(maxCols), isDynamic(isDynamic) {
        this->cells = cells;
        next = cells;

        // Populate living cells
        for (int i = 0; i < rows; i++) {
            for (int j = 0; j < cols; j++) {
                if (cells[i][j].isAlive())
                    livingCells.insert(std::make_pair(i, j));
            }
        }

        changedCells = livingCells;
    }

    void ExtendedGrid::setAlive(int row, int col, const bool alive) {
        cells[row][col] = alive;
        if (alive)
            livingCells.insert(std::make_pair(row, col));
        else
            livingCells.erase(std::make_pair(row, col));
    }

    void ExtendedGrid::setAliveNext(int row, int col, const bool alive) {
        next[row][col] = alive;
        if (alive)
            livingCells.emplace(row, col);
        else
            livingCells.erase(std::make_pair(row, col));

        changedCells.emplace(row, col);
    }

    bool ExtendedGrid::isAlive(const int row, const int col) const {
        return cells[row][col].isAlive();
    }

    bool ExtendedGrid::isObstacle(int row, int col) const {
        return cells[row][col].isObstacle();
    }

    int ExtendedGrid::countNeighbors(const int row, const int col, const bool wrap) const {
        int count = 0;

        for (int i = row - 1; i <= row + 1; i++) {
            for (int j = col - 1; j <= col + 1; j++) {
                if (i == row && j == col) {
                    continue;
                }

                const int wrappedRow = wrap ? (i + rows) % rows : i;
                const int wrappedCol = wrap ? (j + cols) % cols : j;

                if (wrappedRow >= 0 && wrappedRow < rows && wrappedCol >= 0 && wrappedCol < cols) {
                    count += cells[wrappedRow][wrappedCol].isAlive();
                }
            }
        }

        return count;
    }

    void ExtendedGrid::step(const bool wrap, const bool dynamic) {
        // Clear the changed cells
        changedCells.clear();

        // Check if the grid should be resized
        if (isDynamic && !wrap && dynamic) {
            // Check if a living cell is on the edge
            bool onEdgeNorth = false, onEdgeEast = false, onEdgeSouth = false, onEdgeWest = false;
            for (auto &cell : livingCells) {
                if (cell.first == 0) {
                    onEdgeNorth = true;
                } else if (cell.first == rows - 1) {
                    onEdgeSouth = true;
                } else if (cell.second == 0) {
                    onEdgeWest = true;
                } else if (cell.second == cols - 1) {
                    onEdgeEast = true;
                }
            }
            resize(onEdgeNorth ? 1 : 0, onEdgeEast ? 1 : 0, onEdgeSouth ? 1 : 0, onEdgeWest ? 1 : 0);
        }

        if (livingCells.size() > multiThreadedThreshold) {
            // multiThreadedStep(wrap);
            // return;
        }

        const std::pair<int, int> directions[] = {
            { -1, -1 }, { -1, 0 }, { -1, 1 },
            { 0, -1 }, {0, 0}, { 0, 1 },
            { 1, -1 }, { 1, 0 }, { 1, 1 }
        };

        auto cellsToCheck = std::unordered_set<std::pair<int, int>, HashFunction>();

        for (auto &cell : livingCells) {
            const int row = cell.first;
            const int col = cell.second;

            for (auto &direction : directions) {
                const int newRow = row + direction.first;
                const int newCol = col + direction.second;

                if (newRow >= 0 && newRow < rows && newCol >= 0 && newCol < cols) {
                    cellsToCheck.insert(std::make_pair(newRow, newCol));
                }
            }
        }

        // Check each cell
        for (auto &cell : cellsToCheck) {
            const int row = cell.first;
            const int col = cell.second;

            const int neighbors = countNeighbors(row, col, wrap);

            if (!isObstacle(row, col))
                setAliveNext(row, col, cells[row][col].willBeAlive(neighbors));
        }

        // Move the next generation to the current generation
        cells = next;

        // Clear the next generation
        for (auto &cell : livingCells) {
            next[cell.first][cell.second] = false;
        }
    }

    void ExtendedGrid::step(const bool wrap) {
        step(wrap, !wrap);
    }

    void ExtendedGrid::step() {
        step(false, true);
    }

    void ExtendedGrid::multiThreadedStep(const bool wrap) {
        // Assume that the size check has already been done
        // TODO: Add mutexes to prevent concurrent access to the living cells set

        // Directions to check
        const std::pair<int, int> directions[] = {
            { -1, -1 }, { -1, 0 }, { -1, 1 },
            { 0, -1 }, { 0, 0 }, { 0, 1 },
            { 1, -1 }, { 1, 0 }, { 1, 1 }
        };

        auto cellsToCheck = std::unordered_set<std::pair<int, int>, HashFunction>();
        std::mutex cellsToCheckMutex;

        // Function to process a chunk of living cells
        auto checkCells = [&](const int start, const int end) {
            for (int idx = start; idx < end; ++idx) {
                auto cell = *std::next(livingCells.begin(), idx);
                const int row = cell.first;
                const int col = cell.second;

                for (auto &direction : directions) {
                    const int newRow = row + direction.first;
                    const int newCol = col + direction.second;

                    if (newRow >= 0 && newRow < rows && newCol >= 0 && newCol < cols) {
                        std::lock_guard lock(cellsToCheckMutex);
                        cellsToCheck.insert(std::make_pair(newRow, newCol));
                    }
                }
            }
        };

        int numThreads = std::thread::hardware_concurrency();
        int cellsPerThread = livingCells.size() / numThreads;
        std::vector<std::thread> threads;

        for (int i = 0; i < numThreads; ++i) {
            int start = i * cellsPerThread;
            int end = (i == numThreads - 1) ? livingCells.size() : start + cellsPerThread;
            threads.emplace_back(checkCells, start, end);
        }

        for (auto &thread : threads) {
            thread.join();
        }

        // Check each cell
        // Function to update a chunk of cells
        auto updateCells = [&](const int start, const int end) {
            for (int idx = start; idx < end; ++idx) {
                auto cell = *std::next(cellsToCheck.begin(), idx);
                const int row = cell.first;
                const int col = cell.second;

                const int neighbors = countNeighbors(row, col, wrap);

                setAliveNext(row, col, cells[row][col].willBeAlive(neighbors));
            }
        };

        cellsPerThread = cellsToCheck.size() / numThreads;
        threads.clear();

        for (int i = 0; i < numThreads; ++i) {
            int start = i * cellsPerThread;
            int end = (i == numThreads - 1) ? cellsToCheck.size() : start + cellsPerThread;
            threads.emplace_back(updateCells, start, end);
        }

        for (auto &thread : threads) {
            thread.join();
        }

        // Move the next generation to the current generation
        cells = next;

        // Clear the next generation
        for (auto &cell : livingCells) {
            next[cell.first][cell.second] = false;
        }
    }


    void ExtendedGrid::move(const int fromRow, const int fromCol, const int numRows, const int numCols, const int toRow, const int toCol) {
        BaseGrid::move(cells, livingCells, changedCells, fromRow, fromCol, numRows, numCols, toRow, toCol);
    }



    void ExtendedGrid::resize(const int addNorth, const int addEast, const int addSouth, const int addWest) {
        BaseGrid::resize(cells, next, livingCells, addNorth, addEast, addSouth, addWest, rows, cols, maxRows, maxCols);
    }

    /**
     * Inserts a pattern into the grid.
     *
     * @param cells The pattern to insert
     * @param row The row to insert the pattern at
     * @param col The column to insert the pattern at
     * @param hollow If true, only the living cells will be inserted
     */
    void ExtendedGrid::insert(const std::vector<std::vector<Cell>> &cells, const int row, const int col, const bool hollow) {
        BaseGrid::insert(this->cells, livingCells, changedCells, cells, row, col, rows, cols, maxRows, maxCols, hollow);
    }


    void ExtendedGrid::randomize(const float aliveProbability) {
        if (aliveProbability < 0 || aliveProbability > 1)
            return;
        for (int i = 0; i < rows; i++) {
            for (int j = 0; j < cols; j++) {
                if ((rand() % 100) < aliveProbability * 100) {
                    setAlive(i, j, true);
                    changedCells.insert(std::make_pair(i, j));
                }
            }
        }
    }

    void ExtendedGrid::clear() {
        // Clear the living cells
        for (auto &row : cells) {
            std::fill(row.begin(), row.end(), Cell(false, false));
        }
        livingCells.clear();
        next = cells;
    }

    void ExtendedGrid::print() const {
        print(0, 0, rows, cols);
    }

    void ExtendedGrid::print(const int fromRow, const int fromCol, const int toRow, const int toCol) const {
        for (int i = fromRow; i < toRow; i++) {
            for (int j = fromCol; j < toCol; j++) {
                std::cout << (cells[i][j] ? formatConfig.getAliveChar() : formatConfig.getDeadChar()) << formatConfig.getDelimiterChar();
            }
            std::cout << std::endl;
        }
    }

    std::string ExtendedGrid::getText() const {
        std::stringstream ss;
        for (int i = 0; i < rows; i++) {
            for (int j = 0; j < cols; j++) {
                ss << (cells[i][j].isObstacle() ? (cells[i][j] ? livingObstacle : deadObstacle) :
                    (cells[i][j] ? formatConfig.getAliveChar() : formatConfig.getDeadChar()))
                << formatConfig.getDelimiterChar();
            }
            ss << '\n';
        }
        return ss.str();
    }

}
