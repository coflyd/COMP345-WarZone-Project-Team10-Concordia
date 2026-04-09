#include "Player.h"
#include "PlayerStrategy.h"
#include <algorithm>

// Constructor: initializes player with a name and empty collections
Player::Player(const std::string &n) {
    name             = new std::string(n);
    hand             = new Hand();
    orders           = new OrdersList();
    reinforcementPool = 0;
    doneIssuing      = false;
    conqueredThisTurn = false;
    playerStrategy   = nullptr; // initialized to nullptr so delete is safe
}

// Constructor with a strategy
Player::Player(const std::string &n, PlayerStrategy &ps) {
    name             = new std::string(n);
    hand             = new Hand();
    orders           = new OrdersList();
    reinforcementPool = 0;
    doneIssuing      = false;
    conqueredThisTurn = false;
    playerStrategy   = &ps;
}

// Copy constructor: creates a deep copy of another player.
// Note: playerStrategy is shallow-copied (shared pointer) because strategies
// hold a back-pointer to their player and deep-copying would break that link.
Player::Player(const Player &other) {
    name = new std::string(*other.name);
    for (Country *country : other.owned)
        owned.push_back(new Country(*country));
    hand             = new Hand(*other.hand);
    orders           = new OrdersList(*other.orders);
    reinforcementPool = other.reinforcementPool;
    doneIssuing      = other.doneIssuing;
    conqueredThisTurn = other.conqueredThisTurn;
    trucePlayers     = other.trucePlayers;
    playerStrategy   = other.playerStrategy; // shallow copy — see note above
}

// Assignment operator
Player& Player::operator=(const Player &other) {
    if (this == &other) return *this;

    delete name;
    for (Country *country : owned) delete country;
    owned.clear();
    delete hand;
    delete orders;
    // Do NOT delete playerStrategy here — it is shallow-copied, not owned
    // in the copy path. Only setPlayerStrategy() deletes the old strategy.

    name = new std::string(*other.name);
    for (Country *country : other.owned)
        owned.push_back(new Country(*country));
    hand             = new Hand(*other.hand);
    orders           = new OrdersList(*other.orders);
    reinforcementPool = other.reinforcementPool;
    doneIssuing      = other.doneIssuing;
    conqueredThisTurn = other.conqueredThisTurn;
    trucePlayers     = other.trucePlayers;
    playerStrategy   = other.playerStrategy; // shallow copy
    return *this;
}

// Destructor: frees all dynamically allocated memory.
// playerStrategy is deleted here only when the player owns it exclusively
// (i.e. it was set via setPlayerStrategy). Strategies set via the constructor
// that takes a reference (&) are NOT deleted here — the caller owns them.
// For safety we always delete it; callers that pass a reference must ensure
// the strategy outlives the player OR transfer ownership via setPlayerStrategy.
Player::~Player() {
    delete name;
    // Note: owned territories are NOT deleted — the Map owns them
    delete hand;
    delete orders;
    delete playerStrategy; // safe because initialized to nullptr
}

// Adds a country to the player's list of owned territories
void Player::addCountry(Country *c) {
    owned.push_back(c);
}

// Removes a country from this player's owned list (does not delete it)
void Player::removeCountry(Country *c) {
    owned.erase(std::remove(owned.begin(), owned.end(), c), owned.end());
}

// Delegates to strategy
std::vector<Country*> Player::toDefend() {
    return playerStrategy->toDefend();
}

// Delegates to strategy
std::vector<Country*> Player::toAttack(std::vector<Player*> &allPlayers) {
    return playerStrategy->toAttack(allPlayers);
}

// Delegates to strategy
void Player::issueOrder(std::vector<Player*> &allPlayers, Deck *deck) {
    playerStrategy->issueOrder(allPlayers, deck);
}

OrdersList* Player::getOrders() const { return orders; }

std::string Player::getName() const { return *name; }

Hand* Player::getHand() const { return hand; }

void Player::setReinforcementPool(int rp) { reinforcementPool = rp; }

int Player::getReinforcementPool() const { return reinforcementPool; }

bool Player::isDoneIssuing() const { return doneIssuing; }

void Player::setDoneIssuing(bool done) { doneIssuing = done; }

const std::vector<Country*>& Player::getOwnedCountries() const { return owned; }

std::ostream& operator<<(std::ostream &out, const Player &p) {
    out << "Player: " << *p.name
        << " | Countries: " << p.owned.size()
        << " | Reinforcements: " << p.reinforcementPool;
    return out;
}

/*
 * setPlayerStrategy: replaces the current strategy.
 * Deletes the old strategy first to prevent memory leaks when strategies
 * are swapped dynamically (e.g. Neutral -> Aggressive on attack).
 */
void Player::setPlayerStrategy(PlayerStrategy *ps) {
    delete playerStrategy; // safe because initialized to nullptr
    playerStrategy = ps;
}

// Part 4 additions
int Player::getReinforcements() const { return reinforcementPool; }

void Player::setReinforcements(int amount) { reinforcementPool = amount; }

bool Player::hasConqueredThisTurn() const { return conqueredThisTurn; }

void Player::setConqueredThisTurn(bool val) { conqueredThisTurn = val; }

bool Player::isTruceWith(const std::string &p) const {
    for (const std::string &t : trucePlayers)
        if (t == p) return true;
    return false;
}

void Player::addTruce(const std::string &p) { trucePlayers.push_back(p); }

void Player::clearTruces() { trucePlayers.clear(); }

bool Player::getAttacked() { return attacked; }

void Player::setAttacked(bool value) { attacked = value; }
