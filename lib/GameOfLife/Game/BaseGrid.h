#ifndef BASEGRID_H
#define BASEGRID_H
#include "BaseCell.h"


namespace GameOfLife::Game {
    /**
     * Base interface for a grid
     */
    class BaseGrid {
    public:
        BaseGrid() = default;
        virtual ~BaseGrid() = default;

        virtual void setAlive(int row, int col, bool alive) = 0;
        [[nodiscard]] virtual bool isAlive(int row, int col) const = 0;
        [[nodiscard]] virtual int countNeighbors(int row, int col, bool wrap) const = 0;
        virtual void toggle(int row, int col) = 0;

        virtual void step() = 0;
        virtual void step(bool wrap) = 0;
        virtual void randomize() = 0;
        virtual void clear() = 0;

        virtual void print() const = 0;
        virtual void print(int fromRow, int fromCol, int toRow, int toCol) const = 0;

        [[nodiscard]] virtual std::vector<std::vector<BaseCell>> getCells() const = 0;
        [[nodiscard]] virtual std::vector<std::vector<bool>> toBooleanGrid() const = 0;
    };
}


#endif //BASEGRID_H
