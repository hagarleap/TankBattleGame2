#pragma once

#include "common/BattleInfo.h"
#include "common/SatelliteView.h"
#include "common/ActionRequest.h"
#include <queue>
#include <optional>
#include <utility>

class MyBattleInfo : public BattleInfo {
    size_t boardWidth;
    size_t boardHeight;
    const SatelliteView* satelliteView;
    std::optional<std::pair<int, int>> enemyTarget;
    std::optional<std::pair<int, int>> selfPosition; // Self position
    bool shootHint = false;

public:
    MyBattleInfo(size_t width, size_t height, const SatelliteView* view);

    size_t getBoardWidth() const;
    size_t getBoardHeight() const;
    const SatelliteView* getSatelliteView() const;

    void setSelf(std::pair<int, int> pos);
    std::optional<std::pair<int, int>> getSelf() const;

    void setEnemy(std::pair<int, int> pos);
    std::optional<std::pair<int, int>> getEnemy() const;

    void setShootFlag(bool val);
    bool shouldShoot() const;
};