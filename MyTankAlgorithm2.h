#pragma once
#include "BaseTankAlgorithm.h"
#include "Direction.h"
#include <iostream>

class MyTankAlgorithm2 : public BaseTankAlgorithm {
public:
    MyTankAlgorithm2();

    ActionRequest getAction() override;
    void updateBattleInfo(BattleInfo& info) override;

private:
    Direction dir = Direction::R; 
    bool enemyInLine = false;
    std::optional<ActionRequest> rotateAction;
};