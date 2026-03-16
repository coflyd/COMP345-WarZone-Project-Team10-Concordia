#include "CommandProcessing.h"
#include "GameEngine.h"
#include <algorithm>
#include <iostream>
#include <limits>
#include <vector>
#include <random>

using namespace std;

//Command

const std::array<std::string, 11> Command::allowedCommands = { "loadmap",
		"validatemap", "addplayer", "gamestart", "issueorder", "endissueorders",
		"execorder","endexecorders", "win", "play", "end" };

Command::Command(const std::string& cmdType)
    : type(new std::string(cmdType)),
      effect(new std::string("")) {
	  }

Command::Command(const Command& other)
    : Subject(other),
      type(new std::string(*other.type)),
      effect(new std::string(*other.effect)) {
}

Command& Command::operator=(const Command &other) {
	if (this != &other) {
        Subject::operator=(other);
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
	Notify(this);
}

std::string Command::stringToLog() const {
	return "[Command] " + *type + " -> " + *effect;
}

std::ostream& operator<<(std::ostream &os, const Command &c) {
	os << "Command: " << *c.type << " | Effect: " << *c.effect;
	return os;
}

// CommandProcessor

CommandProcessor::CommandProcessor() :
		commands(new std::vector<Command*>()),
		gameEngine(nullptr),
		lastCommandSaved(""){
}

CommandProcessor::CommandProcessor(GameEngine* engine)
    : commands(new std::vector<Command*>()),
      gameEngine(engine),
      lastCommandSaved("")
{
    std::cout << "CommandProcessor created.\n";
}

CommandProcessor::CommandProcessor(const CommandProcessor& other)
    : Subject(other),
      commands(new std::vector<Command*>()),
      gameEngine(other.gameEngine),
      lastCommandSaved(other.lastCommandSaved)
{
    for (Command* cmd : *other.commands) {
        commands->push_back(new Command(*cmd));
    }
}

CommandProcessor& CommandProcessor::operator=(const CommandProcessor& other) {
    if (this != &other) {
        Subject::operator=(other);
        gameEngine = other.gameEngine;
        lastCommandSaved = other.lastCommandSaved;
 
        for (Command* cmd : *commands) delete cmd;
        commands->clear();
 
        for (Command* cmd : *other.commands) {
            commands->push_back(new Command(*cmd));
        }
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
 
    if (std::find(Command::allowedCommands.begin(),
                  Command::allowedCommands.end(), input)
            == Command::allowedCommands.end()) {
        std::cout << "Invalid command: \"" << input << "\", try again.\n";
    } else {
        saveCommand(input);
    }
}

bool CommandProcessor::saveCommand(const std::string& commandType) {
    if (std::find(Command::allowedCommands.begin(),
                  Command::allowedCommands.end(), commandType)
            == Command::allowedCommands.end()) {
        return false;
    }
    commands->push_back(new Command(commandType));
    lastCommandSaved = commandType;
    Notify(this);
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
	if (std::find(Command::allowedCommands.begin(),
			Command::allowedCommands.end(), commandType)
			== Command::allowedCommands.end()) {
		return false;
	}
	if (gameEngine != nullptr && !gameEngine->isValidTransition(commandType)) {
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

std::string CommandProcessor::stringToLog() const {
    return "[CommandProcessor] Saved command: " + lastCommandSaved;
}

std::ostream& operator<<(std::ostream& os, const CommandProcessor& cp) {
    os << "CommandProcessor [" << cp.commands->size() << " commands]:\n";
    for (Command* c : *cp.commands) {
        os << "  " << *c << "\n";
    }
    return os;
}

// FileLineReader

FileLineReader::FileLineReader(const std::string& fn)
    : filename(new std::string(fn)),
      linesRead(new int(0))
{
    file.open(fn);
    if (!file.is_open()) {
        throw std::runtime_error("FileLineReader: cannot open file: " + fn);
    }
}

FileLineReader::FileLineReader(const FileLineReader& other)
    : filename(new std::string(*other.filename)),
      linesRead(new int(0))
{
    file.open(*other.filename);
    if (!file.is_open()) {
        throw std::runtime_error(
            "FileLineReader copy: cannot reopen: " + *other.filename);
    }
    std::string tmp;
    while (*linesRead < *other.linesRead && std::getline(file, tmp)) {
        ++(*linesRead);
    }
}

FileLineReader& FileLineReader::operator=(const FileLineReader& other) {
    if (this != &other) {
        if (file.is_open()) file.close();
        delete filename;
        delete linesRead;
 
        filename  = new std::string(*other.filename);
        linesRead = new int(0);

        file.open(*other.filename);
        if (!file.is_open()) {
            throw std::runtime_error(
                "FileLineReader assign: cannot reopen: " + *other.filename);
        }
        std::string tmp;
        while (*linesRead < *other.linesRead && std::getline(file, tmp)) {
            ++(*linesRead);
        }
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

// FileCommandProcessorAdapter

FileCommandProcessorAdapter::FileCommandProcessorAdapter(
		const std::string &filename) :
		CommandProcessor(), fileReader(new FileLineReader(filename)) {
}

FileCommandProcessorAdapter::FileCommandProcessorAdapter(
        const FileCommandProcessorAdapter& other)
    : CommandProcessor(other),
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
