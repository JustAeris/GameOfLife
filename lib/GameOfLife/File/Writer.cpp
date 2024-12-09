#include "Writer.h"

#include <fstream>
#include <vector>
#include <filesystem>

#include "Utils.h"

namespace GameOfLife::File {
    /**
     * Writes the data to a file
     *
     * @param data The data to write, must implement IWritable
     * @param filename The filename to write to
     */
    void Writer::write(const IWritable &data, const std::string &filename) {
        // Argument validation
        if (filename.empty()) {
            throw std::invalid_argument("Filename cannot be empty");
        }

        // Create the file (and the directory if it doesn't exist)
        const std::filesystem::path out = Utils::makeAbsolutePath(filename).string();
        create_directory(out.parent_path());
        std::ofstream file(out);
        if (!file.is_open()) {
            throw std::runtime_error("Could not open file");
        }

        file << "\xEF\xBB\xBF"; // Forces UTF-8
        const auto text = data.getText();
        file << text;
        file.close();
    }

    /**
     * Writes a vector of IWritable objects to a file
     *
     * @param data The data to write
     * @param outputFolder The folder to write the files to
     * @param startIndex The starting index for the files
     */
    void Writer::writeBulk(const std::vector<IWritable> &data, const std::string &outputFolder, int startIndex) {
        for (int i = 0; i < data.size(); i++) {
            write(data[i], outputFolder + "/output" + std::to_string(startIndex + i) + ".txt");
        }
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
        const int rows = matrix.size();
        const int cols = matrix[0].size();

        // Write the header
        file << "x = " << cols << ", y = " << rows << ", rule = B3/S23\n";

        int count = 0;
        bool last = matrix[0][0];
        std::vector<std::pair<int, bool>> runs;
        for (int row = 0; row < rows; row++) {
            // For each row, count the number of runs of living and dead cells
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
     * Writes a vector of bool matrices to files in RLE (Run-Length Encoding) format
     *
     * @param matrix The 2D array to write
     * @param outputFolder The folder to write the files to
     * @param startIndex The starting index for the files
     */
    void Writer::writeBulkRLE(const std::vector<std::vector<std::vector<bool>>> &matrix,
        const std::string &outputFolder, const int startIndex) {
        for (int i = 0; i < matrix.size(); i++) {
            writeRLE(matrix[i], outputFolder + "/gen" + std::to_string(startIndex + i) + ".rle");
        }
    }
}
