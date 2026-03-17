#pragma once
#include <vector>
#include <string>
#include <iostream>
#include "Map.h"
#include "Orders.h"
#include "Cards.h"

class Player {
private:
    std::string* name;                  // Player's name
    std::vector<Country*>* owned;       // Territories owned by the player
    Hand* hand;                         // Player's hand of cards
    OrdersList* orders;                 // Player's list of orders

public:
    // Constructors and destructor
    Player(const std::string& n);
    Player(const Player& other);
    Player& operator=(const Player& other);
    ~Player();

    // Game methods
    void addCountry(Country* c);
    std::vector<Country*> toDefend();
    std::vector<Country*> toAttack();
    void issueOrder();

    // Getters
    OrdersList* getOrders() const;
    Hand* getHand() const;
    std::string getName() const;

    // Stream insertion operator
    friend std::ostream& operator<<(std::ostream& out, const Player& p);
};