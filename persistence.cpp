#include "persistence.h"

#include <cstdio>
#include <fstream>
#include <sstream>

namespace twenty_questions {
namespace {

constexpr std::size_t kMaximumStoredTextLength = 4096;

bool writeNode(std::ostream& output, const Node& node) {
  const char type = node.isAnswer() ? 'A' : 'Q';
  output << type << ' ' << node.text.size() << '\n' << node.text << '\n';
  if (!output) {
    return false;
  }
  return node.isAnswer() || (writeNode(output, *node.yes) && writeNode(output, *node.no));
}

std::unique_ptr<Node> readNode(std::istream& input) {
  std::string header;
  if (!std::getline(input, header)) {
    return nullptr;
  }

  char type = '\0';
  std::size_t length = 0;
  std::istringstream headerStream(header);
  if (!(headerStream >> type >> length) || (type != 'A' && type != 'Q') ||
      length > kMaximumStoredTextLength) {
    return nullptr;
  }
  headerStream >> std::ws;
  if (!headerStream.eof()) {
    return nullptr;
  }

  std::string text(length, '\0');
  input.read(text.data(), static_cast<std::streamsize>(length));
  if (input.gcount() != static_cast<std::streamsize>(length) || input.get() != '\n' ||
      text.empty()) {
    return nullptr;
  }

  auto node = std::make_unique<Node>();
  node->text = std::move(text);
  if (type == 'Q') {
    node->yes = readNode(input);
    node->no = readNode(input);
    if (!node->yes || !node->no) {
      return nullptr;
    }
  }
  return node;
}

}  // namespace

bool saveTree(const Node& tree, const std::string& path) {
  const std::string temporaryPath = path + ".tmp";
  std::ofstream output(temporaryPath, std::ios::binary | std::ios::trunc);
  if (!output || !writeNode(output, tree)) {
    return false;
  }
  output.close();
  if (!output) {
    return false;
  }
  return std::rename(temporaryPath.c_str(), path.c_str()) == 0;
}

std::unique_ptr<Node> loadTree(const std::string& path) {
  std::ifstream input(path, std::ios::binary);
  if (!input) {
    return nullptr;
  }
  std::unique_ptr<Node> tree = readNode(input);
  if (!tree || input.peek() != std::char_traits<char>::eof() || !treeIsValid(*tree)) {
    return nullptr;
  }
  return tree;
}

}  // namespace twenty_questions
