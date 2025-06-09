#include "MyPlayer2.h"
#include "MyBattleInfo.h"
#include "GameSatelliteView.h"
#include <iostream>

void MyPlayer2::updateTankWithBattleInfo(TankAlgorithm& tank, SatelliteView& satellite_view) {
    std::cout << boardWidth << boardHeight << maxSteps << numShells << playerIndex << "\n";
    const GameSatelliteView* gview = dynamic_cast<const GameSatelliteView*>(&satellite_view);
    MyBattleInfo info(boardWidth, boardHeight, gview);
    tank.updateBattleInfo(info);
}