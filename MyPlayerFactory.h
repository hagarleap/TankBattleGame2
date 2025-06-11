#pragma once
#include "common/PlayerFactory.h"
#include "MyPlayer1.h"
#include "MyPlayer2.h"
#include <memory>
#include <stdexcept>
#include <iostream>

class MyPlayerFactory : public PlayerFactory {
public:
    std::unique_ptr<Player> create(int player_index,
                                   size_t x, size_t y,
                                   size_t max_steps,
                                   size_t num_shells) const override {
        switch(player_index) {
            case 1:
                return std::make_unique<MyPlayer1>(player_index, x, y, max_steps, num_shells);
            case 2:
                return std::make_unique<MyPlayer2>(player_index, x, y, max_steps, num_shells);
            default:
                throw std::invalid_argument("Invalid player_index in PlayerFactory::create");
        }
    }
};
