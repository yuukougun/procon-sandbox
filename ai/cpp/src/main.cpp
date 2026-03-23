#include "SelfPlay.hpp"
#include <iostream>

int main() {
    SelfPlay sp;
    sp.play_random_game();
    sp.save_csv("ai/data/game.csv");
    sp.save_binary("ai/data/game.bin");
    std::cout << "Random self-play finished.\n";
    std::cout << "Result: " << sp.result << " (1:Black win, -1:White win, 0:Draw)\n";
    std::cout << "Moves: " << sp.history.size() << std::endl;
    return 0;
}
