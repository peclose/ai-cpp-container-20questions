# Implementation Plan: Learning CLI 20 Questions

## Overview

Build a single-player C++ command-line game in which the computer identifies a video game title by asking up to 20 yes/no questions. The initial knowledge base will be small and seeded in code. If the computer makes an incorrect final guess, the player supplies the correct title and a distinguishing yes/no question; the game incorporates that information so it is available in later rounds and future program runs. After every completed round, the player can choose to quit or start a new round.

## Assumptions

1. “Add new answers” means the learned knowledge survives restarting the executable, not merely the current round.
2. The 20-question budget counts every yes/no question the computer asks, including a final “Is it <answer>?” guess. Learning prompts occur after a loss and do not consume that budget.
3. The subject category is video game titles. The seed data and learning prompts will make that category clear.
4. A terminal-based, line-oriented interface is sufficient; no external libraries or database are needed.

## Architecture Decisions

- Represent knowledge as a binary decision tree. A non-leaf node contains a yes/no question and `yes`/`no` children; a leaf contains one candidate answer. This makes both play and learning direct tree traversal.
- Store the tree in a small application-owned text format (for example, length-prefixed records) in a data file beside the executable or in the working directory. Length-prefixed strings preserve questions and answers containing spaces or punctuation without adding a JSON dependency.
- Treat malformed/missing knowledge data as recoverable: report the issue, start with the seed tree, and avoid overwriting a malformed file until a successful learning event. This protects the supplied knowledge base.
- Accept only normalized yes/no answers (`y`/`yes`, `n`/`no`) for game decisions. Re-prompt invalid input and handle EOF by exiting cleanly without saving a partial change.

## Dependency Graph

```text
Seed tree + node invariants
        |
        +--> Input normalization and traversal loop
        |          |
        |          +--> Missed-guess learning mutation
        |
        +--> Tree serialization / loading
                    |
                    +--> End-to-end persistence tests
```

## Task List

### Phase 1: Foundation

#### Task 1: Define the decision-tree model

**Description:** Add a small `Node` representation and helper functions for creating question and answer nodes. Seed a starter tree with a few distinct answers that gives the game meaningful first-round behavior.

**Acceptance criteria:**

- [ ] Every question node has non-null yes and no children; every answer leaf has a non-empty candidate answer and no children.
- [ ] The program can construct the seed tree without allocation or invariant failures.
- [ ] Seeded paths end at at least three distinct answers.

**Verification:**

- [ ] Add a focused test that walks the seed tree and checks node invariants.
- [ ] Compile with warnings enabled and run the test executable.

**Dependencies:** None

**Files likely touched:**

- `main.cpp` (or a focused `knowledge_tree.{h,cpp}` extracted from it)
- `tests/knowledge_tree_test.cpp`

**Estimated scope:** Small (2-3 files)

#### Task 2: Implement robust yes/no CLI input

**Description:** Centralize prompts and normalize player answers so traversal, guess confirmation, and learning prompts have identical validation and EOF behavior.

**Acceptance criteria:**

- [ ] Case and surrounding whitespace do not affect valid yes/no answers.
- [ ] Invalid input produces a concise retry prompt and never changes the tree.
- [ ] EOF or input failure exits the current game safely.

**Verification:**

- [ ] Add tests for accepted and rejected normalized inputs.
- [ ] Manually pipe an invalid response followed by a valid one and confirm the prompt recovers.

**Dependencies:** Task 1

**Files likely touched:**

- `main.cpp` (or `cli_input.{h,cpp}`)
- `tests/cli_input_test.cpp`

**Estimated scope:** Small (2-3 files)

### Checkpoint: Foundation

- [ ] Seed-tree and input tests pass.
- [ ] The application compiles with its documented compiler command.
- [ ] Invariants are explicit enough to support a safe learning mutation.

### Phase 2: Core Game

#### Task 3: Deliver one capped game round

**Description:** Traverse the tree by asking stored questions and end at a leaf with a final candidate guess. Count each computer yes/no prompt; if the budget is exhausted before a confirmed answer, announce that the player wins.

**Acceptance criteria:**

- [ ] Yes and no answers choose the correct branch at each question node.
- [ ] A confirmed leaf guess reports a computer win.
- [ ] No game round asks more than 20 computer questions.
- [ ] A rejected guess or exhausted budget enters the learning path.

**Verification:**

- [ ] Add scripted-input tests for a winning seeded path and a rejected guess.
- [ ] Manually play one round and confirm the displayed question count never exceeds 20.

