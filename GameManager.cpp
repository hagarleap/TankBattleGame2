#include "GameManager.h"
#include "TestAlgorithm.h"
#include <iostream>
#include <bits/algorithmfwd.h>
#include <algorithm>
#include <fstream>
#include <sstream>
#include <regex>

GameManager::GameManager(Board& board,
                        std::shared_ptr<StrategyManager> smP1,
                        std::shared_ptr<StrategyManager> smP2,
                        std::vector<Tank>& player1Tanks,
                        std::vector<Tank>& player2Tanks,
                        bool verbose)
    : board(board), strategyP1(smP1), strategyP2(smP2),  player1Tanks(player1Tanks), player2Tanks(player2Tanks), verbose(verbose) {}

void GameManager::run(int maxSteps) {
    while (!gameOver && stepCounter < maxSteps*2) {

        moveShells();
        checkCollisions();
        // Game moves by shell steps
        if (stepCounter%2 == 0){
            if (verbose) {
                std::cout << "Step " << stepCounter/2 << std::endl;
                printBoard();
                std::cout << "----------------------" << std::endl;
            }            
            tick();
            checkCollisions();
        }
        updateBoard();

        // Check win/tie conditions 
        bool anyAlive1 = std::any_of(player1Tanks.begin(), player1Tanks.end(), [](const Tank& t){ return t.isAlive(); });
        bool anyAlive2 = std::any_of(player2Tanks.begin(), player2Tanks.end(), [](const Tank& t){ return t.isAlive(); });
        
        if (!anyAlive1 && !anyAlive2) {
            gameOver = true;
            resultMessage = "Tie: Both tanks destroyed.";
        } else if (!anyAlive1) {
            gameOver = true;
            resultMessage = "Player 2 wins!";
        } else if (!anyAlive2) {
            gameOver = true;
            resultMessage = "Player 1 wins!";
        }
        ++stepCounter;
    }
    if (!gameOver){resultMessage = "Tie: Max steps reached.";}
}

void GameManager::tick() {
    for (auto& tank : player1Tanks) {
        if (!tank.isAlive()) continue;

        ActionRequest action = strategyP1->getAction(tank.getTankId(), tank, board, shells);
        bool success = true;

        
        if (tank.isWaitingForBackward()) {
            if (action == ActionRequest::MoveForward) {
                tank.cancelBackward(); 
            } else {

                success = false;
                recordAction(tank.getPlayerId(), tank.getTankId(), action, success);
                tank.stepBackwardTimer();
                tank.clearJustMovedBackwardFlag();
                continue;
            }
        }

        if (tank.isReadyToMoveBackward()) {
            Position backPos = board.wrapPosition(tank.getPosition().move(opposite(tank.getDirection()), 1));
            Tile& targetTile = board.getTile(backPos);

            if (targetTile.isWall()) {
                success = false;
            } else {

                tank.moveBackward(board);
            }

            recordAction(tank.getPlayerId(), tank.getTankId(), action, success);
            tank.tickCooldown();
            continue;
        }

        if (action == ActionRequest::MoveBackward) {
            
            tank.requestBackward();
            success = true;
            recordAction(tank.getPlayerId(), tank.getTankId(), action, success);
            tank.tickCooldown();
            continue;
        }

        handleActionRequest(tank, action);
        if (tank.hasJustMovedBackward() && action != ActionRequest::MoveBackward) {
            tank.resetBackwardState();
        }
    }

    for (auto& tank : player2Tanks) {
        if (!tank.isAlive()) continue;

        ActionRequest action = strategyP2->getAction(tank.getTankId(), tank, board, shells);
        bool success = true;

        if (tank.isWaitingForBackward()) {
            if (action == ActionRequest::MoveForward) {
                tank.cancelBackward();
            } else {
                success = false;
                recordAction(tank.getPlayerId(), tank.getTankId(), action, success);
                tank.stepBackwardTimer();
                tank.clearJustMovedBackwardFlag();
                continue;
            }
        }

        if (tank.isReadyToMoveBackward()) {
            Position backPos = board.wrapPosition(tank.getPosition().move(opposite(tank.getDirection()), 1));
            Tile& targetTile = board.getTile(backPos);

            if (targetTile.isWall()) {
                success = false;
            } else {
                tank.moveBackward(board);
            }

            recordAction(tank.getPlayerId(), tank.getTankId(), action, success);
            tank.tickCooldown();
            continue;
        }

        if (action == ActionRequest::MoveBackward) {
            tank.requestBackward();
            success = true;
            recordAction(tank.getPlayerId(), tank.getTankId(), action, success);
            tank.tickCooldown();
            continue;
        }

        handleActionRequest(tank, action);
        if (tank.hasJustMovedBackward() && action != ActionRequest::MoveBackward) {
            tank.resetBackwardState();
        }
    }
}


