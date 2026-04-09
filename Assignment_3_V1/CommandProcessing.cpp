#include "CommandProcessing.h"
#include "GameEngine.h"
#include <algorithm>
#include <iostream>
#include <limits>
#include <sstream>
#include <vector>
#include <random>

using namespace std;

// =============================================================================
// Command
// =============================================================================

// "tournament" added for Assignment 3 Part 2
const std::array<std::string, 12> Command::allowedCommands = {
    "loadmap", "validatemap", "addplayer", "gamestart",
    "issueorder", "endissueorders", "execorder", "endexecorders",
    "win", "play", "end", "tournament"
};

Command::Command(const std::string& cmdType)
    : type(new std::string(cmdType)),
      effect(new std::string("")) {
}

Command::Command(const Command& other)
    : type(new std::string(*other.type)),
      effect(new std::string(*other.effect)) {
}

Command& Command::operator=(const Command &other) {
    if (this != &other) {
        delete type;
        delete effect;
        type   = new std::string(*other.type);
        effect = new std::string(*other.effect);
    }
    return *this;
}

Command::~Command() {
    delete type;
    delete effect;
}

std::string Command::getType() const {
    return *type;
}

std::string Command::getEffect() const {
    return *effect;
}

void Command::saveEffect(const std::string &e) {
    *effect = e;
}

std::ostream& operator<<(std::ostream &os, const Command &c) {
    os << "Command: " << *c.type << " | Effect: " << *c.effect;
    return os;
}

// =============================================================================
// CommandProcessor
// =============================================================================

CommandProcessor::CommandProcessor() :
        commands(new std::vector<Command*>()),
        gameEngine(nullptr) {
}

CommandProcessor::CommandProcessor(GameEngine* engine) :
        commands(new std::vector<Command*>()), gameEngine(engine) {
    std::cout << "CommandProcessor created.\n";
}

CommandProcessor::CommandProcessor(const CommandProcessor& other) :
        commands(new std::vector<Command*>()), gameEngine(other.gameEngine) {
    for (Command* cmd : *other.commands)
        commands->push_back(new Command(*cmd));
}

CommandProcessor& CommandProcessor::operator=(const CommandProcessor& other) {
    if (this != &other) {
        gameEngine = other.gameEngine;
        for (Command* cmd : *commands) delete cmd;
        commands->clear();
        for (Command* cmd : *other.commands)
            commands->push_back(new Command(*cmd));
    }
    return *this;
}

CommandProcessor::~CommandProcessor() {
    for (Command* cmd : *commands) delete cmd;
    delete commands;
}

void CommandProcessor::readCommand() {
    std::string input;
    std::cout << "Enter command: ";
    std::getline(std::cin, input);

    if (input.empty()) return;

    // Accept the "tournament" command as a full line (contains spaces and flags)
    // Check if it starts with "tournament"
    std::string firstWord = input.substr(0, input.find(' '));
    if (firstWord == "tournament") {
        commands->push_back(new Command(input)); // store full command string
        return;
    }

    if (std::find(Command::allowedCommands.begin(),
                  Command::allowedCommands.end(), input)
            == Command::allowedCommands.end()) {
        std::cout << "Invalid command: \"" << input << "\", try again.\n";
    } else {
        saveCommand(input);
    }
}

bool CommandProcessor::saveCommand(const std::string& commandType) {
    // For the tournament command the type is the full command string;
    // we only validate the first word against allowedCommands.
    std::string firstWord = commandType.substr(0, commandType.find(' '));
    if (std::find(Command::allowedCommands.begin(),
                  Command::allowedCommands.end(), firstWord)
            == Command::allowedCommands.end()) {
        return false;
    }
    commands->push_back(new Command(commandType));
    return true;
}

Command* CommandProcessor::getCommand() {
    size_t sizeBefore = commands->size();
    while (commands->size() == sizeBefore) {
        readCommand();
    }
    return commands->back();
}

bool CommandProcessor::validate(const std::string &commandType,
        const State &currentState) const {
    // For tournament: validate only the first word
    std::string firstWord = commandType.substr(0, commandType.find(' '));
    if (std::find(Command::allowedCommands.begin(),
                  Command::allowedCommands.end(), firstWord)
            == Command::allowedCommands.end()) {
        return false;
    }
    if (gameEngine != nullptr && !gameEngine->isValidTransition(firstWord)) {
        return false;
    }
    return true;
}

const std::vector<Command*>& CommandProcessor::getCommands() const {
    return *commands;
}

void CommandProcessor::setGameEngine(GameEngine* engine) {
    gameEngine = engine;
}

std::ostream& operator<<(std::ostream& os, const CommandProcessor& cp) {
    os << "CommandProcessor [" << cp.commands->size() << " commands]:\n";
    for (Command* c : *cp.commands)
        os << "  " << *c << "\n";
    return os;
}

// =============================================================================
// parseTournamentCommand  (Assignment 3 Part 2)
// =============================================================================

/*
 * Parses a tournament command of the form:
 *   tournament -M <map1> ... -P <strat1> ... -G <n> -D <n>
 *
 * Validation rules:
 *   M: 1-5 map files
 *   P: 2-4 computer player strategies (Human not allowed)
 *   G: 1-5 games per map
 *   D: 10-50 max turns per game
 *
 * Returns true and fills out-parameters if valid; false otherwise.
 */
