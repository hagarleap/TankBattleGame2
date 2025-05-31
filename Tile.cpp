#include "Tile.h"

Tile::Tile(TileType type, const Position& pos)
    : type(type), position(pos) {}

TileType Tile::getType() const {
    return type;
}

void Tile::setType(TileType newType) {
    type = newType;
    if (newType == TileType::WALL) {wallHealth = 2;}
}

Position Tile::getPosition() const {
    return position;
}

void Tile::setPosition(const Position& pos) {
    position = pos;
}

bool Tile::isWall() const {
    return type == TileType::WALL;
}

bool Tile::isMine() const {
    return type == TileType::MINE;
}

bool Tile:: isShell() const{
    return type == TileType::SHELL;
}

bool Tile:: isTank1() const{
    return type == TileType::TANK1;
}

bool Tile:: isTank2() const{
    return type == TileType::TANK2;
}

bool Tile::isOccupied() const {
    return type == TileType::TANK1 || type == TileType::TANK2;
}


int Tile::getWallHealth() const {
    return wallHealth;
}


void Tile::hitWall() {
    if (isWall()) {
        wallHealth--;
        if (wallHealth <= 0) {
            type = TileType::EMPTY;
        }
    }
}
