#include "../application.h"
#include "../game.h"
#include "../persistence.h"

#include <cassert>
#include <cstdio>
#include <fstream>
#include <sstream>
#include <memory>
#include <string>

int main() {
  using namespace twenty_questions;

  assert(parseYesNo(" yes ") == YesNo::Yes);
  assert(parseYesNo("N") == YesNo::No);
  assert(!parseYesNo("maybe").has_value());

  std::unique_ptr<Node> tree = makeSeedTree();
  assert(treeIsValid(*tree));
  assert(countAnswers(*tree) == 16);
  const Node& racingQuestion = *tree->yes->no->no;
  assert(racingQuestion.text == "Is it a kart racing game?");
  assert(racingQuestion.yes->text == "Mario Kart 64");
  assert(racingQuestion.no->text == "Gran Turismo 2");
  const Node& shooterQuestion = *tree->no->no->yes;
  assert(shooterQuestion.text == "Do players buy weapons with earned credits before each round?");
  assert(shooterQuestion.yes->text == "Valorant");
  assert(shooterQuestion.no->text == "Overwatch");
  const Node& fantasyQuestion = *tree->no->no->no;
  assert(fantasyQuestion.text == "Is it set in a medieval fantasy world?");
  const Node& geraltQuestion = *fantasyQuestion.yes;
  assert(geraltQuestion.text == "Does its protagonist, Geralt, hunt monsters for a living?");
  assert(geraltQuestion.yes->text == "The Witcher 3");
  assert(geraltQuestion.no->text == "Elden Ring");
  const Node& sandboxQuestion = *tree->no->yes;
  assert(sandboxQuestion.text == "Is it a 2D sandbox game?");
  assert(sandboxQuestion.yes->text == "Terraria");
  const Node& blockWorldQuestion = *sandboxQuestion.no;
  assert(blockWorldQuestion.text == "Is it set in a procedurally generated block world?");
  assert(blockWorldQuestion.yes->text == "Minecraft");
  assert(blockWorldQuestion.no->text == "No Man's Sky");

  std::unique_ptr<Node> answer = makeAnswer("Minecraft");
  teach(answer, "Is it a sandbox game?", "Terraria", YesNo::No);
  assert(!answer->isAnswer());
  assert(answer->text == "Is it a sandbox game?");
  assert(answer->yes->text == "Minecraft");
  assert(answer->no->text == "Terraria");
  assert(treeIsValid(*answer));

  std::unique_ptr<Node> secondAnswer = makeAnswer("Minecraft");
  assert(teach(secondAnswer, "Is it a crafting game?", "Terraria", YesNo::Yes));
  assert(secondAnswer->yes->text == "Terraria");
  assert(secondAnswer->no->text == "Minecraft");

  const std::string knowledgePath = "test_knowledge.tmp";
  assert(saveTree(*answer, knowledgePath));
  std::unique_ptr<Node> restored = loadTree(knowledgePath);
  assert(restored);
  assert(treeIsValid(*restored));
  assert(restored->text == "Is it a sandbox game?");
  assert(restored->no->text == "Terraria");
  std::remove(knowledgePath.c_str());

  const std::string invalidPath = "invalid_knowledge.tmp";
  std::ofstream invalidFile(invalidPath);
  invalidFile << "Q 12\nshort\n";
  invalidFile.close();
  assert(!loadTree(invalidPath));
  std::remove(invalidPath.c_str());

  const std::string oversizedPath = "oversized_knowledge.tmp";
  std::ofstream oversizedFile(oversizedPath);
  oversizedFile << "A 999999999\n";
  oversizedFile.close();
  assert(!loadTree(oversizedPath));
  std::remove(oversizedPath.c_str());

  const std::string playPath = "play_knowledge.tmp";
  std::remove(playPath.c_str());
  std::istringstream firstRunInput("no\nno\nno\nno\nno\nno\nPortal 2\n"
                                   "Is it a puzzle game?\nyes\nno\n");
  std::ostringstream firstRunOutput;
  runGame(firstRunInput, firstRunOutput, playPath);
  assert(firstRunOutput.str().find("I learned Portal 2") != std::string::npos);

  std::istringstream secondRunInput("no\nno\nno\nno\nno\nyes\nyes\nno\n");
  std::ostringstream secondRunOutput;
  runGame(secondRunInput, secondRunOutput, playPath);
  assert(secondRunOutput.str().find("I win! I guessed Portal 2") != std::string::npos);
  std::remove(playPath.c_str());

  std::unique_ptr<Node> deepTree = makeAnswer("Final Game");
  for (int index = 0; index < 20; ++index) {
    auto question = std::make_unique<Node>();
    question->text = "Question " + std::to_string(index);
    question->yes = std::move(deepTree);
    question->no = makeAnswer("Other Game");
    deepTree = std::move(question);
  }
  const std::string cappedPath = "capped_knowledge.tmp";
  assert(saveTree(*deepTree, cappedPath));
  std::string cappedInput;
  for (int index = 0; index < 20; ++index) {
    cappedInput += "yes\n";
  }
  cappedInput += "New Game\nIs it new?\nyes\nno\n";
  std::istringstream cappedRunInput(cappedInput);
  std::ostringstream cappedRunOutput;
  runGame(cappedRunInput, cappedRunOutput, cappedPath);
  assert(cappedRunOutput.str().find("I ran out of questions") != std::string::npos);
  assert(cappedRunOutput.str().find("Is it Final Game?") == std::string::npos);
  std::remove(cappedPath.c_str());
}
