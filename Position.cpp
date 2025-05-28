#include "Position.h"

Position::Position(int x, int y) : x(x), y(y) {}

Position Position::move(Direction dir, int steps) const {
    return Position(
        x + dx(dir) * steps,
        y + dy(dir) * steps
    );
}

bool Position::operator==(const Position& other) const {
    return x == other.x && y == other.y;
}

bool Position::operator!=(const Position& other) const {
    return !(*this == other);
}






