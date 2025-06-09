#ifndef GAMEMANAGER_H
#define GAMEMANAGER_H

#include "Board.h"
#include "Tank.h"
#include "Shell.h"
#include "common/ActionRequest.h"
#include "common/Player.h"
#include "MyPlayerFactory.h"
#include "common/TankAlgorithm.h"
#include "MyTankAlgorithmFactory.h"
#include <vector>
#include <string>
#include <fstream>
#include <memory>

class GameManager {
public:
    GameManager(MyPlayerFactory playerFactory,
                MyTankAlgorithmFactory algoFactory,
                bool verbose = false);

    void run();
    void writeLog(const std::string& outputFile) const;
    std::string getResultMessage() const;
    bool readBoard(const std::string& filename);

private:
    Board board;
    MyPlayerFactory playerFactory;
    MyTankAlgorithmFactory algoFactory;
    std::unique_ptr<Player> player1;
    std::unique_ptr<Player> player2;
    std::vector<std::unique_ptr<TankAlgorithm>> tankAlgos1;
    std::vector<std::unique_ptr<TankAlgorithm>> tankAlgos2;
    std::vector<Tank> player1Tanks;
    std::vector<Tank> player2Tanks;
    std::vector<Shell> shells;
    bool verbose = false;
    int stepCounter = 0;
    int maxSteps = 1000;
    std::vector<std::string> log;
    bool gameOver = false;
    std::string resultMessage;

    void tick(const std::vector<ActionRequest>& actions1, const std::vector<ActionRequest>& actions2);
    void handleActionRequest(Tank& tank, ActionRequest& action);
    void moveShells();
    void checkCollisions();
    void updateBoard();
    void recordAction(int playerId, int tankId, ActionRequest action, bool success);
    void printBoard() const;

};

#endif // GAMEMANAGER_H
