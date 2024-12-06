#pragma once
#include "Game/Grid.h"
#include <SFML/Graphics.hpp>
#include "CLI/Arguments.h"
#include "Game/ExtendedGrid.h"

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

        template<typename TGrid>
        void getDimensions(sf::RenderWindow &window, const TGrid &grid, int &cellSize, float &offsetX, float &offsetY) const;

        void drawGrid(sf::RenderWindow &window, Game::Grid &grid);
        void drawGrid(sf::RenderWindow &window, const Game::ExtendedGrid &grid);
        void drawHelp(sf::RenderWindow &window, sf::Font &font) const;
        template<typename TGrid>
        void drawVerbose(sf::RenderWindow &window, const sf::Font &font, const TGrid &grid,
            long long drawTime, long long stepTime, int generation, int delay);

        template<typename TGrid>
        void render(sf::RenderWindow &window, TGrid &grid, sf::Font &font);
    public:
        Main() = delete;
        explicit Main(CLI::Arguments args) : delay(args.getDelay()), verbose(args.isVerbose()), warp(args.doWarp()),
            dynamic(true), args(std::move(args)) {}
        void start();
    };

}
