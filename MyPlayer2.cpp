#include "MyPlayer2.h"
#include "MyBattleInfo.h"
#include "GameSatelliteView.h"
#include <iostream>

void MyPlayer2::updateTankWithBattleInfo(TankAlgorithm& tank, SatelliteView& satellite_view) {
    const GameSatelliteView* gview = dynamic_cast<const GameSatelliteView*>(&satellite_view);
    MyBattleInfo info(boardWidth, boardHeight, gview);
    info.setInitialShells(numShells);
    tank.updateBattleInfo(info);
}