bool parseTournamentCommand(const std::string &cmd,
        std::vector<std::string> &maps,
        std::vector<std::string> &strategies,
        int &numGames,
        int &maxTurns) {

    maps.clear();
    strategies.clear();
    numGames = 0;
    maxTurns = 0;

    std::istringstream ss(cmd);
    std::string token;
    ss >> token; // consume "tournament"
    if (token != "tournament") {
        std::cerr << "[Tournament] Command must start with 'tournament'.\n";
        return false;
    }

    std::string currentFlag;
    while (ss >> token) {
        if (token == "-M" || token == "-P" || token == "-G" || token == "-D") {
            currentFlag = token;
        } else {
            if (currentFlag == "-M") {
                maps.push_back(token);
            } else if (currentFlag == "-P") {
                strategies.push_back(token);
            } else if (currentFlag == "-G") {
                try { numGames = std::stoi(token); }
                catch (...) { numGames = 0; }
            } else if (currentFlag == "-D") {
                try { maxTurns = std::stoi(token); }
                catch (...) { maxTurns = 0; }
            } else {
                std::cerr << "[Tournament] Unexpected token before any flag: "
                          << token << "\n";
                return false;
            }
        }
    }

    // Validate ranges
    if (maps.empty() || maps.size() > 5) {
        std::cerr << "[Tournament] -M: must specify 1 to 5 map files (got "
                  << maps.size() << ").\n";
        return false;
    }
    if (strategies.size() < 2 || strategies.size() > 4) {
        std::cerr << "[Tournament] -P: must specify 2 to 4 strategies (got "
                  << strategies.size() << ").\n";
        return false;
    }
    if (numGames < 1 || numGames > 5) {
        std::cerr << "[Tournament] -G: must be 1 to 5 (got " << numGames << ").\n";
        return false;
    }
    if (maxTurns < 10 || maxTurns > 50) {
        std::cerr << "[Tournament] -D: must be 10 to 50 (got " << maxTurns << ").\n";
        return false;
    }
    for (const std::string &s : strategies) {
        if (s == "Human") {
            std::cerr << "[Tournament] Human strategy not allowed in tournament mode.\n";
            return false;
        }
    }
    return true;
}

// =============================================================================
// FileLineReader
// =============================================================================

FileLineReader::FileLineReader(const std::string& fn)
    : filename(new std::string(fn)),
      linesRead(new int(0)) {
    file.open(fn);
    if (!file.is_open())
        throw std::runtime_error("FileLineReader: cannot open file: " + fn);
}

FileLineReader::FileLineReader(const FileLineReader& other)
    : filename(new std::string(*other.filename)),
      linesRead(new int(0)) {
    file.open(*other.filename);
    if (!file.is_open())
        throw std::runtime_error(
            "FileLineReader copy: cannot reopen: " + *other.filename);
    std::string tmp;
    while (*linesRead < *other.linesRead && std::getline(file, tmp))
        ++(*linesRead);
}

FileLineReader& FileLineReader::operator=(const FileLineReader& other) {
    if (this != &other) {
        if (file.is_open()) file.close();
        delete filename;
        delete linesRead;

        filename  = new std::string(*other.filename);
        linesRead = new int(0);

        file.open(*other.filename);
        if (!file.is_open())
            throw std::runtime_error(
                "FileLineReader assign: cannot reopen: " + *other.filename);
        std::string tmp;
        while (*linesRead < *other.linesRead && std::getline(file, tmp))
            ++(*linesRead);
    }
    return *this;
}

FileLineReader::~FileLineReader() {
    if (file.is_open()) file.close();
    delete filename;
    delete linesRead;
}

std::string FileLineReader::readLine() {
    std::string line;
    while (std::getline(file, line)) {
        ++(*linesRead);
        if (!line.empty()) return line;
    }
    return "";
}

bool FileLineReader::isOpen() const {
    return file.is_open();
}

const std::ifstream* FileLineReader::getFile() const {
    return &file;
}

std::ostream& operator<<(std::ostream& os, const FileLineReader& flr) {
    os << "FileLineReader[\"" << *flr.filename
       << "\", linesRead=" << *flr.linesRead << "]";
    return os;
}

// =============================================================================
// FileCommandProcessorAdapter
// =============================================================================

FileCommandProcessorAdapter::FileCommandProcessorAdapter(
        const std::string &filename) :
        CommandProcessor(), fileReader(new FileLineReader(filename)) {
}

FileCommandProcessorAdapter::FileCommandProcessorAdapter(
        const FileCommandProcessorAdapter& other) :
        CommandProcessor(other),
        fileReader(new FileLineReader(*other.fileReader)) {
}

FileCommandProcessorAdapter& FileCommandProcessorAdapter::operator=(
        const FileCommandProcessorAdapter& other) {
    if (this != &other) {
        CommandProcessor::operator=(other);
        delete fileReader;
        fileReader = new FileLineReader(*other.fileReader);
    }
    return *this;
}

FileCommandProcessorAdapter::~FileCommandProcessorAdapter() {
    delete fileReader;
}

void FileCommandProcessorAdapter::readCommand() {
    std::string line = fileReader->readLine();
    if (line.empty()) return;

    // Accept full tournament command lines from file
    std::string firstWord = line.substr(0, line.find(' '));
    if (firstWord == "tournament") {
        commands->push_back(new Command(line));
        return;
    }

    if (std::find(Command::allowedCommands.begin(),
                  Command::allowedCommands.end(), line)
            != Command::allowedCommands.end()) {
        saveCommand(line);
    } else {
        std::cout << "[FileAdapter] Unknown command ignored: \"" << line << "\"\n";
    }
}

bool FileCommandProcessorAdapter::hasMoreCommands() const {
    return fileReader->isOpen() && !fileReader->getFile()->eof();
}

std::ostream& operator<<(std::ostream& os, const FileCommandProcessorAdapter& a) {
    os << "FileCommandProcessorAdapter[" << *a.fileReader << "]";
    return os;
}
