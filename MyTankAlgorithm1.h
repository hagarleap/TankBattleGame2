#pragma once
#include "common/TankAlgorithm.h"
#include "Direction.h"
#include <queue>
#include <vector>
#include <optional>

class MyTankAlgorithm1 : public TankAlgorithm {
public:
    MyTankAlgorithm1();

    ActionRequest getAction() override;
    void updateBattleInfo(BattleInfo& info) override;

private:
    int stepCounter = 0;
    std::queue<ActionRequest> plannedActions;
    Direction dir = Direction::L; // Only keep direction, as all other info is available from BattleInfo

    // Helper to plan path to closest enemy
    void planPathToClosestEnemy(const BattleInfo& info);
};