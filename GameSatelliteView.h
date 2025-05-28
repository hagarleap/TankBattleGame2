#pragma once
#include "common/SatelliteView.h"
#include "Board.h"
#include "Tank.h"
#include "Shell.h"
#include <vector>

// Concrete implementation of SatelliteView for the current game state
class GameSatelliteView : public SatelliteView {
    const Board& board;
    const std::vector<Tank>& player1Tanks;
    const std::vector<Tank>& player2Tanks;
    const std::vector<Shell>& shells;
    
public:
    GameSatelliteView(const Board& b, const std::vector<Tank>& p1, const std::vector<Tank>& p2, const std::vector<Shell>& s)
        : board(b), player1Tanks(p1), player2Tanks(p2), shells(s) {}

    char getObjectAt(size_t x, size_t y) const override {
        // Out of bounds
        if (x >= board.getWidth() || y >= board.getHeight())
            return '&';
        
        // Shells take precedence (if a shell is above a mine, show '*')
        for (const Shell& shell : shells) {
            if (shell.getPosition().x == x && shell.getPosition().y == y)
                return '*';
        }
        // Player 1 tanks
        for (const Tank& tank : player1Tanks) {
            if (tank.isAlive() && tank.getPosition().x == x && tank.getPosition().y == y)
                return '1'; // Or tank.getId() if you want to show the digit
        }
        // Player 2 tanks
        for (const Tank& tank : player2Tanks) {
            if (tank.isAlive() && tank.getPosition().x == x && tank.getPosition().y == y)
                return '2'; // Or tank.getId() if you want to show the digit
        }
        // Board tiles
        const Tile& tile = board.getTile(Position(x, y));
        if (tile.isWall()) return '#';
        if (tile.isMine()) return '@';
        // Empty space
        return ' ';
    }
};
