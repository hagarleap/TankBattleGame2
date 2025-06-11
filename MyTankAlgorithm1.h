// MyTankAlgorithm1.h
#pragma once

#include "BaseTankAlgorithm.h"
#include "common/ActionRequest.h"
#include "common/SatelliteView.h"
#include "Direction.h"
#include <queue>

class MyTankAlgorithm1 : public BaseTankAlgorithm  {
public:
    MyTankAlgorithm1(int id);

    ActionRequest getAction() override;
    void updateBattleInfo(BattleInfo& info) override;
    int getId() const;

private:
    int tankId = 0; // Unique ID for this tank, if needed
    Direction dir = Direction::L;
    std::queue<ActionRequest> plannedActions;
    void aimAndShoot(int ex, int ey, int myX, int myY);
    
};
