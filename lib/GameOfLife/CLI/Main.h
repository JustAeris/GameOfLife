#pragma once

#include "Arguments.h"

#define BULK_SIZE 10

namespace GameOfLife::CLI {
    /**
     * CLI main class
     */
    class Main {
    private:
        static void work(Arguments &args);
    public:
        static int start(Arguments arguments, bool defaultsToInteractive = false);
        static void clearScreen();
    };
}
