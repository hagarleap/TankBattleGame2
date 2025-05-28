#ifndef DIRECTION_H
#define DIRECTION_H

enum class Direction {
    U,  // Up
    UR, // Up-Right
    R,  // Right
    DR, // Down-Right
    D,  // Down
    DL, // Down-Left
    L,  // Left
    UL  // Up-Left
};

// Rotation functions
Direction rotateL8(Direction dir);
Direction rotateR8(Direction dir);
Direction rotateL4(Direction dir);
Direction rotateR4(Direction dir);
Direction opposite(Direction dir);

// Movement delta helpers (optional)
int dx(Direction dir);
int dy(Direction dir);

#endif // DIRECTION_H
