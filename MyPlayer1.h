#pragma once
#include "common/Player.h"
#include "Direction.h"
#include <queue>


class MyPlayer1 : public Player {
public:
    MyPlayer1(int player_index, size_t width, size_t height, size_t max_steps, size_t num_shells)
        : Player(player_index, width, height, max_steps, num_shells),
          boardWidth(width), boardHeight(height),
          maxSteps(max_steps), numShells(num_shells), playerIndex(player_index) {}

    void updateTankWithBattleInfo(TankAlgorithm& tank, SatelliteView& satellite_view) override;

protected:
    size_t boardWidth;
    size_t boardHeight;
    size_t maxSteps;
    size_t numShells;
    int playerIndex;


};