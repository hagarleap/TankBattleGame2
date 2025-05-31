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
    int boardWidth = 0, boardHeight = 0;
    int myId = 0;
    int myX = 0, myY = 0;
    Direction Dir = Direction::L; // Initial direction (can be changed based on info)

    // Helper to plan path to closest enemy
    void planPathToClosestEnemy(const BattleInfo& info);
};