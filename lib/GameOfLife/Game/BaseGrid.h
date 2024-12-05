#ifndef BASEGRID_H
#define BASEGRID_H
#include <string>
#include <unordered_set>
#include <vector>

#include "HashFunction.h"
#include "File/IWritable.h"


namespace GameOfLife::Game {
    /**
     * Abstract class for a grid
     */
    template<typename TGrid, typename T>
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

        virtual void move(std::vector<std::vector<T>> &grid,
            std::unordered_set<std::pair<int, int>, HashFunction> &livingCells,
            std::unordered_set<std::pair<int, int>, HashFunction> &changedCells,
            int fromRow, int fromCol, int numRows, int numCols, int toRow, int toCol);

        virtual void resize(std::vector<std::vector<T>> &grid,
            std::vector<std::vector<T>> &next,
            std::unordered_set<std::pair<int, int>, HashFunction> &livingCells,
            int addNorth, int addEast, int addSouth, int addWest, int rows, int cols, int maxRows, int maxCols);

        void insert(std::vector<std::vector<T>>& grid,
            std::unordered_set<std::pair<int, int>, HashFunction>& livingCells,
            std::unordered_set<std::pair<int, int>, HashFunction>& changedCells,
            const std::vector<std::vector<T>> &cells,
            int row, int col, int rows, int cols, int maxRows, int maxCols, bool hollow = false);

        [[nodiscard]] virtual int getRows() const = 0;
        [[nodiscard]] virtual int getCols() const = 0;
        [[nodiscard]] virtual TGrid getCells() const = 0;

        [[nodiscard]] std::string getText() const override = 0;
    };
}


#endif //BASEGRID_H
