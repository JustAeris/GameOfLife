#include "UnitTests.h"

#include <cassert>
#include <iostream>

#include "CLI/Arguments.h"
#include "CLI/Main.h"
#include "File/ExtendedParser.h"
#include "File/Parser.h"
#include "File/Utils.h"
#include "File/Writer.h"
#include "Game/Cell.h"
#include "Game/ExtendedGrid.h"
#include "Game/Grid.h"
#include "GUI/Main.h"

namespace GameOfLife::Tests {
    void UnitTests::customAssert(bool condition, const std::string &message, const char *file, int line) {
        if (!condition) {
            std::cerr << "Assertion failed: (" << message << ") in " << file << " line " << line << std::endl;
            std::abort();
        }
    }

    void UnitTests::run() {
        testCell();
        testGrid();
        testExtendedGrid();

        testParser();
        testExtendedParser();
        testWriter();

        testArguments();
        testMainCLI();

        testMainGUI();
    }

    void UnitTests::testCell() {
        // Test the Cell class
        // Test the constructor
        Game::Cell cell(true, false);
        ASSERT(cell.isAlive(), "Cell should be alive");
        ASSERT(!cell.isObstacle(), "Cell should not be an obstacle");

        cell = Game::Cell(false, true);
        ASSERT(!cell.isAlive(), "Cell should not be alive");
        ASSERT(cell.isObstacle(), "Cell should be an obstacle");

        cell = Game::Cell(true, true);
        ASSERT(cell.isAlive(), "Cell should be alive");
        ASSERT(cell.isObstacle(), "Cell should be an obstacle");

        // Test the setAlive method
        cell.setAlive(false);
        ASSERT(!cell.isAlive(), "Cell should not be alive");

        cell.setAlive(true);
        ASSERT(cell.isAlive(), "Cell should be alive");

        // Test the setObstacle method
        cell.setObstacle(false);
        ASSERT(!cell.isObstacle(), "Cell should not be an obstacle");

        cell.setObstacle(true);
        ASSERT(cell.isObstacle(), "Cell should be an obstacle");

        // Test the willBeAlive method
        ASSERT(!cell.willBeAlive(0), "Cell should become dead");
        ASSERT(cell.willBeAlive(3), "Cell should stay alive");

        // Test the operator=
        Game::Cell cell2 = cell;
        ASSERT(cell2.isAlive() == cell.isAlive(), "Cell should be alive");

        // Test the operator==
        ASSERT(cell == cell2, "Cells should be equal");

        // Test the bool operator
        ASSERT(static_cast<bool>(cell), "Cell should be alive");

        std::cout << "Cell tests passed" << std::endl;
    }

    void UnitTests::testGrid() {
        // Test the Grid class
        // Test the constructor
        const int rows = 10;
        const int cols = 10;
        Game::Grid grid(rows, cols);
        ASSERT(grid.getRows() == rows, "Rows should be 10");
        ASSERT(grid.getCols() == cols, "Cols should be 10");

        // Test the setAlive & isAlive method
        grid.setAlive(0, 0, true);
        ASSERT(grid.isAlive(0, 0), "Cell should be alive");

        // Test the countNeighbors method
        grid.setAlive(0, 1, true);
        grid.setAlive(1, 0, true);
        grid.setAlive(1, 1, true);
        ASSERT(grid.countNeighbors(0, 0, false) == 3, "Cell should have 3 neighbors");

        // Test the clear method
        grid.clear();
        ASSERT(!grid.isAlive(0, 0), "Cell should not be alive");

        // Test the randomize method
        grid.randomize(1);
        ASSERT(grid.isAlive(0, 0), "Cell should be alive");
        grid.clear();

        // Test the move method
        grid.setAlive(0, 0, true);
        grid.move(0, 0, 1, 1, 1, 1);
        ASSERT(grid.isAlive(1, 1), "Cell should be alive");

        // Test the resize method
        grid.resize(1, 1, 1, 1);
        ASSERT(grid.getRows() == rows + 2, "Rows should be 12");
        ASSERT(grid.getCols() == cols + 2, "Cols should be 12");
        grid.clear();

        // Test the insert method
        std::vector<std::vector<bool>> cells = {{true}};
        grid.insert(cells, 0, 0);
        ASSERT(grid.isAlive(0, 0), "Cell should be alive");
        grid.clear();

        grid = Game::Grid(cells, 1, 1);
        // Test all getters
        ASSERT(grid.getCells() == cells, "Cells should be equal");
        ASSERT(grid.getRows() == 1, "Rows should be 1");
        ASSERT(grid.getCols() == 1, "Cols should be 1");
        ASSERT(grid.getMaxRows() == DEFAULT_MAX_ROWS, "Max rows should be 2048");
        ASSERT(grid.getMaxCols() == DEFAULT_MAX_COLS, "Max cols should be 2048");
        ASSERT(grid.getLivingCells().size() == 1, "Living cells should be 1");
        ASSERT(grid.getChangedCells().size() == 1, "Changed cells should be 1");

        // Test the FormatConfig
        ASSERT(grid.getFormatConfig().getAliveChar() == 'O', "Alive should be 'O'");
        ASSERT(grid.getFormatConfig().getDeadChar() == '.', "Dead should be '.'");
        ASSERT(grid.getFormatConfig().getDelimiterChar() == '\0', "Delimiter should be '\0'");
        grid.setFormatConfig(File::FormatConfig('X', 'Y', 'Z'));
        ASSERT(grid.getFormatConfig().getAliveChar() == 'X', "Alive should be 'X'");
        ASSERT(grid.getFormatConfig().getDeadChar() == 'Y', "Dead should be 'Y'");
        ASSERT(grid.getFormatConfig().getDelimiterChar() == 'Z', "Delimiter should be 'Z'");

        // Test IWritable method
        ASSERT(!grid.getText().empty(), "Text should not be empty");

        // Test the step method
        grid = Game::Grid(rows, cols);
        grid.setAlive(0, 1, true);
        grid.setAlive(1, 1, true);
        grid.setAlive(2, 1, true);
        grid.step(false, false);
        ASSERT(grid.isAlive(1, 0), "Cell should be alive");
        ASSERT(grid.isAlive(1, 1), "Cell should be alive");
        ASSERT(grid.isAlive(1, 2), "Cell should be alive");

        // Test dynamic step
        grid.step(false, true);
        ASSERT(grid.getCols() == cols + 1, "Cols should be 12");
        ASSERT(grid.isAlive(1, 2), "Cell should be alive");
        ASSERT(grid.isAlive(2, 2), "Cell should be alive");

        // Test wrap step
        grid.clear();
        grid.setAlive(0, 0, true);
        grid.setAlive(0, 1, true);
        grid.setAlive(0, 2, true);
        grid.step(true, false);
        ASSERT(grid.isAlive(0, 1), "Cell should be alive");

        std::cout << "Grid tests passed" << std::endl;
    }

