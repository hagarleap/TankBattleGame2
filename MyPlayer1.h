#pragma once

#include "common/Player.h"
#include "common/TankAlgorithm.h"
#include "common/SatelliteView.h"
#include "MyBattleInfo.h"
#include <memory>
#include <vector>
#include <string>
#include <unordered_map>

class MyPlayer1 : public Player {
    int playerId;
    size_t boardRows;
    size_t boardCols;
    size_t maxSteps;
    size_t numShells;

    size_t scoutIndex = 0;         // First tank created is the scout
    size_t turnCounter = 0;
    size_t totalTanks = 0;
    size_t aliveTanks = 0;

    std::vector<std::string> lastSnapshot;
    size_t lastScoutX = 0, lastScoutY = 0;

public:
    MyPlayer1(int player_index, size_t x, size_t y, size_t max_steps, size_t num_shells);
    ~MyPlayer1() noexcept override = default;

    void setTotalTanks(size_t n);              // set total tanks after creation
    void notifyTankKilled(size_t tankIndex);   // pass tank index, not pointer

    void updateTankWithBattleInfo(TankAlgorithm& tankAlg, SatelliteView& satelliteView) override;

private:
    std::unordered_map<TankAlgorithm*, size_t> tankIndices;
    size_t nextTankIndex = 0;
};
