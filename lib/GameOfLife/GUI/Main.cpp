#include "Main.h"
#include "CLI/Main.h"

#include <iostream>
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <SFML/System.hpp>

#include "File/ExtendedParser.h"
#include "File/Parser.h"
#include "File/Utils.h"
#include "Game/Grid.h"


namespace GameOfLife::GUI {
    /**
     * Draws the help message.
     *
     * @param window The window
     * @param font The font
     */
    void Main::drawHelp(sf::RenderWindow &window, sf::Font &font) const {
        sf::Text text;
        text.setFont(font);
        auto ss = std::stringstream();
        ss << "Space - Start/Stop\n"
            << "R - Randomize\n"
            << "C - Clear\n"
            << "S - Step\n"
            << "Up/Down - Speed\n"
            << "Escape - Exit\n"
            << "V - Verbose " << (verbose ? "(On)" : "(Off)")
            << "\nW - Wrap " << (warp ? "(On)" : "(Off)")
            << "\nD - Dynamic " << (dynamic ? "(On)" : "(Off)")
            << "\n\nB - Insert Blinker\n"
            << "G - Insert Glider\n"
            << "P - Insert Pulsar\n"
            << "M - Insert MWSS\n"
            << "O - Insert Gosper Glider Gun\n";
        text.setString(ss.str());
        text.setCharacterSize(24);
        text.setFillColor(sf::Color::White);
        text.setPosition(10, 10);

        window.draw(text);
    }

    /**
     * Draws the verbose information.
     *
     * @param window The window
     * @param font The font
     * @param grid The grid
     * @param drawTime The time it took to draw the grid
     * @param stepTime The time it took to step the grid
     * @param generation The current generation
     * @param delay The delay between generations
     */
    template<typename TGrid>
    void Main::drawVerbose(sf::RenderWindow &window, const sf::Font &font, const TGrid &grid, const long long drawTime,
        const long long stepTime, const int generation, const int delay) {
        // Print text the frames per second and generation time
        sf::Text text;
        text.setFont(font);
        text.setString("FPS: " + std::to_string(drawTime == 0 ? -1 : 1000000 / drawTime) +
            " - Generation time: " + std::to_string(stepTime) + "us" +
            "\nGrid size: " + std::to_string(grid.getRows()) + "x" + std::to_string(grid.getCols()) +
            "\nLiving cells: " + std::to_string(grid.getLivingCells().size()) +
            "\nDead cells: " + std::to_string(grid.getRows() * grid.getCols() - grid.getLivingCells().size()) +
            "\nAlive ratio: " + std::to_string(grid.getLivingCells().size() * 100.0 / (grid.getRows() * grid.getCols())) + "%" +
            "\nDelay: " + std::to_string(delay) + "ms" + " - Generation: " + std::to_string(generation));
        text.setCharacterSize(24);
        text.setFillColor(sf::Color::White);
        text.setPosition(10, window.getSize().y - 180);

        window.draw(text);
    }

    /**
     * Start the GUI loop
     */
    void Main::start() {
        // If the arguments are invalid or the help flag is set, show the help message
        if (!args.isValid())
            return;

        // Get the screen resolution
        sf::VideoMode desktop = sf::VideoMode::getDesktopMode();

        // Create the window
        sf::RenderWindow window(sf::VideoMode(desktop.width * 0.9, desktop.height * 0.8), "Game Of Life - Press H for help", sf::Style::Close);

        sf::Font font;
        if (!font.loadFromFile(File::Utils::makeAbsolutePath("Ubuntu-Regular.ttf").string())) {
            std::cerr << "Error loading font" << std::endl;
        }

        if (args.isHighPerformance()) {
            File::OutputFormat outputFormat;
            const auto formatConfig = File::FormatConfig('O', '.', '\0');

            // Parse the input file and define the output format
            int rows = 0;
            int cols = 0;
            std::vector<std::vector<bool>> cells;
            if (args.getInputFile().ends_with(".rle")) {
                cells = File::Parser::parseRLE(args.getInputFile(), rows, cols);
                outputFormat = File::OutputFormat::RLE;
            }
            else if (args.getInputFile().ends_with(".cells")) {
                cells = File::Parser(formatConfig).parse(args.getInputFile(), rows, cols);
                outputFormat = File::OutputFormat::PLAINTEXT;
            }
            else {
                std::cerr << "Fast mode only supports .rle and .cells files" << std::endl;
                return;
            }

            // Create the grid
            Game::Grid grid(cells, rows, cols, window.getSize().y - 2, window.getSize().x - 2, args.isDynamic());
            grid.setFormatConfig(formatConfig);

            render(window, grid, font);
            return;
        }

        // Parse the input file and define the output format
        int rows = 0;
        int cols = 0;
        const File::FormatConfig formatConfig(args.getAliveChar(), args.getDeadChar(), args.getSeparator());
        const std::vector<std::vector<Game::Cell>> cells = File::ExtendedParser(formatConfig).parse(args.getInputFile(), rows, cols);

        Game::ExtendedGrid grid(cells, rows, cols, window.getSize().y - 2, window.getSize().x - 2, args.isDynamic());
        grid.setFormatConfig(formatConfig);

        render(window, grid, font);
    }

