#include "Parser.h"

#include <fstream>
#include <sstream>
#include <stdexcept>

#include "Utils.h"

namespace GameOfLife::File {
    /**
     * Parse the input file and return the cells
     *
     * @param filename Source file path
     * @param rows Number of rows, to be set by the function
     * @param cols Number of columns, to be set by the function
     * @return 2D vector of cells
     */
    std::vector<std::vector<bool>> Parser::parse(const std::string &filename, int &rows, int &cols) {
        if (filename.empty()) {
            throw std::invalid_argument("Filename cannot be empty");
        }

        const int fileRows = Utils::getNoOfLines(filename);
        const int fileCols = Utils::getMaxLineLength(filename);

        // Open the file
        std::ifstream file(filename);
        if (!file.is_open()) {
            throw std::runtime_error("Could not open file: " + filename);
        }

        // Read the file line per line
        std::vector<std::vector<bool>> cells;
        cells.resize(fileRows, std::vector<bool>(fileCols));

        std::string line;
        int i = 0;
        while (std::getline(file, line)) {
            // Skip comments
            if (line[0] == config.getCommentChar()) {
                continue;
            }

            // Compute line
            int j = 0;
            line = Utils::removeChar(line, config.getDelimiterChar());
            for (int k = 0; k < line.size(); k++) {
                if (line[j] == config.getAliveChar()) {
                    cells[i][j] = true;
                    j++;
                } else if (line[j] == config.getDeadChar()) {
                    cells[i][j] = false;
                    j++;
                } else {
                    // throw std::invalid_argument("Invalid character in file: " + line[j]);
                }
            }
            i++;
        }

        // Close the file
        file.close();

        rows = fileRows;
        cols = fileCols;
        return cells;
    }

    /**
     * Parse the RLE file and return the cells
     *
     * @param filename RLE file path
     * @param rows Number of rows, to be set by the function
     * @param cols Number of columns, to be set by the function
     * @return 2D vector of cells
     */
    std::vector<std::vector<bool>> Parser::parseRLE(const std::string &filename, int &rows, int &cols) {
        if (filename.empty()) {
            throw std::invalid_argument("Filename cannot be empty");
        }

        // Open the file
        std::ifstream file(filename);
        if (!file.is_open()) {
            throw std::runtime_error("Could not open file: " + filename);
        }

        // Get x, y
        std::string line;
        while (std::getline(file, line)) {
            if (line.find("x =") != std::string::npos) {
                // Parse the x and y values
                size_t xPos = line.find("x =");
                size_t yPos = line.find("y =");
                if (xPos != std::string::npos && yPos != std::string::npos) {
                    cols = std::stoi(line.substr(xPos + 3)); // Extract x after "x ="
                    rows = std::stoi(line.substr(yPos + 3)); // Extract y after "y ="
                }
                break; // Stop searching once dimensions are found
            }
        }

        // Read the file line per line
        std::vector<std::vector<bool>> cells;
        cells.resize(rows, std::vector<bool>(cols));

        // Parse the RLE pattern
        int row = 0, col = 0;
        int runLength = 0;
        while (std::getline(file, line)) {
            // Skip comments and empty lines
            if (line.empty() || line[0] == '#') {
                continue;
            }

            for (char c : line) {
                if (std::isdigit(c)) {
                    // Build the run-length number
                    runLength = runLength * 10 + (c - '0');
                } else if (c == 'b') {
                    // Blank cells
                    runLength = (runLength == 0) ? 1 : runLength;
                    col += runLength;
                    runLength = 0;
                } else if (c == 'o') {
                    // Live cells
                    runLength = (runLength == 0) ? 1 : runLength;
                    for (int i = 0; i < runLength; ++i) {
                        if (col < cols && row < rows) {
                            cells[row][col] = true;
                        }
                        col++;
                    }
                    runLength = 0;
                } else if (c == '$') {
                    // End of line
                    runLength = (runLength == 0) ? 1 : runLength;
                    row += runLength;
                    col = 0;
                    runLength = 0;
                } else if (c == '!') {
                    // End of the pattern
                    return cells;
                }
            }
        }

        return cells;
    }
}
