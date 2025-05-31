#include "GameManager.h"
#include "GameSatelliteView.h"
#include <iostream>
#include <algorithm>
#include <fstream>
#include <sstream>
#include <regex>
#include <memory>

GameManager::GameManager(Board& board,
    const PlayerFactory& playerFactory1,
    const PlayerFactory& playerFactory2,
    const TankAlgorithmFactory& algoFactory1,
    const TankAlgorithmFactory& algoFactory2,
    bool verbose)
    : board(board), verbose(verbose) {
    // Tanks will be initialized in readBoard
    // Players and algorithms will be created after tanks are known
}

// Helper struct to track tank status for output
struct TankOutputStatus {
    bool alive = true;
    bool killedThisStep = false;
    bool ignored = false;
    std::string lastAction;
};

void GameManager::run(int maxSteps) {
    // Prepare output status for all tanks in board order
    std::vector<TankOutputStatus> tankStatus;
    for (const Tank& t : player1Tanks) tankStatus.push_back({true, false, false, ""});
    for (const Tank& t : player2Tanks) tankStatus.push_back({true, false, false, ""});
    std::vector<std::string> outputLines;
    int tanksTotal = (int)player1Tanks.size() + (int)player2Tanks.size();
    int tanksP1 = (int)player1Tanks.size();
    int tanksP2 = (int)player2Tanks.size();
    int round = 0;
    int noShellsCounter = 0;
    const int NO_SHELLS_TIE_STEPS = 40;
    while (!gameOver && stepCounter < maxSteps*2) {
        moveShells();
        checkCollisions();
        if (stepCounter%2 == 0) {
            // --- Output logging for this round ---
            std::vector<std::string> actionsThisRound;
            // Player 1 tanks
            for (size_t i = 0; i < player1Tanks.size(); ++i) {
                Tank& tank = player1Tanks[i];
                TankOutputStatus& status = tankStatus[i];
                std::string actionStr;
                if (!status.alive) {
                    actionStr = "killed";
                } else {
                    auto& algo = tankAlgos1[i];
                    ActionRequest action = algo->getAction();
                    actionStr = to_string(action);
                    if (tank.isWaitingForBackward() && action != ActionRequest::MoveForward) {
                        actionStr += " (ignored)";
                        status.ignored = true;
                    }
                    if (!tank.isAlive()) {
                        actionStr += status.ignored ? " (killed)" : " (killed)";
                        status.alive = false;
                        status.killedThisStep = true;
                    } else {
                        status.killedThisStep = false;
                    }
                }
                actionsThisRound.push_back(actionStr);
                status.lastAction = actionStr;
                status.ignored = false;
            }
            // Player 2 tanks
            for (size_t i = 0; i < player2Tanks.size(); ++i) {
                Tank& tank = player2Tanks[i];
                TankOutputStatus& status = tankStatus[tanksP1 + i];
                std::string actionStr;
                if (!status.alive) {
                    actionStr = "killed";
                } else {
                    auto& algo = tankAlgos2[i];
                    ActionRequest action = algo->getAction();
                    actionStr = to_string(action);
                    if (tank.isWaitingForBackward() && action != ActionRequest::MoveForward) {
                        actionStr += " (ignored)";
                        status.ignored = true;
                    }
                    if (!tank.isAlive()) {
                        actionStr += status.ignored ? " (killed)" : " (killed)";
                        status.alive = false;
                        status.killedThisStep = true;
                    } else {
                        status.killedThisStep = false;
                    }
                }
                actionsThisRound.push_back(actionStr);
                status.lastAction = actionStr;
                status.ignored = false;
            }
            // Join actions for this round
            std::ostringstream oss;
            for (size_t i = 0; i < actionsThisRound.size(); ++i) {
                if (i > 0) oss << ", ";
                oss << actionsThisRound[i];
            }
            outputLines.push_back(oss.str());
            // --- End output logging for this round ---
            if (verbose) {
                std::cout << "Step " << round << std::endl;
                printBoard();
                std::cout << "----------------------" << std::endl;
            }
            tick();
            checkCollisions();
            ++round;
        }
        updateBoard();
        // Check win/tie conditions 
        bool anyAlive1 = std::any_of(player1Tanks.begin(), player1Tanks.end(), [](const Tank& t){ return t.isAlive(); });
        bool anyAlive2 = std::any_of(player2Tanks.begin(), player2Tanks.end(), [](const Tank& t){ return t.isAlive(); });
        bool allShellsUsed = std::all_of(player1Tanks.begin(), player1Tanks.end(), [](const Tank& t){ return t.getShellsLeft() == 0 || !t.isAlive(); }) &&
                             std::all_of(player2Tanks.begin(), player2Tanks.end(), [](const Tank& t){ return t.getShellsLeft() == 0 || !t.isAlive(); });
        if (allShellsUsed) {
            ++noShellsCounter;
        } else {
            noShellsCounter = 0;
        }
        if (!anyAlive1 && !anyAlive2) {
            gameOver = true;
            resultMessage = "Tie, both players have zero tanks";
        } else if (!anyAlive1) {
            gameOver = true;
            int alive2 = std::count_if(player2Tanks.begin(), player2Tanks.end(), [](const Tank& t){ return t.isAlive(); });
            resultMessage = "Player 2 won with " + std::to_string(alive2) + " tanks still alive";
        } else if (!anyAlive2) {
            gameOver = true;
            int alive1 = std::count_if(player1Tanks.begin(), player1Tanks.end(), [](const Tank& t){ return t.isAlive(); });
            resultMessage = "Player 1 won with " + std::to_string(alive1) + " tanks still alive";
        } else if (noShellsCounter >= NO_SHELLS_TIE_STEPS) {
            gameOver = true;
            resultMessage = "Tie, both players have zero shells for " + std::to_string(NO_SHELLS_TIE_STEPS) + " steps";
        }
        ++stepCounter;
    }
    if (!gameOver) {
        int alive1 = std::count_if(player1Tanks.begin(), player1Tanks.end(), [](const Tank& t){ return t.isAlive(); });
        int alive2 = std::count_if(player2Tanks.begin(), player2Tanks.end(), [](const Tank& t){ return t.isAlive(); });
        resultMessage = "Tie, reached max steps = " + std::to_string(maxSteps) + ", player 1 has " + std::to_string(alive1) + " tanks, player 2 has " + std::to_string(alive2) + " tanks";
    }
    // Write output file
    std::ofstream out("game_output.txt");
    for (const auto& line : outputLines) out << line << "\n";
    out << resultMessage << "\n";
}

