#ifndef COMMANDPROCESSING_H
#define COMMANDPROCESSING_H

#include <string>
#include <vector>
#include <fstream>
#include <array>
#include <iostream>
#include "LoggingObserver.h"

class GameEngine;
class State;


class Command : public Subject, public ILoggable {
private:
    std::string* type;      
    std::string* effect;    

public:
    static const std::array<std::string, 11> allowedCommands;

	explicit Command(const std::string& cmdType);
    Command(const Command& other);
    Command& operator=(const Command& other);
    ~Command();  

	std::string getType() const;
	std::string getEffect() const;
	void saveEffect(const std::string &e);
	std::string stringToLog() const override;

	friend std::ostream& operator<<(std::ostream &os, const Command &c);

};

class CommandProcessor : public Subject, public ILoggable {
protected:
	std::vector<Command*>* commands;
	GameEngine *gameEngine;
	std::string lastCommandSaved;
	virtual void readCommand();
    bool saveCommand(const std::string& commandType);
public:
	CommandProcessor();
	CommandProcessor(GameEngine *engine);
	CommandProcessor(const CommandProcessor &other);
	CommandProcessor& operator=(const CommandProcessor &other);
	virtual ~CommandProcessor();

	Command* getCommand();
	bool validate(const std::string &commandType,
			const State &currentState) const;
	const std::vector<Command*>& getCommands() const;
    void setGameEngine(GameEngine* engine);
    std::string stringToLog() const override;

	friend std::ostream& operator<<(std::ostream &os,
			const CommandProcessor &cp);
    friend class GameEngine;
};

class FileLineReader {
private:
    std::string*   filename;
	std::ifstream file;
    int* linesRead;
public:
	explicit FileLineReader(const std::string& filename);
    FileLineReader(const FileLineReader& other);
    FileLineReader& operator=(const FileLineReader& other);
	~FileLineReader();

	std::string readLine();
    bool isOpen() const;
	const std::ifstream* getFile() const;

    friend std::ostream& operator<<(std::ostream& os, const FileLineReader& flr);
};

class FileCommandProcessorAdapter: public CommandProcessor {
private:
	FileLineReader *fileReader;
protected:
    void readCommand() override;
public:
	 explicit FileCommandProcessorAdapter(const std::string& filename);
    FileCommandProcessorAdapter(const FileCommandProcessorAdapter& other);
    FileCommandProcessorAdapter& operator=(const FileCommandProcessorAdapter& other);
    ~FileCommandProcessorAdapter() override;

	bool hasMoreCommands() const;
 
    friend std::ostream& operator<<(std::ostream& os, const FileCommandProcessorAdapter& a);
};

#endif
