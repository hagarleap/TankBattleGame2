#pragma once

#include "common/BattleInfo.h"
#include <vector>
#include <string>

class MyBattleInfo : public BattleInfo {
    std::vector<std::string> snapshot;
    size_t tankX;
    size_t tankY;

public:
    MyBattleInfo(const std::vector<std::string>& snapshot, size_t x, size_t y);

    // Getters
    const std::vector<std::string>& getSnapshot() const;
    size_t getX() const;
    size_t getY() const;

};
