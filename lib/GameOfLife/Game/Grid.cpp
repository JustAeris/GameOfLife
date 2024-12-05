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

        changedCells = livingCells;
    }

    Grid::Grid(const std::vector<std::vector<Cell> > &cells, int rows, int cols, int maxRows, int maxCols, bool isDynamic) :
    rows(rows), cols(cols), maxRows(maxRows), maxCols(maxCols), isDynamic(isDynamic) {
        // Initialize the cells
        this->cells.resize(rows, std::vector<bool>(cols));
        for (int i = 0; i < rows; i++) {
            for (int j = 0; j < cols; j++) {
                this->cells[i][j] = cells[i][j].isAlive();
            }
        }
        next = this->cells;

        isMultiThreaded = false;

        // Initialize living cells
        for (int i = 0; i < rows; i++) {
            for (int j = 0; j < cols; j++) {
                if (this->cells[i][j])
                    livingCells.insert(std::make_pair(i, j));
            }
        }

        changedCells = livingCells;
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
            livingCells.emplace(row, col);
        else
            livingCells.erase(std::make_pair(row, col));

        changedCells.emplace(row, col);
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
     * @param dynamic If true, the grid will resize if a living cell is on the edge (overrides isDynamic property).
     */
    void Grid::step(bool wrap, bool dynamic) {
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

        // Clear the changed cells
        changedCells.clear();

        // Directions to check
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



    void Grid::resize(int addNorth, int addEast, int addSouth, int addWest) {
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
                cells[i].resize(cols + addEast);
            }
            cols += addEast;
        }
        if (addSouth > 0) {
            cells.resize(rows + addSouth, std::vector<bool>(cols));
            rows += addSouth;
        }

        if (addNorth > 0) {
            // Insert new rows at the top
            cells.insert(cells.begin(), addNorth, std::vector<bool>(cols));
            rows += addNorth;
        }

        if (addWest > 0) {
            // Insert new columns at the left
            for (int i = 0; i < rows; ++i) {
                cells[i].insert(cells[i].begin(), addWest, false);
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
        next = cells;
    }

    /**
     * Inserts a pattern into the grid.
     *
     * @param cells The pattern to insert
     * @param row The row to insert the pattern at
     * @param col The column to insert the pattern at
     * @param hollow If true, only the living cells will be inserted
     */
    void Grid::insert(const std::vector<std::vector<bool>> &cells, int row, int col, bool hollow) {
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
                    this->cells[row + i][col + j] = cells[i][j];
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
            changedCells.insert(cell);
        }
        livingCells.clear();
    }

    std::vector<std::vector<Cell> > Grid::getCells() const {
        std::vector baseCells(rows, std::vector<Cell>(cols));
        for (int i = 0; i < rows; i++) {
            for (int j = 0; j < cols; j++) {
                baseCells[i][j] = Cell(cells[i][j]);
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
