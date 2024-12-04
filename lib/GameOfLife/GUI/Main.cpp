#include "Main.h"
#include "CLI/Main.h"

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

        Game::Grid grid(cells, rows, cols);

        // Get the screen resolution
        sf::VideoMode desktop = sf::VideoMode::getDesktopMode();

        // Create the window
        sf::RenderWindow window(sf::VideoMode(desktop.width * 0.9, desktop.height * 0.8), "Game Of Life", sf::Style::Close);



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
            // Calculate the offset to center the grid
            float offsetX = (window.getSize().x - (grid.getCols() * cellSize)) / 2.0f;
            float offsetY = (window.getSize().y - (grid.getRows() * cellSize)) / 2.0f;

            auto baseCells = grid.toBooleanGrid();

            // Draw the cells
            for (int i = 0; i < grid.getRows(); i++) {
                for (int j = 0; j < grid.getCols(); j++) {
                    sf::RectangleShape rectangle(sf::Vector2f(cellSize, cellSize));
                    rectangle.setPosition(j * cellSize + offsetX, i * cellSize + offsetY);
                    rectangle.setFillColor(baseCells[i][j] ? sf::Color::White : sf::Color::Black);
                    window.draw(rectangle);
                }
            }

            window.display();

            // Sleep
            sf::sleep(sf::milliseconds(1));
        }
    }
}
