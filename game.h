#ifndef TWENTY_QUESTIONS_GAME_H
#define TWENTY_QUESTIONS_GAME_H

#include <memory>
#include <optional>
#include <string>

namespace twenty_questions {

enum class YesNo { Yes, No };

struct Node {
  std::string text;
  std::unique_ptr<Node> yes;
  std::unique_ptr<Node> no;

  bool isAnswer() const;
};

std::optional<YesNo> parseYesNo(const std::string& input);
std::unique_ptr<Node> makeAnswer(const std::string& title);
std::unique_ptr<Node> makeSeedTree();
bool treeIsValid(const Node& node);
int countAnswers(const Node& node);
bool teach(std::unique_ptr<Node>& answer, const std::string& question,
           const std::string& newTitle, YesNo answerForNewTitle);

}  // namespace twenty_questions

#endif
