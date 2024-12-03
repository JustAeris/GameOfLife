#ifndef HASHFUNCTION_H
#define HASHFUNCTION_H

#include <utility>
#include <functional>

namespace GameOfLife::Game {
    struct HashFunction {
        std::size_t operator()(const std::pair<int, int>& p) const noexcept {
            return std::hash<int>()(p.first) ^ (std::hash<int>()(p.second) << 1);
        }
    };
}

#endif // HASHFUNCTION_H
