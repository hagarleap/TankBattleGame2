#pragma once
#include "common/TankAlgorithm.h"
#include "Direction.h"
#include <optional>

class MyTankAlgorithm2 : public TankAlgorithm {
public:
    MyTankAlgorithm2();

    ActionRequest getAction() override;
    void updateBattleInfo(BattleInfo& info) override;

private:
    Direction dir = Direction::R; 
    bool enemyInLine = false;
    std::optional<ActionRequest> rotateAction;
};