**Dependencies:** Tasks 1-2

**Files likely touched:**

- `main.cpp` (or `game.{h,cpp}`)
- `tests/game_test.cpp`

**Estimated scope:** Medium (3-4 files)

#### Task 4: Teach the tree after a computer loss

**Description:** At the missed answer leaf, ask for the player’s answer, a question that distinguishes it from the incorrect candidate, and which answer applies to the new candidate. Replace that leaf with a question node whose two children are the new and previous answer leaves in the proper orientation.

**Acceptance criteria:**

- [ ] The original incorrect candidate remains reachable after learning.
- [ ] The supplied new answer becomes reachable using the supplied answer to the distinguishing question.
- [ ] Empty answers/questions are rejected before mutation.
- [ ] A second round can distinguish both candidates in memory.

**Verification:**

- [ ] Add a unit test for each yes/no orientation of the replacement node.
- [ ] Run two scripted rounds in one process: lose/teach, then answer the new question and confirm the computer guesses the newly taught answer.

**Dependencies:** Task 3

**Files likely touched:**

- `main.cpp` (or `knowledge_tree.{h,cpp}` and `game.{h,cpp}`)
- `tests/knowledge_tree_test.cpp`
- `tests/game_test.cpp`

**Estimated scope:** Medium (3-4 files)

### Checkpoint: Core Game

- [ ] A seeded answer can be guessed correctly.
- [ ] A missed guess can be taught and recalled in the same process.
- [ ] Input errors and the 20-question cap do not corrupt the tree.

### Phase 3: Persistence and Finish

#### Task 5: Load and save the knowledge tree safely

**Description:** Serialize the full tree recursively, load it at startup when valid, and save only after a completed learning mutation. Use a temporary file and rename for writes so an interrupted save cannot leave a partial primary knowledge file.

**Acceptance criteria:**

- [ ] A missing save file starts the seed tree.
- [ ] A learned tree, once saved and reloaded, retains both the new answer and the prior candidate.
- [ ] Invalid or truncated saved data produces a clear message and falls back to seed data without crashing.
- [ ] A failed save is reported and leaves the in-memory game usable.

**Verification:**

- [ ] Add a round-trip serialization test using a temporary test directory.
- [ ] Add malformed-file tests.
- [ ] Run the executable twice against a temporary data path: teach in the first run, then prove it knows the taught answer in the second.

**Dependencies:** Task 4

**Files likely touched:**

- `main.cpp` (or `persistence.{h,cpp}`)
- `tests/persistence_test.cpp`
- `test_runner.sh`

**Estimated scope:** Medium (3-4 files)

#### Task 6: Add replay flow and project documentation

**Description:** Let the player choose another round after each completed game; make game outcomes, learning, and persistence status clear. Update the README with build/run commands, data-file location/override, and the learning flow.

**Acceptance criteria:**

- [ ] The player can play multiple rounds and exit normally.
- [ ] The program handles EOF at replay cleanly.
- [ ] README explains how to start fresh by removing or choosing a different knowledge-file path.

**Verification:**

- [ ] Manually play a win, a loss-and-learn, replay, and exit sequence.
- [ ] Run the complete automated test command from the README.

**Dependencies:** Task 5

**Files likely touched:**

- `main.cpp`
- `README.md`
- `test_runner.sh`
- `tests/` test sources

**Estimated scope:** Small (3-4 files)

### Checkpoint: Complete

- [ ] All automated tests pass from a clean checkout.
- [ ] The application builds and runs from the documented command.
- [ ] A newly taught answer is recognized after a process restart.
- [ ] The 20-question limit, invalid input, EOF, and bad data-file paths have been exercised.

## Risks and Mitigations

| Risk | Impact | Mitigation |
|---|---|---|
| Ambiguous question orientation reverses branches | High | Ask explicitly whether the new answer’s response is yes or no; cover both orientations with tests. |
| Free-form input breaks an ad-hoc save format | High | Use length-prefixed strings and validate record types/lengths while loading. |
| A partial write loses learned knowledge | Medium | Write a complete temporary file, flush/close it, then rename it into place. |
| The starting tree has too few useful paths | Low | Keep initial data intentionally small but provide at least three clearly distinguishable video game titles. |

## Open Questions for Approval

All previously open product decisions are confirmed:

1. Learned knowledge persists across program restarts.
2. The category is video game titles.
3. A final candidate guess counts toward the 20-question limit.
4. After every completed game, the player is asked whether to quit or begin a new round.
