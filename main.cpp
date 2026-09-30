#include "application.h"

#include <iostream>

int main() {
  twenty_questions::runGame(std::cin, std::cout, "video_game_knowledge.txt");
  return 0;
}
