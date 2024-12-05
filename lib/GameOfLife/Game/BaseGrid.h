#ifndef BASEGRID_H
#define BASEGRID_H
#include <string>
#include "File/IWritable.h"


namespace GameOfLife::Game {
    /**
     * Base interface for a grid
     */
    template<typename TGrid>
    class BaseGrid<TGrid> : public File::IWritable {
    public:
        BaseGrid() = default;
        virtual ~BaseGrid() = default;

        virtual void setAlive(int row, int col, bool alive) = 0;
        [[nodiscard]] virtual bool isAlive(int row, int col) const = 0;
        [[nodiscard]] virtual int countNeighbors(int row, int col, bool wrap) const = 0;

        virtual void step() = 0;
        virtual void step(bool wrap) = 0;
        virtual void randomize(float aliveProbability) = 0;
        virtual void clear() = 0;

        virtual void print() const = 0;
        virtual void print(int fromRow, int fromCol, int toRow, int toCol) const = 0;

        [[nodiscard]] virtual int getRows() const = 0;
        [[nodiscard]] virtual int getCols() const = 0;
        [[nodiscard]] virtual TGrid getCells() const = 0;

        [[nodiscard]] std::string getText() const override = 0;
    };
}


#endif //BASEGRID_H
