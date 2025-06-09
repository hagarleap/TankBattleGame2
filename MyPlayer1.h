#pragma once
#include "common/Player.h"
#include "Direction.h"
#include <vector>
#include <utility>

class MyPlayer1 : public Player {
public:
    MyPlayer1(int player_index, size_t width, size_t height, size_t max_steps, size_t num_shells);

    void updateTankWithBattleInfo(TankAlgorithm& tank, SatelliteView& satellite_view) override;

protected:
    size_t boardWidth;
    size_t boardHeight;
    size_t maxSteps;
    size_t numShells;
    int playerIndex;

    std::pair<int, int> findClosestEnemy(std::pair<int, int> from,
                                         const std::vector<std::pair<int, int>>& enemies) const;
};
