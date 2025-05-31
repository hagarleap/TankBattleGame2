#pragma once
#include "TileType.h"
#include "Position.h"

class Tile {
public:
Tile(TileType type = TileType::EMPTY, const Position& pos = Position());

    TileType getType() const;
    void setType(TileType type);

    Position getPosition() const;
    void setPosition(const Position& pos);

    bool isWall() const;
    bool isMine() const;
    bool isShell() const;
    bool isTank1() const;
    bool isTank2() const;
    bool isOccupied() const;

    int getWallHealth() const;
    void hitWall();

private:
    TileType type;
    Position position;
    int wallHealth = 2; // default for walls
};
