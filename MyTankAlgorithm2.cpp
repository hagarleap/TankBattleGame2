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
    if (enemyInLine) {
        return ActionRequest::Shoot;
    }
    // If we need to rotate, do so
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
        int dx = ex - myX,  dy = ey - myY;
        std::optional<Direction> rayOpt = computeDirection(dx, dy);  // strict version
        if (!rayOpt) continue;                              // not on a firing ray

        int steps = std::max(std::abs(dx), std::abs(dy));
        bool blocked = false;
        for (int s = 1; s < steps && !blocked; ++s) {
            int tx = myX + (dx == 0 ? 0 : (dx > 0 ?  s : -s));
            int ty = myY + (dy == 0 ? 0 : (dy > 0 ?  s : -s));
            if (view->getObjectAt(tx, ty) == '2') blocked = true;   // friendly in path
        }
        if (!blocked) viable.push_back({ex, ey, steps, *rayOpt});
    }

    std::cout << "[TankAlgo2] viable targets: " << viable.size() << ", total enemies: " << enemies.size() << "\n";


    // 1. If we found a target
    if (!viable.empty()) {
        const Target& tgt = *std::min_element(
            viable.begin(), viable.end(),
            [](const Target& a, const Target& b){ return a.dist < b.dist; });

        ActionRequest rot = rotateToward(dir, tgt.ray);
        if (rot == ActionRequest::DoNothing) {
            enemyInLine = true;      // next getAction() will Shoot
        } else {
            rotateAction = rot;      // schedule one-step turn
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