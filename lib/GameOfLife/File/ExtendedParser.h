#ifndef EXTENDEDPARSER_H
#define EXTENDEDPARSER_H
#include <utility>
#include <vector>

#include "BaseParser.h"
#include "FormatConfig.h"
#include "Game/Cell.h"

namespace GameOfLife::File {

    class ExtendedParser final : public BaseParser<std::vector<std::vector<Game::Cell>>> {
    private:
        FormatConfig config;
        char livingObstacle = 'o';
        char deadObstacle = 'x';
    public:
        ExtendedParser() = delete;
        explicit ExtendedParser(FormatConfig config) : config(std::move(config)) {};
        ~ExtendedParser() override = default;

        void setFormatConfig(const FormatConfig &config) { this->config = config; }
        void setLivingObstacle(const char livingObstacle) { this->livingObstacle = livingObstacle; }
        void setDeadObstacle(const char deadObstacle) { this->deadObstacle = deadObstacle; }

        [[nodiscard]] std::vector<std::vector<Game::Cell>> parse(const std::string &filename, int &rows, int &cols) override;
    };

}

#endif //EXTENDEDPARSER_H
