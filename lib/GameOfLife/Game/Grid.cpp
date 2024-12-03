#include "Grid.h"

#include <iostream>
#include <thread>

#include "HashFunction.h"

namespace GameOfLife::Game {
    Grid::Grid(int rows, int cols, int maxRows, int maxCols, bool isDynamic) :
    rows(rows), cols(cols), maxRows(maxRows), maxCols(maxCols), isDynamic(isDynamic) {
        cells.resize(rows, std::vector<bool>(cols));
        next = cells;

        isMultiThreaded = false;
    }

    Grid::Grid(const std::vector<std::vector<bool>> &cells, int rows, int cols, int maxRows, int maxCols, bool isDynamic) :
    rows(rows), cols(cols), maxRows(maxRows), maxCols(maxCols), isDynamic(isDynamic) {
        this->cells = cells;
        next = cells;

        isMultiThreaded = false;

        // Populate living cells
        for (int i = 0; i < rows; i++) {
            for (int j = 0; j < cols; j++) {
                if (cells[i][j])
                    livingCells.insert(std::make_pair(i, j));
            }
        }
    }

    void Grid::setAlive(int row, int col, bool alive) {
        cells[row][col] = alive;
        if (alive)
            livingCells.insert(std::make_pair(row, col));
        else
            livingCells.erase(std::make_pair(row, col));
    }

    void Grid::setAliveNext(int row, int col, bool alive) {
        next[row][col] = alive;
        if (alive)
            livingCells.insert(std::make_pair(row, col));
        else
            livingCells.erase(std::make_pair(row, col));
    }


    bool Grid::isAlive(int row, int col) const {
        return cells[row][col];
    }

    void Grid::toggle(int row, int col) {
        cells[row][col] = !cells[row][col];
    }

    int Grid::countNeighbors(int row, int col, bool wrap) const {
        int count = 0;

        for (int i = row - 1; i <= row + 1; i++) {
            for (int j = col - 1; j <= col + 1; j++) {
                if (i == row && j == col) {
                    continue;
                }

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
     * Steps the grid to the next generation.
     *
     * @param wrap If true, the grid will wrap around the edges. This will disable dynamic resizing.
     */
    void Grid::step(bool wrap) {
        // Check if the grid should be resized
        if (isDynamic && !wrap) {
            // Check if a living cell is on the edge
            bool onEdge = false;
            for (auto &cell : livingCells) {
                if (cell.first == 0 || cell.first == rows - 1 || cell.second == 0 || cell.second == cols - 1) {
                    onEdge = true;
                    break;
                }
            }
            // Add 10% to the size (rounded up) if a living cell is on the edge
            if (onEdge) {
                resize(rows / 10 + 1, cols / 10 + 1);
            }
        }

        if (livingCells.size() > multiThreadedThreshold) {
            multiThreadedStep(wrap);
            return;
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

            if (cells[row][col]) {
                if (neighbors < 2 || neighbors > 3) {
                    setAliveNext(row, col, false);
                } else {
                    setAliveNext(row, col, true);
                }
            } else {
                if (neighbors == 3) {
                    setAliveNext(row, col, true);
                } else {
                    setAliveNext(row, col, false);
                }
            }
        }

        // Move the next generation to the current generation
        cells = next;

        // Clear the next generation
        for (auto &cell : livingCells) {
            next[cell.first][cell.second] = false;
        }
    }

    void Grid::step() {
        step(false);
    }


    void Grid::multiThreadedStep(bool wrap) {
        // Assume that the size check has already been done

        const std::pair<int, int> directions[] = {
            { -1, -1 }, { -1, 0 }, { -1, 1 },
            { 0, -1 }, {0, 0}, { 0, 1 },
            { 1, -1 }, { 1, 0 }, { 1, 1 }
        };

        auto cellsToCheck = std::unordered_set<std::pair<int, int>, HashFunction>();
        std::mutex cellsToCheckMutex;

        // Function to process a chunk of living cells
        auto checkCells = [&](int start, int end) {
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
        auto updateCells = [&](int start, int end) {
            for (int idx = start; idx < end; ++idx) {
                auto cell = *std::next(cellsToCheck.begin(), idx);
                const int row = cell.first;
                const int col = cell.second;

                const int neighbors = countNeighbors(row, col, wrap);

                if (cells[row][col]) {
                    if (neighbors < 2 || neighbors > 3) {
                        setAliveNext(row, col, false);
                    } else {
                        setAliveNext(row, col, true);
                    }
                } else {
                    if (neighbors == 3) {
                        setAliveNext(row, col, true);
                    } else {
                        setAliveNext(row, col, false);
                    }
                }
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


    void Grid::move(int fromRow, int fromCol, int numRows, int numCols, int toRow, int toCol) {
        // Extract the submatrix
        std::vector submatrix(numRows, std::vector<bool>(numCols));
        for (int i = 0; i < numRows; ++i) {
            for (int j = 0; j < numCols; ++j) {
                submatrix[i][j] = cells[fromRow + i][fromCol + j];
            }
        }

        // Clear the original location
        for (int i = 0; i < numRows; ++i) {
            for (int j = 0; j < numCols; ++j) {
                cells[fromRow + i][fromCol + j] = false; // Assuming false is the default value
            }
        }

        // Insert the submatrix at the new location
        for (int i = 0; i < numRows; ++i) {
            for (int j = 0; j < numCols; ++j) {
                cells[toRow + i][toCol + j] = submatrix[i][j];
            }
        }

        // Update the living cells
        livingCells.clear();
        for (int i = 0; i < numRows; ++i) {
            for (int j = 0; j < numCols; ++j) {
                if (submatrix[i][j]) {
                    livingCells.insert(std::make_pair(toRow + i, toCol + j));
                }
            }
        }
    }



    void Grid::resize(int addRows, int addCols) {
        // Calculate new dimensions
        const int newRows = rows + addRows * 2;
        const int newCols = cols + addCols * 2;

        // Resize the cells
        cells.resize(newRows, std::vector<bool>(newCols));
        // Resize each row
        for (int i = 0; i < rows; ++i) {
            cells[i].resize(newCols);
        }

        move(0, 0, rows, cols, addRows, addCols);

        // Update the dimensions
        rows = newRows;
        cols = newCols;

        // Resize the next generation
        next = cells;
    }

    void Grid::randomize() {
        for (int i = 0; i < rows; i++) {
            for (int j = 0; j < cols; j++) {
                setAlive(i, j, rand() % 2);
            }
        }
    }

    void Grid::clear() {
        // Clear the living cells
        for (auto &cell : livingCells) {
            setAlive(cell.first, cell.second, false);
        }
        livingCells.clear();
    }

    std::vector<std::vector<BaseCell> > Grid::getCells() const {
        std::vector baseCells(rows, std::vector<BaseCell>(cols));
        for (int i = 0; i < rows; i++) {
            for (int j = 0; j < cols; j++) {
                baseCells[i][j] = BaseCell(cells[i][j]);
            }
        }
        return baseCells;
    }

    std::vector<std::vector<bool> > Grid::toBooleanGrid() const {
        return cells;
    }

    void Grid::print() const {
        print(0, 0, rows, cols);
    }

    void Grid::print(int fromRow, int fromCol, int toRow, int toCol) const {
        for (int i = fromRow; i < toRow; i++) {
            for (int j = fromCol; j < toCol; j++) {
                std::cout << (cells[i][j] ? 'O' : '.') << ' ';
            }
            std::cout << std::endl;
        }
    }

}
