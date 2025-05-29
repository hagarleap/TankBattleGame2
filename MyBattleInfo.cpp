#include "MyBattleInfo.h"

MyBattleInfo::MyBattleInfo(const std::vector<std::string>& snapshot, size_t x, size_t y)
    : snapshot(snapshot), tankX(x), tankY(y) {}

const std::vector<std::string>& MyBattleInfo::getSnapshot() const {
    return snapshot;
}

size_t MyBattleInfo::getX() const {
    return tankX;
}

size_t MyBattleInfo::getY() const {
    return tankY;
}
