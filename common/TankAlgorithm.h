#ifndef TANK_ALGORITHM_H
#define TANK_ALGORITHM_H
#include <string>
#include "BattleInfo.h"
#include "ActionRequest.h"

class TankAlgorithm {
    public:
        virtual ~TankAlgorithm() {}
        virtual ActionRequest getAction() = 0;
        virtual void updateBattleInfo(BattleInfo& info) = 0;
    };

#endif // TANK_ALGORITHM_H