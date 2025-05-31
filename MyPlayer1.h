#pragma once
#include "common/Player.h"

class MyPlayer1 : public Player {
public:
    MyPlayer1(int player_index, size_t x, size_t y, size_t max_steps, size_t num_shells)
        : Player(player_index, x, y, max_steps, num_shells) {}

    void updateTankWithBattleInfo(TankAlgorithm& tank, SatelliteView& satellite_view) override;
};