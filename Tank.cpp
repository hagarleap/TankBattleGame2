#include "Tank.h"
#include <sstream>
#include <iostream>

Tank::Tank(int playerId, int tankId, Position startPos, Direction startDir, int shells)
    : playerId(playerId), tankId(tankId), position(startPos), previousPosition(startPos),
      direction(startDir), shellsLeft(shells), shootCooldown(0), destroyed(false),
      backwardStepCounter(-1), inBackwardMode(false), justMovedBackward(false) {}

int Tank::getPlayerId() const {
    return playerId;
}

int Tank::getTankId() const {
    return tankId;
}

Position Tank::getPosition() const {
    return position;
}

Direction Tank::getDirection() const {
    return direction;
}

void Tank::moveForward(Board &board) {
    previousPosition = position;
    position = board.wrapPosition(position.move(direction, 1)); // move 1 step in current direction
}

void Tank::moveBackward(Board &board) {
    previousPosition = position;
    position = board.wrapPosition(position.move(opposite(direction), 1)); // move 1 step backward
    justMovedBackward = true;
}

void Tank::requestBackward() {
    if (justMovedBackward){
        backwardStepCounter = 2;
    }
    else if (backwardStepCounter == -1) {
        backwardStepCounter = 0; // start wait timer
        inBackwardMode = true;
    }
}

void Tank::stepBackwardTimer() {
    if (backwardStepCounter >= 0 && backwardStepCounter < 2) {
        ++backwardStepCounter;
    }
}

bool Tank::isWaitingForBackward() const {
    return backwardStepCounter >= 0 && backwardStepCounter < 2;
}

bool Tank::isReadyToMoveBackward() {
    return backwardStepCounter >= 2;
}

void Tank::cancelBackward() {
    backwardStepCounter = -1;
    inBackwardMode = false;
}

void Tank::resetBackwardState() {
    backwardStepCounter = -1;
    inBackwardMode = false;
    justMovedBackward = false;
}

bool Tank::hasJustMovedBackward() const {
    return justMovedBackward;
}

void Tank::clearJustMovedBackwardFlag() {
    justMovedBackward = false;
}


void Tank::rotateRight8() {
    setDirection(rotateR8(direction)); // turn 1/8 right
}

void Tank::rotateLeft8() {
    setDirection(rotateL8(direction)); // turn 1/8 left
}

void Tank::rotateRight4() {
    setDirection(rotateR4(direction)); // turn 1/4 right (2 steps)
}

void Tank::rotateLeft4() {
    setDirection(rotateL4(direction)); // turn 1/4 left (2 steps)
}

void Tank::cancelMove() {
    position = previousPosition;
}

void Tank::setPosition(const Position &newPos){
    previousPosition = position;
    position = newPos;
}

void Tank::setDirection(Direction newDir){
    direction = newDir;
}

bool Tank::canShoot() const {
    return shootCooldown == 0 && shellsLeft > 0;
}

void Tank::onShoot() {
    if (canShoot()) {
        shootCooldown = 4;
        --shellsLeft;
    }
}

void Tank::tickCooldown() {
    if (shootCooldown > 0) --shootCooldown;
}

int Tank::getShellsLeft() const {
    return shellsLeft;
}

bool Tank::isAlive() const {
    return !destroyed;
}

void Tank::destroy() {
    destroyed = true;
}


std::string Tank::toString() const {
    std::ostringstream oss;
    oss << "Tank(P" << playerId << ", ID " << tankId << ") at (" << position.x << "," << position.y << ")";
    return oss.str();
}
