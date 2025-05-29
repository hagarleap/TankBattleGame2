#include "Board.h"
#include "Tile.h"
#include "TileType.h"
#include <fstream>
#include <iostream>

Board::Board(int width, int height) : width(width), height(height) {
    initEmpty();
}

void Board::initEmpty() {
    tiles.resize(height, std::vector<Tile>(width, Tile(TileType::EMPTY, Position{0, 0})));
    for (int y = 0; y < height; ++y)
        for (int x = 0; x < width; ++x)
            tiles[y][x] = Tile(TileType::EMPTY, Position{x, y});
}

bool Board::loadFromFile(const std::string& filename, std::vector<Position>& tankP1, std::vector<Position>& tankP2) {
    std::ifstream inFile(filename);
    if (!inFile.is_open()) return false;

    std::string line;
    int row = 0;
    while (std::getline(inFile, line) && row < height) {
        for (int col = 0; col < std::min(width, (int)line.length()); ++col) {
            char c = line[col];
            Position pos{col, row};
            switch (c) {
                case '#': tiles[row][col] = Tile(TileType::WALL, pos); break;
                case '@': tiles[row][col] = Tile(TileType::MINE, pos); break;
                case '1':
                    tiles[row][col] = Tile(TileType::TANK1, pos);
                    tankP1.push_back(pos);
                    break;
                case '2':
                    tiles[row][col] = Tile(TileType::TANK2, pos);
                    tankP2.push_back(pos);
                    break;
                default:
                    tiles[row][col] = Tile(TileType::EMPTY, pos);
            }
        }
        ++row;
    }

    return true;
}

void Board::placeWall(const Position& pos) {
    tiles[pos.y][pos.x] = Tile(TileType::WALL, pos);
}

void Board::placeMine(const Position& pos) {
    tiles[pos.y][pos.x] = Tile(TileType::MINE, pos);
}

void Board::placeTank(const Position& pos, int playerId) {
    if (playerId == 1) {
        tiles[pos.y][pos.x] = Tile(TileType::TANK1, pos);
    } else if (playerId == 2) {
        tiles[pos.y][pos.x] = Tile(TileType::TANK2, pos);
    }
}


Position Board::wrapPosition(const Position& pos) const {
    int newX = (pos.x + width) % width;
    int newY = (pos.y + height) % height;
    return Position(newX, newY);
}

int Board::getWidth() const { return width; }
int Board::getHeight() const { return height; }

Tile& Board::getTile(Position pos){
    return tiles[pos.y][pos.x];
}

Tile& Board::getTile(int x, int y) {
    return tiles[y][x];
}

const Tile& Board::getTile(int x, int y) const {
    return tiles[y][x];
}

const Tile& Board::getTile(Position pos) const {
    return tiles[pos.y][pos.x];
}


void Board::print() const {
    // Top border
    std::cout << "+" << std::string(width, '-') << "+" << std::endl;

    for (const auto& row : tiles) {
        std::cout << "|";
        for (const auto& tile : row) {
            switch (tile.getType()) {
                case TileType::WALL: std::cout << "#"; break;
                case TileType::MINE: std::cout << "@"; break;
                case TileType::TANK1: std::cout << "1"; break;
                case TileType::TANK2: std::cout << "2"; break;
                default: std::cout << " ";
            }
        }
        std::cout << "|" << std::endl;
    }

    // Bottom border
    std::cout << "+" << std::string(width, '-') << "+" << std::endl;
}

std::vector<std::string> Board::toString() const {
    std::vector<std::string> lines;
    for (const auto& row : tiles) {
        std::string line;
        for (const auto& tile : row) {
            switch (tile.getType()) {
                case TileType::WALL: line += '#'; break;
                case TileType::MINE: line += '@'; break;
                case TileType::TANK1: line += '1'; break;
                case TileType::TANK2: line += '2'; break;
                default: line += ' ';
            }
        }
        lines.push_back(line);
    }
    return lines;
}
