#ifndef EXTENDEDGRID_H
#define EXTENDEDGRID_H
#include <vector>

#include "Cell.h"
#include "BaseGrid.h"
#include "File/FormatConfig.h"


namespace GameOfLife::Game {
    /**
     * Basic grid implementation
     */
    class ExtendedGrid final : public BaseGrid {
    private:
        int rows;
        int cols;
        std::vector<std::vector<Cell>> cells;
        std::vector<std::vector<Cell>> next;
        ExtendedGrid() = delete;
    public:
        ExtendedGrid(int rows, int cols);
        ExtendedGrid(const std::vector<std::vector<Cell>> &cells, int rows, int cols);

        void setAlive(int row, int col, bool alive) override;
        [[nodiscard]] bool isAlive(int row, int col) const override;
        [[nodiscard]] int countNeighbors(int row, int col, bool wrap) const override;
        void toggle(int row, int col) override;

        void step() override;
        void step(bool wrap) override;
        void randomize() override;
        void clear() override;

        void print() const override;
        void print(int fromRow, int fromCol, int toRow, int toCol) const override;
        void print(File::FormatConfig &format) const;
        void print(File::FormatConfig &format, int fromRow, int fromCol, int toRow, int toCol) const;

        [[nodiscard]] std::vector<std::vector<Cell>> getCells() const override;
        [[nodiscard]] std::vector<std::vector<bool>> toBooleanGrid() const override;
    };
}


#endif //EXTENDEDGRID_H
