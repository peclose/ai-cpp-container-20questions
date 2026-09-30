# Spec: Persistent Video Game 20 Questions CLI

## Objective

Build a C++ command-line game for one player. The player thinks of a video game title and the computer asks no more than 20 yes/no questions to identify it.

The game starts with a built-in decision tree of 16 video game titles. When the computer loses, the player teaches it the correct title and one yes/no question that distinguishes the title from the incorrect guess. The learned knowledge is saved and reused after the executable is restarted.

After each completed round, the program asks the player whether to begin a new round or quit.

### User stories

- As a player, I can answer the computer’s yes/no questions and see it make a title guess.
- As a player, I can teach the program a missed title and a useful distinguishing question.
- As a returning player, I can play against titles taught in an earlier run.
- As a player, I can start another round or quit after every game.

## Tech Stack

- C++17
- C++ standard library only; no external dependencies
- A recursive binary decision tree held with standard smart pointers
- A length-prefixed text data file named `video_game_knowledge.txt` in the current working directory

## Commands

Build:

```bash
make
```

Run interactively:

```bash
./app
```

Run the project test entry point:

```bash
./test_runner.sh
```

The test runner will be expanded during implementation to compile and execute application tests without relying on external test frameworks.

## Project Structure

```text
main.cpp                                      application entry point and CLI game flow
tests/                                        automated C++ test sources
test_runner.sh                                compiles and runs automated tests
specs/20-questions-spec.md                   this requirements specification
specs/20-questions-implementation-plan.md    approved implementation tasks and checkpoints
video_game_knowledge.txt                     runtime-created learned knowledge; not source code
```

The implementation may split `main.cpp` into focused tree, input, game, and persistence files when that improves testability. Those files remain in the repository root unless a later approved structure change establishes a `src/` directory.

## Game Behavior

1. At the start of a round, the program tells the player to think of a video game title.
2. It traverses the decision tree by asking stored yes/no questions.
3. Each yes/no prompt from the computer counts toward the 20-question limit, including a final prompt such as “Is it Minecraft?”
4. The program accepts `y`/`yes` and `n`/`no`, ignoring case and surrounding whitespace. It re-prompts any other response.
5. If the player confirms a final guess, the program reports a computer win.
6. If the player rejects a final guess, or the program reaches 20 questions without a confirmed title, the program reports a player win and starts the learning flow.
7. Learning asks for the correct video game title, a question that separates it from the computer’s candidate, and whether the correct title’s answer is yes or no. It then replaces the incorrect-answer leaf with that question and its two answer leaves.
8. The program saves a completed learning update. A saved title must be available in a later process run.
9. After a completed round, the program asks whether to play again. A no answer exits normally; a yes answer begins a fresh round using the current tree.

## Persistence and Error Handling

- If `video_game_knowledge.txt` does not exist, the program uses a starter tree containing exactly 16 distinct video game titles.
- The save format must preserve arbitrary non-empty titles and questions, including spaces and punctuation.
- If persisted data is unreadable or invalid, the program reports the problem and starts with the built-in tree instead of crashing.
- If input reaches EOF, the program exits cleanly and does not apply an incomplete learning update.
- A failed save is reported, while the updated in-memory tree remains usable for the rest of the process.

## Code Style

Use clear, small functions; PascalCase type names; camelCase functions and variables; and explicit ownership for tree nodes.

```cpp
enum class YesNo { Yes, No };

struct Node {
  std::string text;
  std::unique_ptr<Node> yes;
  std::unique_ptr<Node> no;

  bool isAnswer() const {
    return !yes && !no;
  }
};
```

Question nodes have both children and store a question. Answer nodes have neither child and store a non-empty video game title. User input is validated before it changes tree state.

## Testing Strategy

Automated tests are C++ tests run through `./test_runner.sh`. They must not depend on manual terminal interaction.

- Unit tests: tree invariants, yes/no normalization, learning mutation in both yes/no orientations, and serialization round trips.
- Integration tests: scripted rounds for a seeded win, a rejected guess followed by learning, and recognition of the learned title in a subsequent program run.
- Error tests: invalid yes/no input, empty teaching responses, EOF, missing data file, and malformed data file.
- Manual smoke test: play a win, teach a missed game, select a new round, verify the taught game, then quit.

## Boundaries

- Always:
  - Keep answers within the video game title category.
  - Validate user input before changing the tree.
  - Run `./test_runner.sh` and the warning-enabled build command before claiming completion.
  - Update this specification before changing approved behavior.

- Ask first:
  - Add third-party dependencies or a unit-test framework.
  - Change the persistence location or format in a way that discards existing learned data.
  - Add categories beyond video games, a GUI, networking, or shared knowledge.
  - Change build tooling or CI configuration.

- Never:
  - Commit `video_game_knowledge.txt` containing runtime-player data.
  - Store secrets or network credentials.
  - Remove failing tests to make the test suite pass.
  - Exceed 20 computer yes/no prompts in one game round.

## Success Criteria

- The executable builds with the documented C++17 command.
- The starter tree contains exactly 16 video game titles that can be reached by valid answer paths.
- Every computer yes/no prompt, including the final guess, is counted and no round exceeds 20.
- A player can teach a missed video game title and distinguishing question.
- Both the prior title and the new title remain reachable after learning.
- A title learned in one program run is recognized in a later run.
- Invalid input, EOF, and malformed persisted data do not crash the program or partially mutate knowledge.
- Every completed round offers a clear quit-or-play-again choice.
- Automated tests pass through `./test_runner.sh`.

## Open Questions

None. The implementation plan is approved and provides the task order, dependencies, and checkpoints.
