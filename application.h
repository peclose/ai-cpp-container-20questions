#ifndef TWENTY_QUESTIONS_APPLICATION_H
#define TWENTY_QUESTIONS_APPLICATION_H

#include <iosfwd>
#include <string>

namespace twenty_questions {

void runGame(std::istream& input, std::ostream& output, const std::string& knowledgePath);

}  // namespace twenty_questions

#endif
