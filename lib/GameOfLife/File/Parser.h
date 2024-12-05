#ifndef PARSER_H
#define PARSER_H
#include <string>
#include <utility>
#include <vector>

#include "BaseParser.h"
#include "Game/Cell.h"
#include "FormatConfig.h"

namespace GameOfLife::File {
    /**
     * Parser for reading files
     */
    class Parser final : public BaseParser<std::vector<std::vector<bool>>> {
    private:
        FormatConfig config;
    public:
        Parser() = delete;
        explicit Parser(FormatConfig config) : config(std::move(config)) {};
        ~Parser() override = default;

        void setFormatConfig(const FormatConfig &config) { this->config = config; }

        [[nodiscard]] std::vector<std::vector<bool>> parse(const std::string &filename, int &rows, int &cols) override;

        static std::vector<std::vector<bool>> parseRLE(const std::string &filename, int& rows, int& cols);
    };

}

#endif //PARSER_H
