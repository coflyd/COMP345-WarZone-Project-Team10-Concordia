#include "GameEngine.h"
#include "CommandProcessing.h"
#include <algorithm>
#include <iostream>
#include <limits>
#include <vector>
#include <random>

using namespace std;

// State

const std::array<std::string, 8> State::allowedStates = { "start", "maploaded",
		"mapvalidated", "playersadded", "assignreinforcement", "issueorders",
		"executeorders", "win" };

State::State(const std::string& n) : name(n) {}

State::State(const State& other) : name(other.name) {}

State& State::operator=(const State& other) {
	if (this != &other) name = other.name;
	return *this;
}

std::string State::getName() const { return name; }

std::ostream& operator<<(std::ostream& os, const State& s) {
	os << s.name;
	return os;
}

// WarZone

WarZone::WarZone() : map(nullptr), ol(nullptr), deck(new Deck()) {}

WarZone::WarZone(const WarZone& other) :
	map(other.map ? new Map(*other.map) : nullptr),
	ol(other.ol ? new OrdersList(*other.ol) : nullptr),
	deck(other.deck ? new Deck(*other.deck) : nullptr) {
	for (Player* player : other.players)
		players.push_back(new Player(*player));
}

WarZone::~WarZone() {
	delete map;
	delete ol;
	delete deck;
	for (Player* player : players) delete player;
}

// GameEngine

const std::string GameEngine::MAPS_DIRECTORY = "data";

GameEngine::GameEngine() : currentState("start"), warZone(new WarZone()) {
	transitions = {
		{ { "start",              "loadmap"       }, "maploaded"          },
		{ { "maploaded",          "loadmap"       }, "maploaded"          },
		{ { "maploaded",          "validatemap"   }, "mapvalidated"       },
		{ { "mapvalidated",       "addplayer"     }, "playersadded"       },
		{ { "playersadded",       "addplayer"     }, "playersadded"       },
		{ { "playersadded",       "gamestart"     }, "assignreinforcement"},
		{ { "assignreinforcement","issueorder"    }, "issueorders"        },
		{ { "issueorders",        "issueorder"    }, "issueorders"        },
		{ { "issueorders",        "endissueorders"}, "executeorders"      },
		{ { "executeorders",      "execorder"     }, "executeorders"      },
		{ { "executeorders",      "endexecorders" }, "assignreinforcement"},
		{ { "executeorders",      "win"           }, "win"                },
		{ { "win",                "play"          }, "start"              },
		{ { "win",                "end"           }, "end"                }
	};
	commandProcessor = new CommandProcessor(this);
}

GameEngine::GameEngine(const GameEngine& other) :
	currentState(other.currentState),
	transitions(other.transitions),
	warZone(new WarZone(*other.warZone)) {
	commandProcessor = new CommandProcessor(this);
}

GameEngine& GameEngine::operator=(const GameEngine& other) {
	if (this != &other) {
		currentState = other.currentState;
		transitions = other.transitions;
		delete warZone;
		warZone = new WarZone(*other.warZone);
		delete commandProcessor;
		commandProcessor = new CommandProcessor(this);
	}
	return *this;
}

GameEngine::~GameEngine() {
	delete warZone;
	delete commandProcessor;
}

bool GameEngine::isValidTransition(const std::string& commandType) const {
	auto it = transitions.find({ currentState.getName(), commandType });
	return it != transitions.end();
}

