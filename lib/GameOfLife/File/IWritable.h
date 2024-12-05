#ifndef IWRITABLE_H
#define IWRITABLE_H

namespace GameOfLife::File {
    /**
     * Writable component interface
     */
    class IWritable {
    protected:
        ~IWritable() = default;

    public:
        [[nodiscard]] virtual std::string getText() const = 0;
    };

}

#endif //IWRITABLE_H
