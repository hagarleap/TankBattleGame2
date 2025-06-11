#include "MyPlayer1.h"
#include "MyBattleInfo.h"
#include "GameSatelliteView.h"
#include <cmath>
#include <iostream>

MyPlayer1::MyPlayer1(int player_index, size_t width, size_t height, size_t max_steps, size_t num_shells)
    : Player(player_index, width, height, max_steps, num_shells),
      boardWidth(width), boardHeight(height),
      maxSteps(max_steps), numShells(num_shells), playerIndex(player_index) {}

void MyPlayer1::updateTankWithBattleInfo(TankAlgorithm& tank, SatelliteView& satellite_view) {
    const GameSatelliteView* gview = dynamic_cast<const GameSatelliteView*>(&satellite_view);
    MyBattleInfo info(boardWidth, boardHeight, gview);

    std::pair<int, int> self{-1, -1};
    std::vector<std::pair<int, int>> enemies;

    std::cout << "boardWidth: " << boardWidth << ", boardHeight: " << boardHeight << "\n";
    for (size_t y = 0; y < boardHeight; ++y) {
        for (size_t x = 0; x < boardWidth; ++x) {
            char c = gview->getObjectAt(x, y);
            if (c == '%'){
                self = {static_cast<int>(x), static_cast<int>(y)};
                info.setSelf(self);
            } 
            else if (c == '2'){
                enemies.emplace_back(static_cast<int>(x), static_cast<int>(y));
            } 
        }
    }

    if (self.first == -1 || enemies.empty()) {
        tank.updateBattleInfo(info);
        return;
    }

    auto target = findClosestEnemy(self, enemies);
    info.setEnemy(target);

    int dx = toroidalDist(target.first, self.first, boardWidth);
    int dy = toroidalDist(target.second, self.second, boardHeight);
    if (dx <= 2 && dy <= 2){
        std::cout << "Target is close enough (dx=" << dx << ", dy=" << dy << "), setting shoot flag\n";
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
        int dist = dx + dy;  // Manhattan distance with wraparound
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

