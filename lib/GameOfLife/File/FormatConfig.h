#ifndef FILECONFIG_H
#define FILECONFIG_H

#include <string>

namespace GameOfLife::File {
    /**
     * Configuration for priting/file format
     */
    class FormatConfig {
        char delimiter;
        char alive;
        char dead;
        char comment = '!';
        std::string extension = ".txt";
    public:
        FormatConfig(const char alive, const char dead, const char delimiter = '\0', const char comment = '!', const std::string& extension = ".txt") :
            delimiter(delimiter), alive(alive), dead(dead), comment(comment), extension(extension) {}

        [[nodiscard]] char getDelimiterChar() const { return delimiter; }

        [[nodiscard]] char getAliveChar() const { return alive; }

        [[nodiscard]] char getDeadChar() const { return dead; }

        [[nodiscard]] char getCommentChar() const { return comment; }

        [[nodiscard]] std::string getExtension() const { return extension; }
    };

    enum class OutputFormat {
        CUSTOM,
        PLAINTEXT,
        RLE
    };
}


#endif //FILECONFIG_H
