#pragma once
#include <vector>
#include <string>
#include <iostream>
#include "Map.h"
#include "Orders.h"
#include "Cards.h"

class Player {
private:
	std::string* name;              // Player's name
	std::vector<Country*> owned;    // Territories owned by the player
	Hand* hand;                     // Player's hand of cards
	OrdersList* orders;             // Player's list of orders
	int reinforcementPool;          // Pool of armies to be deployed
	bool doneIssuing;               // Part 3 tracks if player is done issuing this turn
	std::vector<std::string> trucePlayers; // Part 4 stores active truces for this turn
	bool conqueredThisTurn = false;        // Part 4 tracks if player captured a territory this turn

public:
	// Constructors and destructor
	Player(const std::string& n);
	Player(const Player& other);
	Player& operator=(const Player& other);
	~Player();

	// Game methods
	void addCountry(Country* c);
	void removeCountry(Country* c);                                          // Part 3
	std::vector<Country*> toDefend();
	std::vector<Country*> toAttack(std::vector<Player*>& allPlayers);       // Part 3
	void issueOrder(Map* map, std::vector<Player*>& allPlayers, Deck* deck); // Part 3

	// Getters / Setters
	OrdersList* getOrders() const;
	Hand* getHand() const;
	std::string getName() const;
	void setReinforcementPool(int rp);
	int getReinforcementPool() const;   // Part 3
	bool isDoneIssuing() const;         // Part 3
	void setDoneIssuing(bool done);     // Part 3
	const std::vector<Country*>& getOwnedCountries() const;
	int getReinforcements() const;      // Part 4
	void setReinforcements(int amount); // Part 4

	// Stream insertion operator
	friend std::ostream& operator<<(std::ostream& out, const Player& p);

	// Functions used by Part 4
	bool hasConqueredThisTurn() const;
	void setConqueredThisTurn(bool val);
	bool isTruceWith(const std::string& playerName) const;
	void addTruce(const std::string& playerName);
	void clearTruces();
};
