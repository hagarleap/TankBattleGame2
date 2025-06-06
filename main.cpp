#include "Board.h"
#include "Tank.h"
#include "GameManager.h"
#include "MyPlayerFactory.h"
#include "MyTankAlgorithmFactory.h"
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
    GameManager game(MyPlayerFactory{}, MyTankAlgorithmFactory{}, true);
    std::cout << "Processing board" << std::endl;
    game.readBoard(inputFile);

    std::cout << "✅ Starting game loop" << std::endl;
    game.run(); 
    game.writeLog("output_" + inputFile);

    std::cout << "✅ GameManager finished running" << std::endl;
    std::cout << game.getResultMessage() << std::endl;
    return 0;
}