#ifndef GRID_H
#define GRID_H
#include <unordered_set>
#include <vector>

#include "BaseGrid.h"
#include "HashFunction.h"

#define DEFAULT_MAX_ROWS 2048
#define DEFAULT_MAX_COLS 2048

namespace GameOfLife::Game {

    class Grid {
    private:
        std::vector<std::vector<bool>> cells;
        std::vector<std::vector<bool>> next;
        std::unordered_set<std::pair<int, int>, HashFunction> livingCells;
        int rows;
        int cols;
        int maxRows;
        int maxCols;

        bool isMultiThreaded;
        int multiThreadedThreshold;

        bool isDynamic;

        void setAliveNext(int row, int col, bool alive);
        void multiThreadedStep(bool wrap);

    public:
        Grid(int rows, int cols, int maxRows = DEFAULT_MAX_ROWS, int maxCols = DEFAULT_MAX_COLS, bool isDynamic = true);
        Grid(const std::vector<std::vector<bool>> &cells, int rows, int cols, int maxRows = DEFAULT_MAX_ROWS,
            int maxCols = DEFAULT_MAX_COLS, bool isDynamic = true);
        Grid(const std::vector<std::vector<BaseCell>> &cells, int rows, int cols, int maxRows = DEFAULT_MAX_ROWS,
            int maxCols = DEFAULT_MAX_COLS, bool isDynamic = true);
        ~Grid() = default;

        void setAlive(int row, int col, bool alive);
        [[nodiscard]] bool isAlive(int row, int col) const;
        [[nodiscard]] int countNeighbors(int row, int col, bool wrap) const;
        void toggle(int row, int col);

        void step();
        void step(bool wrap);
        void randomize();
        void clear();

        void move(int fromRow, int fromCol, int numRows, int numCols, int toRow, int toCol);
        void resize(int addRows, int addCols);

        void print() const;
        void print(int fromRow, int fromCol, int toRow, int toCol) const;

        [[nodiscard]] std::vector<std::vector<BaseCell>> getCells() const;
        [[nodiscard]] std::vector<std::vector<bool>> toBooleanGrid() const;

        [[nodiscard]] int getRows() const { return rows; }
        [[nodiscard]] int getCols() const { return cols; }

        [[nodiscard]] int getMaxRows() const { return maxRows; }
        [[nodiscard]] int getMaxCols() const { return maxCols; }
    };

}

#endif //GRID_H
