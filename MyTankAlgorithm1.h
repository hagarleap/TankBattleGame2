// MyTankAlgorithm1.h
#pragma once

#include "BaseTankAlgorithm.h"
#include "common/ActionRequest.h"
#include "common/SatelliteView.h"
#include "Direction.h"
#include <queue>

class MyTankAlgorithm1 : public BaseTankAlgorithm  {
public:
    MyTankAlgorithm1();

    ActionRequest getAction() override;
    void updateBattleInfo(BattleInfo& info) override;

private:
    Direction dir = Direction::L;
    std::queue<ActionRequest> plannedActions;
    void aimAndShoot(int ex, int ey, int myX, int myY);
};
