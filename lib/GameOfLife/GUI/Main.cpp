#include "Main.h"
#include "CLI/Main.h"

#include <iostream>
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <SFML/System.hpp>

#include "File/Parser.h"
#include "File/Utils.h"
#include "Game/Grid.h"


namespace GameOfLife::GUI {
    /**
     * Start the GUI loop
     */
    void Main::start() {
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

        sf::RenderTexture renderTexture;
        renderTexture.create(window.getSize().x, window.getSize().y);

        while (window.isOpen()) {
            sf::Event event{};
            while (window.pollEvent(event)) {
                if (event.type == sf::Event::Closed)
                    window.close();

                if (event.type == sf::Event::KeyPressed) {
                    if (event.key.code == sf::Keyboard::Space)
                        run = !run;

                    if (event.key.code == sf::Keyboard::R)
                        grid.randomize();

                    if (event.key.code == sf::Keyboard::C)
                        grid.clear();

                    if (event.key.code == sf::Keyboard::S && !run) {
                        grid.step();
                        drawGrid(window, grid);
                        window.display();
                    }

                    if (event.key.code == sf::Keyboard::Up)
                        delay = std::max(delay - 10, 0);

                    if (event.key.code == sf::Keyboard::Down)
                        delay += 10;

                    if (event.key.code == sf::Keyboard::Escape)
                        window.close();

                    if (event.key.code == sf::Keyboard::H)
                        showHelp = !showHelp;
                }
            }

            window.clear(sf::Color::Black);

            if (showHelp) {
                sf::Font font;
                if (!font.loadFromFile(R"(C:\Users\matth\CLionProjects\GameOfLife-GroupA\cmake-build-debug\Ubuntu-Regular.ttf)")) {
                    std::cerr << "Error loading font" << std::endl;
                }
                sf::Text text;
                text.setFont(font);
                text.setString("Space - Start/Stop\nR - Randomize\nC - Clear\nS - Step\nUp/Down - Speed\nEscape - Exit");
                text.setCharacterSize(24);
                text.setFillColor(sf::Color::White);
                text.setPosition(10, 10);

                window.draw(text);
            }

            if (run) {
                // Step and draw the grid
                grid.step();
                drawGrid(window, grid);
                window.display();
            }

            // Sleep
            sleep(sf::milliseconds(delay));
        }
    }

    void Main::drawGrid(sf::RenderWindow &window, Game::Grid &grid, bool drawBorder) {
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

        if (!drawBorder)
            return;

        // Draw a border
        sf::RectangleShape border(sf::Vector2f(grid.getCols() * cellSize, grid.getRows() * cellSize));
        border.setPosition(offsetX, offsetY);
        border.setFillColor(sf::Color::Transparent);
        border.setOutlineThickness(1);
        border.setOutlineColor(sf::Color::White);
        window.draw(border);
    }
}