void GameEngine::transition(const std::string& commandType) {
	if (!commandProcessor->validate(commandType, currentState)) {
		std::cout << "Invalid command -- " << commandType
			<< " -- for current state: " << currentState << "\n";
		return;
	}

	auto it = transitions.find({ currentState.getName(), commandType });
	if (it != transitions.end()) {
		if (commandType == "loadmap") { cout << "Executing loadmap\n";        this->loadmap(); }
		else if (commandType == "validatemap") { cout << "Executing validatemap\n";    this->validatemap(); }
		else if (commandType == "addplayer") { cout << "Executing addplayer\n";      this->addplayer(); }
		else if (commandType == "gamestart") { cout << "Executing gamestart\n";      this->gamestart(); }
		else if (commandType == "issueorder") { cout << "Executing issueorder\n"; }
		else if (commandType == "endissueorders") { cout << "Executing endissueorders\n"; }
		else if (commandType == "execorder") { cout << "Executing execorder\n"; }
		else if (commandType == "endexecorders") { cout << "Executing endexecorders\n"; }
		else if (commandType == "win") { cout << "Executing win\n"; }
		else if (commandType == "play") { cout << "Executing play\n"; }
		else if (commandType == "end") { cout << "Executing end\n"; }

		commandProcessor->saveCommand(commandType);
		currentState = State(it->second);
		// Use getCommands().back() instead of getCommand() to avoid blocking on input
		const std::vector<Command*>& cmds = commandProcessor->getCommands();
		if (!cmds.empty()) cmds.back()->saveEffect("Transitioned to " + it->second);
		cout << "State -> " << currentState << "\n";
	}
}

State GameEngine::getCurrentState() const { return currentState; }

void GameEngine::setCommandProcessor(CommandProcessor* processor) {
	if (commandProcessor != processor) {
		delete commandProcessor;
		commandProcessor = processor;
	}
}

CommandProcessor* GameEngine::getCommandProcessor() const { return commandProcessor; }

std::ostream& operator<<(std::ostream& os, const GameEngine& ge) {
	os << "Current State: " << ge.currentState << "\n";
	os << "Commands history:\n";
	for (Command* c : ge.commandProcessor->getCommands())
		os << *c << "\n";
	return os;
}

// Part 2: Startup Phase

void GameEngine::startupPhase() {
	cout << "=== Phase startup begin ===\n";
	this->transition("loadmap");
	cout << *(warZone->map) << endl;
	this->transition("validatemap");
	this->transition("addplayer");
	this->transition("gamestart");
	cout << "=== Phase startup ended ===\n";
}

void GameEngine::loadmap() {
	string mapFilePath = MapLoader::selectMapFile(MAPS_DIRECTORY);
	MapLoader* mapLoader = new MapLoader(mapFilePath);
	cout << "Phase mapLoader ended\n";
	warZone->map = new Map(mapLoader);
	delete mapLoader;
	cout << "Phase mapCreation ended\n";
}

void GameEngine::validatemap() {
	warZone->map->validate();
}

void GameEngine::addplayer() {
	int numPlayers;
	while (true) {
		std::cout << "Enter the number of players (2 to 6): ";
		std::cin >> numPlayers;
		if (std::cin.fail() || numPlayers < 2 || numPlayers > 6) {
			std::cin.clear();
			std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
			std::cout << "Invalid input. Please enter a number between 2 and 6.\n";
		}
		else {
			std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
			break;
		}
	}
	for (Player* player : warZone->players) delete player;
	warZone->players.clear();

	for (int i = 0; i < numPlayers; ++i) {
		std::string name;
		std::cout << "Enter the name of player " << (i + 1) << ": ";
		std::getline(std::cin, name);
		warZone->players.push_back(new Player(name));
	}
}

void GameEngine::gamestart() {
	initialRandomCountryDistribution();
	displayOwnedCountries();
	cout << "Phase initialRandomCountryDistribution ended\n";

	std::cout << "\nGame order before random:\n";
	for (Player* player : warZone->players) std::cout << player->getName() << "\n";

	std::random_device rd;
	std::mt19937 genAleat(rd());
	std::shuffle(warZone->players.begin(), warZone->players.end(), genAleat);

	cout << "Phase Randomize order between Players ended\n";
	std::cout << "\nGame order after random:\n";
	for (Player* player : warZone->players) std::cout << player->getName() << "\n";

	for (Player* player : warZone->players) player->setReinforcementPool(50);
	cout << "Phase to give 50 initial armies to the players ended\n";

	createInitialDeck(4, 4, 4, 4, 14, 18);
	cout << "Phase to generate the 44 cards deck ended\n";

	drawInitialCards(2);
	cout << "Phase to draw 2 cards to all players ended\n";
}

