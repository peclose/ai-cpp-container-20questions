#include "game.h"

#include <algorithm>
#include <cctype>

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

std::unique_ptr<Node> makeQuestion(const std::string& question,
                                   std::unique_ptr<Node> yes,
                                   std::unique_ptr<Node> no) {
  auto node = std::make_unique<Node>();
  node->text = question;
  node->yes = std::move(yes);
  node->no = std::move(no);
  return node;
}

}  // namespace

bool Node::isAnswer() const {
  return !yes && !no;
}

std::optional<YesNo> parseYesNo(const std::string& input) {
  std::string normalized = trim(input);
  std::transform(normalized.begin(), normalized.end(), normalized.begin(),
                 [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
  if (normalized == "y" || normalized == "yes") {
    return YesNo::Yes;
  }
  if (normalized == "n" || normalized == "no") {
    return YesNo::No;
  }
  return std::nullopt;
}

std::unique_ptr<Node> makeAnswer(const std::string& title) {
  auto node = std::make_unique<Node>();
  node->text = trim(title);
  return node;
}

std::unique_ptr<Node> makeSeedTree() {
  return makeQuestion(
      "Was it released before 2000?",
      makeQuestion(
          "Is it primarily a platform game?",
          makeQuestion(
              "Does it feature a blue hedgehog?", makeAnswer("Sonic the Hedgehog 2"),
              makeQuestion("Does it star an Italian plumber?", makeAnswer("Super Mario 64"),
                           makeAnswer("Crash Bandicoot"))),
          makeQuestion(
              "Is it a fantasy role-playing game?",
              makeQuestion("Does it feature the hero Link?",
                           makeAnswer("The Legend of Zelda: Ocarina of Time"),
                           makeAnswer("Final Fantasy VII")),
              makeQuestion("Is it a kart racing game?", makeAnswer("Mario Kart 64"),
                           makeAnswer("Gran Turismo 2")))),
      makeQuestion(
          "Is it a sandbox game?",
          makeQuestion(
              "Is it a 2D sandbox game?", makeAnswer("Terraria"),
              makeQuestion("Is it set in a procedurally generated block world?",
                           makeAnswer("Minecraft"), makeAnswer("No Man's Sky"))),
          makeQuestion(
              "Is it a competitive online shooter?",
              makeQuestion("Do players buy weapons with earned credits before each round?",
                           makeAnswer("Valorant"), makeAnswer("Overwatch")),
              makeQuestion(
                  "Is it set in a medieval fantasy world?",
                  makeQuestion("Does its protagonist, Geralt, hunt monsters for a living?",
                               makeAnswer("The Witcher 3"), makeAnswer("Elden Ring")),
                  makeQuestion("Is it set in the American Old West?", makeAnswer("Red Dead Redemption 2"),
                               makeAnswer("Grand Theft Auto V"))))));
}

bool treeIsValid(const Node& node) {
  if (trim(node.text).empty()) {
    return false;
  }
  if (node.isAnswer()) {
    return true;
  }
  if (!node.yes || !node.no) {
    return false;
  }
  return treeIsValid(*node.yes) && treeIsValid(*node.no);
}

int countAnswers(const Node& node) {
  if (node.isAnswer()) {
    return 1;
  }
  return countAnswers(*node.yes) + countAnswers(*node.no);
}

bool teach(std::unique_ptr<Node>& answer, const std::string& question,
           const std::string& newTitle, YesNo answerForNewTitle) {
  const std::string cleanQuestion = trim(question);
  const std::string cleanTitle = trim(newTitle);
  if (!answer || !answer->isAnswer() || cleanQuestion.empty() || cleanTitle.empty()) {
    return false;
  }

  std::unique_ptr<Node> previousAnswer = std::move(answer);
  std::unique_ptr<Node> newAnswer = makeAnswer(cleanTitle);
  if (answerForNewTitle == YesNo::Yes) {
    answer = makeQuestion(cleanQuestion, std::move(newAnswer), std::move(previousAnswer));
  } else {
    answer = makeQuestion(cleanQuestion, std::move(previousAnswer), std::move(newAnswer));
  }
  return true;
}

}  // namespace twenty_questions
