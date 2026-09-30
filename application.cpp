#include "application.h"

#include "game.h"
#include "persistence.h"

#include <algorithm>
#include <cctype>
#include <fstream>
#include <memory>
#include <optional>
#include <string>

namespace twenty_questions {
namespace {

std::string trim(const std::string& value) {
  const auto first = std::find_if_not(value.begin(), value.end(), [](unsigned char c) {
    return std::isspace(c);
  });
  const auto last = std::find_if_not(value.rbegin(), value.rend(), [](unsigned char c) {
    return std::isspace(c);
  }).base();
  return first >= last ? "" : std::string(first, last);
}

std::optional<YesNo> askYesNo(std::istream& input, std::ostream& output,
                              const std::string& prompt) {
  std::string response;
  while (true) {
    output << prompt << " (yes/no): ";
    if (!std::getline(input, response)) {
      return std::nullopt;
    }
    const std::optional<YesNo> answer = parseYesNo(response);
    if (answer) {
      return answer;
    }
    output << "Please answer yes or no.\n";
  }
}

std::optional<std::string> askNonEmpty(std::istream& input, std::ostream& output,
                                       const std::string& prompt) {
  std::string response;
  while (true) {
    output << prompt;
    if (!std::getline(input, response)) {
      return std::nullopt;
    }
    response = trim(response);
    if (!response.empty()) {
      return response;
    }
    output << "Please enter a non-empty response.\n";
  }
}

std::unique_ptr<Node>& firstAnswer(std::unique_ptr<Node>& node) {
  return node->isAnswer() ? node : firstAnswer(node->yes);
}

bool learn(std::istream& input, std::ostream& output, std::unique_ptr<Node>& tree,
           std::unique_ptr<Node>& answer, const std::string& knowledgePath) {
  const std::optional<std::string> title = askNonEmpty(
      input, output, "What video game were you thinking of? ");
  if (!title) {
    return false;
  }
  const std::optional<std::string> question = askNonEmpty(
      input, output, "Give me a yes/no question that distinguishes " + *title +
                         " from " + answer->text + ": ");
  if (!question) {
    return false;
  }
  const std::optional<YesNo> newTitleAnswer = askYesNo(
      input, output, "For " + *title + ", what is the answer to that question?");
  if (!newTitleAnswer) {
    return false;
  }
  if (!teach(answer, *question, *title, *newTitleAnswer)) {
    output << "I could not add that game.\n";
    return true;
  }
  if (saveTree(*tree, knowledgePath)) {
    output << "I learned " << *title << ".\n";
  } else {
    output << "I learned " << *title << " for this run, but could not save it.\n";
  }
  return true;
}

bool playRound(std::istream& input, std::ostream& output, std::unique_ptr<Node>& tree,
               const std::string& knowledgePath) {
  output << "\nThink of a video game title.\n";
  std::unique_ptr<Node>* current = &tree;
  int questionCount = 0;
  while (!(*current)->isAnswer() && questionCount < 20) {
    const std::optional<YesNo> answer = askYesNo(input, output, (*current)->text);
    if (!answer) {
      return false;
    }
    ++questionCount;
    current = *answer == YesNo::Yes ? &(*current)->yes : &(*current)->no;
  }

  if ((*current)->isAnswer() && questionCount < 20) {
    const std::optional<YesNo> correct =
        askYesNo(input, output, "Is it " + (*current)->text + "?");
    if (!correct) {
      return false;
    }
    ++questionCount;
    if (*correct == YesNo::Yes) {
      output << "I win! I guessed " << (*current)->text << " in " << questionCount
             << " questions.\n";
      return true;
    }
    output << "You win.\n";
    return learn(input, output, tree, *current, knowledgePath);
  }

  output << "You win. I ran out of questions.\n";
  return learn(input, output, tree, firstAnswer(*current), knowledgePath);
}

}  // namespace

void runGame(std::istream& input, std::ostream& output, const std::string& knowledgePath) {
  bool fileExists = false;
  {
    std::ifstream existingKnowledge(knowledgePath, std::ios::binary);
    fileExists = static_cast<bool>(existingKnowledge);
  }
  std::unique_ptr<Node> tree = loadTree(knowledgePath);
  if (!tree) {
    output << (fileExists ? "Knowledge data is invalid; using starter games.\n"
                          : "Using starter video games.\n");
    tree = makeSeedTree();
  }

  while (playRound(input, output, tree, knowledgePath)) {
    const std::optional<YesNo> playAgain = askYesNo(input, output, "Play another round?");
    if (!playAgain || *playAgain == YesNo::No) {
      output << "Thanks for playing.\n";
      return;
    }
  }
  output << "Input ended. Thanks for playing.\n";
}

}  // namespace twenty_questions
