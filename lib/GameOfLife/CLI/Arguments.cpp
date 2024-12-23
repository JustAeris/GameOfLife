#include "Arguments.h"

#include <iostream>
#include <fstream>
#include <cstring>

#include "File/Utils.h"

namespace GameOfLife::CLI {
    /**
     * Parse the command line arguments and return the parsed arguments.
     *
     * @param argc The number of arguments
     * @param argv The arguments
     * @return The parsed arguments
     */
    Arguments Arguments::parse(int argc, char *argv[]) {
        // Check for interactive mode
        if (argc == 1 || (argc == 2 && (strcmp(argv[1], "-i") == 0 || strcmp(argv[1], "--interactive") == 0))) {
            return {"", "", 1000, 100, false, true, true, false, false, false, false, false, '1', '0', ' '};
        }

        // Check for help
        if (argc == 2 && (strcmp(argv[1], "-h") == 0 || strcmp(argv[1], "--help") == 0)) {
            printHelp();
            return {};
        }

        // Check for unit tests
        if (argc == 2 && (strcmp(argv[1], "-t") == 0 || strcmp(argv[1], "--unit-tests") == 0)) {
            return {"", "", 1000, 100, false, true, false, false, false, false, false, true, '1', '0', ' '};
        }

        // Check for the minimum number of arguments
        if (argc < 3) {
            printHelp();
            return {};
        }

        // Get the input file and output folder
        std::string inputFile = argv[argc - 2];
        std::string outputFolder = argv[argc - 1];

        // Check if the input file exists by opening it
        std::ifstream file(inputFile);
        if (!file.good()) {
            std::cerr << "Input file does not exist: " << inputFile << std::endl;
            return {};
        }
        file.close();
        auto ext = File::Utils::getExtension(inputFile);

        // Default values
        int generations = 1000;
        int delay = 100;
        bool highPerformance = ext == ".rle" || ext == ".cells";
        bool endIfStatic = false;
        bool interactive = false;
        bool warp = false;
        bool dynamic = false;
        bool verbose = false;
        bool GUI = false;
        char aliveChar = '1';
        char deadChar = '0';
        char separator = ' ';

        // Parse the arguments
        for (int i = 1; i < argc; i++) {
            std::string arg = argv[i];
            if (arg == "-h" || arg == "--help") {
                printHelp();
                return {};
            }

            // Numbered arguments
            if (arg == "-g" || arg == "--generations") {
                if (i + 1 < argc) {
                    try {
                        generations = std::stoi(argv[i + 1]);
                    } catch ([[maybe_unused]] std::invalid_argument &e) {
                        std::cerr << "Invalid number of generations: " << argv[i + 1] << std::endl;
                        return {};
                    }
                    i++;
                }
            }
            if (arg == "-x" || arg == "--delay") {
                if (i + 1 < argc) {
                    try {
                        delay = std::stoi(argv[i + 1]);
                    } catch ([[maybe_unused]] std::invalid_argument &e) {
                        std::cerr << "Invalid delay: " << argv[i + 1] << std::endl;
                        return {};
                    }
                    i++;
                }
            }

            // Boolean arguments
            // It is now irrelevant to ask for high performance mode
            /*if (arg == "-p" || arg == "--high-performance") {
                highPerformance = true;
            }*/
            if (arg == "-s" || arg == "--end-if-static") {
                endIfStatic = true;
            }
            if (arg == "-i" || arg == "--interactive") {
                interactive = true;
            }
            if (arg == "-w" || arg == "--warp") {
                warp = true;
            }
            if (arg == "-y" || arg == "--dynamic") {
                dynamic = true;
            }
            if (arg == "-v" || arg == "--verbose") {
                verbose = true;
            }
            if (arg == "-u" || arg == "--user-interface") {
                GUI = true;
            }

            // Character arguments
            if (arg == "-a" || arg == "--alive-char") {
                if (i + 1 < argc) {
                    if (strlen(argv[i + 1]) != 1) {
                        std::cerr << "Invalid alive char: " << argv[i + 1] << std::endl;
                        return {};
                    }
                    aliveChar = argv[i + 1][0];
                    i++;
                }
            }
            if (arg == "-d" || arg == "--dead-char") {
                if (i + 1 < argc) {
                    if (strlen(argv[i + 1]) != 1) {
                        std::cerr << "Invalid dead char: " << argv[i + 1] << std::endl;
                        return {};
                    }
                    deadChar = argv[i + 1][0];
                    i++;
                }
            }
            if (arg == "-e" || arg == "--separator") {
                if (i + 1 < argc) {
                    if (strlen(argv[i + 1]) != 1) {
                        std::cerr << "Invalid separator: " << argv[i + 1] << std::endl;
                        return {};
                    }
                    separator = argv[i + 1][0];
                    i++;
                }
            }
        }

        // Return the parsed arguments
        return {inputFile, outputFolder, generations, delay, highPerformance, endIfStatic, interactive,
            warp, dynamic, verbose, GUI, false, aliveChar, deadChar, separator};
    }

