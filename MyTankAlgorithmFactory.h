#include "common/TankAlgorithmFactory.h"
#include "MyTankAlgorithm1.h"
#include "MyTankAlgorithm2.h"

class MyTankAlgorithmFactory : public TankAlgorithmFactory{

public:

std::unique_ptr<TankAlgorithm> create(int player_index, int tank_index) const {
    switch (player_index) {
        case 1:
            return std::make_unique<MyTankAlgorithm1>();
        case 2:
            return std::make_unique<MyTankAlgorithm1>();
        default:
            throw std::invalid_argument("Invalid player index");
    }
}


};




