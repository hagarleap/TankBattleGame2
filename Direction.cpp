#include "Direction.h"

Direction rotateL8(Direction dir) {
    return static_cast<Direction>((static_cast<int>(dir) + 7) % 8);
}

Direction rotateR8(Direction dir) {
    return static_cast<Direction>((static_cast<int>(dir) + 1) % 8);
}

Direction rotateL4(Direction dir) {
    return static_cast<Direction>((static_cast<int>(dir) + 6) % 8); // = left8 twice
}

Direction rotateR4(Direction dir) {
    return static_cast<Direction>((static_cast<int>(dir) + 2) % 8); // = right8 twice
}

Direction opposite(Direction dir) {
    return static_cast<Direction>((static_cast<int>(dir) + 4) % 8);
}

//directional deltas for movement
int dx(Direction dir) {
    switch (dir) {
        case Direction::L:  return -1;
        case Direction::R:  return 1;
        case Direction::UL: return -1;
        case Direction::DL: return -1;
        case Direction::UR: return 1;
        case Direction::DR: return 1;
        default:            return 0;
    }
}

int dy(Direction dir) {
    switch (dir) {
        case Direction::U:  return -1;
        case Direction::D:  return 1;
        case Direction::UL: return -1;
        case Direction::UR: return -1;
        case Direction::DL: return 1;
        case Direction::DR: return 1;
        default:            return 0;
    }
}
