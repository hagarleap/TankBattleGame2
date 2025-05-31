#include "common/TankAlgorithmFactory.h"
#include "MyTankAlgorithm1.h"
#include "MyTankAlgorithm2.h"

std::unique_ptr<TankAlgorithm> TankAlgorithmFactory::create(int player_index, int tank_index) const {
    switch (player_index) {
        case 1:
            return std::make_unique<MyTankAlgorithm1>();
        case 2:
            return std::make_unique<MyTankAlgorithm1>();
        default:
            throw std::invalid_argument("Invalid player index");
    }
}
