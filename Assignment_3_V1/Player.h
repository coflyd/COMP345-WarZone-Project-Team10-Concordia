#pragma once
#include <vector>
#include <string>
#include <iostream>
#include "Map.h"
#include "Orders.h"
#include "Cards.h"

class PlayerStrategy;

class Player {
private:
    std::string *name;                     // Player's name
    std::vector<Country*> owned;           // Territories owned by the player
    Hand *hand;                            // Player's hand of cards
    OrdersList *orders;                    // Player's list of orders
    int reinforcementPool;                 // Pool of armies to be deployed
    bool doneIssuing;                      // Tracks if player is done issuing this turn
    std::vector<std::string> trucePlayers; // Active truces for this turn
    PlayerStrategy *playerStrategy;        // Strategy (owned by this player)
    bool conqueredThisTurn;                // Tracks if player captured a territory this turn
    bool attacked;                         // Tracks if this player was attacked this turn

public:
    Player(const std::string &n);
    Player(const std::string &n, PlayerStrategy &playerStrategy);
    Player(const Player &other);
    Player& operator=(const Player &other);
    ~Player();

    // Game methods
    void addCountry(Country *c);
    void removeCountry(Country *c);
    std::vector<Country*> toDefend();
    std::vector<Country*> toAttack(std::vector<Player*> &allPlayers);
    void issueOrder(std::vector<Player*> &allPlayers, Deck *deck);

    // Getters / Setters
    OrdersList* getOrders() const;
    Hand* getHand() const;
    std::string getName() const;
    void setReinforcementPool(int rp);
    int getReinforcementPool() const;
    bool isDoneIssuing() const;
    void setDoneIssuing(bool done);
    const std::vector<Country*>& getOwnedCountries() const;
    int getReinforcements() const;
    void setReinforcements(int amount);

    /*
     * setPlayerStrategy: takes ownership of the given strategy pointer.
     * The old strategy is deleted before the new one is stored.
     * Use this for dynamic strategy changes (e.g. Neutral -> Aggressive).
     */
    void setPlayerStrategy(PlayerStrategy *playerStrategy);

    friend std::ostream& operator<<(std::ostream &out, const Player &p);

    // Truce / conquest / attack flags
    bool hasConqueredThisTurn() const;
    void setConqueredThisTurn(bool val);
    bool isTruceWith(const std::string &playerName) const;
    void addTruce(const std::string &playerName);
    void clearTruces();
    bool getAttacked();
    void setAttacked(bool value);
};
