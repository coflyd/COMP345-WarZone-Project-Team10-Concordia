#ifndef GAMEENGINE_H
#define GAMEENGINE_H

#include <string>
#include <vector>
#include <map>
#include <array>
#include "Map.h"
#include "Player.h"
#include "CommandProcessing.h"  // Command + CommandProcessor

// WarZone: holds the entire game state
struct WarZone {
	Map* map;
	std::vector<Player*> players;
	OrdersList* ol;
	Deck* deck;
	WarZone();
	WarZone(const WarZone& other);
	~WarZone();
};

// State: represents a game phase (start, maploaded, etc.)
class State {
private:
	std::string name;

public:
	static const std::array<std::string, 8> allowedStates;

	State(const std::string& n);
	State(const State& other);
	State& operator=(const State& other);
	~State() = default;

	std::string getName() const;
	friend std::ostream& operator<<(std::ostream& os, const State& s);
};

// GameEngine: controls game flow
class GameEngine {
private:
	static const std::string MAPS_DIRECTORY;
	State currentState;
	std::map<std::pair<std::string, std::string>, std::string> transitions;
	WarZone* warZone;

public:
	CommandProcessor* commandProcessor;

	GameEngine();
	GameEngine(const GameEngine& other);
	GameEngine& operator=(const GameEngine& other);
	~GameEngine();

	// Parts 1 & 2
	void setCommandProcessor(CommandProcessor* processor);
	void transition(const std::string& commandType);
	State getCurrentState() const;
	CommandProcessor* getCommandProcessor() const;
	bool isValidTransition(const std::string& commandType) const;
	void startupPhase();
	void loadmap();
	void validatemap();
	void addplayer();
	void gamestart();
	void initialRandomCountryDistribution();
	void createInitialDeck(int bo, int bl, int ai, int di, int re, int te);
	void displayOwnedCountries();
	void drawInitialCards(int nbCards);

	// Part 3: Main Game Loop
	void mainGameLoop();
	void reinforcementPhase();
	void issueOrdersPhase();
	void executeOrdersPhase();
	bool checkWinCondition(Player*& winner) const;
	void removeDefeatedPlayers();

	friend std::ostream& operator<<(std::ostream& os, const GameEngine& ge);
};

#endif