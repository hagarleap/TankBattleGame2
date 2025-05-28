#include "Shell.h"
#include "Board.h"


Shell::Shell(Position pos, Direction dir)
    : position(pos), direction(dir), active(true) {}

void Shell::advance(Board& board) {
    if (!active) return;
    Position newPos = position.move(direction, 1);
    position = board.wrapPosition(newPos);
    
}

Position Shell::getPosition() const {
    return position;
}

Direction Shell::getDirection() const {
    return direction;
}

bool Shell::isActive() const {
    return active;
}
