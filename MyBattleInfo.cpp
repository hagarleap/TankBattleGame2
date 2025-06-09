#include "MyBattleInfo.h"

MyBattleInfo::MyBattleInfo(size_t width, size_t height, const SatelliteView* view)
    : boardWidth(width), boardHeight(height), satelliteView(view) {}

size_t MyBattleInfo::getBoardWidth() const {
    return boardWidth;
}

size_t MyBattleInfo::getBoardHeight() const {
    return boardHeight;
}

const SatelliteView* MyBattleInfo::getSatelliteView() const {
    return satelliteView;
}
void MyBattleInfo::setSelf(std::pair<int, int> pos) {
    selfPosition = pos;
}

std::optional<std::pair<int, int>> MyBattleInfo::getSelf() const {
    return selfPosition;
}

void MyBattleInfo::setEnemy(std::pair<int, int> pos) {
    enemyTarget = pos;
}

std::optional<std::pair<int, int>> MyBattleInfo::getEnemy() const {
    return enemyTarget;
}

void MyBattleInfo::setShootFlag(bool val) {
    shootHint = val;
}

bool MyBattleInfo::shouldShoot() const {
    return shootHint;
}