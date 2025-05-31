#include "MyTankAlgorithm1.h"
#include "common/ActionRequest.h"
#include "common/BattleInfo.h"
#include "common/SatelliteView.h"

MyTankAlgorithm1::MyTankAlgorithm1() {}

ActionRequest MyTankAlgorithm1::getAction() {
    stepCounter++;
    if (stepCounter % 5 == 0) {
        return ActionRequest::GetBattleInfo;
    }
    if (!plannedActions.empty()) {
        ActionRequest next = plannedActions.front();
        plannedActions.pop();
        return next;
    }
    return ActionRequest::DoNothing;
}

void MyTankAlgorithm1::updateBattleInfo(BattleInfo& info) {
    // Use info.getSatelliteView() to access the board and tank positions
    // Use BFS to find closest enemy, plan up to 4 actions, and store in plannedActions
    planPathToClosestEnemy(info);
}

void MyTankAlgorithm1::planPathToClosestEnemy(const BattleInfo& info) {
    // Implement BFS here, fill plannedActions with up to 4 ActionRequests
    // Take into account current cannon direction and position
    // (You can refer to ChasingAlgorithm in TankBattleGame for logic)
}