#include "MyPlayer1.h"
#include "MyBattleInfo.h"
#include "GameSatelliteView.h"
#include "MyTankAlgorithm1.h"
#include <cmath>
#include <iostream>

MyPlayer1::MyPlayer1(int player_index, size_t width, size_t height, size_t max_steps, size_t num_shells)
    : Player(player_index, width, height, max_steps, num_shells),
      boardWidth(width), boardHeight(height),
      maxSteps(max_steps), numShells(num_shells), playerIndex(player_index) {}

void MyPlayer1::updateTankWithBattleInfo(TankAlgorithm& tank, SatelliteView& satellite_view) {
    const GameSatelliteView* gview = dynamic_cast<const GameSatelliteView*>(&satellite_view);
    MyBattleInfo info(boardWidth, boardHeight, gview);
    info.setInitialShells(numShells);

    std::pair<int, int> self{-1, -1};
    std::vector<std::pair<int, int>> enemies;

    for (size_t y = 0; y < boardHeight; ++y) {
        for (size_t x = 0; x < boardWidth; ++x) {
            char c = gview->getObjectAt(x, y);
            if (c == '%') {
                self = {static_cast<int>(x), static_cast<int>(y)};
                info.setSelf(self);
            } else if (c == '2') {
                enemies.emplace_back(static_cast<int>(x), static_cast<int>(y));
            }
        }
    }

    if (self.first == -1 || enemies.empty()) {
        assignedEnemies.clear();
        prevEnemyLayout.clear();
        tank.updateBattleInfo(info);
        return;
    }

    std::unordered_set<std::pair<int, int>, pair_hash> currentEnemies(enemies.begin(), enemies.end());
    if (currentEnemies != prevEnemyLayout) {
        assignedEnemies.clear();
        prevEnemyLayout = currentEnemies;
    }

    int tankId = -1;
    if (auto* myTank = dynamic_cast<MyTankAlgorithm1*>(&tank)) {
        tankId = myTank->getId();
    }

    // If this tank was already assigned an enemy in the last step, reuse that enemy
    auto it = tankAssignments.find(tankId);
    if (it != tankAssignments.end() && currentEnemies.count(it->second)) {
        info.setEnemy(it->second);
    } else {
        std::pair<int, int> target = {-1, -1};
        int minDist = 1e9;
        for (const auto& e : enemies) {
            if (assignedEnemies.count(e)) continue;
            int dx = toroidalDist(self.first, e.first, boardWidth);
            int dy = toroidalDist(self.second, e.second, boardHeight);
            int dist = dx + dy;
            if (dist < minDist) {
                minDist = dist;
                target = e;
            }
        }
        if (target.first == -1) {
            target = findClosestEnemy(self, enemies);  
        }

        assignedEnemies.insert(target);
        tankAssignments[tankId] = target;
        info.setEnemy(target);
    }

    auto optTarget = info.getEnemy();
    auto [ex, ey] = *optTarget;

    int dx = toroidalDist(ex, self.first, boardWidth);
    int dy = toroidalDist(ey, self.second, boardHeight);

    if (dx <= 1 && dy <= 1) {
        info.setShootFlag(true);
    }

    tank.updateBattleInfo(info);
}

std::pair<int, int> MyPlayer1::findClosestEnemy(std::pair<int, int> from,
                                                const std::vector<std::pair<int, int>>& enemies) const {
    int minDist = 1e9;
    std::pair<int, int> closest = enemies.front();
    for (const auto& e : enemies) {
        int dx = toroidalDist(from.first, e.first, boardWidth);
        int dy = toroidalDist(from.second, e.second, boardHeight);
        int dist = dx + dy;
        if (dist < minDist) {
            minDist = dist;
            closest = e;
        }
    }
    return closest;
}

int MyPlayer1::toroidalDist(int a, int b, int max) {
    int direct = std::abs(a - b);
    return std::min(direct, max - direct);
}