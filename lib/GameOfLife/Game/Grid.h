#ifndef GRID_H
#define GRID_H
#include <unordered_set>
#include <vector>

#include "BaseGrid.h"
#include "HashFunction.h"
#include "File/FormatConfig.h"

#define DEFAULT_MAX_ROWS 2048
#define DEFAULT_MAX_COLS 2048

namespace GameOfLife::Game {

    class Grid : public BaseGrid<std::vector<std::vector<bool>>, bool> {
    private:
        std::vector<std::vector<bool>> cells;
        std::vector<std::vector<bool>> next;
        std::unordered_set<std::pair<int, int>, HashFunction> livingCells;
        std::unordered_set<std::pair<int, int>, HashFunction> changedCells;
        int rows;
        int cols;
        int maxRows;
        int maxCols;

        int multiThreadedThreshold = 1000;
        bool isDynamic;

        File::FormatConfig formatConfig = File::FormatConfig('O', '.', '\0');

        void setAliveNext(int row, int col, bool alive);
        void multiThreadedStep(bool wrap);

    public:
        Grid() = delete;
        Grid(int rows, int cols, int maxRows = DEFAULT_MAX_ROWS, int maxCols = DEFAULT_MAX_COLS, bool isDynamic = true);
        Grid(const std::vector<std::vector<bool>> &cells, int rows, int cols, int maxRows = DEFAULT_MAX_ROWS,
            int maxCols = DEFAULT_MAX_COLS, bool isDynamic = true);
        ~Grid() override = default;

        void setAlive(int row, int col, bool alive) override;
        [[nodiscard]] bool isAlive(int row, int col) const override;
        [[nodiscard]] int countNeighbors(int row, int col, bool wrap) const override;

        void step() override;
        void step(bool wrap) override;
        void step(bool wrap, bool dynamic = true);
        void randomize(float aliveProbability) override;
        void clear() override;

        void move(int fromRow, int fromCol, int numRows, int numCols, int toRow, int toCol);
        void resize(int addNorth, int addEast, int addSouth, int addWest);
        void insert(const std::vector<std::vector<bool>> &cells, int row, int col, bool hollow = false);

        void print() const override;
        void print(int fromRow, int fromCol, int toRow, int toCol) const override;

        [[nodiscard]] std::vector<std::vector<bool>> getCells() const override { return cells; }

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

#endif //GRID_H
