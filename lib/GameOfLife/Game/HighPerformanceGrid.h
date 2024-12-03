#ifndef HIGHPERFORMANCEGRID_H
#define HIGHPERFORMANCEGRID_H
#include <vector>
#include "BaseGrid.h"


namespace GameOfLife::Game {
    /**
     * High performance grid, limited to boolean values, optimized for speed using multithreading
     */
    class HighPerformanceGrid : public BaseGrid {
    private:
        int rows;
        int cols;
        std::vector<std::vector<bool>> cells;
        std::vector<std::vector<bool>> next;

        void sequentialStep(bool wrap);
        bool isMultiThreaded = false;
        void multiThreadedStep(bool wrap);

        HighPerformanceGrid() = delete;
    public:
        HighPerformanceGrid(int rows, int cols);
        HighPerformanceGrid(const std::vector<std::vector<bool>> &cells, int rows, int cols);

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

        [[nodiscard]] std::vector<std::vector<bool>> toBooleanGrid() const override;
    };
}


#endif //HIGHPERFORMANCEGRID_H
