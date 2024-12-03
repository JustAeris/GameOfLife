#ifndef CELL_H
#define CELL_H

namespace GameOfLife::Game {
    /**
     * Basic cell implementation, it can be derived to add more complex behavior
     */
    class BaseCell {
    private:
        bool alive;
    public:
        BaseCell();
        explicit BaseCell(bool alive);
        virtual ~BaseCell() = default;

        [[nodiscard]] virtual bool isAlive() const;
        void setAlive(bool alive);
        [[nodiscard]] virtual bool willBeAlive(int aliveNeighbours) const;

        void toggle();
        void print() const;

        BaseCell& operator=(bool alive);
        bool operator==(const BaseCell &cell) const;
    };
}
#endif //CELL_H
