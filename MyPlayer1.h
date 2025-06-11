#pragma once
#include "common/Player.h"
#include "Direction.h"
#include <vector>
#include <utility>
#include <unordered_map>
#include <unordered_set>

struct pair_hash {
    template <class T1, class T2>
    std::size_t operator()(const std::pair<T1, T2>& p) const {
        return std::hash<T1>()(p.first) ^ std::hash<T2>()(p.second << 1);
    }
};

class MyPlayer1 : public Player {
public:
    MyPlayer1(int player_index, size_t width, size_t height, size_t max_steps, size_t num_shells);

    void updateTankWithBattleInfo(TankAlgorithm& tank, SatelliteView& satellite_view) override;

protected:
    size_t boardWidth;
    size_t boardHeight;
    size_t maxSteps;
    size_t numShells;
    int playerIndex;

    std::unordered_map<int, std::pair<int, int>> tankPositions;
    std::unordered_set<std::pair<int, int>, pair_hash> assignedEnemies;
    std::unordered_set<std::pair<int, int>, pair_hash> prevEnemyLayout; // tracks last step enemies
    std::unordered_map<int, std::pair<int, int>> tankAssignments;


    std::pair<int, int> findClosestEnemy(std::pair<int, int> from,
                                         const std::vector<std::pair<int, int>>& enemies) const;

    static int toroidalDist(int a, int b, int max);
};