void GameManager::tick() {
    // Player 1
    for (size_t i = 0; i < player1Tanks.size(); ++i) {
        Tank& tank = player1Tanks[i];
        if (!tank.isAlive()) continue;
        auto& algo = tankAlgos1[i];
        ActionRequest action = algo->getAction();
        bool success = true;
        if (action == ActionRequest::GetBattleInfo) {
            GameSatelliteView satelliteView(
                board, player1Tanks, player2Tanks, shells,
                tank.getPosition().x, tank.getPosition().y, tank.getPlayerId()
            );
            player1->updateTankWithBattleInfo(*algo, satelliteView);
            recordAction(tank.getPlayerId(), tank.getTankId(), action, success);
            tank.tickCooldown();
            continue;
        }

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
    // Player 2
    for (size_t i = 0; i < player2Tanks.size(); ++i) {
        Tank& tank = player2Tanks[i];
        if (!tank.isAlive()) continue;
        auto& algo = tankAlgos2[i];
        ActionRequest action = algo->getAction();
        bool success = true;
        if (action == ActionRequest::GetBattleInfo) {
            GameSatelliteView satelliteView(
                board, player1Tanks, player2Tanks, shells,
                tank.getPosition().x, tank.getPosition().y, tank.getPlayerId()
            );
            player2->updateTankWithBattleInfo(*algo, satelliteView);
            recordAction(tank.getPlayerId(), tank.getTankId(), action, success);
            tank.tickCooldown();
            continue;
        }

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
    std::ofstream errorFile("input_errors.txt");
    if (!errorFile) {
        std::cerr << "Error: Cannot open input_errors.txt for writing." << std::endl;
        return false;
    }
    std::ifstream inputFile(filename);
    if (!inputFile) {
        errorFile << "Error: Cannot open input file: " << filename << std::endl;
        return false;
    }

    std::string line;
    if (!std::getline(inputFile, line)) {
        errorFile << "Error: Missing map name/description line." << std::endl;
        return false;
    }
    int maxSteps = 0, numShells = 0, rows = 0, cols = 0;
    std::regex reNum(R"(=\s*(\d+))");
    for (int i = 0; i < 4; ++i) {
        if (!std::getline(inputFile, line)) {
            errorFile << "Error: Missing header line " << (i+2) << std::endl;
            return false;
        }
        std::smatch m;
        if (i == 0 && std::regex_search(line, m, reNum)) maxSteps = std::stoi(m[1]);
        if (i == 1 && std::regex_search(line, m, reNum)) numShells = std::stoi(m[1]);
        if (i == 2 && std::regex_search(line, m, reNum)) rows = std::stoi(m[1]);
        if (i == 3 && std::regex_search(line, m, reNum)) cols = std::stoi(m[1]);
    }
    if (rows <= 0 || cols <= 0) {
        errorFile << "Error: Invalid board dimensions (rows=" << rows << ", cols=" << cols << ")." << std::endl;
        return false;
    }
    board = Board(cols, rows);
    player1Tanks.clear();
    player2Tanks.clear();
    int row = 0;
    bool errorFound = false;
    while (std::getline(inputFile, line) && row < rows) {
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
            } else if (ch != ' ' && ch != '\r' && ch != '\n') {
                errorFile << "Error: Unrecognized character '" << ch << "' at (" << row << "," << col << ")." << std::endl;
                errorFound = true;
            }
        }
        ++row;
    }
    if (row < rows) {
        errorFile << "Error: Missing board rows. Expected " << rows << ", got " << row << "." << std::endl;
        return false;
    }
    // Check for too many tanks (optional, if spec requires)
    // if (player1Tanks.size() > MAX_TANKS) { errorFile << ... }
    // if (player2Tanks.size() > MAX_TANKS) { errorFile << ... }
    // Do NOT treat missing tanks as an error; run() will handle immediate loss
    if (errorFound) return false;
    // After tanks are known, create players and tank algorithms
    // (Assume factories are stored as members or passed in)
    // player1 = playerFactory1.create(1, ...); // fill in args
    // player2 = playerFactory2.create(2, ...);
    // for each tank: tankAlgos1.push_back(algoFactory1.create(...));
    // for each tank: tankAlgos2.push_back(algoFactory2.create(...));
    return true;
}

