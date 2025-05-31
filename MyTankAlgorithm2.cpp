#include "MyTankAlgorithm2.h"
#include "common/ActionRequest.h"
#include "common/BattleInfo.h"
#include "common/SatelliteView.h"
#include "MyBattleInfo.h" // Include your derived BattleInfo header
#include <cmath>
#include <vector>

MyTankAlgorithm2::MyTankAlgorithm2() {}

ActionRequest MyTankAlgorithm2::getAction() {
    if (enemyInLine) {
        return ActionRequest::Shoot;
    }
    // If we need to rotate, do so
    if (rotateAction.has_value()) {
        ActionRequest act = rotateAction.value();
        rotateAction.reset();
        return act;
    }
    return ActionRequest::GetBattleInfo;
}

void MyTankAlgorithm2::updateBattleInfo(BattleInfo& info) {
    enemyInLine = false;
    rotateAction.reset();

    const MyBattleInfo* myInfo = dynamic_cast<const MyBattleInfo*>(&info);
    if (!myInfo) return;

    const SatelliteView* view = myInfo->getSatelliteView();
    if (!view) return;

    size_t width = myInfo->getBoardWidth();
    size_t height = myInfo->getBoardHeight();

    // Find my tank and enemy tanks
    int myX = -1, myY = -1;
    std::vector<std::pair<int, int>> enemies;
    for (size_t y = 0; y < height; ++y) {
        for (size_t x = 0; x < width; ++x) {
            char obj = view->getObjectAt(x, y);
            if (obj == '%') { // self
                myX = x; myY = y;
            } else if (obj == '1' || obj == '2') { // enemy tanks
                enemies.emplace_back(x, y);
            }
        }
    }
    if (myX == -1 || myY == -1) return; // couldn't find self

    // 1. Check if any enemy is in line with cannon
    for (const auto& [ex, ey] : enemies) {
        if (dir == Direction::U && ex == myX && ey < myY) { enemyInLine = true; return; }
        if (dir == Direction::R && ey == myY && ex > myX) { enemyInLine = true; return; }
        if (dir == Direction::D && ex == myX && ey > myY) { enemyInLine = true; return; }
        if (dir == Direction::L && ey == myY && ex < myX) { enemyInLine = true; return; }
    }

    // 2. If not, rotate toward the closest enemy (axis-aligned only)
    if (!enemies.empty()) {
        int minDist = 1e9;
        Direction targetDir = dir;
        for (const auto& [ex, ey] : enemies) {
            int dx = ex - myX;
            int dy = ey - myY;
            int dist = std::abs(dx) + std::abs(dy);
            if (dist < minDist) {
                if (dx == 0 && dy < 0) { minDist = dist; targetDir = Direction::U; }
                else if (dy == 0 && dx > 0) { minDist = dist; targetDir = Direction::R; }
                else if (dx == 0 && dy > 0) { minDist = dist; targetDir = Direction::D; }
                else if (dy == 0 && dx < 0) { minDist = dist; targetDir = Direction::L; }
            }
        }
        // Plan rotation if not already facing
        if (targetDir != dir) {
            // Only 90-degree turns supported by ActionRequest
            int diff = (static_cast<int>(targetDir) - static_cast<int>(dir) + 8) % 8;
            if (diff == 2) rotateAction = ActionRequest::RotateRight90;
            else if (diff == 6) rotateAction = ActionRequest::RotateLeft90;
            // If opposite (diff == 4), can pick either
            else if (diff == 4) rotateAction = ActionRequest::RotateRight90;
        }
    }
}