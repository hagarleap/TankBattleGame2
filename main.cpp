#include "Board.h"
#include "Tank.h"
#include "GameManager.h"
#include "InputParser.h"
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
    std::vector<Tank> player1Tanks;
    std::vector<Tank> player2Tanks;
    std::vector<std::string> inputErrors;

    // Parse board dimensions
    auto [width, height] = InputParser::getBoardDimensions(inputFile);
    if (width <= 0 || height <= 0) {
        std::cerr << "Failed to read board dimensions." << std::endl;
        return 1;
    }
    
    Board board(width, height); 
    // Parse full input file
    if (!InputParser::parseFile(inputFile, board, player1Tanks, player2Tanks, inputErrors)) {
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

    if (!inputErrors.empty()) {
        std::ofstream errFile("input_errors.txt");
        for (const std::string& err : inputErrors) {
            errFile << err << std::endl;
        }
    }
    std::cout << "✅ Starting game loop" << std::endl;
  
    auto sm1 = std::make_shared<StrategyManager>(true, false); /// passing true twice means 1. using the common sense algorithm and 2. using verbose mode shows tank details action made and action overrides if relevant. useful for debugging.
    auto sm2 = std::make_shared<StrategyManager>(true, false); // it is reccommended to use false if using the UserInputAlgorithm, otherwise it will interfere with the user input if it thinks it is a bad move.

    sm1->assignAlgorithm(0, std::make_shared<ChasingAlgorithm>());
    sm2->assignAlgorithm(0, std::make_shared<ShootingAlgorithm>());

    GameManager manager(board, sm1, sm2, player1Tanks, player2Tanks, false); // true means verbose mode, false/nothing (default false) means silent mode

    manager.run(100); // maxSteps = 100
    manager.writeLog("output_" + inputFile);

    std::cout << "✅ GameManager finished running" << std::endl;
    std::cout << manager.getResultMessage() << std::endl;
    return 0;
}