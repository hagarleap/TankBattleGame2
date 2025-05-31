#pragma once
#include "common/BattleInfo.h"
#include "common/SatelliteView.h"

class MyBattleInfo : public BattleInfo {
    size_t boardWidth;
    size_t boardHeight;
    const SatelliteView* satelliteView;
public:
    MyBattleInfo(size_t width, size_t height, const SatelliteView* view);
    size_t getBoardWidth() const;
    size_t getBoardHeight() const;
    const SatelliteView* getSatelliteView() const;
};