    /**
     * Renders the grid and handles input.
     *
     * @param window The window
     * @param grid The grid
     * @param font The font
     */
    template<typename TGrid>
    void Main::render(sf::RenderWindow &window, TGrid &grid, sf::Font &font) {
        int generation = 0;
        auto previous = std::chrono::system_clock::now();

        while (window.isOpen()) {
            sf::Event event{};
            while (window.pollEvent(event)) {
                if (event.type == sf::Event::Closed)
                    window.close();

                if (event.type == sf::Event::KeyPressed) {
                    if (event.key.code == sf::Keyboard::Space)
                        run = !run;

                    if (event.key.code == sf::Keyboard::R)
                        grid.randomize(0.2);

                    if (event.key.code == sf::Keyboard::C)
                        grid.clear();

                    if (event.key.code == sf::Keyboard::S && !run) {
                        grid.step();
                        generation++;
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

                    if (event.key.code == sf::Keyboard::V)
                        verbose = !verbose;

                    if (event.key.code == sf::Keyboard::W) {
                        warp = !warp;
                        if (warp && dynamic)
                            dynamic = false;
                    }

                    if (event.key.code == sf::Keyboard::D) {
                        dynamic = !dynamic;
                        if (dynamic && warp)
                            warp = false;
                    }

                    if (event.key.code == sf::Keyboard::B) {
                        // Parse blinker.rle and insert it into the grid where the mouse is
                        auto mousePos = sf::Mouse::getPosition(window);
                        auto [gridRow, gridCols] = mousePosToGridPos(window, grid, mousePos.x, mousePos.y);
                        insertPattern(grid, File::Utils::makeAbsolutePath("Patterns/blinker.rle").string(), gridRow, gridCols);
                    }

                    if (event.key.code == sf::Keyboard::G) {
                        // Parse glider.rle and insert it into the grid where the mouse is
                        auto mousePos = sf::Mouse::getPosition(window);
                        auto [gridRow, gridCols] = mousePosToGridPos(window, grid, mousePos.x, mousePos.y);
                        insertPattern(grid, File::Utils::makeAbsolutePath("Patterns/glider.rle").string(), gridRow, gridCols);
                    }

                    if (event.key.code == sf::Keyboard::P) {
                        // Parse pulsar.rle and insert it into the grid where the mouse is
                        auto mousePos = sf::Mouse::getPosition(window);
                        auto [gridRow, gridCols] = mousePosToGridPos(window, grid, mousePos.x, mousePos.y);
                        insertPattern(grid, File::Utils::makeAbsolutePath("Patterns/pulsar.rle").string(), gridRow, gridCols);
                    }

                    if (event.key.code == sf::Keyboard::M) {
                        // Parse mwss.rle and insert it into the grid where the mouse is
                        auto mousePos = sf::Mouse::getPosition(window);
                        auto [gridRow, gridCols] = mousePosToGridPos(window, grid, mousePos.x, mousePos.y);
                        insertPattern(grid, File::Utils::makeAbsolutePath("Patterns/mwss.rle").string(), gridRow, gridCols);
                    }

                    if (event.key.code == sf::Keyboard::O) {
                        // Parse gosperglidergun.rle and insert it into the grid where the mouse is
                        auto mousePos = sf::Mouse::getPosition(window);
                        auto [gridRow, gridCols] = mousePosToGridPos(window, grid, mousePos.x, mousePos.y);
                        insertPattern(grid, File::Utils::makeAbsolutePath("Patterns/gosperglidergun.rle").string(), gridRow, gridCols);
                    }
                }
            }

            window.clear(sf::Color::Black);

            if (showHelp) {
                drawHelp(window, font);
            }

            auto now = std::chrono::system_clock::now();
            auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - previous).count();
            if (run && elapsed >= delay) {
                // Step and draw the grid
                grid.step(warp, dynamic);
                auto stepTime = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::system_clock::now() - now).count();
                generation++;
                drawGrid(window, grid);
                auto drawTime = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::system_clock::now() - now).count();

                if (verbose) {
                    drawVerbose(window, font, grid, drawTime, stepTime, generation, delay);
                }

                window.display();
                previous = std::chrono::system_clock::now();
            }
        }
    }

    /**
     * Converts the mouse position to a grid position.
     *
     * @param window The window
     * @param grid The grid
     * @param mouseX The mouse x position
     * @param mouseY The mouse y position
     * @return The grid position
     */
    template<typename TGrid>
    std::pair<int, int> Main::mousePosToGridPos(sf::RenderWindow &window, const TGrid &grid, int mouseX, int mouseY) {
        int cellSize;
        float offsetX, offsetY;
        getDimensions(window, grid, cellSize, offsetX, offsetY);

        int gridX = static_cast<int>((mouseX - offsetX) / cellSize);
        int gridY = static_cast<int>((mouseY - offsetY) / cellSize);

        return {gridY, gridX};
    }

    /**
     * Draws the grid to the window.
     *
     * @param window The window
     * @param grid The grid
     */
    void Main::drawGrid(sf::RenderWindow &window, Game::Grid &grid) {
        int cellSize;
        float offsetX, offsetY;
        getDimensions(window, grid, cellSize, offsetX, offsetY);

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

    }

    /**
     * Draws the grid to the window.
     *
     * @param window The window
     * @param grid The grid
     */
    void Main::drawGrid(sf::RenderWindow &window, const Game::ExtendedGrid &grid) {
        int cellSize;
        float offsetX, offsetY;
        getDimensions<Game::ExtendedGrid>(window, grid, cellSize, offsetX, offsetY);

        for (int i = 0; i < grid.getRows(); i++) {
            for (int j = 0; j < grid.getCols(); j++) {
                if (grid.isObstacle(i, j)) {
                    // Draw the obstacle
                    sf::RectangleShape rectangle(sf::Vector2f(cellSize, cellSize));
                    rectangle.setPosition(j * cellSize + offsetX, i * cellSize + offsetY);
                    rectangle.setFillColor(grid.isAlive(i, j) ? sf::Color::Blue : sf::Color::Red);
                    window.draw(rectangle);
                    continue;
                }
                sf::RectangleShape rectangle(sf::Vector2f(cellSize, cellSize));
                rectangle.setPosition(j * cellSize + offsetX, i * cellSize + offsetY);
                rectangle.setFillColor(grid.isAlive(i, j) ? sf::Color::White : sf::Color::Black);
                window.draw(rectangle);
            }
        }
    }

    /**
     * Gets the cell size and offset for the grid.
     *
     * @param window The window
     * @param grid The grid
     * @param cellSize The cell size
     * @param offsetX The x offset
     * @param offsetY The y offset
     */
    template<typename TGrid>
    void Main::getDimensions(sf::RenderWindow &window, const TGrid &grid, int &cellSize, float &offsetX, float &offsetY) const {
        // Calculate the cell size based on the window size and grid dimensions
        cellSize = std::min(window.getSize().x / grid.getCols(), window.getSize().y / grid.getRows());
        cellSize = std::max(cellSize, 1);
        // Calculate the offset to center the grid
        offsetX = (window.getSize().x - (grid.getCols() * cellSize)) / 2.0f;
        offsetY = (window.getSize().y - (grid.getRows() * cellSize)) / 2.0f;
    }

    /**
     * Inserts a pattern into the grid.
     *
     * @param grid The grid
     * @param pattern The pattern
     * @param row The row to insert the pattern
     * @param col The column to insert the pattern
     */
    void Main::insertPattern(Game::Grid &grid, const std::string &pattern, int row, int col) {
        if (row > grid.getRows() && col > grid.getCols() && row < 0 && col < 0)
            return;

        int rows = 0;
        int cols = 0;
        const std::vector<std::vector<bool>> cells = File::Parser::parseRLE(pattern, rows, cols);

        if (row < 0 || col < 0 || row + rows > grid.getRows() || col + cols > grid.getCols())
            return;

        grid.insert(cells, row, col, false);
    }

    /**
     * Inserts a pattern into the grid.
     *
     * @param grid The grid
     * @param pattern The pattern
     * @param row The row to insert the pattern
     * @param col The column to insert the pattern
     */
    void Main::insertPattern(Game::ExtendedGrid &grid, const std::string &pattern, int row, int col) {
        if (row > grid.getRows() && col > grid.getCols() && row < 0 && col < 0)
            return;

        int rows = 0;
        int cols = 0;
        const std::vector<std::vector<bool>> cells = File::Parser::parseRLE(pattern, rows, cols);

        if (row < 0 || col < 0 || row + rows > grid.getRows() || col + cols > grid.getCols())
            return;

        auto cellObjects = std::vector(rows, std::vector(cols, Game::Cell(false, false)));
        for (int i = 0; i < rows; i++) {
            for (int j = 0; j < cols; j++) {
                cellObjects[i][j] = Game::Cell(cells[i][j], false);
            }
        }
        grid.insert(cellObjects, row, col, false);
    }
}
