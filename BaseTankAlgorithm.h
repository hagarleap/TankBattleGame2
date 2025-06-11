#pragma once
#include "common/TankAlgorithm.h"
#include "MyBattleInfo.h"
#include "Direction.h"
#include <optional>

class BaseTankAlgorithm : public TankAlgorithm {
protected:
    Direction dir = Direction::L; // or R, initialize in concrete class
    int cooldownCounter = 0;
    std::optional<int> shells;

    void initShellsFromInfo(const BattleInfo& info) {
        if (!shells.has_value()) {
            if (auto* myInfo = dynamic_cast<const MyBattleInfo*>(&info)) {
                shells = myInfo->getRemainingShells();
            }
        }
    }

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


    ActionRequest rotateToward(Direction from, Direction to) const {
            int fromIdx = static_cast<int>(from);
            int toIdx = static_cast<int>(to);
            int diff = (toIdx - fromIdx + 8) % 8; // rotate clockwise difference

            if (diff == 0) return ActionRequest::DoNothing;
            if (diff == 1) return ActionRequest::RotateRight45;
            if (diff == 2) return ActionRequest::RotateRight90;
            if (diff == 3) return ActionRequest::RotateLeft45;  // shorter than 5 right
            if (diff == 4) return ActionRequest::RotateRight90; // or left90, arbitrary
            if (diff == 5) return ActionRequest::RotateRight45; // back to 3
            if (diff == 6) return ActionRequest::RotateLeft90;
            if (diff == 7) return ActionRequest::RotateLeft45;

            return ActionRequest::DoNothing; // fallback
    }


    inline int wrap(int val, int max) {
        return (val + max) % max;
}
};
