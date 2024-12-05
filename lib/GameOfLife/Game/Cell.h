#ifndef CELL_H
#define CELL_H

namespace GameOfLife::Game {
    /**
     * Basic cell implementation, it can be derived to add more complex behavior
     */
    class Cell {
    private:
        bool alive;
    public:
        Cell();
        explicit Cell(bool alive);
        virtual ~Cell() = default;

        [[nodiscard]] virtual bool isAlive() const;
        void setAlive(bool alive);
        [[nodiscard]] virtual bool willBeAlive(int aliveNeighbours) const;

        void toggle();
        void print() const;

        Cell& operator=(bool alive);
        bool operator==(const Cell &cell) const;
    };
}
#endif //CELL_H
