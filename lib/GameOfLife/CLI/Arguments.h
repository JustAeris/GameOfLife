#ifndef ARGUMENTS_H
#define ARGUMENTS_H
#include <string>
#include <utility>

namespace GameOfLife::CLI {
    /**
     * Represents the command line arguments.
     */
    class Arguments {
    private:
        std::string inputFile;
        std::string outputFolder;
        int generations;
        int delay;

        bool highPerformance;
        bool endIfStatic;
        bool interactive;
        bool warp;
        bool dynamic;
        bool verbose;
        bool GUI;
        bool unitTests;

        char aliveChar;
        char deadChar;
        char separator;

        bool valid;

        Arguments() : valid(false) {};

    public:
        Arguments(std::string  inputFile, std::string  outputFolder, const int generations = 1000, const int delay = 100,
                const bool highPerformance = false, const bool endIfStatic = true, const bool interactive = false, bool warp = false, bool dynamic = false, bool verbose = false, bool GUI = false, bool unitTests = false,
                const char aliveChar = '1', const char deadChar = '0', const char separator = ' ') :
                inputFile(std::move(inputFile)), outputFolder(std::move(outputFolder)), generations(generations), delay(delay),
                highPerformance(highPerformance), endIfStatic(endIfStatic), interactive(interactive), warp(warp), dynamic(dynamic), verbose(verbose), GUI(GUI), unitTests(unitTests),
                aliveChar(aliveChar), deadChar(deadChar), separator(separator), valid(true) {}

        static Arguments parse(int argc, char *argv[]);
        static Arguments interactiveParse();
        static void printHelp();

        std::string getInputFile() { return inputFile; }
        std::string getOutputFolder() { return outputFolder; }
        [[nodiscard]] int getGenerations() const { return generations; }
        [[nodiscard]] int getDelay() const { return delay; }

        [[nodiscard]] bool isHighPerformance() const { return highPerformance; }
        [[nodiscard]] bool doEndIfStatic() const { return endIfStatic; }
        [[nodiscard]] bool isInteractive() const { return interactive; }
        [[nodiscard]] bool doWarp() const { return warp; }
        [[nodiscard]] bool isDynamic() const { return dynamic; }
        [[nodiscard]] bool isVerbose() const { return verbose; }
        [[nodiscard]] bool isGUI() const { return GUI; }
        [[nodiscard]] bool doUnitTests() const { return unitTests; }

        [[nodiscard]] char getAliveChar() const { return aliveChar; }
        [[nodiscard]] char getDeadChar() const { return deadChar; }
        [[nodiscard]] char getSeparator() const { return separator; }

        [[nodiscard]] bool isValid() const { return valid; }
    };

}

#endif //ARGUMENTS_H
