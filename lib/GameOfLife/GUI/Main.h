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
        bool verbose = false;
        bool warp = false;
        bool dynamic = true;
        CLI::Arguments args;

        void drawHelp(sf::RenderWindow &window, sf::Font &font) const;
        void drawVerbose(sf::RenderWindow &window, const sf::Font &font, const Game::Grid &grid,
            long long drawTime, long long stepTime, int generation, int delay);
    public:
        Main() = delete;
        explicit Main(CLI::Arguments args) : delay(args.getDelay()), verbose(args.isVerbose()), warp(args.doWarp()),
            dynamic(true), args(std::move(args)) {}
        void start();
        void drawGrid(sf::RenderWindow &window, Game::Grid &grid, bool drawBorder = false);
    };

}
