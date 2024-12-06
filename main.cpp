#include "CLI/Main.h"
#include "GUI/Main.h"
#include "Tests/UnitTests.h"


int main(int argc, char *argv[]) {
    auto args = GameOfLife::CLI::Arguments::parse(argc, argv);

    if (args.isGUI()) {
        GameOfLife::GUI::Main gui(args);
        gui.start();
    } else if (args.doUnitTests())
        GameOfLife::Tests::UnitTests::run();
    else
        GameOfLife::CLI::Main::start(args);

    return 0;
}