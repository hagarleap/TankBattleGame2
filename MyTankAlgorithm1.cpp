#include "MyTankAlgorithm1.h"
#include "common/ActionRequest.h"
#include "MyBattleInfo.h"
#include <queue>
#include <set>
#include <tuple>
#include <cmath>


MyTankAlgorithm1::MyTankAlgorithm1() : dir(Direction::L) {}

ActionRequest MyTankAlgorithm1::getAction() {
    stepCounter++;
    if (stepCounter % 5 == 0) {
        return ActionRequest::GetBattleInfo;
    }
    if (!plannedActions.empty()) {
        ActionRequest next = plannedActions.front();
        plannedActions.pop();
        // Update dir if rotating
        if (next == ActionRequest::RotateRight90) dir = rotateR4(dir);
        else if (next == ActionRequest::RotateLeft90) dir = rotateL4(dir);
        else if (next == ActionRequest::RotateRight45) dir = rotateR8(dir);
        else if (next == ActionRequest::RotateLeft45) dir = rotateL8(dir);
        return next;
    }
    return ActionRequest::DoNothing;
}

void MyTankAlgorithm1::updateBattleInfo(BattleInfo& info) {
    planPathToClosestEnemy(info);
}

void MyTankAlgorithm1::planPathToClosestEnemy(const BattleInfo& info) {
    plannedActions = std::queue<ActionRequest>();
    const MyBattleInfo* myInfo = dynamic_cast<const MyBattleInfo*>(&info);
    if (!myInfo) return;
    const SatelliteView* view = myInfo->getSatelliteView();
    if (!view) return;
    size_t width = myInfo->getBoardWidth();
    size_t height = myInfo->getBoardHeight();

    // Find my tank and enemy tanks
    int myX = -1, myY = -1;
    std::vector<std::pair<int, int>> enemies;
    for (size_t y = 0; y < height; ++y) {
        for (size_t x = 0; x < width; ++x) {
            char obj = view->getObjectAt(x, y);
            if (obj == '%') { myX = x; myY = y; }
            else if (obj == '2') { enemies.emplace_back(x, y); }
        }
    }
    if (myX == -1 || myY == -1 || enemies.empty()) return;

    // BFS to find closest enemy (all directions)
    struct Node { int x, y, dist; std::vector<ActionRequest> path; Direction dir; };
    std::queue<Node> q;
    std::set<std::tuple<int, int, Direction>> visited;
    q.push({myX, myY, 0, {}, dir});
    visited.insert({myX, myY, dir});
    int minDist = 1e9;
    std::vector<ActionRequest> bestPath;
    while (!q.empty()) {
        Node node = q.front(); q.pop();
        for (auto& e : enemies) {
            if (node.x == e.first && node.y == e.second) {
                if (node.dist < minDist) {
                    minDist = node.dist;
                    bestPath = node.path;
                }
            }
        }
        if (node.dist >= 20) continue;

        // Try moving forward in current direction
        int dx = 0, dy = 0;
        switch (node.dir) {
            case Direction::U:  dy = -1; break;
            case Direction::UR: dx = 1; dy = -1; break;
            case Direction::R:  dx = 1; break;
            case Direction::DR: dx = 1; dy = 1; break;
            case Direction::D:  dy = 1; break;
            case Direction::DL: dx = -1; dy = 1; break;
            case Direction::L:  dx = -1; break;
            case Direction::UL: dx = -1; dy = -1; break;
            default: break;
        }
        int nx = node.x + dx, ny = node.y + dy;
        if (nx >= 0 && nx < (int)width && ny >= 0 && ny < (int)height) {
            char obj = view->getObjectAt(nx, ny);
            if (obj != '#' && obj != '*') {
                if (!visited.count({nx, ny, node.dir})) {
                    auto newPath = node.path; newPath.push_back(ActionRequest::MoveForward);
                    q.push({nx, ny, node.dist+1, newPath, node.dir});
                    visited.insert({nx, ny, node.dir});
                }
            }
        }
        // Try all rotations (90 and 45 degrees)
        Direction dirs[4] = {
            rotateR4(node.dir), rotateL4(node.dir),
            rotateR8(node.dir), rotateL8(node.dir)
        };
        ActionRequest acts[4] = {
            ActionRequest::RotateRight90, ActionRequest::RotateLeft90,
            ActionRequest::RotateRight45, ActionRequest::RotateLeft45
        };
        for (int i = 0; i < 4; ++i) {
            if (!visited.count({node.x, node.y, dirs[i]})) {
                auto newPath = node.path; newPath.push_back(acts[i]);
                q.push({node.x, node.y, node.dist+1, newPath, dirs[i]});
                visited.insert({node.x, node.y, dirs[i]});
            }
        }
    }
    // Plan up to 4 actions to get closer
    for (size_t i = 0; i < bestPath.size() && i < 4; ++i) {
        plannedActions.push(bestPath[i]);
    }
}