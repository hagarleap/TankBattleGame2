#ifndef POSITION_H
#define POSITION_H
#include "Direction.h"

class Position {
public:
    int x;
    int y;

    Position(int x = 0, int y = 0);

    Position move(Direction dir, int steps = 1) const;

    bool operator==(const Position& other) const;
    bool operator!=(const Position& other) const;

    
};

#endif // POSITION_H
