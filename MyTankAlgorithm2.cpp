#include "MyTankAlgorithm2.h"
#include "common/ActionRequest.h"
#include "common/BattleInfo.h"
#include "common/SatelliteView.h"
#include "MyBattleInfo.h" // Include your derived BattleInfo header
#include <cmath>
#include <vector>
#include <set>
#include <algorithm>


MyTankAlgorithm2::MyTankAlgorithm2() {}

ActionRequest MyTankAlgorithm2::getAction() {
    if (cooldownCounter > 0)
        cooldownCounter--;

    if (enemyInLine) {
        enemyInLine = false;
        cooldownCounter = 4;
        return ActionRequest::Shoot;
    }

    if (rotateAction.has_value()) {
        ActionRequest act = rotateAction.value();

        if (act == ActionRequest::RotateRight90) dir = rotateR4(dir);
        else if (act == ActionRequest::RotateLeft90) dir = rotateL4(dir);
        else if (act == ActionRequest::RotateRight45) dir = rotateR8(dir);
        else if (act == ActionRequest::RotateLeft45) dir = rotateL8(dir);

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

    int myX = -1, myY = -1;
    std::vector<std::pair<int,int>> enemies;

    //locate self & enemies
    for (size_t y = 0; y < myInfo->getBoardHeight(); ++y)
        for (size_t x = 0; x < myInfo->getBoardWidth();  ++x) {
            char c = view->getObjectAt(x, y);
            if (c == '%')            { myX = x; myY = y; }
            else if (c == '1')       { enemies.emplace_back(x, y); }
        }

    if (myX == -1) return;   // could not find self

    struct Target { int x, y, dist; Direction ray; };
    std::vector<Target> viable;

    for (auto [ex, ey] : enemies) {
        int dxRaw = ex - myX;
        int dyRaw = ey - myY;

        int w = (int)myInfo->getBoardWidth();
        int h = (int)myInfo->getBoardHeight();

        int dx = (std::abs(dxRaw) <= w / 2) ? dxRaw : (dxRaw > 0 ? dxRaw - w : dxRaw + w);
        int dy = (std::abs(dyRaw) <= h / 2) ? dyRaw : (dyRaw > 0 ? dyRaw - h : dyRaw + h);

        std::optional<Direction> rayOpt = computeDirection(dx, dy);  // strict direction
        if (!rayOpt) continue;

        Direction ray = *rayOpt;
        int tx = myX, ty = myY;
        bool blocked = false;
        int steps = 0;

        while (true) {
            // Step in direction
            switch (ray) {
                case Direction::U:  ty -= 1; break;
                case Direction::UR: tx += 1; ty -= 1; break;
                case Direction::R:  tx += 1; break;
                case Direction::DR: tx += 1; ty += 1; break;
                case Direction::D:  ty += 1; break;
                case Direction::DL: tx -= 1; ty += 1; break;
                case Direction::L:  tx -= 1; break;
                case Direction::UL: tx -= 1; ty -= 1; break;
                default: break;
            }

            // Wraparound
            tx = wrap(tx, (int)myInfo->getBoardWidth());
            ty = wrap(ty, (int)myInfo->getBoardHeight());
            steps++;

            if (tx == ex && ty == ey) break;  // reached target

            char obj = view->getObjectAt(tx, ty);
            if (obj == '2') {
                blocked = true;
                break;
            }
        }

        if (!blocked)
            viable.push_back({ex, ey, steps, ray});
    }


    std::cout << "[TankAlgo2] viable targets: " << viable.size() << ", total enemies: " << enemies.size() << "\n";


    // 1. If we found a target
    if (!viable.empty()) {
        const Target& tgt = *std::min_element(
            viable.begin(), viable.end(),
            [](const Target& a, const Target& b){ return a.dist < b.dist; });

        ActionRequest rot = rotateToward(dir, tgt.ray);
        if (rot == ActionRequest::DoNothing) {
            if (cooldownCounter == 0) {
                enemyInLine = true;  // next getAction() will Shoot
            } else {
                std::cout << "[TankAlgo2] Facing enemy but cooldown = " << cooldownCounter << ", waiting\n";
            }
        } else {
            rotateAction = rot;
            std::cout << to_string(rot) << std::endl;
        }
        return;
    }

    // 2. If not, rotate toward the closest enemy (axis-aligned only)
    if (!enemies.empty()) {
        // choose the enemy with smallest Manhattan distance
        auto it = std::min_element(
            enemies.begin(), enemies.end(),
            [myX,myY](auto a, auto b){
                return (std::abs(a.first-myX)+std::abs(a.second-myY)) <
                       (std::abs(b.first-myX)+std::abs(b.second-myY));
            });
        int dx = it->first - myX,  dy = it->second - myY;

        Direction axisDir = dir;
        if      (std::abs(dx) >= std::abs(dy)) axisDir = (dx > 0 ? Direction::R : (dx < 0 ? Direction::L : axisDir));
        else                                    axisDir = (dy > 0 ? Direction::D : Direction::U);

        ActionRequest rot = rotateToward(dir, axisDir);
        if (rot != ActionRequest::DoNothing) rotateAction = rot;
    }
    
}