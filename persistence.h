#ifndef TWENTY_QUESTIONS_PERSISTENCE_H
#define TWENTY_QUESTIONS_PERSISTENCE_H

#include "game.h"

#include <memory>
#include <string>

namespace twenty_questions {

bool saveTree(const Node& tree, const std::string& path);
std::unique_ptr<Node> loadTree(const std::string& path);

}  // namespace twenty_questions

#endif
