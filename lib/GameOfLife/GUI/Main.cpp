#include "Main.h"

#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <SFML/System.hpp>


namespace GameOfLife::GUI {
    /**
     * Start the GUI loop
     *
     * @param argc Arguments count
     * @param argv Arguments
     */
    void Main::start(int argc, char *argv[]) {
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
