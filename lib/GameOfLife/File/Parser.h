#ifndef PARSER_H
#define PARSER_H
#include <string>
#include <vector>

#include "Game/Cell.h"
#include "FormatConfig.h"

namespace GameOfLife::File {
    /**
     * Parser for reading files
     */
    class Parser {
    private:
        FormatConfig config;
        Parser() = default;
    public:
        Parser(char alive, char dead, char delimiter = '\0', char comment = '!') : config(alive, dead, delimiter, comment) {};
        explicit Parser(const FormatConfig &config) : config(config) {};

        [[nodiscard]] char getDelimiter() const;
        [[nodiscard]] char getAlive() const;
        [[nodiscard]] char getDead() const;

        [[nodiscard]] std::vector<std::vector<Game::Cell>> parse(const std::string &filename, int &rows, int &cols) const;

        static std::vector<std::vector<Game::Cell>> parseRLE(const std::string &filename, int& rows, int& cols);
    };

}

#endif //PARSER_H
