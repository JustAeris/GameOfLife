#ifndef WRITER_H
#define WRITER_H
#include <string>
#include <vector>

#include "IWritable.h"

namespace GameOfLife::File {
    /**
     * Writes the grid to a file
     */
    class Writer {
    public:

        static void write(const IWritable &data, const std::string &filename);
        static void writeBulk(const std::vector<IWritable> &data, const std::string &outputFolder, int startIndex);

        static void writeRLE(const std::vector<std::vector<bool>>& matrix, const std::string &filename);
        static void writeBulkRLE(const std::vector<std::vector<std::vector<bool>>>& matrix, const std::string &outputFolder, int startIndex);
    };

}

#endif //WRITER_H