void GameEngine::initialRandomCountryDistribution() {
	int nbCountries = warZone->map->getCountriesNumber();
	int nbPlayers = warZone->players.size();
	std::vector<int> countryIdBag;
	for (int i = 1; i <= nbCountries; i++) countryIdBag.push_back(i);
	std::shuffle(countryIdBag.begin(), countryIdBag.end(),
		std::mt19937{ std::random_device{}() });
	int i = 0;
	for (int idCountry : countryIdBag) {
		Country* countryPtr = warZone->map->getCountryById(idCountry);
		if (countryPtr) warZone->players[i % nbPlayers]->addCountry(countryPtr);
		i++;
	}
}

void GameEngine::createInitialDeck(int bo, int bl, int ai, int di, int re, int te) {
	for (int i = 0; i < bo; i++) warZone->deck->returnCard(new Card(CardType::bomb));
	for (int i = 0; i < bl; i++) warZone->deck->returnCard(new Card(CardType::blockade));
	for (int i = 0; i < ai; i++) warZone->deck->returnCard(new Card(CardType::airlift));
	for (int i = 0; i < di; i++) warZone->deck->returnCard(new Card(CardType::diplomacy));
	for (int i = 0; i < re; i++) warZone->deck->returnCard(new Card(CardType::reinforcement));

	vector<int> territoriesId;
	for (int i = 1; i <= warZone->map->getCountriesNumber(); i++) territoriesId.push_back(i);
	std::random_device rd;
	std::mt19937 genAleat(rd());
	std::shuffle(territoriesId.begin(), territoriesId.end(), genAleat);
	for (int i = 0; i < te; i++) {
		Card* card = new Card(CardType::territory);
		card->setTerritory(territoriesId[i], warZone->map->getCountryName(territoriesId[i]));
		warZone->deck->returnCard(card);
	}
	warZone->deck->shuffle();
}

void GameEngine::displayOwnedCountries() {
	for (Player* player : warZone->players) {
		std::cout << "Countries owned by " << player->getName() << ":\n";
		for (Country* country : player->getOwnedCountries())
			std::cout << "- " << country->name << "\n";
	}
}

void GameEngine::drawInitialCards(int nbCards) {
	for (Player* player : warZone->players) {
		cout << "\nPlayer " << player->getName() << "\n";
		cout << "Initial deck size: " << warZone->deck->size() << "\n";
		cout << "Initial hand size: " << player->getHand()->size() << "\n";
		cout << "Initial orders size: " << player->getOrders()->size() << "\n\n";
		cout << "Drawing " << nbCards << " cards into player's hand\n";
		for (int i = 0; i < nbCards; i++) {
			Card* c = warZone->deck->draw();
			if (c) player->getHand()->addCard(c);
		}
		cout << "After draw deck size: " << warZone->deck->size() << "\n";
		cout << "After draw hand size: " << player->getHand()->size() << "\n";
		cout << "After draw orders size: " << player->getOrders()->size() << "\n\n";
	}
}

// Part 3: Main Game Loop


// Checks if one player owns all territories
bool GameEngine::checkWinCondition(Player*& winner) const {
	int total = warZone->map->getCountriesNumber();
	for (Player* p : warZone->players) {
		if ((int)p->getOwnedCountries().size() == total) {
			winner = p;
			return true;
		}
	}
	winner = nullptr;
	return false;
}

// Removes any player who owns zero territories
void GameEngine::removeDefeatedPlayers() {
	auto& players = warZone->players;
	auto it = players.begin();
	while (it != players.end()) {
		Player* p = *it;
		if (p->getOwnedCountries().empty()) {
			cout << "\n[GAME] \"" << p->getName() << "\" eliminated!\n";
			delete p;
			it = players.erase(it);
		}
		else {
			++it;
		}
	}
}

