#ifndef BOARD_H
#define BOARD_H

#include "Tile.h"
#include <vector>
#include <string>
#include "Position.h"

class Board {
public:
    Board(int width, int height);
    
    bool loadFromFile(const std::string& filename, std::vector<Position>& tankPositionsP1, std::vector<Position>& tankPositionsP2);
    void placeWall(const Position& pos);
    void placeMine(const Position& pos);
    void placeTank(const Position& pos, int playerId);

    Position wrapPosition(const Position& pos) const;
    int getWidth() const;
    int getHeight() const;
    
    Tile& getTile(int x, int y);
    Tile& getTile(Position pos);
    const Tile& getTile(int x, int y) const;
    const Tile& getTile(Position pos) const;

    void print() const;
    std::vector<std::string> toString() const;

private:
    int width;
    int height;
    std::vector<std::vector<Tile>> tiles;

    void initEmpty();
};

#endif // BOARD_H
