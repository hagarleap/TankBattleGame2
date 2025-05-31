#ifndef TANK_ALGORITHM_FACTORY_H
#define TANK_ALGORITHM_FACTORY_H
#include <memory>
#include "TankAlgorithm.h"


class TankAlgorithmFactory {
    public:
        virtual ~TankAlgorithmFactory() {}
        virtual std::unique_ptr<TankAlgorithm> create(int player_index, int tank_index) const = 0;
    };

#endif // TANK_ALGORITHM_FACTORY_H