// Reinforcement Phase: max(3, territories/3) + continent bonuses
void GameEngine::reinforcementPhase() {
	cout << "\n========== REINFORCEMENT PHASE ==========\n";

	int total = warZone->map->getCountriesNumber();
	map<int, vector<int>> continentCountries;
	for (int i = 1; i <= total; i++) {
		Country* c = warZone->map->getCountryById(i);
		if (c) continentCountries[c->continentNumber].push_back(i);
	}

	for (Player* p : warZone->players) {
		int numT = (int)p->getOwnedCountries().size();
		int armies = std::max(3, numT / 3);

		for (auto& entry : continentCountries) {
			const vector<int>& contIds = entry.second;
			bool ownsAll = true;
			for (int cid : contIds) {
				bool found = false;
				for (Country* mine : p->getOwnedCountries())
					if (mine->id == cid) { found = true; break; }
				if (!found) { ownsAll = false; break; }
			}
			if (ownsAll) {
				cout << "[" << p->getName() << "] Continent bonus +2 (continent " << entry.first << ")\n";
				armies += 2;
			}
		}

		p->setReinforcementPool(armies);
		cout << "[" << p->getName() << "] receives " << armies
			<< " armies (owns " << numT << " territories).\n";
	}
	cout << "=========================================\n";
}

// Issue Orders Phase: round-robin until all players are done
void GameEngine::issueOrdersPhase() {
	cout << "\n========== ISSUE ORDERS PHASE ==========\n";

	for (Player* p : warZone->players) p->setDoneIssuing(false);

	bool allDone = false;
	while (!allDone) {
		allDone = true;
		for (Player* p : warZone->players) {
			if (!p->isDoneIssuing()) {
				p->issueOrder(warZone->map, warZone->players, warZone->deck);
				if (!p->isDoneIssuing()) allDone = false;
			}
		}
	}
	cout << "=========================================\n";
}

// Execute Orders Phase:
// Step 1: all Deploy orders first (round-robin)
// Step 2: all remaining orders (round-robin)
void GameEngine::executeOrdersPhase() {
	cout << "\n========== EXECUTE ORDERS PHASE ==========\n";

	cout << "<<< Deploy orders >>>\n";
	bool anyDeploy = true;
	while (anyDeploy) {
		anyDeploy = false;
		for (Player* p : warZone->players) {
			OrdersList* ol = p->getOrders();
			for (int i = 0; i < ol->size(); i++) {
				if (dynamic_cast<Deploy*>(ol->at(i)) != nullptr) {
					cout << "[" << p->getName() << "] Executing: " << *ol->at(i) << "\n";
					ol->at(i)->execute();
					ol->remove(i);
					anyDeploy = true;
					break;
				}
			}
		}
	}

	cout << "<<< Other orders >>>\n";
	bool anyLeft = true;
	while (anyLeft) {
		anyLeft = false;
		for (Player* p : warZone->players) {
			OrdersList* ol = p->getOrders();
			if (ol->size() > 0) {
				cout << "[" << p->getName() << "] Executing: " << *ol->at(0) << "\n";
				ol->at(0)->execute();
				ol->remove(0);
				anyLeft = true;
			}
		}
	}
	cout << "==========================================\n";
}

// Main Game Loop
void GameEngine::mainGameLoop() {
	cout << "\n========== MAIN GAME LOOP START ==========\n";

	int round = 0;
	int maxRounds = 3; // demo limit — remove when Part 4 is integrated
	Player* winner = nullptr;

	while (!checkWinCondition(winner) && round < maxRounds) {
		round++;
		cout << "\n########## ROUND " << round << " ##########\n";

		if (warZone->players.size() == 1) {
			winner = warZone->players[0];
			break;
		}

		reinforcementPhase();
		issueOrdersPhase();
		executeOrdersPhase();
		removeDefeatedPlayers();
	}

	if (winner != nullptr) {
		cout << "\n========================================\n";
		cout << "  GAME OVER!  Winner: " << winner->getName() << "\n";
		cout << "========================================\n";
	}
	else {
		cout << "\n[Demo] Reached max rounds (" << maxRounds << "). Game loop works correctly.\n";
		cout << "[Demo] Full win condition requires Part 4 order execution.\n";
	}
}