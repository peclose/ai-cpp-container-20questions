# Persistent Video Game 20 Questions

A C++ command-line game that tries to guess a video game title in 20 yes/no
questions. When it misses, teach it the correct game and a question that
distinguishes it from its incorrect guess. The game remembers what it learns
after restart.

## Build and run

```bash
make
./app
```

The program accepts `y`/`yes` and `n`/`no`, ignoring case and surrounding
spaces. After every round it asks whether to play again.

## Knowledge file

Learned games are saved in `video_game_knowledge.txt` in the directory where
you run the program. If the file is absent, the game starts with 16 built-in
titles. To begin again, remove that file before running the game.

## Tests

```bash
make test
```

`./test_runner.sh` is also supported and works no matter which directory you
launch it from.

The tests cover input parsing, tree invariants, teaching in both branches,
save/load round trips, malformed data, and learning a game across a restart.

## Project layout

- `application.cpp` — terminal game loop and replay flow
- `game.cpp` — decision-tree and learning logic
- `persistence.cpp` — knowledge-file save/load logic
- `tests/` — automated test source
- `specs/` — approved specification and implementation plan
