#pragma once
#include "Game/Grid.h"
#include <SFML/Graphics.hpp>

namespace GameOfLife::GUI {

    class Main {
    public:
        void start(int argc, char *argv[]);
        void drawGrid(sf::RenderWindow &window, Game::Grid &grid, bool drawBorder = false);
    };

}
