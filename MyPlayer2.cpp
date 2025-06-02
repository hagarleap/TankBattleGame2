#include "MyPlayer2.h"
#include "MyBattleInfo.h"
#include <iostream>

void MyPlayer2::updateTankWithBattleInfo(TankAlgorithm& tank, SatelliteView& satellite_view) {
    std::cout << boardWidth << boardHeight << maxSteps << numShells << playerIndex;
    MyBattleInfo info(boardWidth, boardHeight, &satellite_view);
    tank.updateBattleInfo(info);
}