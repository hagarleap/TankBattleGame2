#pragma once
#include "common/TankAlgorithm.h"
#include "Direction.h"
#include <optional>

class BaseTankAlgorithm : public TankAlgorithm {
protected:
    Direction dir = Direction::L; // or R, initialize in concrete class

    Direction computeDirection(int dx, int dy) const {
        if (dx == 0 && dy < 0) return Direction::U;
        if (dx > 0 && dy < 0) return Direction::UR;
        if (dx > 0 && dy == 0) return Direction::R;
        if (dx > 0 && dy > 0) return Direction::DR;
        if (dx == 0 && dy > 0) return Direction::D;
        if (dx < 0 && dy > 0) return Direction::DL;
        if (dx < 0 && dy == 0) return Direction::L;
        if (dx < 0 && dy < 0) return Direction::UL;
        return dir;
    }

    std::optional<Direction> computeRayDirection(int dx, int dy) const{
        if (dx == 0 && dy < 0)                 return Direction::U;
        if (dx  > 0 && dy < 0 && dx == -dy)    return Direction::UR;
        if (dx  > 0 && dy == 0)                return Direction::R;
        if (dx  > 0 && dy > 0 && dx ==  dy)    return Direction::DR;
        if (dx == 0 && dy > 0)                 return Direction::D;
        if (dx  < 0 && dy > 0 && dx == -dy)    return Direction::DL;
        if (dx  < 0 && dy == 0)                return Direction::L;
        if (dx  < 0 && dy < 0 && dx ==  dy)    return Direction::UL;
        return std::nullopt;                   // Not on a shooting ray
    }

    ActionRequest rotateToward(Direction current, Direction target) const {
        int cur = static_cast<int>(current);
        int tgt = static_cast<int>(target);
        int diff = (tgt - cur + 8) % 8;
        if (diff == 0) return ActionRequest::DoNothing;
        if (diff == 1 || diff == 2) return ActionRequest::RotateRight45;
        if (diff == 7 || diff == 6) return ActionRequest::RotateLeft45;
        return (diff <= 4) ? ActionRequest::RotateRight90 : ActionRequest::RotateLeft90;
    }

    int directionDelta(Direction from, Direction to) const {
        return (static_cast<int>(to) - static_cast<int>(from) + 8) % 8;
    }
};
