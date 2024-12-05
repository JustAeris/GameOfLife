#include "Cell.h"

#include <iostream>

namespace GameOfLife::Game {
    /**
     * Default constructor
     */
    Cell::Cell() : alive(false), obstacle(false) {}

    /**
     * Base constructor
     * @param alive Alive
     */
    Cell::Cell(const bool alive) : alive(alive), obstacle(false) {}

    /**
     * Constructor
     * @param alive Alive
     * @param obstacle Obstacle
     */
    Cell::Cell(bool alive, bool obstacle) : alive(alive), obstacle(obstacle) {}

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
     * Print the cell
     */
    void Cell::print() const {
        std::cout << (alive ? obstacle ? 'o' : 'x' : obstacle ? 'O' : '.') << " ";
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

    bool Cell::isObstacle() const {
        return obstacle;
    }

    void Cell::setObstacle(bool obstacle) {
        this->obstacle = obstacle;
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
        return this->alive == cell.alive && this->obstacle == cell.obstacle;
    }
}