    void UnitTests::testExtendedGrid() {
        // Test the ExtendedGrid class
        // Test the constructor
        const int rows = 10;
        const int cols = 10;
        Game::ExtendedGrid grid(rows, cols);
        ASSERT(grid.getRows() == rows, "Rows should be 10");
        ASSERT(grid.getCols() == cols, "Cols should be 10");

        // Test the setAlive & isAlive method
        grid.setAlive(0, 0, true);
        ASSERT(grid.isAlive(0, 0), "Cell should be alive");

        // Test the countNeighbors method
        grid.setAlive(0, 1, true);
        grid.setAlive(1, 0, true);
        grid.setAlive(1, 1, true);
        ASSERT(grid.countNeighbors(0, 0, false) == 3, "Cell should have 3 neighbors");

        // Test the clear method
        grid.clear();
        ASSERT(!grid.isAlive(0, 0), "Cell should not be alive");

        // Test the randomize method
        grid.randomize(1);
        ASSERT(grid.isAlive(0, 0), "Cell should be alive");
        grid.clear();

        // Test the move method
        grid.setAlive(0, 0, true);
        grid.move(0, 0, 1, 1, 1, 1);
        ASSERT(grid.isAlive(1, 1), "Cell should be alive");

        // Test the resize method
        grid.resize(1, 1, 1, 1);
        ASSERT(grid.getRows() == rows + 2, "Rows should be 12");
        ASSERT(grid.getCols() == cols + 2, "Cols should be 12");
        grid.clear();

        // Test the insert method
        std::vector<std::vector<Game::Cell>> cells = {{Game::Cell(true, false)}};
        grid.insert(cells, 0, 0);
        ASSERT(grid.isAlive(0, 0), "Cell should be alive");
        grid.clear();

        grid = Game::ExtendedGrid(cells, 1, 1);
        // Test all getters
        ASSERT(grid.getCells() == cells, "Cells should be equal");
        ASSERT(grid.getRows() == 1, "Rows should be 1");
        ASSERT(grid.getCols() == 1, "Cols should be 1");
        ASSERT(grid.getMaxRows() == 1000, "Max rows should be 1000");
        ASSERT(grid.getMaxCols() == 1000, "Max cols should be 1000");
        ASSERT(grid.getLivingCells().size() == 1, "Living cells should be 1");
        ASSERT(grid.getChangedCells().size() == 1, "Changed cells should be 1");

        // Test the FormatConfig
        ASSERT(grid.getFormatConfig().getAliveChar() == '1', "Alive should be '1'");
        ASSERT(grid.getFormatConfig().getDeadChar() == '0', "Dead should be '0'");
        ASSERT(grid.getFormatConfig().getDelimiterChar() == ' ', "Delimiter should be ' '");
        grid.setFormatConfig(File::FormatConfig('X', 'Y', 'Z'));
        ASSERT(grid.getFormatConfig().getAliveChar() == 'X', "Alive should be 'X'");
        ASSERT(grid.getFormatConfig().getDeadChar() == 'Y', "Dead should be 'Y'");
        ASSERT(grid.getFormatConfig().getDelimiterChar() == 'Z', "Delimiter should be 'Z'");

        // Test IWritable method
        ASSERT(!grid.getText().empty(), "Text should not be empty");

        // Test the step method
        grid = Game::ExtendedGrid(rows, cols);
        grid.setAlive(0, 1, true);
        grid.setAlive(1, 1, true);
        grid.setAlive(2, 1, true);
        grid.step(false, false);
        ASSERT(grid.isAlive(1, 0), "Cell should be alive");
        ASSERT(grid.isAlive(1, 1), "Cell should be alive");
        ASSERT(grid.isAlive(1, 2), "Cell should be alive");

        // Test dynamic step
        grid.step(false, true);
        ASSERT(grid.getCols() == cols + 1, "Cols should be 12");
        ASSERT(grid.isAlive(1, 2), "Cell should be alive");
        ASSERT(grid.isAlive(2, 2), "Cell should be alive");

        // Test wrap step
        grid.clear();
        grid.setAlive(0, 0, true);
        grid.setAlive(0, 1, true);
        grid.setAlive(0, 2, true);
        grid.step(true, false);
        ASSERT(grid.isAlive(0, 1), "Cell should be alive");

        std::cout << "ExtendedGrid tests passed" << std::endl;
    }

