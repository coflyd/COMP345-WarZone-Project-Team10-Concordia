#include "Player.h"
#include <algorithm>

// Constructor: initializes player with a name and empty collections
Player::Player(const std::string& n) {
	name = new std::string(n);
	hand = new Hand();
	orders = new OrdersList();
	reinforcementPool = 0;
	doneIssuing = false; // Part 3
	conqueredThisTurn = false; // Part 4
}

// Copy constructor: creates a deep copy of another player
Player::Player(const Player& other) {
	name = new std::string(*other.name);
	for (Country* country : other.owned) {
		owned.push_back(new Country(*country));
	}
	hand = new Hand(*other.hand);
	orders = new OrdersList(*other.orders);
	reinforcementPool = other.reinforcementPool;
	doneIssuing = other.doneIssuing; // Part 3
	conqueredThisTurn = other.conqueredThisTurn; // Part 4
	trucePlayers = other.trucePlayers; // Part 4
}

// Assignment operator: deep copy with cleanup of existing data
Player& Player::operator=(const Player& other) {
	if (this == &other)
		return *this;

	delete name;
	for (Country* country : owned) {
		delete country;
	}
	owned.clear();
	delete hand;
	delete orders;

	name = new std::string(*other.name);
	for (Country* country : other.owned) {
		owned.push_back(new Country(*country));
	}
	hand = new Hand(*other.hand);
	orders = new OrdersList(*other.orders);
	reinforcementPool = other.reinforcementPool;
	doneIssuing = other.doneIssuing; // Part 3
	conqueredThisTurn = other.conqueredThisTurn; // Part 4
	trucePlayers = other.trucePlayers; // Part 4
	return *this;
}

// Destructor: frees all dynamically allocated memory
Player::~Player() {
	delete name;
	// Note: owned territories are NOT deleted here because the Map owns them
	delete hand;
	delete orders;
}

// Adds a country to the player's list of owned territories
void Player::addCountry(Country* c) {
	owned.push_back(c);
}

// Part 3: Removes a country from this player's owned list (does not delete it)
void Player::removeCountry(Country* c) {
	owned.erase(std::remove(owned.begin(), owned.end(), c), owned.end());
}

// Returns list of territories to defend (weakest first - fewest armies)
std::vector<Country*> Player::toDefend() {
	std::vector<Country*> defend = owned;
	std::sort(defend.begin(), defend.end(), [](Country* a, Country* b) {
		return a->armies < b->armies;
		});
	return defend;
}

// Part 3 Returns list of enemy territories adjacent to any of this player's territories
// For now uses a simple heuristic: first territory of each other player
std::vector<Country*> Player::toAttack(std::vector<Player*>& allPlayers) {
	std::vector<Country*> targets;
	for (Player* other : allPlayers) {
		if (other == this) continue;
		const std::vector<Country*>& enemyOwned = other->getOwnedCountries();
		for (Country* c : enemyOwned) {
			targets.push_back(c);
		}
	}
	return targets;
}

// Part 3 Full issueOrder with real decision logic
// Phase A Deploy all armies first (no other orders while pool > 0)
// Phase B Advance to defend (move armies between own territories)
// Phase C Advance to attack (move armies to enemy territory)
// Phase D Play one card from hand
// Then mark doneIssuing = true
void Player::issueOrder(Map* map, std::vector<Player*>& allPlayers, Deck* deck) {

	// Phase A Deploy
	if (reinforcementPool > 0) {
		std::vector<Country*> defend = toDefend();
		if (!defend.empty()) {
			Country* target = defend[0]; // weakest territory
			int armiesToDeploy = reinforcementPool;

			// Add armies to territory
			target->armies += armiesToDeploy;

			orders->add(new Deploy(target->name, armiesToDeploy));
			std::cout << "[" << *name << "] Deploy order: "
				<< armiesToDeploy << " armies -> " << target->name << "\n";

			reinforcementPool = 0;
		}
		// Must finish deploying before any other order
		return;
	}

	// Phase B Advance to defend
	std::vector<Country*> defend = toDefend();
	if (defend.size() >= 2) {
		Country* weakest = defend[0];
		Country* strongest = defend[defend.size() - 1];
		int available = strongest->armies;
		if (available > 1) {
			int toMove = available / 2;
			orders->add(new Advance(strongest->name, weakest->name, toMove));
			std::cout << "[" << *name << "] Advance (defend): "
				<< toMove << " armies " << strongest->name
				<< " -> " << weakest->name << "\n";
		}
	}

	// Phase C Advance to attack
	std::vector<Country*> attackTargets = toAttack(allPlayers);
	if (!attackTargets.empty()) {
		// Find our strongest territory to attack from
		Country* source = nullptr;
		int maxArmies = 0;
		for (Country* mine : owned) {
			int armies = mine->armies;
			if (armies > maxArmies) {
				maxArmies = armies;
				source = mine;
			}
		}
		if (source != nullptr && maxArmies > 1) {
			Country* target = attackTargets[0];
			int toSend = maxArmies - 1; // keep 1 army back
			orders->add(new Advance(source->name, target->name, toSend));
			std::cout << "[" << *name << "] Advance (attack): "
				<< toSend << " armies " << source->name
				<< " -> " << target->name << "\n";
		}
	}

	// Phase D Play one card
	const std::vector<Card*>* cards = hand->getCards();
	if (cards != nullptr && !cards->empty() && deck != nullptr) {
		Card* card = (*cards)[0];
		std::cout << "[" << *name << "] Playing card: " << *card << "\n";
		card->play(this, deck);
	}

	// Done issuing for this turn
	doneIssuing = true;
	std::cout << "[" << *name << "] Done issuing orders.\n";
}

// Returns the player's list of orders
OrdersList* Player::getOrders() const {
	return orders;
}

// Returns the player's name
std::string Player::getName() const {
	return *name;
}

// Returns the player's hand of cards
Hand* Player::getHand() const {
	return hand;
}

void Player::setReinforcementPool(int rp) {
	this->reinforcementPool = rp;
}

// Part 3 getter for reinforcement pool
int Player::getReinforcementPool() const {
	return reinforcementPool;
}

// Part 3 done issuing flag getters/setters
bool Player::isDoneIssuing() const {
	return doneIssuing;
}

void Player::setDoneIssuing(bool done) {
	doneIssuing = done;
}

const std::vector<Country*>& Player::getOwnedCountries() const {
	return owned;
}

// Stream insertion operator: outputs player info
std::ostream& operator<<(std::ostream& out, const Player& p) {
	out << "Player: " << *p.name << " | Countries: " << p.owned.size()
		<< " | Reinforcements: " << p.reinforcementPool;
	return out;
}

// Part 4 additions
int Player::getReinforcements() const { return reinforcementPool; }
void Player::setReinforcements(int amount) { reinforcementPool = amount; }
bool Player::hasConqueredThisTurn() const { return conqueredThisTurn; }
void Player::setConqueredThisTurn(bool val) { conqueredThisTurn = val; }
bool Player::isTruceWith(const std::string& p) const {
	for (const std::string& t : trucePlayers) if (t == p) return true;
	return false;
}
void Player::addTruce(const std::string& p) { trucePlayers.push_back(p); }
void Player::clearTruces() { trucePlayers.clear(); }
