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
        File::Parser parser(args.getAliveChar(), args.getDeadChar(), args.getSeparator());
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
            while (window.pollEvent(event))
            {
                if (event.type == sf::Event::Closed)
                    window.close();

            }

            window.clear(sf::Color::Black);

            window.display();
        }
    }
}
