#include "BaseCell.h"

#include <iostream>

namespace GameOfLife::Game {
    /**
     * Default constructor
     */
    BaseCell::BaseCell() : alive(false) {}

    /**
     * Base constructor
     * @param alive Alive
     */
    BaseCell::BaseCell(const bool alive) : alive(alive) {}

    /**
     * Check if the cell is alive
     * @return Alive
     */
    bool BaseCell::isAlive() const {
        return alive;
    }

    /**
     * Set cell alive
     * @param alive Alive
     */
    void BaseCell::setAlive(const bool alive) {
        this->alive = alive;
    }

    /**
     * Toggle cell
     */
    void BaseCell::toggle() {
        alive = !alive;
    }

    /**
     * Print the cell
     */
    void BaseCell::print() const {
        std::cout << (alive ? "X" : ".");
    }

    /**
     * Check if the cell will be alive in the next generation
     * @param livingNeighbors Living neighbors
     * @return Will be alive
     */
    bool BaseCell::willBeAlive(const int livingNeighbors) const {
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
    BaseCell& BaseCell::operator=(bool alive) {
        this->alive = alive;
        return *this;
    }

    /**
     * Compare cells
     * @param cell Cell
     * @return Equal
     */
    bool BaseCell::operator==(const BaseCell &cell) const {
        return this->alive == cell.alive;
    }
}
