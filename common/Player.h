#ifndef PLAYER_H
#define PLAYER_H
#include <cstddef> // for size_t
#include "TankAlgorithm.h"
#include "SatelliteView.h"


class Player {
    public:
        Player( int, size_t, size_t, size_t, size_t ) {}
        virtual ~Player() {}
        virtual void updateTankWithBattleInfo(TankAlgorithm& tank, SatelliteView& satellite_view) = 0;
    };

#endif // PLAYER_H