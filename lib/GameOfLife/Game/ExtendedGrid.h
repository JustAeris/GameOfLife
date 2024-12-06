#ifndef EXTENDEDGRID_H
#define EXTENDEDGRID_H
#include <unordered_set>
#include <vector>

#include "Cell.h"
#include "BaseGrid.h"
#include "HashFunction.h"
#include "File/FormatConfig.h"


namespace GameOfLife::Game {
    /**
     * Advanced implementation of a grid, supporting obstacles
     */
    class ExtendedGrid : public BaseGrid<std::vector<std::vector<Cell>>, Cell> {
    private:
        std::vector<std::vector<Cell>> cells;
        std::vector<std::vector<Cell>> next;
        std::unordered_set<std::pair<int, int>, HashFunction> livingCells;
        std::unordered_set<std::pair<int, int>, HashFunction> changedCells;

        int rows;
        int cols;
        int maxRows;
        int maxCols;

        int multiThreadedThreshold = 100000;
        bool isDynamic;

        File::FormatConfig formatConfig = File::FormatConfig('X', '.', '\0');
        char livingObstacle = 'x';
        char deadObstacle = 'o';

        void setAliveNext(int row, int col, bool alive);
        void multiThreadedStep(bool wrap);

    public:
        ExtendedGrid() = delete;
        ExtendedGrid(int rows, int cols, int maxRows = 1000, int maxCols = 1000, bool isDynamic = true);
        ExtendedGrid(const std::vector<std::vector<Cell>> &cells, int rows, int cols, int maxRows = 1000,
            int maxCols = 1000, bool isDynamic = true);

        void setAlive(int row, int col, bool alive) override;
        [[nodiscard]] bool isAlive(int row, int col) const override;
        [[nodiscard]] bool isObstacle(int row, int col) const;
        [[nodiscard]] int countNeighbors(int row, int col, bool wrap) const override;

        void step() override;
        void step(bool wrap) override;
        void step(bool wrap, bool dynamic = true);
        void randomize(float aliveProbability) override;
        void clear() override;

        void move(int fromRow, int fromCol, int numRows, int numCols, int toRow, int toCol);
        void resize(int addNorth, int addEast, int addSouth, int addWest);
        void insert(const std::vector<std::vector<Cell>> &cells, int row, int col, bool hollow = false);

        void print() const override;
        void print(int fromRow, int fromCol, int toRow, int toCol) const override;

        [[nodiscard]] std::vector<std::vector<Cell>> getCells() const override { return cells; }

        [[nodiscard]] int getRows() const override { return rows; }
        [[nodiscard]] int getCols() const override { return cols; }

        [[nodiscard]] int getMaxRows() const { return maxRows; }
        [[nodiscard]] int getMaxCols() const { return maxCols; }

        [[nodiscard]] std::unordered_set<std::pair<int, int>, HashFunction> getLivingCells() const { return livingCells; }
        [[nodiscard]] std::unordered_set<std::pair<int, int>, HashFunction> getChangedCells() const { return changedCells; }

        void setFormatConfig(const File::FormatConfig &formatConfig) { this->formatConfig = formatConfig; }
        [[nodiscard]] File::FormatConfig getFormatConfig() const { return formatConfig; }

        [[nodiscard]] std::string getText() const override;
    };
}


#endif //EXTENDEDGRID_H
