#include "Cell.h"

#include <iostream>

namespace GameOfLife::Game {
    /**
     * Default constructor
     */
    Cell::Cell() : alive(false) {}

    /**
     * Base constructor
     * @param alive Alive
     */
    Cell::Cell(const bool alive) : alive(alive) {}

    /**
     * Check if the cell is alive
     * @return Alive
     */
    bool Cell::isAlive() const {
        return alive;
    }

    /**
     * Set cell alive
     * @param alive Alive
     */
    void Cell::setAlive(const bool alive) {
        this->alive = alive;
    }

    /**
     * Toggle cell
     */
    void Cell::toggle() {
        alive = !alive;
    }

    /**
     * Print the cell
     */
    void Cell::print() const {
        std::cout << (alive ? "X" : ".");
    }

    /**
     * Check if the cell will be alive in the next generation
     * @param livingNeighbors Living neighbors
     * @return Will be alive
     */
    bool Cell::willBeAlive(const int livingNeighbors) const {
        if (alive) {
            return livingNeighbors == 2 || livingNeighbors == 3;
        }
        return livingNeighbors == 3;
    }

    /**
     * Assign alive value
     * @param alive Alive
     * @return Cell
     */
    Cell& Cell::operator=(bool alive) {
        this->alive = alive;
        return *this;
    }

    /**
     * Compare cells
     * @param cell Cell
     * @return Equal
     */
    bool Cell::operator==(const Cell &cell) const {
        return this->alive == cell.alive;
    }
}
