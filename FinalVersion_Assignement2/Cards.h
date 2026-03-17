#pragma once
#include <vector>
#include <iostream>

class Player;
class Deck;

enum class CardType {
	bomb, reinforcement, blockade, airlift, diplomacy, territory
};

class Card {
private:
	CardType type;
	std::string territoryName;
	int territoryId;

public:
	Card();
	Card(CardType t);
	Card(const Card &other);
	Card& operator=(const Card &other);
	~Card();

	void play(Player *player, Deck *deck);
	CardType getType() const;

	friend std::ostream& operator<<(std::ostream &out, const Card &card);
	int getTerritoryId() const;
	const std::string& getTerritoryName() const;
	void setTerritory(int territoryId, const std::string &territoryName);
};

class Deck {
private:
	std::vector<Card*> *cards;

public:
	Deck();
	Deck(const Deck &other);
	Deck& operator=(const Deck &other);
	~Deck();

	Card* draw();
	void returnCard(Card *card);
	int size() const;
	void shuffle();

	friend std::ostream& operator<<(std::ostream &out, const Deck &deck);
};

class Hand {
private:
	std::vector<Card*> *cards;

public:
	Hand();
	Hand(const Hand &other);
	Hand& operator=(const Hand &other);
	~Hand();

	void addCard(Card *card);
	bool removeCard(Card *card);
	int size() const;
	const std::vector<Card*>* getCards() const;

	friend std::ostream& operator<<(std::ostream &out, const Hand &hand);
};
