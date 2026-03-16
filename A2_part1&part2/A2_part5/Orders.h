#pragma once

#include <iostream>
#include <string>
#include <vector>
#include "LoggingObserver.h"

// Forward declarations
class Player;
class Country;
class Map;
class Deck;

/*
    ===========================
    Order (Abstract Base Class)
    ===========================
    - Parent class for all orders.
*/
class Order : public Subject, public ILoggable {
protected:
    // All data members are pointers (as required by the assignment)
    std::string* description;  // What the order is
    std::string* effect;       // What happened after executing
    bool* executed;            // Tracks if execute() was called successfully

    // Helper used by derived classes to store the effect and mark executed as true
    void setEffect(const std::string& newEffect);

public:
    // Constructors / Rule of 3
    Order();
    explicit Order(const std::string& desc);
    Order(const Order& other);
    Order& operator=(const Order& other);
    virtual ~Order();

    // Pure virtual functions and every derived class must implement them
    virtual bool validate() const = 0;
    virtual void execute() = 0;

    // Used for deep copying through base pointers
    virtual Order* clone() const = 0;

    // Getters used in operator
    std::string getDescription() const;
    std::string getEffect() const;
    bool wasExecuted() const;
    std::string stringToLog() const override;
    // Stream insertion to print order info
    friend std::ostream& operator<<(std::ostream& os, const Order& o);
};

/*
    ===========================
    Deploy Order
    ===========================
    - Moves armies from the player's reinforcement pool onto the targeted country.
    - Invalid if the target territory does not belong to the issuing player,
      armies are <=0 or the player does not have enough armies in their pool.
*/

class Deploy : public Order {
private:
    Player*  issuingPlayer;
    Country* targetCountry;
    int*     armies;

public:
    // Convenience constructor: used by play() and simple testing
    // Takes a name string instead of a real Country pointer since the card
    // does not have access to the actual game objects when it is played.
    Deploy(const std::string& targetName, int armies);
    // Full constructor: used during actual game play because it includes the player making
    // the order
    Deploy(Player* player, Country* target, int armies);
    Deploy(const Deploy& other);
    Deploy& operator=(const Deploy& other);
    ~Deploy() override;
    // validate() makes sure that the targetCountry belongs to the issuingPlayer
    bool validate() const override;
    // execute() deducts from the pool and add to targetCountry armies
    void execute() override;
    Order* clone() const override;
    friend std::ostream& operator<<(std::ostream& os, const Deploy& o);
};

/*
    ===========================
    Advance Order
    ===========================
    - Moves armies from one territory to an adjacent territory.
    - If target belongs to the same player, armies are transferred.
    - If target belongs to an enemy, a battle is simulated:
        each attacker has 60% chance to kill one defender,
        each defender has 70% chance to kill one attacker at the same time.
    - If all defenders are eliminated, attacker captures the territory.
    - Player receives one card if they conquer at least one territory this turn.
*/

class Advance : public Order {
private:
    Player*  issuingPlayer;
    Country* fromCountry;
    Country* toCountry;
    Map*     map;           // Need the map to check for adjacency
    Deck*    deck;
    int*     armies;

public:
    Advance(const std::string& from, const std::string& to, int armies);
    Advance(Player* player, Country* from, Country* to, Map* map, Deck* deck, int armies);
    Advance(const Advance& other);
    Advance& operator=(const Advance& other);
    ~Advance() override;

    bool validate() const override;
    void execute() override;
    Order* clone() const override;
    friend std::ostream& operator<<(std::ostream& os, const Advance& o);
};

/*
    ===========================
    Bomb Order
    ===========================
    - Removes half the armies from a target enemy territory.
    - Invalid if the target belongs to the issuing player,
      is not adjacent to any of the issuing player's territories,
      or a negotiate truce is active with the target player.
*/
class Bomb : public Order {
private:
    Player*  issuingPlayer;
    Country* targetCountry;
    Map*     map;

public:
    Bomb(const std::string& targetName);
    Bomb(Player* player, Country* target, Map* map);
    Bomb(const Bomb& other);
    Bomb& operator=(const Bomb& other);
    ~Bomb() override;

    bool validate() const override;
    void execute() override;
    Order* clone() const override;
    friend std::ostream& operator<<(std::ostream& os, const Bomb& o);
};

/*
    ===========================
    Blockade Order
    ===========================
    - Doubles the armies on one of the issuing player's territories,
      then transfers ownership to the Neutral player.
    - Invalid if the target territory belongs to an enemy player.
*/
class Blockade : public Order {
private:
    Player*  issuingPlayer;
    Country* targetCountry;
    Player*  neutralPlayer;    // Neutral player that will receive ownership, since all terrorties must be
                               // owned by a player
public:
    Blockade(const std::string& targetName);
    Blockade(Player* player, Country* target, Player* neutral);
    Blockade(const Blockade& other);
    Blockade& operator=(const Blockade& other);
    ~Blockade() override;

    bool validate() const override;
    void execute() override;
    Order* clone() const override;
    friend std::ostream& operator<<(std::ostream& os, const Blockade& o);
};

/*
    ===========================
    Airlift Order
    ===========================
    - Moves armies from any owned territory to any other owned territory,
      regardless of adjacency between the two territories.
    - Invalid if either the source or target does not belong to the issuing player.
*/
class Airlift : public Order {
private:
    Player*  issuingPlayer;
    Country* fromCountry;
    Country* toCountry;
    int*     armies;

public:
    Airlift(const std::string& from, const std::string& to, int armies);
    Airlift(Player* player, Country* from, Country* to, int armies);
    Airlift(const Airlift& other);
    Airlift& operator=(const Airlift& other);
    ~Airlift() override;

    bool validate() const override;
    void execute() override;
    Order* clone() const override;
    friend std::ostream& operator<<(std::ostream& os, const Airlift& o);
};

/*
    ===========================
    Negotiate Order
    ===========================
    - Prevents both players from successfully attacking each other
      for the remainder of the current turn.
    - Invalid if the target is the same player as the issuer.
*/
class Negotiate : public Order {
private:
    Player* issuingPlayer;
    Player* targetPlayer;

public:
    Negotiate(const std::string& playerAName, const std::string& playerBName);
    Negotiate(Player* issuer, Player* target);
    Negotiate(const Negotiate& other);
    Negotiate& operator=(const Negotiate& other);
    ~Negotiate() override;

    bool validate() const override;
    void execute() override;
    Order* clone() const override;
    friend std::ostream& operator<<(std::ostream& os, const Negotiate& o);
};

/*
    ===========================
    OrdersList
    ===========================
    - Stores a list of Order pointers using polymorphism.
    - When you remove an order or the list gets destroyed, the orders
      get deleted from memory automatically to not cause any leaks, so it does deep
      copies instead of shallow copies.
    - Copying the list makes brand new copies of every order inside it,
      not just copies of the pointers.
    - remove() takes an order out of the list and deletes it.
    - move() lets you shuffle orders around in the list.
*/
class OrdersList : public Subject, public ILoggable {
private:
    std::vector<Order*>* orders;
    std::string lastAddedOrder;

public:
    OrdersList();
    OrdersList(const OrdersList& other);
    OrdersList& operator=(const OrdersList& other);
    ~OrdersList();

    void addOrder(Order* order);
    void add(Order* order);
    bool remove(int index);
    bool move(int fromIndex, int toIndex);
    int size() const;
    Order* at(int index) const;
    std::string stringToLog() const override;
    friend std::ostream& operator<<(std::ostream& os, const OrdersList& ol);
};
