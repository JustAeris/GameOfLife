#pragma once

#include "Arguments.h"
#include "vector"
#include "File/FormatConfig.h"

namespace GameOfLife::CLI {
    /**
     * CLI main class
     */
    class Main {
    private:
        static void workWrapper(Arguments &args);
        static void fastWorkWrapper(Arguments &args);
        template <typename TGrid, typename T>
        static void simulate(TGrid &grid, Arguments &args, bool canBeRLE, std::vector<std::vector<std::vector<T>>> &bulk, File::OutputFormat outputFormat);
        static void clearScreen();
    public:
        static int start(Arguments arguments, bool defaultsToInteractive = false);
    };
}
