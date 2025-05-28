#include "Board.h"
#include "Tank.h"
#include "GameManager.h"
#include "StrategyManager.h"
#include "UserInputAlgorithm.h"
#include "ChasingAlgorithm.h"
#include "ShootingAlgorithm.h"
#include <iostream>
#include <memory>
#include <vector>

int main(int argc, char* argv[]) {
    std::cout << "🛠️ Program started..." << std::endl;

    if (argc != 2) {
        std::cerr << "Usage: ./tanks_game <input_file>" << std::endl;
        return 1;
    }

    std::string inputFile = argv[1];
    // Create empty containers for tanks
    std::vector<Tank> player1Tanks;
    std::vector<Tank> player2Tanks;
    // Create a dummy board (will be resized in readBoard)
    Board board(1, 1);

    auto sm1 = std::make_shared<StrategyManager>(true, false);
    auto sm2 = std::make_shared<StrategyManager>(true, false);
    sm1->assignAlgorithm(0, std::make_shared<ChasingAlgorithm>());
    sm2->assignAlgorithm(0, std::make_shared<ShootingAlgorithm>());

    GameManager manager(board, sm1, sm2, player1Tanks, player2Tanks, false);
    if (!manager.readBoard(inputFile)) {
        std::cerr << "Failed to parse input file." << std::endl;
        return 1;
    }
    std::cout << "✅ Parsed input file" << std::endl;

    if (player1Tanks.empty()) {
        std::cerr << "Error: No tanks found for Player 1. Game cannot start." << std::endl;
        return 1;
    }
    if (player2Tanks.empty()) {
        std::cerr << "Error: No tanks found for Player 2. Game cannot start." << std::endl;
        return 1;
    }

    std::cout << "✅ Starting game loop" << std::endl;
    manager.run(100); // maxSteps = 100
    manager.writeLog("output_" + inputFile);

    std::cout << "✅ GameManager finished running" << std::endl;
    std::cout << manager.getResultMessage() << std::endl;
    return 0;
}