#include "MyTankAlgorithm2.h"
#include "common/ActionRequest.h"
#include "common/BattleInfo.h"
#include "common/SatelliteView.h"
#include "MyBattleInfo.h" // Include your derived BattleInfo header
#include <cmath>
#include <vector>
#include <set>


MyTankAlgorithm2::MyTankAlgorithm2() {}

ActionRequest MyTankAlgorithm2::getAction() {
    if (enemyInLine) {
        return ActionRequest::Shoot;
    }
    // If we need to rotate, do so
    if (rotateAction.has_value()) {
        ActionRequest act = rotateAction.value();
        // Update dir if rotating
        if (act == ActionRequest::RotateRight90) {
            dir = rotateR4(dir);
        } else if (act == ActionRequest::RotateLeft90) {
            dir = rotateL4(dir);
        } else if (act == ActionRequest::RotateRight45) {
            dir = rotateR8(dir);
        } else if (act == ActionRequest::RotateLeft45) {
            dir = rotateL8(dir);
        }
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
            } else if (obj == '1') { // enemy tanks
                enemies.emplace_back(x, y);
            }
        }
    }
    if (myX == -1 || myY == -1) return; // couldn't find self

    // 1. Find the enemy that is easiest to aim at (no friendly in the way, least rotation needed)
    int bestScore = 1000; // lower is better
    Direction bestDir = dir;
    int bestEx = -1, bestEy = -1;
    bool canShoot = false;
    // Consider all 8 directions
    std::vector<Direction> allDirs = {Direction::U, Direction::UR, Direction::R, Direction::DR, Direction::D, Direction::DL, Direction::L, Direction::UL};
    for (const auto& [ex, ey] : enemies) {
        for (Direction d : allDirs) {
            bool inLine = false;
            int dist = 0;
            int dx = ex - myX;
            int dy = ey - myY;
            // Check if enemy is in this direction
            switch (d) {
                case Direction::U:
                    if (dx == 0 && dy < 0) { inLine = true; dist = -dy; }
                    break;
                case Direction::UR:
                    if (dx == -dy && dx > 0) { inLine = true; dist = dx; }
                    break;
                case Direction::R:
                    if (dy == 0 && dx > 0) { inLine = true; dist = dx; }
                    break;
                case Direction::DR:
                    if (dx == dy && dx > 0) { inLine = true; dist = dx; }
                    break;
                case Direction::D:
                    if (dx == 0 && dy > 0) { inLine = true; dist = dy; }
                    break;
                case Direction::DL:
                    if (dx == -dy && dx < 0) { inLine = true; dist = -dx; }
                    break;
                case Direction::L:
                    if (dy == 0 && dx < 0) { inLine = true; dist = -dx; }
                    break;
                case Direction::UL:
                    if (dx == dy && dx < 0) { inLine = true; dist = -dx; }
                    break;
                default: break;
            }
            if (!inLine) continue;
            // Check for friendly in the way
            bool friendlyInWay = false;
            int steps = dist;
            for (int step = 1; step < steps; ++step) {
                int tx = myX, ty = myY;
                switch (d) {
                    case Direction::U: ty -= step; break;
                    case Direction::UR: tx += step; ty -= step; break;
                    case Direction::R: tx += step; break;
                    case Direction::DR: tx += step; ty += step; break;
                    case Direction::D: ty += step; break;
                    case Direction::DL: tx -= step; ty += step; break;
                    case Direction::L: tx -= step; break;
                    case Direction::UL: tx -= step; ty -= step; break;
                    default: break;
                }
                char obj = view->getObjectAt(tx, ty);
                if (obj == '2') { friendlyInWay = true; break; }
            }
            if (friendlyInWay) continue;
            // Score: 0 if already aimed, otherwise minimal rotation
            int diff = (static_cast<int>(d) - static_cast<int>(dir) + 8) % 8;
            int rotScore = (diff == 0) ? 0 : (diff <= 4 ? diff : 8 - diff); // minimal turns
            int score = rotScore * 100 + dist; // prioritize less rotation, then closer
            if (score < bestScore) {
                bestScore = score;
                bestDir = d;
                bestEx = ex; bestEy = ey;
                canShoot = (rotScore == 0);
            }
        }
    }
    // If we found a target
    if (bestEx != -1 && bestEy != -1) {
        if (canShoot) {
            // Before shooting, always get battle info
            rotateAction.reset(); enemyInLine = false; rotateAction = ActionRequest::GetBattleInfo; return;
        } else {
            // Plan rotation toward bestDir
            int diff = (static_cast<int>(bestDir) - static_cast<int>(dir) + 8) % 8;
            if (diff == 1) rotateAction = ActionRequest::RotateRight45;
            else if (diff == 2) rotateAction = ActionRequest::RotateRight90;
            else if (diff == 3) rotateAction = ActionRequest::RotateRight90; // or two 45s
            else if (diff == 4) rotateAction = ActionRequest::RotateRight90; // or left, 180
            else if (diff == 5) rotateAction = ActionRequest::RotateLeft45;
            else if (diff == 6) rotateAction = ActionRequest::RotateLeft90;
            else if (diff == 7) rotateAction = ActionRequest::RotateLeft45;
            return;
        }
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
            int diff = (static_cast<int>(targetDir) - static_cast<int>(dir) + 8) % 8;
            if (diff == 1) rotateAction = ActionRequest::RotateRight45;
            else if (diff == 2) rotateAction = ActionRequest::RotateRight90;
            else if (diff == 3) rotateAction = ActionRequest::RotateRight90; // or two 45s
            else if (diff == 4) rotateAction = ActionRequest::RotateRight90; // or left, 180
            else if (diff == 5) rotateAction = ActionRequest::RotateLeft45;
            else if (diff == 6) rotateAction = ActionRequest::RotateLeft90;
            else if (diff == 7) rotateAction = ActionRequest::RotateLeft45;
        }
    }
}