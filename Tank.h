#ifndef TANK_H
#define TANK_H

#include "Position.h"
#include "Direction.h"
#include "ActionRequest.h"
#include "Board.h"
#include <queue>
#include <string>

class Tank {
public:
    Tank(int playerId, int tankId, Position startPos, Direction startDir);

    int getPlayerId() const;
    int getTankId() const;

    Position getPosition() const;
    Direction getDirection() const;

    void setPosition(const Position& newPos);
    void setDirection(Direction newDir);

    bool canShoot() const;
    void onShoot();
    void moveForward(Board &board);
    void rotateRight4();
    void rotateRight8();
    void rotateLeft4();
    void rotateLeft8();

    void moveBackward(Board &board);
    void requestBackward();
    void cancelBackward();
    bool isWaitingForBackward() const;
    bool isReadyToMoveBackward();
    void stepBackwardTimer();
    void resetBackwardState();
    bool hasJustMovedBackward() const;
    void clearJustMovedBackwardFlag();

    void cancelMove();
    void tickCooldown();

    int getShellsLeft() const;

    bool isAlive() const;
    void destroy();


    std::string toString() const;

private:
int playerId;
int tankId;
Position position;
Position previousPosition; 
Direction direction;
int shellsLeft = 16;
int shootCooldown = 0;
bool destroyed = false;
int backwardStepCounter = -1;  // -1 means no backward requested
bool inBackwardMode = false;
bool justMovedBackward = false;
};

#endif // TANK_H
