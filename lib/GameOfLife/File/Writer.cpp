#include "Writer.h"

#include <fstream>
#include <vector>
#include <filesystem>

#include "Utils.h"
#include "Game/BaseGrid.h"

namespace GameOfLife::File {
    /**
     * Writes the grid to a file
     *
     * @param grid The grid to write
     * @param filename The filename to write to
     */
    void Writer::write(const Game::BaseGrid &grid, const std::string &filename) const {
        // Overload the function to write a boolean matrix
        write(grid.toBooleanGrid(), filename);
    }

    /**
     * Writes a bool matrix to a file in plaintext format
     * @param matrix The matrix to write
     * @param filename The filename to write to
     */
    void Writer::write(const std::vector<std::vector<bool>>& matrix, const std::string &filename) const {
        // Overload to the static function
        write(matrix, filename, config);
    }

    /**
     * Writes a grid to a file in a custom format
     *
     * @param matrix The 2D matrix to write
     * @param filename The filename to write to
     */
    void Writer::write(const std::vector<std::vector<bool>> &matrix, const std::string &filename, FormatConfig format) {
        // Argument validation
        if (filename.empty()) {
            throw std::invalid_argument("Filename cannot be empty");
        }

        // Create the file (and the directory if it doesn't exist)
        std::filesystem::path out = Utils::makeAbsolutePath(filename).string();
        create_directory(out.parent_path());
        std::ofstream file(out);
        if (!file.is_open()) {
            throw std::runtime_error("Could not open file");
        }

        // Write the matrix to the file
        for (const auto& row : matrix) {
            for (const bool cell : row) {
                file << (cell ? format.getAliveChar() : format.getDeadChar());
                if (format.getDelimiterChar() != '\0') {
                    file << format.getDelimiterChar();
                }
            }
            file << "\n";
        }

        // Close the file
        file.close();
    }

    /**
     * Writes a grid to a file in RLE format
     * @param grid The grid to write
     * @param filename The filename to write to
     */
    void Writer::writeRLE(const Game::BaseGrid &grid, const std::string &filename) {
        // Overload the function to write a boolean matrix
        writeRLE(grid.toBooleanGrid(), filename);
    }

    /**
     * Writes a bool matrix to a file in RLE (Run-Length Encoding) format
     *
     * @param matrix The 2D matrix to write
     * @param filename The filename to write to
     */
    void Writer::writeRLE(const std::vector<std::vector<bool>>& matrix, const std::string &filename) {
        // Argument validation
        if (filename.empty()) {
            throw std::invalid_argument("Filename cannot be empty");
        }

        // Open the file
        std::ofstream file(filename);
        if (!file.is_open()) {
            throw std::runtime_error("Could not open file");
        }

        // Count the number of rows and columns, assuming the matrix is not empty
        int rows = matrix.size();
        int cols = matrix[0].size();

        // Write the header
        file << "x = " << cols << ", y = " << rows << ", rule = B3/S23\n";

        int count = 0;
        bool last = matrix[0][0];
        std::vector<std::pair<int, bool>> runs;
        for (int row = 0; row < rows; row++) {
            // For each row, count the number of runs of alive and dead cells
            for (int col = 0; col < cols; col++) {
                if (matrix[row][col] == last) {
                    count++;
                } else {
                    runs.emplace_back(count, last);
                    count = 1;
                    last = matrix[row][col];
                }
            }
            // Add the last run
            runs.emplace_back(count, last);

            // Drop trailing dead cells
            if (runs.back().second == false) {
                runs.pop_back();
            }

            // Write the run-length encoding
            for (const auto& run : runs) {
                if (run.first == 1) {
                    file << (run.second ? 'o' : 'b');
                } else if (run.first == 0) {
                    // Do nothing
                }
                else {
                    file << run.first << (run.second ? 'o' : 'b');
                }
            }
            // Write the end of the row and reset the run-length encoding
            file << "$";
            last = matrix[row][0];
            count = 0;
            runs.clear();
        }

        // Write the end of the file and close it
        file << "!";
        file.close();
    }

    /**
     * Bulk-write a list of grids to a folder
     *
     * @param bulk The bulk of grids to write
     * @param outputFolder The folder to write to
     * @param startIndex The index to start from
     */
    void Writer::writeBulk(const std::vector<std::vector<std::vector<Game::BaseCell>>> &bulk,
        const std::string &outputFolder, int startIndex) const {
        // Overload to the static function
        writeBulk(bulk, outputFolder, config, startIndex);
    }

    /**
     * Bulk-write a list of grids to a folder
     *
     * @param bulk The bulk of grids to write
     * @param outputFolder The folder to write to
     * @param format The format to write in
     * @param startIndex The index to start from
     */
    void Writer::writeBulk(const std::vector<std::vector<std::vector<Game::BaseCell>>> &bulk,
        const std::string &outputFolder, const FormatConfig& format, int startIndex) {
        // Create destination directory
        std::filesystem::path out = Utils::makeAbsolutePath(outputFolder);
        create_directory(out);

        // Write each grid to a file
        for (int j = 0; j < bulk.size(); j++) {
            // Convert the grid to a boolean matrix
            std::vector<std::vector<bool>> matrix;
            matrix.reserve(bulk[j].size());
            for (const auto& row : bulk[j]) {
                std::vector<bool> boolRow;
                boolRow.reserve(row.size());
                for (const auto& cell : row) {
                    boolRow.push_back(cell.isAlive());
                }
                matrix.push_back(boolRow);
            }

            // Write the matrix to a file
            if (format.getExtension() == ".rle")
                writeRLE(matrix, out.string() + "\\gen" + std::to_string(j + startIndex) + format.getExtension());
            else
                write(matrix, out.string() + "\\gen" + std::to_string(j + startIndex) + format.getExtension(), format);
        }
    }

    /**
     * Bulk-write a list of grids to a folder based on the output format
     *
     * @param bulk The bulk of grids to write
     * @param outputFolder The folder to write to
     * @param format The format to write in
     * @param startIndex The index to start from
     */
    void Writer::writeBulk(const std::vector<std::vector<std::vector<Game::BaseCell>>> &bulk,
        const std::string &outputFolder, const OutputFormat format, int startIndex) {
        switch (format) {
            case OutputFormat::CUSTOM:
            default:
                writeBulk(bulk, outputFolder, FormatConfig('1', '0', ' '), startIndex);
                break;
            case OutputFormat::PLAINTEXT:
                writeBulk(bulk, outputFolder, FormatConfig('O', '.', '\0', '!', ".cells"), startIndex);
                break;
            case OutputFormat::RLE:
                writeBulk(bulk, outputFolder, FormatConfig('o', 'b', '\0', '#', ".rle"), startIndex);
                break;
        }
    }
}
