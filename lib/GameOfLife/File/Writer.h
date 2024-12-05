#ifndef WRITER_H
#define WRITER_H
#include <string>
#include <vector>

#include "FormatConfig.h"
#include "Game/BaseGrid.h"

namespace GameOfLife::File {
    /**
     * Writes the grid to a file
     */
    class Writer {
    private:
        FormatConfig config;
    public:
        explicit Writer(FormatConfig config) : config(config) {}
        Writer(char alive, char dead, char delimiter) : config(alive, dead, delimiter) {}

        void write(const Game::BaseGrid &grid, const std::string &filename) const;
        void write(const std::vector<std::vector<bool>>& matrix, const std::string &filename) const;
        static void write(const std::vector<std::vector<bool>>& matrix, const std::string &filename, FormatConfig format);

        static void writeRLE(const Game::BaseGrid &grid, const std::string &filename);
        static void writeRLE(const std::vector<std::vector<bool>>& matrix, const std::string &filename);

        void writeBulk(const std::vector<std::vector<std::vector<Game::Cell>>> &bulk, const std::string &outputFolder, int startIndex = 0) const;
        static void writeBulk(const std::vector<std::vector<std::vector<Game::Cell>>> &bulk, const std::string &outputFolder, const FormatConfig& format, int startIndex = 0);
        static void writeBulk(const std::vector<std::vector<std::vector<Game::Cell>>> &bulk, const std::string &outputFolder, OutputFormat format, int startIndex = 0);

    };

}

#endif //WRITER_H
