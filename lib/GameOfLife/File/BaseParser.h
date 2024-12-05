#ifndef BASEPARSER_H
#define BASEPARSER_H
#include <string>

namespace GameOfLife::File {
    /**
     * Base interface for a parser
     */
    template<typename T>
    class BaseParser<T> {
    public:
        BaseParser() = default;
        virtual ~BaseParser() = default;

        virtual T parse(const std::string &filename, int &rows, int &cols) = 0;
    };
}

#endif //BASEPARSER_H