    void UnitTests::testParser() {
        // Test the Parser class
        File::Parser parser2(File::FormatConfig('O', '.', '\0'));
        int rows = 0;
        int cols = 0;
        auto cells = parser2.parse(File::Utils::makeAbsolutePath("test.cells").string(), rows, cols);

        Game::Grid grid(cells, rows, cols);
        ASSERT(grid.isAlive(5, 0), "Cell should be alive");

        grid.print();

        std::cout << "Parser tests passed" << std::endl;
    }

    void UnitTests::testExtendedParser() {
        // Test the ExtendedParser class
        File::ExtendedParser parser(File::FormatConfig('1', '0', ' '));
        int rows = 0;
        int cols = 0;
        auto cells = parser.parse(File::Utils::makeAbsolutePath("test.txt").string(), rows, cols);

        Game::ExtendedGrid grid2(cells, rows, cols);
        ASSERT(grid2.isAlive(1, 1), "Cell should be alive");

        grid2.print();

        std::cout << "ExtendedParser tests passed" << std::endl;
    }

    void UnitTests::testWriter() {
        // Test the Writer class
        File::Writer writer;
        Game::Grid grid(10, 10);
        grid.setAlive(0, 0, true);
        grid.setAlive(0, 1, true);
        grid.setAlive(0, 2, true);
        writer.write(grid, File::Utils::makeAbsolutePath("test_out.cells").string());

        Game::ExtendedGrid grid2(10, 10);
        grid2.setAlive(0, 0, true);
        grid2.setAlive(0, 1, true);
        grid2.setAlive(0, 2, true);
        writer.write(grid2, File::Utils::makeAbsolutePath("test_out.txt").string());

        std::cout << "Writer tests passed" << std::endl;
    }

    void UnitTests::testArguments() {
        // Test the Arguments class
        std::vector<std::string> args = {"GameOfLife", "-v", "-x", "200", "test.txt", "test"};
        std::vector<char*> cstr_args;
        for (auto& arg : args) {
            cstr_args.push_back(&arg[0]);
        }
        auto arguments = CLI::Arguments::parse(cstr_args.size(), cstr_args.data());
        ASSERT(arguments.getInputFile() == "test.txt", "Input file should be test.txt");
        ASSERT(arguments.getOutputFolder() == "test", "Output file should be test.cells");
        ASSERT(arguments.isVerbose(), "Verbose should be true");
        ASSERT(arguments.getDelay() == 200, "Delay should be 200");

        std::cout << "Arguments tests passed" << std::endl;
    }

    void UnitTests::testMainCLI() {
        // Test the MainCLI class
        std::vector<std::string> args = {"GameOfLife", "-v", "-g", "2", "test.txt", "test"};
        std::vector<char*> cstr_args;
        for (auto& arg : args) {
            cstr_args.push_back(&arg[0]);
        }
        auto arguments = CLI::Arguments::parse(cstr_args.size(), cstr_args.data());
        CLI::Main::start(arguments);

        std::cout << "MainCLI tests passed" << std::endl;
    }

    void UnitTests::testMainGUI() {
        // Test the MainGUI class
        std::vector<std::string> args = {"GameOfLife", "-g", "2", "test.txt", "test"};
        std::vector<char*> cstr_args;
        for (auto& arg : args) {
            cstr_args.push_back(&arg[0]);
        }
        auto arguments = CLI::Arguments::parse(cstr_args.size(), cstr_args.data());
        std::cout << "MainGUI tests started, close window to continue" << std::endl;
        GUI::Main gui(arguments);

        std::cout << "MainGUI tests passed" << std::endl;
    }
}
