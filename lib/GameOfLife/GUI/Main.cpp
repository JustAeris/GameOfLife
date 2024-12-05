#include "Main.h"
#include "CLI/Main.h"

#include <iostream>
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <SFML/System.hpp>

#include "File/Parser.h"
#include "Game/Grid.h"


namespace GameOfLife::GUI {
    /**
     * Start the GUI loop
     *
     * @param argc Arguments count
     * @param argv Arguments
     */
    void Main::start(int argc, char *argv[]) {
        // Parse the arguments
        auto args = CLI::Arguments::parse(argc, argv);

        // If the arguments are invalid or the help flag is set, show the help message
        if (!args.isValid())
            return;

        // Initialize the grid
        File::Parser parser('O', '.', '\0');
        int rows = 0;
        int cols = 0;
        auto cells = parser.parse(args.getInputFile(), rows, cols);

        // Get the screen resolution
        sf::VideoMode desktop = sf::VideoMode::getDesktopMode();

        // Create the window
        sf::RenderWindow window(sf::VideoMode(desktop.width * 0.9, desktop.height * 0.8), "Game Of Life", sf::Style::Close);

        Game::Grid grid(cells, rows, cols, window.getSize().y - 1, window.getSize().x - 1);

        while (window.isOpen()) {
            sf::Event event{};
            while (window.pollEvent(event)) {
                if (event.type == sf::Event::Closed)
                    window.close();
            }

            window.clear(sf::Color::Black);

            // Step the grid
            grid.step();

            // Calculate the cell size based on the window size and grid dimensions
            int cellSize = std::min(window.getSize().x / grid.getCols(), window.getSize().y / grid.getRows());
            cellSize = std::max(cellSize, 1);
            // Calculate the offset to center the grid
            float offsetX = (window.getSize().x - (grid.getCols() * cellSize)) / 2.0f;
            float offsetY = (window.getSize().y - (grid.getRows() * cellSize)) / 2.0f;

            // Get the cells that changed state
            auto changedCells = grid.getChangedCells();

            // Redraw only the cells that changed state
            for (const auto &cell : changedCells) {
                int i = cell.first;
                int j = cell.second;
                sf::RectangleShape rectangle(sf::Vector2f(cellSize, cellSize));
                rectangle.setPosition(j * cellSize + offsetX, i * cellSize + offsetY);
                rectangle.setFillColor(grid.isAlive(i, j) ? sf::Color::White : sf::Color::Black);
                window.draw(rectangle);
            }

            // Draw a border
            sf::RectangleShape border(sf::Vector2f(grid.getCols() * cellSize, grid.getRows() * cellSize));
            border.setPosition(offsetX, offsetY);
            border.setFillColor(sf::Color::Transparent);
            border.setOutlineThickness(1);
            border.setOutlineColor(sf::Color::White);
            window.draw(border);

            window.display();

            // Sleep
            sf::sleep(sf::milliseconds(100));
        }
    }
}
