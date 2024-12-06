#include "Main.h"

#include <fstream>
#include <iomanip>
#include <iostream>
#include <thread>

#include "Arguments.h"
#include "File/ExtendedParser.h"
#include "File/Parser.h"
#include "File/Writer.h"
#include "Game/ExtendedGrid.h"
#include "Game/Grid.h"

namespace GameOfLife::CLI {
    /**
     * Start the CLI application.
     *
     * @param arguments Parsed arguments.
     * @param defaultsToInteractive If true, the application will force interactive mode.
     * @return The exit code.
     */
    int Main::start(Arguments arguments, const bool defaultsToInteractive) {
        // Check if the arguments are correctly parsed
        if (!arguments.isValid()) {
            return 1;
        }

        // Set decimal precision
        std::cout << std::fixed << std::setprecision(2);

        // Interactive mode (manual input)
        if (arguments.isInteractive() || defaultsToInteractive) {
            auto interactiveArguments = Arguments::interactiveParse();

            interactiveArguments.isHighPerformance() ? fastWorkWrapper(interactiveArguments) : workWrapper(interactiveArguments);
            return 0;
        }

        arguments.isHighPerformance() ? fastWorkWrapper(arguments) : workWrapper(arguments);
        return 0;
    }

    /**
     * Simulation processing
     * @param args Simulation arguments
     */
    void Main::workWrapper(Arguments &args) {
        // Default output format
        const auto outputFormat = File::OutputFormat::CUSTOM;

        // Parse the input file and define the output format
        int rows = 0;
        int cols = 0;
        const File::FormatConfig formatConfig(args.getAliveChar(), args.getDeadChar(), args.getSeparator());
        const std::vector<std::vector<Game::Cell>> cells = File::ExtendedParser(formatConfig).parse(args.getInputFile(), rows, cols);

        // Create the grid
        Game::ExtendedGrid grid(cells, rows, cols);
        grid.setFormatConfig(formatConfig);

        // Create the array for static grid detection
        std::vector<std::vector<std::vector<Game::Cell>>> bulk;
        bulk.reserve(2);

        simulate<Game::ExtendedGrid, Game::Cell>(grid, args, false, bulk, outputFormat);
    }

    void Main::fastWorkWrapper(Arguments &args) {
        // Default output format
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
        Game::Grid grid(cells, rows, cols);
        grid.setFormatConfig(formatConfig);

        // Create the array for static grid detection
        std::vector<std::vector<std::vector<bool>>> bulk;
        bulk.reserve(2);

        simulate<Game::Grid, bool>(grid, args, true, bulk, outputFormat);
    }

    template<typename TGrid, typename T>
    void Main::simulate(TGrid &grid, Arguments &args, const bool canBeRLE, std::vector<std::vector<std::vector<T>>> &bulk,
        const File::OutputFormat outputFormat) {
        // Get rows and cols
        const int rows = grid.getRows();
        const int cols = grid.getCols();

        const auto now = std::chrono::system_clock::now();

        // Simulation loop
        int i = 0;
        for (i = 0; i < args.getGenerations(); i++) {
            // Step the grid and push the cells to the bulk list
            grid.step(args.doWarp(), true);
            bulk.push_back(grid.getCells());

            // Print the grid
            clearScreen();
            std::cout << "Generation: " << i + 1 << " (Ctrl+C to exit)" << std::endl;
            // If verbose mode is enabled, print the time elapsed and the number of living cells
            if (args.isVerbose()) {
                std::cout << "Time elapsed: " << std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now() - now).count() << "s" << std::endl;
                // Count the number of living cells
                const int alive = grid.getLivingCells().size();
                std::cout << "Living cells: " << alive << std::endl;
                std::cout << "Dead cells: " << (rows * cols - alive) << std::endl;
                std::cout << "Alive ratio: " << (alive * 100.0 / (rows * cols)) << "%" << std::endl;
            }
            std::cout << std::endl;
            grid.print();

            // Write current grid
            if (canBeRLE && outputFormat == File::OutputFormat::RLE) {
                auto input = grid.getCells();
                std::vector<std::vector<bool>> boolArray(input.size(), std::vector<bool>(input[0].size()));
                for (size_t k = 0; k < input.size(); ++k) {
                    for (size_t j = 0; j < input[i].size(); ++j) {
                        boolArray[k][j] = static_cast<bool>(input[k][j]);
                    }
                }
                File::Writer::writeRLE(boolArray, args.getOutputFolder() + "/gen" + std::to_string(i) + ".rle");
            }
            else
                File::Writer::write(grid, args.getOutputFolder() + "/gen" + std::to_string(i) + ".txt");

            // Check if the grid is static
            if (args.doEndIfStatic() && i > 0 && bulk.size() > 2) {
                if (bulk[bulk.size() - 1] == bulk[bulk.size() - 2]) {
                    std::cout << "Grid is static, ending simulation" << std::endl;
                    break;
                }
            }

            // Wait at least the given delay
            std::this_thread::sleep_for(std::chrono::milliseconds(args.getDelay()));
        }

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