void GameManager::handleActionRequest(Tank& tank, ActionRequest& action) {
    if (!tank.isAlive()) return;

    bool success = true;
    switch (action) {
        case ActionRequest::Shoot:
            if (tank.canShoot()) {
                tank.onShoot();
                shells.emplace_back(board.wrapPosition(tank.getPosition().move(tank.getDirection())), tank.getDirection());
            } else {
                success = false;
            }
            break;

        case ActionRequest::MoveForward: {
            Position newPos = board.wrapPosition(tank.getPosition().move(tank.getDirection(), 1));
            Tile& targetTile = board.getTile(newPos);
            // Don't move if wall or tank ahead — mines/shells handled in checkCollisions
            if (targetTile.isWall()) {
                success = false;
            } else {
                tank.moveForward(board);  
            }
            break;
        }

        case ActionRequest::MoveBackward: {
            if (tank.isWaitingForBackward()) {
                // Already waiting, ignore
                success = false;
            } else if (tank.isReadyToMoveBackward()) {
                Position backPos = board.wrapPosition(tank.getPosition().move(opposite(tank.getDirection()), 1));
                Tile& targetTile = board.getTile(backPos);
                if (targetTile.isWall()) {
                    success = false;
                } else {
                    tank.moveBackward(board);
                }
                tank.resetBackwardState();
            } else {
                tank.requestBackward();
            }
            break;
        }
        case ActionRequest::RotateLeft45:
            tank.rotateLeft8();
            break;

        case ActionRequest::RotateRight45:
            tank.rotateRight8();
            break;

        case ActionRequest::RotateLeft90:
            tank.rotateLeft4();
            break;

        case ActionRequest::RotateRight90:
            tank.rotateRight4();
            break;

        case ActionRequest::DoNothing:
            break;
        default:
            success = false;
            break;
    }

    recordAction(tank.getPlayerId(), tank.getTankId(), action, success);
    tank.tickCooldown();
}

void GameManager::moveShells() {
    for (Shell& shell : shells) {
        
        shell.advance(board);
        
    }
}
// after a moveing part has moved (shell/tank) and no collisions - update the location on the borad
void GameManager::updateBoard() {
    // 1. Clear dynamic elements (TANK1, TANK2, SHELL) from the board
    for (int y = 0; y < board.getHeight(); ++y) {
        for (int x = 0; x < board.getWidth(); ++x) {
            Position pos(x, y);
            Tile& tile = board.getTile(pos);

            // Don't clear walls or mines
            if (tile.getType() == TileType::TANK1 ||
                tile.getType() == TileType::TANK2) {
                tile.setType(TileType::EMPTY);
            }
        }
    }

    // 2. Place alive tanks on the board
    for (const Tank& tank : player1Tanks) {
        if (tank.isAlive()) {
            board.getTile(tank.getPosition()).setType(TileType::TANK1);
        }
    }

    for (const Tank& tank : player2Tanks) {
        if (tank.isAlive()) {
            board.getTile(tank.getPosition()).setType(TileType::TANK2);
        }
    }

}

void GameManager::checkCollisions() {
    std::vector<size_t> shellsToRemove;

    // === 1. Shell collisions ===
    for (size_t i = 0; i < shells.size(); ++i) {
        Shell& shell1 = shells[i];
        Position pos1 = shell1.getPosition();
        Tile& currTile = board.getTile(pos1);
        // 1.1 Shell–shell
        for (size_t j = i + 1; j < shells.size(); ++j) {
            if (shells[j].getPosition() == pos1) {
                shellsToRemove.push_back(i);
                shellsToRemove.push_back(j);

                goto next_shell;
            }
        }

        // 1.2 Shell–wall
        if (currTile.isWall()) {
            currTile.hitWall();
            shellsToRemove.push_back(i);
            // NEW:
            if (strategyP1) strategyP1->notifyMapChangedAll();
            if (strategyP2) strategyP2->notifyMapChangedAll();
            continue;
        }

        // 1.3 Shell–tank1
        if (currTile.isTank1()) {
            for (Tank& tank : player1Tanks) {
                if (tank.isAlive() && tank.getPosition() == pos1) {
                    tank.destroy();
                    currTile.setType(TileType::EMPTY);
                    shellsToRemove.push_back(i);
                    goto next_shell;
                }
            }
        }

        // 1.4 Shell–tank2
        if (currTile.isTank2()) {
            for (Tank& tank : player2Tanks) {
                if (tank.isAlive() && tank.getPosition() == pos1) {
                    tank.destroy();
                    currTile.setType(TileType::EMPTY);
                    shellsToRemove.push_back(i);
                    goto next_shell;
                }
            }
        }

    next_shell:
        continue;
    }

    // === 2. Tank–object collisions ===
    auto checkTankCollision = [&](Tank& tank, std::vector<Tank>& enemyTanks) {
        if (!tank.isAlive()) return;

        Position pos = tank.getPosition();
        Tile& tile = board.getTile(pos);

        // 2.1 Tank on wall – revert move
        if (tile.isWall()) {
            tank.cancelMove();
            return;
        }

        // 2.2 Tank on mine – destroy
        if (tile.isMine()) {
            tank.destroy();
            tile.setType(TileType::EMPTY);
            return;
        }

        // 2.3 Tank on shell – destroy
        for (Shell& shell : shells) {
            if (shell.getPosition() == pos) {
                tank.destroy();
                tile.setType(TileType::EMPTY);
                break;
            }
        }

        // 2.4 Tank collides with enemy tank – destroy both
        for (Tank& enemy : enemyTanks) {
            if (enemy.isAlive() && enemy.getPosition() == pos) {
                tank.destroy();
                enemy.destroy();
                tile.setType(TileType::EMPTY);
                break;
            }
        }
    };

    for (Tank& t1 : player1Tanks) checkTankCollision(t1, player2Tanks);
    for (Tank& t2 : player2Tanks) checkTankCollision(t2, player1Tanks);

    // === 3. Remove dead shells ===
    std::sort(shellsToRemove.begin(), shellsToRemove.end());
    shellsToRemove.erase(std::unique(shellsToRemove.begin(), shellsToRemove.end()), shellsToRemove.end());

    for (auto it = shellsToRemove.rbegin(); it != shellsToRemove.rend(); ++it) {
        shells.erase(shells.begin() + *it);
    }
}

