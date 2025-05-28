#include "MyTankAlgorithm.h"
#include <cmath>

namespace {
    int sign(int x) { return (x > 0) - (x < 0); }
}

ActionRequest MyTankAlgorithm::getAction() {
    ++round;

    if (round % refreshRate == 1 || !cachedInfo) {
        return ActionRequest::GetBattleInfo;
    }

    if (!currentTarget && cachedInfo) {
        // Find nearest enemy from cache
        auto [myX, myY] = cachedInfo->getMyLocation();
        currentTarget = cachedInfo->findClosestEnemy(myX, myY);
    }

    if (currentTarget) {
        auto [tx, ty] = *currentTarget;
        auto [myX, myY] = cachedInfo->getMyLocation();
        int dx = static_cast<int>(tx) - static_cast<int>(myX);
        int dy = static_cast<int>(ty) - static_cast<int>(myY);

        if (std::abs(dx) + std::abs(dy) <= 1) {
            return ActionRequest::Shoot;
        }

        // Naive logic: prefer horizontal
        return dx != 0 ? ActionRequest::MoveForward : ActionRequest::RotateLeft90;
    }

    return ActionRequest::DoNothing;
}

void MyTankAlgorithm::updateBattleInfo(BattleInfo& info) {
    auto* realInfo = dynamic_cast<MyBattleInfo*>(&info);
    if (realInfo) {
        cachedInfo = std::make_unique<MyBattleInfo>(*realInfo);
        currentTarget = std::nullopt; // reassign target next turn
    }
}
