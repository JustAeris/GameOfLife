#include "Utils.h"

#include <fstream>
#include <stdexcept>

namespace GameOfLife::File {
    /**
     * Get the number of lines in a file
     *
     * @param filename File to read
     * @param comment Character that indicates a comment line
     * @return Number of lines in the file
     */
    int Utils::getNoOfLines(const std::string &filename, char comment) {
        // Argument validation
        if (filename.empty()) {
            throw std::invalid_argument("Filename cannot be empty");
        }

        // Existence check
        std::ifstream file(filename);
        if (!file.is_open()) {
            throw std::runtime_error("Could not open file: " + filename);
        }

        // Count lines, excluding comments
        int lines = 0;
        std::string line;
        while (std::getline(file, line)) {
            if (line[0] == comment) {
                continue;
            }
            lines++;
        }

        file.close();
        return lines;
    }

    /**
     * Get the maximum line length in a file
     *
     * @param filename File to read
     * @param comment Character that indicates a comment line
     * @param delimiter Character to remove from the line
     * @return Maximum line length
     */
    int Utils::getMaxLineLength(const std::string &filename, char comment, char delimiter) {
        // Argument validation
        if (filename.empty()) {
            throw std::invalid_argument("Filename cannot be empty");
        }

        // Existence check
        std::ifstream file(filename);
        if (!file.is_open()) {
            throw std::runtime_error("Could not open file: " + filename);
        }

        // Find the maximum line length, excluding comments and delimiters
        int maxLength = 0;
        std::string line;
        while (std::getline(file, line)) {
            if (line.empty() || line[0] == comment) {
                continue;
            }
            line = removeChar(line, delimiter);
            maxLength = std::max(maxLength, static_cast<int>(line.size()));
        }

        file.close();
        return maxLength;
    }

    /**
     * Remove a character from a string
     *
     * @param str Original string
     * @param character Character to remove
     * @return String without the character
     */
    std::string Utils::removeChar(const std::string &str, char character) {
        // Argument validation
        if (str.empty() || character == '\0') {
            return str;
        }

        // Remove the character using a loop
        std::string result;
        for (char c : str) {
            if (c != character) {
                result += c;
            }
        }

        return result;
    }

    /**
     * Transform a relative path to an absolute path
     *
     * @param path Original path
     * @return Absolute path
     */
    std::filesystem::path Utils::makeAbsolutePath(const std::filesystem::path &path) {
        // If the path is already absolute, return it
        if (path.is_absolute()) {
            return path;
        }

        // Otherwise, make it absolute using the executable path
        std::filesystem::path exePath = std::filesystem::canonical("GameOfLife.exe");
        std::filesystem::path exeDir = exePath.parent_path();
        return exeDir / path;
    }

    /**
     * Get the extension of a file
     *
     * @param filename File to check
     * @return Extension of the file
     */
    std::string Utils::getExtension(const std::string &filename) {
        // Return an empty string if the filename is empty
        if (filename.empty()) {
            return "";
        }

        // Get the extension using the filesystem library
        std::filesystem::path path = filename;
        return path.extension().string();
    }
}
