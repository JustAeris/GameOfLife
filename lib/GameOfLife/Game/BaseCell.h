#ifndef BASECELL_H
#define BASECELL_H

namespace GameOfLife::Game {
    /**
     * Basic cell interface, it can be derived to add more complex behavior
     */
    class BaseCell {
    private:
        bool alive = false;
    public:
        BaseCell() = default;
        explicit BaseCell(bool alive);
        virtual ~BaseCell() = default;

        [[nodiscard]] virtual bool isAlive() const = 0;
        virtual void setAlive(bool alive) = 0;
        [[nodiscard]] virtual bool willBeAlive(int aliveNeighbours) const = 0;

        virtual void print() const = 0;

        virtual BaseCell& operator=(bool alive) = 0;
        virtual bool operator==(const BaseCell &cell) const;
    };
}

#endif //BASECELL_H
