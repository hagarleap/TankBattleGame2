#include "PlayerFactory.h"
#include "MyPlayer1.h"
#include "MyPlayer2.h"
#include <memory>
#include <stdexcept>

std::unique_ptr<Player> PlayerFactory::create(int player_index,
                                              size_t x, size_t y,
                                              size_t max_steps,
                                              size_t num_shells) const {
    if (player_index == 1) {
        return std::make_unique<MyPlayer1>(player_index, x, y, max_steps, num_shells);
    } else if (player_index == 2) {
        return std::make_unique<MyPlayer2>(player_index, x, y, max_steps, num_shells);
    } else {
        throw std::invalid_argument("Invalid player_index in PlayerFactory::create");
    }
}