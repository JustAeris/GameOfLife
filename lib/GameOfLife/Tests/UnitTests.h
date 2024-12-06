#ifndef UNITTESTS_H
#define UNITTESTS_H

#define ASSERT(condition, message) customAssert(condition, message, __FILE__, __LINE__)
#include <string>

namespace GameOfLife::Tests {

    class UnitTests {
    private:
        static void customAssert(bool condition, const std::string &message, const char *file, int line);
    public:
        static void run();

        static void testCell();
        static void testGrid();
        static void testExtendedGrid();

        static void testParser();
        static void testExtendedParser();
        static void testWriter();

        static void testArguments();
        static void testMainCLI();

        static void testMainGUI();
    };

}

#endif //UNITTESTS_H
