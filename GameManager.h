#ifndef GAMEMANAGER_H
#define GAMEMANAGER_H

#include "Board.h"
#include "Tank.h"
#include "Shell.h"
#include "common/ActionRequest.h"
#include "StrategyManager.h"
#include <vector>
#include <string>
#include <fstream>

class GameManager {
public:
    GameManager(Board& board,
                std::shared_ptr<StrategyManager> smP1,
                std::shared_ptr<StrategyManager> smP2,
                std::vector<Tank>& player1Tanks,
                std::vector<Tank>& player2Tanks,
                bool verbose = false
                );

    void run(int maxSteps = 1000);
    void writeLog(const std::string& outputFile) const;
    std::string getResultMessage() const;


private:
    Board& board;

    std::shared_ptr<StrategyManager> strategyP1;
    std::shared_ptr<StrategyManager> strategyP2;
    std::vector<Tank>& player1Tanks;
    std::vector<Tank>& player2Tanks;
    std::vector<Shell> shells;
    
    bool verbose = false;
    int stepCounter = 0;
    std::vector<std::string> log;

    bool gameOver = false;
    std::string resultMessage;

    void tick();
    void handleActionRequest(Tank& tank, ActionRequest& action);
    void moveShells();
    void checkCollisions();
    void updateBoard();
    void recordAction(int playerId, int tankId, ActionRequest action, bool success);
    void printBoard() const;


};

#endif // GAMEMANAGER_H