    /**
     * Parse the command line arguments interactively and return the parsed arguments.
     *
     * @return The parsed arguments
     */
    Arguments Arguments::interactiveParse() {
        std::cout << "Interactive mode (Ctrl+C to exit)\n" << std::endl;

        std::string filePath;
        std::cout << "Enter the path to the file: ";
        std::cin >> filePath;
        File::Utils::makeAbsolutePath(filePath);
        // Check if the input file exists
        std::ifstream file(filePath);
        if (!file.good()) {
            std::cerr << "Input file does not exist: " << filePath << std::endl;
            return {};
        }
        file.close();
        auto ext = File::Utils::getExtension(filePath);
        bool highPerformance = ext == ".rle" || ext == ".cells";

        std::string outputFolder;
        std::cout << "Enter the path to the output folder: ";
        std::cin >> outputFolder;

        int maxGenerations;
        std::cout << "Enter the number of generations: ";
        std::cin >> maxGenerations;

        int delay;
        std::cout << "Enter the delay between generations (ms): ";
        std::cin >> delay;

        // It is now irrelevant to ask for high performance mode
        // std::cout << "Use high performance mode? (y/n): ";
        // std::string highPerformanceStr;
        // std::cin >> highPerformanceStr;
        // bool highPerformance = highPerformanceStr == "y";

        std::cout << "End if static? (y/n): ";
        std::string endIfStaticStr;
        std::cin >> endIfStaticStr;
        bool endIfStatic = endIfStaticStr == "y";

        std::cout << "Wrap mode (toroidal grid)? (y/n): ";
        std::string warpStr;
        std::cin >> warpStr;
        bool warp = warpStr == "y";

        std::cout << "Dynamic grid size (will disable wrap)? (y/n): ";
        std::string dynamicStr;
        std::cin >> dynamicStr;
        bool dynamic = dynamicStr == "y";
        warp = !dynamic && warp;

        std::cout << "Verbose mode? (y/n): ";
        std::string verboseStr;
        std::cin >> verboseStr;
        bool verbose = verboseStr == "y";

        std::cout << "Enable GUI? (y/n): ";
        std::string guiStr;
        std::cin >> guiStr;
        bool GUI = guiStr == "y";

        // Clear the input buffer
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

        std::cout << "Alive char (for parsing, leave empty if using .cells or .rle): ";
        std::string aliveChar;
        std::getline(std::cin, aliveChar);

        std::cout << "Dead char (for parsing, leave empty if using .cells or .rle): ";
        std::string deadChar;
        std::getline(std::cin, deadChar);

        std::cout << "Separator (for parsing, leave empty if using .cells or .rle): ";
        std::string separator;
        std::getline(std::cin, separator);

        Arguments interactiveArguments(filePath, outputFolder, maxGenerations, delay, highPerformance,
            endIfStatic, true, warp, dynamic, verbose, GUI, false, aliveChar[0], deadChar[0], separator[0]);

        return interactiveArguments;
    }

    /**
     * Print the help message, displaying the usage and options.
     */
    void Arguments::printHelp() {
        std::cout << "Usage: GameOfLife [options] <input file> <output folder>\n";
        std::cout << "Options:\n";
        std::cout << "  -h, --help\t\t\tPrint this help message\n";
        std::cout << "  -g, --generations <n>\t\tNumber of generations to simulate (default: 1000)\n";
        std::cout << "  -x, --delay <ms>\t\tDelay between generations in milliseconds (default: 100)\n";
        // std::cout << "  -p, --high-performance\tUse high performance mode\n";
        std::cout << "  -s, --end-if-static\t\tEnd simulation if the grid is static or does not evolve\n";
        std::cout << "  -w, --wrap\t\t\tWarp around the grid (toroidal grid)\n";
        std::cout << "  -y, --dynamic\t\t\tDynamic grid size (takes priority on wrap)\n";
        std::cout << "  -v, --verbose\t\t\tDisplay extended informations\n";
        std::cout << "  -a, --alive-char <c>\t\tCharacter to represent alive cells (default: 1, unused if file is .cells or .rle)\n";
        std::cout << "  -d, --dead-char <c>\t\tCharacter to represent dead cells (default: 0, unused if file is .cells or .rle)\n";
        std::cout << "  -e, --separator <c>\t\tCharacter to separate cells (default: space, unused if file is .cells or .rle)\n";
        std::cout << "  -i, --interactive\t\tInteractive mode (overwrites other options)\n";
        std::cout << "  -u, --user-interface\t\t\tStart the GUI (some options will carry over)\n";
        std::cout << "  -t, --unit-tests\t\tRun the unit tests\n";
    }
}
