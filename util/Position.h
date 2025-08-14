#ifndef POSITION_H
#define POSITION_H
#include <cstdint>

class Position {
    uint32_t row = 0;

public:
    Position() = default;

    explicit Position(const uint32_t &row) {
        this->row = row;
    }

    [[nodiscard]] uint32_t GetRow() const {
        return row;
    }
};
#endif //POSITION_H
