// MyTankAlgorithm1.cpp
#include "MyTankAlgorithm1.h"
#include "MyBattleInfo.h"
#include <cmath>
#include <queue>
#include <set>
#include <iostream>
#include <tuple>

MyTankAlgorithm1::MyTankAlgorithm1(int id) : tankId(id), boardWidth(0), boardHeight(0) {}

int MyTankAlgorithm1::getId() const {
    return tankId;
}

ActionRequest MyTankAlgorithm1::getAction() {
    if (cooldownCounter > 0)
        cooldownCounter--;

    if (!plannedActions.empty()) {
        ActionRequest next = plannedActions.front();

        if (next == ActionRequest::MoveForward && latestEnemyPos.has_value() && latestSelfPos.has_value()) {
            int dx = ::dx(dir);
            int dy = ::dy(dir);
            int nx = wrap(latestSelfPos->first + dx, boardWidth);
            int ny = wrap(latestSelfPos->second + dy, boardHeight);
            if (nx == latestEnemyPos->first && ny == latestEnemyPos->second) {
                while (!plannedActions.empty()) plannedActions.pop();
                return ActionRequest::GetBattleInfo;
            }
            // Update our internal position
            latestSelfPos = {nx, ny};
        }

        plannedActions.pop();

        if (next == ActionRequest::Shoot) {
            if (shells && *shells > 0) {
                --(*shells);
                cooldownCounter = 4;
            } else next = ActionRequest::DoNothing;
        }

        if (next == ActionRequest::RotateRight90) dir = rotateR4(dir);
        else if (next == ActionRequest::RotateLeft90) dir = rotateL4(dir);
        else if (next == ActionRequest::RotateRight45) dir = rotateR8(dir);
        else if (next == ActionRequest::RotateLeft45) dir = rotateL8(dir);

        return next;
    }
    return ActionRequest::GetBattleInfo;
}

void MyTankAlgorithm1::updateBattleInfo(BattleInfo& info) {
    initShellsFromInfo(info);
    const MyBattleInfo* myInfo = dynamic_cast<const MyBattleInfo*>(&info);
    if (!myInfo) return;

    plannedActions = std::queue<ActionRequest>();

    auto enemyOpt = myInfo->getEnemy();
    if (!enemyOpt.has_value()) return;
    auto [ex, ey] = enemyOpt.value();
    latestEnemyPos = {ex, ey};

    auto selfOpt = myInfo->getSelf();
    if (!selfOpt.has_value()) return;
    auto [myX, myY] = selfOpt.value();
    latestSelfPos = {myX, myY};

    const SatelliteView* view = myInfo->getSatelliteView();
    boardWidth = myInfo->getBoardWidth();
    boardHeight = myInfo->getBoardHeight();

    if (myInfo->shouldShoot()) {
        aimAndShoot(ex, ey, myX, myY);
        return;
    }

    struct Node {
        int x, y, dist;
        std::vector<ActionRequest> path;
        Direction dir;
    };
    std::queue<Node> q;
    std::set<std::tuple<int, int, int>> visited;
    q.push({myX, myY, 0, {}, dir});
    visited.insert(std::make_tuple(myX, myY, static_cast<int>(dir)));

    std::vector<ActionRequest> bestPath;
    bool pathFound = false;

    while (!q.empty()) {
        Node node = q.front(); q.pop();
        if (node.x == ex && node.y == ey) {
            bestPath = node.path;
            pathFound = true;
            break;
        }
        if (node.dist >= 20) continue;

        int dx = ::dx(node.dir);
        int dy = ::dy(node.dir);
        int nx = wrap(node.x + dx, (int)boardWidth);
        int ny = wrap(node.y + dy, (int)boardHeight);
        char obj = view->getObjectAt(nx, ny);

        // Allow moving into the enemy's cell only
        bool isTarget = (nx == ex && ny == ey);
        bool isBlocked = (obj == '#' || obj == '@' || obj == '%');
        bool isEnemyButNotTarget = (obj == '2' && !isTarget);

        if (!isBlocked && !isEnemyButNotTarget) {
            if (!visited.count(std::make_tuple(nx, ny, static_cast<int>(node.dir)))) {
                auto newPath = node.path; newPath.push_back(ActionRequest::MoveForward);
                q.push({nx, ny, node.dist + 1, newPath, node.dir});
                visited.insert(std::make_tuple(nx, ny, static_cast<int>(node.dir)));
            }
        }

        Direction dirs[4] = {
            rotateR4(node.dir), rotateL4(node.dir),
            rotateR8(node.dir), rotateL8(node.dir)
        };
        ActionRequest acts[4] = {
            ActionRequest::RotateRight90, ActionRequest::RotateLeft90,
            ActionRequest::RotateRight45, ActionRequest::RotateLeft45
        };
        for (int i = 0; i < 4; ++i) {
            if (!visited.count(std::make_tuple(node.x, node.y, static_cast<int>(dirs[i])))) {
                auto newPath = node.path; newPath.push_back(acts[i]);
                q.push({node.x, node.y, node.dist + 1, newPath, dirs[i]});
                visited.insert(std::make_tuple(node.x, node.y, static_cast<int>(dirs[i])));
            }
        }
    }

    if (!pathFound) {
        aimAndShoot(ex, ey, myX, myY);
        return;
    }

    for (size_t i = 0; i < bestPath.size() && i < 3; ++i) {
        plannedActions.push(bestPath[i]);
    }
}

void MyTankAlgorithm1::aimAndShoot(int ex, int ey, int myX, int myY) {
    Direction desiredDir = computeDirection(ex - myX, ey - myY);
    Direction current = dir;
    int futureCooldown = cooldownCounter;

    for (int i = 0; i < 3; ++i) {
        ActionRequest rot = rotateToward(current, desiredDir);

        if (rot == ActionRequest::DoNothing) {
            if (futureCooldown == 0) {
                plannedActions.push(ActionRequest::Shoot);
            } else {
                plannedActions.push(ActionRequest::DoNothing);
            }
        } else {
            plannedActions.push(rot);
            switch (rot) {
                case ActionRequest::RotateRight45: current = rotateR8(current); break;
                case ActionRequest::RotateLeft45:  current = rotateL8(current); break;
                case ActionRequest::RotateRight90: current = rotateR4(current); break;
                case ActionRequest::RotateLeft90:  current = rotateL4(current); break;
                default: break;
            }
        }

        futureCooldown = std::max(0, futureCooldown - 1);
    }

    std::queue<ActionRequest> fixed;
    std::vector<ActionRequest> temp;

    while (!plannedActions.empty()) {
        temp.push_back(plannedActions.front());
        plannedActions.pop();
    }

    int simCooldown = cooldownCounter;
    for (size_t i = 0; i < temp.size(); ++i) {
        ActionRequest act = temp[i];
        if (act == ActionRequest::Shoot && simCooldown > 0) {
            fixed.push(ActionRequest::DoNothing);
        } else {
            fixed.push(act);
        }

        if (act != ActionRequest::GetBattleInfo)
            simCooldown = std::max(0, simCooldown - 1);

        if (act == ActionRequest::Shoot)
            simCooldown = 4;
    }

    plannedActions = fixed;
}
