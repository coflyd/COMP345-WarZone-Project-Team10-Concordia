#ifndef GAMEENGINE_H
#define GAMEENGINE_H

#include <string>
#include <vector>
#include <iostream>

class State {
private:
    std::string* name;

public:
    State();
    State(const std::string& n);
    State(const State& other);
    State& operator=(const State& other);
    ~State();

    std::string getName() const;

    friend std::ostream& operator<<(std::ostream& os, const State& s);
};

class Command {
private:
    std::string* command;
    std::string* effect;

public:
    Command(const std::string& cmd);
    Command(const Command& other);
    Command& operator=(const Command& other);
    ~Command();

    std::string getCommand() const;
    std::string getEffect() const;
    void saveEffect(const std::string& e);

    friend std::ostream& operator<<(std::ostream& os, const Command& c);
};

class GameEngine {
private:
    State* currentState;
    std::vector<State*>* states;
    std::vector<Command*>* commands;

    bool validate(const std::string& command);

public:
    GameEngine();
    GameEngine(const GameEngine& other);
    GameEngine& operator=(const GameEngine& other);
    ~GameEngine();

    void transition(const std::string& command);
    State* getCurrentState() const;

    friend std::ostream& operator<<(std::ostream& os, const GameEngine& ge);
};

#endif
