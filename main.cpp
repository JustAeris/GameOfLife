#include <iostream>
#include <chrono>
#include <vector>
#include <algorithm>
#include <numeric>

#include "CLI/Main.h"
#include "File/Parser.h"
#include "File/Writer.h"
#include "Game/ExtendedGrid.h"
#include "Game/Grid.h"
#include "Game/HighPerformanceGrid.h"
#include "GUI/Main.h"


void runTest(GameOfLife::Game::ExtendedGrid &grid, int iterations) {
    grid.randomize();
    for (int i = 0; i < iterations; i++) {
        grid.step();
    }
}

void runHighPerformanceTest(GameOfLife::Game::HighPerformanceGrid &grid, int iterations) {
    grid.randomize();
    for (int i = 0; i < iterations; i++) {
        grid.step(true);
    }
}

void chronoTest(bool highPerformance = false) {
    int iterations = 1000;
    std::vector<double> durations;
    std::vector<int> gridSizes = {10, 100, 1000};

    for (int gridSize : gridSizes) {
        for (int i = 0; i < iterations; i++) {
            auto start = std::chrono::high_resolution_clock::now();

            if (highPerformance) {
                //GameOfLife::Game::HighPerformanceGrid grid(gridSize, gridSize);
                //runHighPerformanceTest(grid, 1000);
            } else {
                GameOfLife::Game::ExtendedGrid grid(gridSize, gridSize);
                runTest(grid, 1000);
            }

            auto end = std::chrono::high_resolution_clock::now();
            std::chrono::duration<double> duration = end - start;
            durations.push_back(duration.count());
        }

        double sum = std::accumulate(durations.begin(), durations.end(), 0.0);
        double mean = sum / durations.size();
        double min = *std::min_element(durations.begin(), durations.end());
        double max = *std::max_element(durations.begin(), durations.end());

        std::nth_element(durations.begin(), durations.begin() + durations.size() / 2, durations.end());
        double median = durations[durations.size() / 2];

        std::cout << std::fixed << std::setprecision(2);
        std::cout << "Iterations: " << iterations << " * 1000 generations" << std::endl;
        std::cout << "Total time: " << sum << " seconds" << std::endl;
        std::cout << "Grid size: " << gridSize << "x" << gridSize << std::endl;
        std::cout << "Average time: " << mean * 1000 << " milliseconds" << std::endl;
        std::cout << "Median time: " << median * 1000 << " milliseconds" << std::endl;
        std::cout << "Min time: " << min * 1000 << " milliseconds" << std::endl;
        std::cout << "Max time: " << max * 1000 << " milliseconds" << std::endl;
        std::cout << "Generations per second: " << 1 / mean << std::endl;
        std::cout << std::endl;

        durations.clear();
    }
}

void regularTest() {
    GameOfLife::Game::ExtendedGrid grid(10, 10);

    // Blinker
    grid.setAlive(4, 3, true);
    grid.setAlive(4, 4, true);
    grid.setAlive(4, 5, true);

    for (int i = 0; i < 4; i++) {
        grid.step();
        grid.print();
    }

    // Glider
    grid.clear();
    grid.setAlive(1, 3, true);
    grid.setAlive(2, 4, true);
    grid.setAlive(3, 2, true);
    grid.setAlive(3, 3, true);
    grid.setAlive(3, 4, true);

    for (int i = 0; i < 50; i++) {
        grid.step(true);
        grid.print();
    }
}

void benchmarkGridSize(int rows, int cols) {
    //GameOfLife::Game::HighPerformanceGrid grid(rows, cols);
    //grid.randomize();

    auto start = std::chrono::high_resolution_clock::now();

    //grid.step(true);

    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> duration = end - start;
    std::cout << "Grid size: " << rows << "x" << cols << ", Time: " << duration.count() << " seconds "
              <</* (useMultithreading ? "Multithreaded" : "Sequential") <<*/ std::endl;
}

void sizeTest() {
    std::vector<std::pair<int, int>> gridSizes = {
        {10, 10}, {50, 50}, {100, 100}, {200, 200}, {500, 500}, {1000, 1000},
        {2000, 2000}, {5000, 5000}, {10000, 10000},
        {10, 1000}, {1000, 10}, {500, 1000}, {1000, 500},
        {1000, 10000}, {10000, 1000}, {5000, 10000}, {10000, 5000}
    };

    for (const auto& size : gridSizes) {
        int rows = size.first;
        int cols = size.second;
        benchmarkGridSize(rows, cols);
    }
}

void parserTest() {
    GameOfLife::File::Parser parser('1', '0', ' ');
    int rows = 0;
    int cols = 0;
    auto cells = parser.parse(R"(C:\Users\matth\Downloads\test.txt)", rows, cols);

    GameOfLife::Game::ExtendedGrid grid(cells, rows, cols);

    grid.print();

    GameOfLife::File::Parser parser2('O', '.', '\0');
    rows = 0;
    cols = 0;
    cells = parser2.parse(R"(C:\Users\matth\Downloads\test.cells)", rows, cols);

    grid = GameOfLife::Game::ExtendedGrid(cells, rows, cols);

    grid.print();

    cells = GameOfLife::File::Parser::parseRLE(R"(C:\Users\matth\Downloads\test.rle)", rows, cols);
    grid = GameOfLife::Game::ExtendedGrid(cells, rows, cols);

    grid.print();
}

void writerTest() {
    GameOfLife::Game::ExtendedGrid grid(10, 10);
    grid.randomize();
    grid.print();

    GameOfLife::File::Writer writer('O', '.', '\0');
    writer.write(grid, "test.cells");

    grid.randomize();
    grid.print();

    writer = GameOfLife::File::Writer('1', '0', ' ');
    writer.write(grid, "test.txt");

    grid.randomize();
    grid.print();

    GameOfLife::File::Writer::writeRLE(grid, "test.rle");
}

int main(int argc, char *argv[]) {
    auto args = GameOfLife::CLI::Arguments::parse(argc, argv);

    if (args.isGUI()) {
        GameOfLife::GUI::Main gui(args);
        gui.start();
    } else
        GameOfLife::CLI::Main::start(args);

    return 0;
}