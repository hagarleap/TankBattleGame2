#pragma once
#include "common/TankAlgorithm.h"
#include "MyBattleInfo.h"
#include <memory>
#include <optional>

class MyTankAlgorithm : public TankAlgorithm {
    int round = 0;
    const int refreshRate = 4;

    std::unique_ptr<MyBattleInfo> cachedInfo;
    std::optional<std::pair<size_t, size_t>> currentTarget; // x, y

public:
    ActionRequest getAction() override;
    void updateBattleInfo(BattleInfo& info) override;
};
