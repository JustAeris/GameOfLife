#include "ExtendedGrid.h"
#include <iostream>


namespace GameOfLife::Game {
    /**
     * Base constructor
     * @param rows No of rows
     * @param cols No of columns
     */
    ExtendedGrid::ExtendedGrid(const int rows, const int cols) : rows(rows), cols(cols) {
        cells.resize(rows, std::vector<BaseCell>(cols));
        next = cells;
    }

    /**
     * Constructor with cells
     * @param cells Cells
     * @param rows No of rows
     * @param cols No of columns
     */
    ExtendedGrid::ExtendedGrid(const std::vector<std::vector<BaseCell>> &cells, const int rows, const int cols) : rows(rows),
        cols(cols) {
        this->cells = cells;
        next = cells;
    }

    /**
     * Print the grid
     */
    void ExtendedGrid::print() const {
        print(0, 0, rows, cols);
    }

    /**
     * Print the grid from a specific range
     * @param fromRow Start row
     * @param fromCol Start column
     * @param toRow End row
     * @param toCol End column
     */
    void ExtendedGrid::print(const int fromRow, const int fromCol, const int toRow, const int toCol) const {
        for (int i = fromRow; i < toRow; i++) {
            for (int j = fromCol; j < toCol; j++) {
                cells[i][j].print();
                std::cout << " ";
            }
            std::cout << std::endl;
        }
        std::cout << std::endl;
    }

    /**
     * Print the grid with a specific format
     * @param format Format
     */
    void ExtendedGrid::print(File::FormatConfig &format) const {
        print(format, 0, 0, rows, cols);
    }

    /**
     * Print the grid with a specific format from a specific range
     * @param format Format
     * @param fromRow Start row
     * @param fromCol Start column
     * @param toRow End row
     * @param toCol End column
     */
    void ExtendedGrid::print(File::FormatConfig &format, int fromRow, int fromCol, int toRow, int toCol) const {
        for (int i = fromRow; i < toRow; i++) {
            for (int j = fromCol; j < toCol; j++) {
                std::cout << (cells[i][j].isAlive() ? format.getAliveChar() : format.getDeadChar());
                if (j < toCol - 1) {
                    std::cout << format.getDelimiterChar();
                }
            }
            std::cout << std::endl;
        }
        std::cout << std::endl;
    }

    /**
     * Set cell alive
     * @param row Row
     * @param col Column
     * @param alive Alive
     */
    void ExtendedGrid::setAlive(const int row, const int col, const bool alive) {
        cells[row][col].setAlive(alive);
    }

    /**
     * Toggle cell
     * @param row Row
     * @param col Column
     */
    void ExtendedGrid::toggle(const int row, const int col) {
        cells[row][col].toggle();
    }

    /**
     * Check if the cell is alive
     * @param row Row
     * @param col Column
     * @return Alive
     */
    bool ExtendedGrid::isAlive(const int row, const int col) const {
        return cells[row][col].isAlive();
    }

    /**
     * Count living neighbors
     * @param row Row
     * @param col Column
     * @param wrap Wrap, if true, the grid is considered to be a torus
     * @return Living neighbors
     */
    [[nodiscard]]
    int ExtendedGrid::countNeighbors(const int row, const int col, bool wrap) const {
        int count = 0;
        const int rowStart = row - 1;
        const int rowEnd = row + 1;
        const int colStart = col - 1;
        const int colEnd = col + 1;

        for (int i = rowStart; i <= rowEnd; i++) {
            for (int j = colStart; j <= colEnd; j++) {
                // Skip the cell itself
                if (i == row && j == col) {
                    continue;
                }
                // If wrap is true, the grid is considered to be a torus
                int wrappedRow = wrap ? (i + rows) % rows : i;
                int wrappedCol = wrap ? (j + cols) % cols : j;
                if (wrappedRow >= 0 && wrappedRow < rows && wrappedCol >= 0 && wrappedCol < cols) {
                    count += cells[wrappedRow][wrappedCol].isAlive();
                }
            }
        }
        return count;
    }

    /**
     * Step the grid
     * @param wrap Wrap, if true, the grid is considered to be a torus
     */
    void ExtendedGrid::step(const bool wrap) {
        for (int i = 0; i < rows; i++) {
            for (int j = 0; j < cols; j++) {
                const int aliveNeighbours = countNeighbors(i, j, wrap);
                next[i][j].setAlive(cells[i][j].willBeAlive(aliveNeighbours));
            }
        }

        std::swap(cells, next);
    }

    /**
     * Step the grid
     */
    void ExtendedGrid::step() {
        step(false);
    }

    /**
     * Randomize the grid
     */
    void ExtendedGrid::randomize() {
        for (int i = 0; i < rows; i++) {
            for (int j = 0; j < cols; j++) {
                cells[i][j].setAlive(rand() % 2 == 0);
            }
        }
    }

    /**
     * Clear the grid
     */
    void ExtendedGrid::clear() {
        for (int i = 0; i < rows; i++) {
            for (int j = 0; j < cols; j++) {
                cells[i][j].setAlive(false);
            }
        }
    }

    /**
     * Get the cells
     * @return Cells
     */
    [[nodiscard]]
    std::vector<std::vector<BaseCell> > ExtendedGrid::getCells() const {
        return cells;
    }

    /**
     * Convert the grid to a boolean grid
     * @return Boolean grid
     */
    [[nodiscard]]
    std::vector<std::vector<bool>> ExtendedGrid::toBooleanGrid() const {
        std::vector boolGrid(rows, std::vector<bool>(cols));
        for (int i = 0; i < rows; i++) {
            for (int j = 0; j < cols; j++) {
                boolGrid[i][j] = cells[i][j].isAlive();
            }
        }
        return boolGrid;
    }
}
