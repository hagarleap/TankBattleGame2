#include "TankAlgorithmFactory.h"
#include "ScoutAlgorithm.h"
#include "ChasingAlgorithm.h"

std::unique_ptr<TankAlgorithm> TankAlgorithmFactory::create(int player_index, int tank_index) {
    switch (tank_index) {
        case 0:
            return std::make_unique<ScoutAlgorithm>();
        default:
            return std::make_unique<ChasingAlgorithm>();
    }
}
