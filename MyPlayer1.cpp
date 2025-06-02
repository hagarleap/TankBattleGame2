#include "MyPlayer1.h"
#include "MyBattleInfo.h"
#include <iostream>
void MyPlayer1::updateTankWithBattleInfo(TankAlgorithm& tank, SatelliteView& satellite_view) {
    std::cout << boardWidth << boardHeight << maxSteps << numShells << playerIndex;
    MyBattleInfo info(boardWidth, boardHeight, &satellite_view);
    tank.updateBattleInfo(info);
}