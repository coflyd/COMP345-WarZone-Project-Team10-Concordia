#include <algorithm>
#include <random>
#include "Cards.h"
#include "Player.h"
#include "Orders.h"

using namespace std;

Card::Card() {
	type = CardType::bomb;
	territoryId = 0;
	territoryName = "";
}

Card::Card(CardType t) {
	type = t;
	territoryId = 0;
	territoryName = "";
}

Card::Card(const Card &other) {
	type = other.type;
	territoryId = other.territoryId;
	territoryName = other.territoryName;
}

Card& Card::operator=(const Card &other) {
	if (this != &other) {
		type = other.type;
		territoryId = other.territoryId;
		territoryName = other.territoryName;
	}
	return *this;
}

// Has no pointers to delete so it stays empty
Card::~Card() {
}

void Card::play(Player *player, Deck *deck) {
	if (player == nullptr || deck == nullptr)
		return;

//  Initilaizing the Order for play function
	Order *o = nullptr;

	switch (type) {

// Passing dummy values depending on the type of the card to have the correct order type
	case CardType::reinforcement:
		o = new Deploy("TerritoryA", 5);
		break;
	case CardType::bomb:
		o = new Bomb("TerritoryB");
		break;
	case CardType::blockade:
		o = new Blockade("TerritoryC");
		break;
	case CardType::airlift:
		o = new Airlift("TerritoryA", "TerritoryB", 5);
		break;
	case CardType::diplomacy:
		o = new Negotiate(player->getName(), "Enemy");
		break;
	case CardType::territory:
		break;
	}

//  Adding the new order we just made to the player's list
	player->getOrders()->add(o);

// Removes the card from the player's hand
	player->getHand()->removeCard(this);

// Returning the card to the deck
	deck->returnCard(this);
}

CardType Card::getType() const {
	return type;
}

// Prints the correct card type instead of printing the number it is inside the enum
ostream& operator<<(ostream &out, const Card &card) {
	switch (card.type) {
	case CardType::bomb:
		out << "Bomb";
		break;
	case CardType::reinforcement:
		out << "Reinforcement";
		break;
	case CardType::blockade:
		out << "Blockade";
		break;
	case CardType::airlift:
		out << "Airlift";
		break;
	case CardType::diplomacy:
		out << "Diplomacy";
		break;
	case CardType::territory:
		out << "Territory : Id "<< card.territoryId <<" "<<card.territoryName;
		break;
	}
	return out;
}

int Card::getTerritoryId() const {
	return territoryId;
}

const std::string& Card::getTerritoryName() const {
	return territoryName;
}

void Card::setTerritory(int territoryId, const std::string &territoryName) {
	this->territoryId = territoryId;
	this->territoryName = territoryName;
}




// For my Deck copy constructers, they are deep copies since the deck owns the cards
// and is not just referencing them or holding them
Deck::Deck() {
	cards = new vector<Card*>();
}

// Makes a deep copy of the Deck
Deck::Deck(const Deck &other) {
	cards = new vector<Card*>();
	for (Card *c : *other.cards) {
		cards->push_back(new Card(*c));
	}
}
// Creates a deep copy
Deck& Deck::operator=(const Deck &other) {
	if (this != &other) {
		for (Card *c : *cards)
			delete c;
		cards->clear();
		for (Card *c : *other.cards) {
			cards->push_back(new Card(*c));
		}
	}
	return *this;
}

Deck::~Deck() {
	for (Card *c : *cards)
		delete c;
	delete cards;
}
//  Returns the top card from the deck, which is the last element in the vector
Card* Deck::draw() {
	if (cards->empty())
		return nullptr;
	Card *c = cards->back();
	cards->pop_back();
	return c;
}
void Deck::shuffle() {
	std::random_device rd;
	std::mt19937 genAleat(rd());
	std::shuffle(cards->begin(), cards->end(), genAleat);
}
// Returns the card to the bottom of the deck, first element in vector
void Deck::returnCard(Card *card) {
	if (card != nullptr)
		cards->insert(cards->begin(), card);
}

int Deck::size() const {
	return cards->size();
}

ostream& operator<<(ostream &out, const Deck &deck) {
	out << "Deck(" << deck.size() << ")";
	return out;
}

// For my Hand copy constructers, they are shallow copies since the deck owns the cards
// and the hand is just temporarily holding the cards
Hand::Hand() {
	cards = new vector<Card*>();
}
// Creates a different vector of hands that points to the same cards
Hand::Hand(const Hand &other) {
	cards = new vector<Card*>();
	for (Card *c : *other.cards)
		cards->push_back(c);
}
// Creates a shallow copy
Hand& Hand::operator=(const Hand &other) {
	if (this != &other) {
		cards->clear();
		for (Card *c : *other.cards)
			cards->push_back(c);
	}
	return *this;
}

Hand::~Hand() {
	delete cards;
}
// Adds the card to the top of the hand, the last element of the vector
void Hand::addCard(Card *card) {
	if (card != nullptr)
		cards->push_back(card);
}

//  Removes a certain card from the hand
bool Hand::removeCard(Card *card) {
	if (card == nullptr)
		return false;
	for (auto it = cards->begin(); it != cards->end(); ++it) {
		if (*it == card) {
			cards->erase(it);
			return true;
		}
	}
	return false;
}

int Hand::size() const {
	return cards->size();
}

const vector<Card*>* Hand::getCards() const {
	return cards;
}

ostream& operator<<(ostream &out, const Hand &hand) {
	out << "Hand(" << hand.size() << "): ";
	for (Card *c : *hand.cards)
		out << *c << " ";
	return out;
}


