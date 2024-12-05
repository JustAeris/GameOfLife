#pragma once
#include "Game/Grid.h"
#include <SFML/Graphics.hpp>
#include "CLI/Arguments.h"

namespace GameOfLife::GUI {

    class Main {
    private:
        bool run = true;
        int delay = 100;
        bool showHelp = false;
        CLI::Arguments args;
        Main() = default;
    public:
        Main(CLI::Arguments args) : args(std::move(args)) { delay = this->args.getDelay(); }
        void start();
        void drawGrid(sf::RenderWindow &window, Game::Grid &grid, bool drawBorder = false);
    };

}
