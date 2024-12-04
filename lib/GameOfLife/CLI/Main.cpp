#include "Main.h"

#include <fstream>
#include <iomanip>
#include <iostream>
#include <thread>

#include "Arguments.h"
#include "File/Parser.h"
#include "File/Writer.h"
#include "Game/ExtendedGrid.h"
#include "Game/Grid.h"

namespace GameOfLife::CLI {
    /**
     * Start the CLI application.
     *
     * @param argc The number of arguments.
     * @param argv The arguments.
     * @param defaultsToInteractive If true, the application will force interactive mode.
     * @return The exit code.
     */
    int Main::start(int argc, char *argv[], bool defaultsToInteractive) {
        Arguments arguments = Arguments::parse(argc, argv);

        // Check if the arguments are correctly parsed
        if (!arguments.isValid()) {
            return 1;
        }

        // Interactive mode (manual input)
        if (arguments.isInteractive() || defaultsToInteractive) {

            std::cout << "Interactive mode (Ctrl+C to exit)\n" << std::endl;

            std::string filePath;
            std::cout << "Enter the path to the file: ";
            std::cin >> filePath;
            // Check if the input file exists
            std::ifstream file(filePath);
            if (!file.good()) {
                std::cerr << "Input file does not exist: " << filePath << std::endl;
                return {};
            }
            file.close();

            std::string outputFolder;
            std::cout << "Enter the path to the output folder: ";
            std::cin >> outputFolder;

            int maxGenerations;
            std::cout << "Enter the number of generations: ";
            std::cin >> maxGenerations;

            int delay;
            std::cout << "Enter the delay between generations (ms): ";
            std::cin >> delay;

            std::cout << "Use high performance mode? (y/n): ";
            std::string highPerformanceStr;
            std::cin >> highPerformanceStr;
            bool highPerformance = highPerformanceStr == "y";

            std::cout << "End if static? (y/n): ";
            std::string endIfStaticStr;
            std::cin >> endIfStaticStr;
            bool endIfStatic = endIfStaticStr == "y";

            std::cout << "Warp mode (toroidal grid)? (y/n): ";
            std::string warpStr;
            std::cin >> warpStr;
            bool warp = warpStr == "y";

            std::cout << "Verbose mode? (y/n): ";
            std::string verboseStr;
            std::cin >> verboseStr;
            bool verbose = verboseStr == "y";

            std::cout << "Alive char (for parsing, leave empty is using .cells or .rle): ";
            char aliveChar;
            std::cin >> aliveChar;

            std::cout << "Dead char (for parsing, leave empty is using .cells or .rle): ";
            char deadChar;
            std::cin >> deadChar;

            std::cout << "Separator (for parsing, leave empty is using .cells or .rle): ";
            char separator;
            std::cin >> separator;

            Arguments interactiveArguments(filePath, outputFolder, maxGenerations, delay, highPerformance,
                endIfStatic, true, warp, verbose, deadChar, separator);
            work(interactiveArguments);
            return 0;
        }

        work(arguments);
        return 0;
    }

    /**
     * Simulation processing
     * @param args Simulation arguments
     */
    void Main::work(Arguments &args) {
        // Default output format
        auto outputFormat = File::OutputFormat::CUSTOM;

        // Parse the input file and define the output format
        int rows = 0;
        int cols = 0;
        std::vector<std::vector<Game::BaseCell>> cells;
        if (args.getInputFile().ends_with(".rle")) {
            cells = File::Parser::parseRLE(args.getInputFile(), rows, cols);
            outputFormat = File::OutputFormat::RLE;
        }
        else if (args.getInputFile().ends_with(".cells")) {
            cells = File::Parser('O', '.', '\0').parse(args.getInputFile(), rows, cols);
            outputFormat = File::OutputFormat::PLAINTEXT;
        }
        else {
            cells = File::Parser(args.getAliveChar(), args.getDeadChar(), args.getSeparator())
                    .parse(args.getInputFile(), rows, cols);
        }

        // Create the grid
        Game::Grid grid(cells, rows, cols);

        // Create the array for bulk writing
        std::vector<std::vector<std::vector<Game::BaseCell>>> bulk;
        bulk.reserve(BULK_SIZE);

        // Set decimal precision
        std::cout << std::fixed << std::setprecision(2);
        auto now = std::chrono::system_clock::now();

        // Simulation loop
        int bulkCounter = 0;
        int i = 0;
        for (i = 0; i < args.getGenerations(); i++) {
            // Step the grid and push the cells to the bulk list
            grid.step(args.doWarp());
            bulk.push_back(grid.getCells());
            bulkCounter++;

            // Print the grid
            clearScreen();
            std::cout << "Generation: " << i + 1 << " (Ctrl+C to exit)" << std::endl;
            // If verbose mode is enabled, print the time elapsed and the number of living cells
            if (args.isVerbose()) {
                std::cout << "Time elapsed: " << std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now() - now).count() << "s" << std::endl;
                // Count the number of living cells
                int alive = 0;
                for (const auto& row : grid.getCells()) {
                    for (const auto& cell : row) {
                        if (cell.isAlive()) {
                            alive++;
                        }
                    }
                }
                std::cout << "Living cells: " << alive << std::endl;
                std::cout << "Dead cells: " << (rows * cols - alive) << std::endl;
                std::cout << "Alive ratio: " << (alive * 100.0 / (rows * cols)) << "%" << std::endl;
            }
            std::cout << std::endl;
            grid.print();

            // Check if the grid is static
            if (args.doEndIfStatic() && i > 0 && bulk.size() > 2) {
                if (bulk[bulk.size() - 1] == bulk[bulk.size() - 2]) {
                    std::cout << "Grid is static, ending simulation" << std::endl;
                    // Write the last bulk
                    File::Writer::writeBulk(bulk, args.getOutputFolder(), outputFormat, i - bulk.size() + 1);
                    break;
                }
            }

            // Write the bulk
            if (bulkCounter >= BULK_SIZE) {
                File::Writer::writeBulk(bulk, args.getOutputFolder(), outputFormat, i - bulk.size() + 1);
                bulk.clear();
                bulkCounter = 0;
            }

            // Wait
            std::this_thread::sleep_for(std::chrono::milliseconds(args.getDelay()));
        }

        // Write the last bulk
        File::Writer::writeBulk(bulk, args.getOutputFolder(), outputFormat, i - bulk.size() + 1);

        // Print the simulation time
        std::cout << "Simulation finished after " << i << " generations in " <<
            std::chrono::duration_cast<std::chrono::duration<double>>(std::chrono::system_clock::now() - now).count() << "s" << std::endl;
    }

    /**
     * Clear the screen
     */
    void Main::clearScreen() {
        // \033[2J: Clears the entire screen.
        // \033[1;1H: Moves the cursor to the top-left corner (row 1, column 1).
        std::cout << "\033[2J\033[1;1H";
    }
}
