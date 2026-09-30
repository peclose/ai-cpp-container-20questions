CXX ?= g++
CXXFLAGS ?= -std=c++17 -Wall -Wextra -pedantic

APP := app
TEST_PROGRAM := tests/test_game
APP_SOURCES := main.cpp game.cpp persistence.cpp application.cpp
TEST_SOURCES := game.cpp persistence.cpp application.cpp tests/test_game.cpp

.PHONY: all test clean

all: $(APP)

$(APP): $(APP_SOURCES) application.h game.h persistence.h
	$(CXX) $(CXXFLAGS) $(APP_SOURCES) -o $@

$(TEST_PROGRAM): $(TEST_SOURCES) application.h game.h persistence.h
	$(CXX) $(CXXFLAGS) $(TEST_SOURCES) -o $@

test: $(TEST_PROGRAM)
	./$(TEST_PROGRAM)

clean:
	rm -f $(APP) $(TEST_PROGRAM)
