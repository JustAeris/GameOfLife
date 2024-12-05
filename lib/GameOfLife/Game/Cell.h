#ifndef CELL_H
#define CELL_H
#include "BaseCell.h"

namespace GameOfLife::Game {
    /**
     * Basic cell implementation, it can be derived to add more complex behavior
     */
    class Cell : public BaseCell {
    private:
        bool alive;
        bool obstacle;
    public:
        Cell();
        explicit Cell(bool alive);
        Cell(bool alive, bool obstacle);
        ~Cell() override = default;

        [[nodiscard]] bool isAlive() const override;
        void setAlive(bool alive) override;
        [[nodiscard]] bool willBeAlive(int aliveNeighbours) const override;

        [[nodiscard]] bool isObstacle() const;
        void setObstacle(bool obstacle);

        void print() const override;

        Cell& operator=(bool alive) override;
        bool operator==(const Cell &cell) const;
        explicit operator bool() const;
    };
}
#endif //CELL_H
