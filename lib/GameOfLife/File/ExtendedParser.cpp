#include "ExtendedParser.h"

#include <fstream>
#include <stdexcept>

#include "Utils.h"

namespace GameOfLife::File {
    /**
     * Parse the input file and return the cells.
     * Supports obstacle cells.
     *
     * @param filename Source file path
     * @param rows Number of rows, to be set by the function
     * @param cols Number of columns, to be set by the function
     * @return 2D vector of cells
     */
    std::vector<std::vector<Game::Cell>> ExtendedParser::parse(const std::string &filename, int &rows, int &cols) {
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
        std::vector<std::vector<Game::Cell>> cells;
        cells.resize(fileRows, std::vector<Game::Cell>(fileCols));

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
                    cells[i][j] = Game::Cell(true);
                    j++;
                } else if (line[j] == config.getDeadChar()) {
                    cells[i][j] = Game::Cell(false);
                    j++;
                } else if (line[j] == livingObstacle) {
                    cells[i][j] = Game::Cell(true, true);
                    j++;
                } else if (line[j] == deadObstacle) {
                    cells[i][j] = Game::Cell(false, true);
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

}
