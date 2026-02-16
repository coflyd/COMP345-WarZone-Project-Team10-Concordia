#include "GameEngine.h"


// State 

State::State() {
    name = new std::string("start");
}

State::State(const std::string& n) {
    name = new std::string(n);
}

State::State(const State& other) {
    name = new std::string(*other.name);
}

State& State::operator=(const State& other) {
    if (this != &other) {
        delete name;
        name = new std::string(*other.name);
    }
    return *this;
}

State::~State() {
    delete name;
}

std::string State::getName() const {
    return *name;
}

std::ostream& operator<<(std::ostream& os, const State& s) {
    os << *s.name;
    return os;
}



// Command

Command::Command(const std::string& cmd) {
    command = new std::string(cmd);
    effect = new std::string("");
}

Command::Command(const Command& other) {
    command = new std::string(*other.command);
    effect = new std::string(*other.effect);
}

Command& Command::operator=(const Command& other) {
    if (this != &other) {
        delete command;
        delete effect;
        command = new std::string(*other.command);
        effect = new std::string(*other.effect);
    }
    return *this;
}

Command::~Command() {
    delete command;
    delete effect;
}

std::string Command::getCommand() const {
    return *command;
}

std::string Command::getEffect() const {
    return *effect;
}

void Command::saveEffect(const std::string& e) {
    *effect = e;
}

std::ostream& operator<<(std::ostream& os, const Command& c) {
    os << "Command: " << c.getCommand()
       << " | Effect: " << c.getEffect();
    return os;
}




GameEngine::GameEngine() {
    states = new std::vector<State*>;
    commands = new std::vector<Command*>;
    currentState = new State("start");
}

GameEngine::GameEngine(const GameEngine& other) {
    states = new std::vector<State*>;
    commands = new std::vector<Command*>;
    currentState = new State(*other.currentState);
    for (State* s : *other.states)
        states->push_back(new State(*s));
    for (Command* c : *other.commands)
        commands->push_back(new Command(*c));
}

GameEngine& GameEngine::operator=(const GameEngine& other) {
    if (this != &other) {
        delete currentState;
        for (State* s : *states)
            delete s;
        delete states;
        for (Command* c : *commands)
            delete c;
        delete commands;
        states = new std::vector<State*>;
        commands = new std::vector<Command*>;
        currentState = new State(*other.currentState);
        for (State* s : *other.states)
            states->push_back(new State(*s));
        for (Command* c : *other.commands)
            commands->push_back(new Command(*c));
    }
    return *this;
}

GameEngine::~GameEngine() {
    delete currentState;
    for (State* s : *states)
        delete s;
    delete states;
    for (Command* c : *commands)
        delete c;
    delete commands;
}



bool GameEngine::validate(const std::string& cmd) {
    std::string state = currentState->getName();
    if (state == "start" && cmd == "loadmap") return true;
    if (state == "maploaded" && cmd == "validatemap") return true;
    if (state == "mapvalidated" && cmd == "addplayer") return true;
    if (state == "playersadded" && cmd == "assigncountries") return true;
    if (state == "assignreinforcement" && cmd == "issueorder") return true;
    if (state == "issueorders" && cmd == "endissueorders") return true;
    if (state == "executeorders" && cmd == "endexecorders") return true;
    if (state == "executeorders" && cmd == "win") return true;
    if (state == "win" && cmd == "play") return true;
    if (cmd == "end") return true;
    return false;
}


// Transition

void GameEngine::transition(const std::string& cmd) {

    Command* newCommand = new Command(cmd);
    commands->push_back(newCommand);
    if (!validate(cmd)) {
        std::cout << "Invalid command for current state.\n";
        newCommand->saveEffect("Rejected");
        return;
    }
    std::string state = currentState->getName();
    if (cmd == "loadmap")
        currentState = new State("maploaded");
    else if (cmd == "validatemap")
        currentState = new State("mapvalidated");
    else if (cmd == "addplayer")
        currentState = new State("playersadded");
    else if (cmd == "assigncountries")
        currentState = new State("assignreinforcement");
    else if (cmd == "issueorder")
        currentState = new State("issueorders");
    else if (cmd == "endissueorders")
        currentState = new State("executeorders");
    else if (cmd == "win")
        currentState = new State("win");
    else if (cmd == "play")
        currentState = new State("start");
    else if (cmd == "end")
        currentState = new State("end");
    newCommand->saveEffect("Transitioned to " + currentState->getName());
}


State* GameEngine::getCurrentState() const {
    return currentState;
}

std::ostream& operator<<(std::ostream& os, const GameEngine& ge) {
    os << "Current State: " << *ge.currentState << "\n";
    os << "Commands history:\n";
    for (Command* c : *ge.commands)
        os << *c << "\n";
    return os;
}
