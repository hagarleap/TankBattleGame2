#include "MyTankAlgorithm2.h"
#include "common/ActionRequest.h"
#include "common/BattleInfo.h"
#include "common/SatelliteView.h"
#include "MyBattleInfo.h"

#include <cmath>
#include <vector>
#include <algorithm>
#include <optional>
#include <iostream>

/* ---------- small helpers ------------------------------------------------ */


int rotationCost(Direction from, Direction to) {
    int a = static_cast<int>(from), b = static_cast<int>(to);
    int diff = (b - a + 8) % 8;

    if (diff == 0) return 0;           // already facing
    if (diff == 4) return 2;           // opposite direction
    return 1;                          // all others: one 45° or 90°
}


/* ------------------------------------------------------------------------- */

MyTankAlgorithm2::MyTankAlgorithm2() {}

ActionRequest MyTankAlgorithm2::getAction() {
    if (cooldownCounter > 0) --cooldownCounter;

    if (enemyInLine) {
        enemyInLine = false;
        cooldownCounter = 4;
        return ActionRequest::Shoot;
    }

    if (rotateAction) {
        ActionRequest act = *rotateAction;

        switch (act) {
            case ActionRequest::RotateRight90: dir = rotateR4(dir); break;
            case ActionRequest::RotateLeft90:  dir = rotateL4(dir); break;
            case ActionRequest::RotateRight45: dir = rotateR8(dir); break;
            case ActionRequest::RotateLeft45:  dir = rotateL8(dir); break;
            default: break;
        }

        rotateAction.reset();
        return act;
    }

    return ActionRequest::GetBattleInfo;
}

void MyTankAlgorithm2::updateBattleInfo(BattleInfo& info) {
    enemyInLine = false;
    rotateAction.reset();

    const auto* myInfo = dynamic_cast<const MyBattleInfo*>(&info);
    if (!myInfo) return;

    const SatelliteView* view = myInfo->getSatelliteView();
    if (!view) return;

    int myX = -1, myY = -1;
    std::vector<std::pair<int,int>> enemies;

    for (size_t y = 0; y < myInfo->getBoardHeight(); ++y)
        for (size_t x = 0; x < myInfo->getBoardWidth(); ++x) {
            char c = view->getObjectAt(x, y);
            if      (c == '%') { myX = (int)x; myY = (int)y; }
            else if (c == '1') { enemies.emplace_back((int)x, (int)y); }
        }

    if (myX == -1) return;

    struct Target {
        int x, y;
        int dist;         // wrapped distance (still useful as tie-breaker)
        Direction ray;
        int walls;       // number of objects in the way (not counting ' ')
    };
    std::vector<Target> viable;
    int W = (int)myInfo->getBoardWidth();
    int H = (int)myInfo->getBoardHeight();

    for (auto [ex, ey] : enemies) {
        for (Direction ray : {
            Direction::U, Direction::UR, Direction::R, Direction::DR,
            Direction::D, Direction::DL, Direction::L, Direction::UL }) {

            int tx = myX, ty = myY;
            int dx = ::dx(ray);
            int dy = ::dy(ray);
            const int maxSteps = W * H;
            int steps = 0;
            bool blocked = false;
            int wallCount = 0;
            bool reached = false;
            while (steps++ < maxSteps) {
                tx = wrap(tx + dx, W);
                ty = wrap(ty + dy, H);

                if (tx == ex && ty == ey) {
                    reached = true;
                    break;
                }

                char obj = view->getObjectAt(tx, ty);
                if (obj == '2') {
                    blocked = true;
                    break;
                }
                if (obj == '#') wallCount++;
            }

            if (!reached || blocked) {
                continue;
            }

            // now safe to add
            int dxDist = std::abs(ex - myX);
            int dyDist = std::abs(ey - myY);
            dxDist = std::min(dxDist, W - dxDist);
            dyDist = std::min(dyDist, H - dyDist);
            int distance = std::max(dxDist, dyDist);
            viable.push_back({ex, ey, distance, ray, wallCount});
        }
    }

    if (!viable.empty()) {
        const auto& tgt = *std::min_element(
            viable.begin(), viable.end(),
            [this](const Target& a, const Target& b) {
                if (a.walls != b.walls) return a.walls < b.walls;

                int ca = rotationCost(dir, a.ray);
                int cb = rotationCost(dir, b.ray);
                if (ca != cb) return ca < cb;

                return a.dist < b.dist;
            });

        if (dir == tgt.ray) {
            if (cooldownCounter == 0)
                enemyInLine = true;
        } else {
            // rotate one step toward tgt.ray manually
            int from = static_cast<int>(dir);
            int to   = static_cast<int>(tgt.ray);
            int diff = (to - from + 8) % 8;

            switch (diff) {
                case 1: rotateAction = ActionRequest::RotateRight45; break;
                case 2: rotateAction = ActionRequest::RotateRight90; break;
                case 3: rotateAction = ActionRequest::RotateLeft45;  break;
                case 4: rotateAction = ActionRequest::RotateLeft90;  break;
                case 5: rotateAction = ActionRequest::RotateRight45; break;
                case 6: rotateAction = ActionRequest::RotateLeft90;  break;
                case 7: rotateAction = ActionRequest::RotateLeft45;  break;
                default: break;  // already aligned (shouldn't happen)
            }
        }
        return;
    }

    if (!enemies.empty()) {
        auto it = std::min_element(enemies.begin(), enemies.end(),
            [myX,myY,W,H](auto a, auto b) {
                auto manhat = [myX,myY,W,H](auto p){
                    int dx = std::abs(p.first  - myX);
                    int dy = std::abs(p.second - myY);
                    dx = std::min(dx, W - dx);
                    dy = std::min(dy, H - dy);
                    return dx + dy;
                };
                return manhat(a) < manhat(b);
            });

        int dx = it->first  - myX;
        int dy = it->second - myY;
        dx = (std::abs(dx) <= W/2) ? dx : (dx > 0 ? dx - W : dx + W);
        dy = (std::abs(dy) <= H/2) ? dy : (dy > 0 ? dy - H : dy + H);

        Direction axisDir = dir;
        if (std::abs(dx) >= std::abs(dy))
            axisDir = (dx > 0 ? Direction::R : (dx < 0 ? Direction::L : axisDir));
        else
            axisDir = (dy > 0 ? Direction::D : Direction::U);

        ActionRequest rot = rotateToward(dir, axisDir);
        if (rot != ActionRequest::DoNothing) rotateAction = rot;
    }
}
