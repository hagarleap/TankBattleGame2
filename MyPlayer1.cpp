#include "MyPlayer1.h"

MyPlayer::MyPlayer(int player_index, size_t x, size_t y, size_t max_steps, size_t num_shells)
    : Player(player_index, x, y, max_steps, num_shells),  // <--- this is the fix
      playerId(player_index),
      boardRows(x), boardCols(y),
      maxSteps(max_steps), numShells(num_shells) {}

void MyPlayer::setTotalTanks(size_t n) {
    totalTanks = n;
    aliveTanks = n;
}

void MyPlayer::notifyTankKilled(size_t tankIndex) {
    if (tankIndex == scoutIndex) {
        // Assign new scout to lowest surviving index (simplified)
        scoutIndex = (aliveTanks > 1) ? (scoutIndex + 1) % totalTanks : 0;
    }
    --aliveTanks;
}

void MyPlayer::updateTankWithBattleInfo(TankAlgorithm& tankAlg, SatelliteView& satelliteView) {
    // First time seeing this tank?
    if (tankIndices.count(&tankAlg) == 0) {
        tankIndices[&tankAlg] = nextTankIndex++;
    }

    size_t tankIndex = tankIndices[&tankAlg];

    // Build snapshot
    std::vector<std::string> snapshot(boardRows, std::string(boardCols, ' '));
    for (size_t i = 0; i < boardRows; ++i)
        for (size_t j = 0; j < boardCols; ++j)
            snapshot[i][j] = satelliteView.getObjectAt(i, j);

    // Get tank position
    size_t x = 0, y = 0;
    for (size_t i = 0; i < boardRows; ++i) {
        for (size_t j = 0; j < boardCols; ++j) {
            if (snapshot[i][j] == '%') {
                x = i;
                y = j;
                break;
            }
        }
    }

    bool shouldActAsScout = false;

    if (aliveTanks == 1) {
        shouldActAsScout = (turnCounter % 5 == 0);
        if (tankIndex == scoutIndex) ++turnCounter;
    } else {
        shouldActAsScout = (tankIndex == scoutIndex);
    }

    if (shouldActAsScout) {
        lastSnapshot = snapshot;
        lastScoutX = x;
        lastScoutY = y;
    }

    MyBattleInfo info(lastSnapshot, lastScoutX, lastScoutY);
    tankAlg.updateBattleInfo(info);
}
