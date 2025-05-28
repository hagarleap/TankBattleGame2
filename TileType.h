#ifndef TILETYPE_H
#define TILETYPE_H

enum class TileType {
    EMPTY,  // Default walkable tile
    WALL,   // Needs 2 hits to be destroyed
    MINE,   // Destroys tank on contact
    TANK1,   // Tile occupied by a tank (can be handled dynamically)
    TANK2,
    SHELL,
    // Spawn1, // Initial position of player 1's tank
    // Spawn2  // Initial position of player 2's tank
};

#endif // TILETYPE_H