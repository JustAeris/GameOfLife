#ifndef UTILS_H
#define UTILS_H
#include <string>
#include <filesystem>

namespace GameOfLife::File {
    /**
     * Utility functions
     */
    class Utils {
    public:
        static int getNoOfLines(const std::string &filename, char comment = '!');
        static int getMaxLineLength(const std::string &filename, char comment = '!', char delimiter = ' ');
        static std::string removeChar(const std::string &str, char character);
        static std::filesystem::path makeAbsolutePath(const std::filesystem::path &path);
        static std::string getExtension(const std::string &filename);
    };

}

#endif //UTILS_H
