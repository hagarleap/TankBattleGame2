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
