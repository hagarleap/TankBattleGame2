#pragma once
#include "common/BattleInfo.h"
#include <vector>
#include <utility>

class MyBattleInfo : public BattleInfo {
    std::vector<std::string> boardSnapshot;
    size_t myX = 0, myY = 0;

public:
    MyBattleInfo(const std::vector<std::string>& snapshot, size_t x, size_t y)
        : boardSnapshot(snapshot), myX(x), myY(y) {}

    std::pair<size_t, size_t> getMyLocation() const { return {myX, myY}; }

    std::optional<std::pair<size_t, size_t>> findClosestEnemy(size_t x, size_t y) const {
        size_t bestDist = SIZE_MAX;
        std::optional<std::pair<size_t, size_t>> best;
        for (size_t i = 0; i < boardSnapshot.size(); ++i) {
            for (size_t j = 0; j < boardSnapshot[i].size(); ++j) {
                char c = boardSnapshot[i][j];
                if (c == '1' || c == '2') { // replace with logic to check enemy vs self
                    size_t dist = std::abs((int)x - (int)i) + std::abs((int)y - (int)j);
                    if (dist < bestDist) {
                        bestDist = dist;
                        best = {{i, j}};
                    }
                }
            }
        }
        return best;
    }
};