void GameManager::recordAction(int playerId, int tankId, ActionRequest action, bool success) {
    std::string logEntry = "P" + std::to_string(playerId) + "-T" + std::to_string(tankId) +
                           ": " + std::string(to_string(action)) +
                           (success ? "" : " (BAD STEP)");
    log.push_back(logEntry);
}

void GameManager::writeLog(const std::string& outputFile) const {
    std::ofstream out(outputFile);
    for (const auto& entry : log)
        out << entry << "\n";
    out << "Result: " << resultMessage << "\n";
}

std::string GameManager::getResultMessage() const {
    return resultMessage;
}

void GameManager::printBoard() const {
    auto boardLines = board.toString();

    for (const Shell& shell : shells) {
        Position p = shell.getPosition();
        if (p.y >= 0 && p.y < board.getHeight() && p.x >= 0 && p.x < board.getWidth()) {
            boardLines[p.y][p.x] = '*';
        }
    }

    for (const std::string& line : boardLines) {
        std::cout << line << "\n";
    }
}

bool GameManager::readBoard(const std::string& filename) {
    std::ifstream inputFile(filename);
    if (!inputFile) {
        std::cerr << "Error: Cannot open input file: " << filename << std::endl;
        return false;
    }

    std::string line;
    // 1. Map name/description (ignore)
    std::getline(inputFile, line);

    // 2. MaxSteps
    int maxSteps = 0, numShells = 0, rows = 0, cols = 0;
    std::regex reNum(R"(=\s*(\d+))");
    for (int i = 0; i < 4; ++i) {
        if (!std::getline(inputFile, line)) {
            std::cerr << "Error: Missing header line " << (i+2) << std::endl;
            return false;
        }
        std::smatch m;
        if (i == 0 && std::regex_search(line, m, reNum)) maxSteps = std::stoi(m[1]);
        if (i == 1 && std::regex_search(line, m, reNum)) numShells = std::stoi(m[1]);
        if (i == 2 && std::regex_search(line, m, reNum)) rows = std::stoi(m[1]);
        if (i == 3 && std::regex_search(line, m, reNum)) cols = std::stoi(m[1]);
    }
    if (rows <= 0 || cols <= 0) {
        std::cerr << "Error: Invalid board dimensions." << std::endl;
        return false;
    }
    // Resize board
    board = Board(cols, rows);
    player1Tanks.clear();
    player2Tanks.clear();

    int row = 0;
    while (std::getline(inputFile, line) && row < rows) {
        // Fill missing columns with spaces
        if ((int)line.size() < cols) line += std::string(cols - line.size(), ' ');
        for (int col = 0; col < cols; ++col) {
            char ch = line[col];
            Tile& tile = board.getTile(col, row);
            if (ch == '#') {
                board.placeWall(Position(col, row));
            } else if (ch == '@') {
                board.placeMine(Position(col, row));
            } else if (ch >= '0' && ch <= '9') {
                int playerId = (ch == '1' || ch == '0') ? 1 : 2;
                int tankIdx = (playerId == 1) ? player1Tanks.size() : player2Tanks.size();
                if (playerId == 1) {
                    player1Tanks.emplace_back(1, tankIdx, Position(col, row), Direction::L);
                    tile.setType(TileType::TANK1);
                } else {
                    player2Tanks.emplace_back(2, tankIdx, Position(col, row), Direction::R);
                    tile.setType(TileType::TANK2);
                }
            }
            // else: treat as empty
        }
        ++row;
    }
    // Fill missing rows with empty lines
    while (row < rows) {
        for (int col = 0; col < cols; ++col) {
            board.getTile(col, row).setType(TileType::EMPTY);
        }
        ++row;
    }
    return true;
}

