#include "Orders.h"
#include "Player.h"
#include "Map.h"
#include "Cards.h"
#include <algorithm>
#include <cstdlib>
#include <ctime>
#include <iostream>

// Order

// Default
Order::Order()
    : description(new std::string("")),
      effect(new std::string("")),
      executed(new bool(false)) {}

// With Descriptions
Order::Order(const std::string& desc)
    : description(new std::string(desc)),
      effect(new std::string("")),
      executed(new bool(false)) {}

// Copy constructor with deep copy pointer members
Order::Order(const Order& other)
    : Subject(other),
      description(new std::string(*other.description)),
      effect(new std::string(*other.effect)),
      executed(new bool(*other.executed)) {}

// Assignment operator with deep copy contents
Order& Order::operator=(const Order& other) {
    
    if (this != &other) {
        Subject::operator=(other);
        *description = *other.description;
        *effect = *other.effect;
        *executed = *other.executed;
    }
    return *this;
}

// Destructor
Order::~Order() {
    delete description;
    delete effect;
    delete executed;
    description = nullptr;
    effect= nullptr;
    executed = nullptr;
}

// Used in derived execute() methods to store what happened
void Order::setEffect(const std::string& newEffect) {
    *effect = newEffect;
    *executed = true;
    Notify(this);
}

// Getters
std::string Order::getDescription() const { return *description; }
std::string Order::getEffect() const { return *effect; }
bool Order::wasExecuted()const { return *executed; }

std::string Order::stringToLog() const {
    return "[Order] " + *description + " | Effect: " + *effect;
}

// Prints the order description and the effect if it was executed
std::ostream& operator<<(std::ostream& os, const Order& o) {
    os << "Order: " << o.getDescription();
    if (o.wasExecuted()) {
        os << " | Effect: " << o.getEffect();
    }
    return os;
}

// Deploy

Deploy::Deploy(const std::string& targetName, int a)
    : Order("Deploy to " + targetName + " (" + std::to_string(a) + ")"),
      issuingPlayer(nullptr),
      targetCountry(nullptr),
      armies(new int(a)) {}

Deploy::Deploy(Player* player, Country* target, int a)
    : Order("Deploy to " + (target ? target->name : "?") + " (" + std::to_string(a) + ")"),
      issuingPlayer(player),
      targetCountry(target),
      armies(new int(a)) {}

// Copy constructor with deep copy pointer members
// issuingPlayer and targetCountry are not owned so they are shallow copied
Deploy::Deploy(const Deploy& other)
    : Order(other),
      issuingPlayer(other.issuingPlayer),
      targetCountry(other.targetCountry),
      armies(new int(*other.armies)) {}

// Assignment operator with deep copy contents
Deploy& Deploy::operator=(const Deploy& other) {
    if (this != &other) {
        Order::operator=(other);
        issuingPlayer = other.issuingPlayer;
        targetCountry = other.targetCountry;
        *armies = *other.armies;
    }
    return *this;
}

// Destructor
Deploy::~Deploy() {
    delete armies;
    armies = nullptr;
}

// Validate Deploy:
bool Deploy::validate() const {
    if (armies == nullptr || *armies <= 0) return false;
    if (issuingPlayer == nullptr || targetCountry == nullptr) return true;
    if (targetCountry->playerName == nullptr) return false;
    if (*targetCountry->playerName != issuingPlayer->getName()) return false;
    if (issuingPlayer->getReinforcements() < *armies) return false;
    return true;
}

// Execute Deploy:
void Deploy::execute() {
    if (!validate()) {
        setEffect("Invalid Deploy: target does not belong to issuing player "
                  "or not enough armies in reinforcement pool.");
        return;
    }
    if (issuingPlayer != nullptr && targetCountry != nullptr) {
        issuingPlayer->setReinforcements(
        issuingPlayer->getReinforcements() - *armies);
        targetCountry->armies += *armies;
        setEffect("Deployed " + std::to_string(*armies) +
                  " armies to " + targetCountry->name +
                  ". Territory now has " + std::to_string(targetCountry->armies) + " armies.");
    } else {
        setEffect("Deployed " + std::to_string(*armies) + " armies (convenience mode).");
    }
}

Order* Deploy::clone() const { return new Deploy(*this);}
std::ostream& operator<<(std::ostream& os, const Deploy& o) {
    os << static_cast<const Order&>(o);
    return os;
}

// Advance

Advance::Advance(const std::string& from, const std::string& to, int a)
    : Order("Advance " + from + " -> " + to + " (" + std::to_string(a) + ")"),
      issuingPlayer(nullptr),
      fromCountry(nullptr),
      toCountry(nullptr),
      map(nullptr),
      deck(nullptr),
      armies(new int(a)) {}

Advance::Advance(Player* player, Country* from, Country* to,
                 Map* m, Deck* d, int a)
    : Order("Advance " + (from ? from->name : "?") +
            " -> " + (to ? to->name : "?") +
            " (" + std::to_string(a) + ")"),
      issuingPlayer(player),
      fromCountry(from),
      toCountry(to),
      map(m),
      deck(d),
      armies(new int(a)) {}

// Copy constructor with deep copy pointer members
// Game object pointers are not owned so they are shallow copied
Advance::Advance(const Advance& other)
    : Order(other),
      issuingPlayer(other.issuingPlayer),
      fromCountry(other.fromCountry),
      toCountry(other.toCountry),
      map(other.map),
      deck(other.deck),
      armies(new int(*other.armies)) {}

// Assignment operator with deep copy contents
Advance& Advance::operator=(const Advance& other) {
    if (this != &other) {
        Order::operator=(other);
        issuingPlayer = other.issuingPlayer;
        fromCountry = other.fromCountry;
        toCountry = other.toCountry;
        map = other.map;
        deck = other.deck;
        *armies = *other.armies;
    }
    return *this;
}

// Destructor
Advance::~Advance() {
    delete armies;
    armies = nullptr;
}

// Validate Advance:

bool Advance::validate() const {
    if (armies == nullptr || *armies <= 0) return false;
    if (issuingPlayer == nullptr || fromCountry == nullptr || toCountry == nullptr)
    return true;
    
    if (fromCountry->playerName == nullptr) return false;
    if (*fromCountry->playerName != issuingPlayer->getName()) return false;
    if (toCountry->playerName != nullptr &&
        *toCountry->playerName != issuingPlayer->getName()) {
        if (issuingPlayer->isTruceWith(*toCountry->playerName)) return false;
    }
    if (map != nullptr && !map->areAdjacent(fromCountry->id, toCountry->id))
        return false;
    return true;
}

// Execute Advance:
void Advance::execute() {
    if (!validate()) {
        setEffect("Invalid Advance: source not owned, territories not adjacent "
                  "or negotiate truce is active.");
        return;
    }
    if (issuingPlayer == nullptr || fromCountry == nullptr || toCountry == nullptr) {
        setEffect("Advanced " + std::to_string(*armies) + " armies (convenience mode).");
        return;
    }

    // Friendly transfer between own territories
    if (toCountry->playerName != nullptr &&
        *toCountry->playerName == issuingPlayer->getName()) {
        int moved = std::min(*armies, fromCountry->armies);
        fromCountry->armies -= moved;
        toCountry->armies += moved;
        setEffect("Moved " + std::to_string(moved) +
                  " armies from " + fromCountry->name +
                  " to " + toCountry->name + " (friendly transfer).");
        return;
    }

    // Battle simulation
    srand(static_cast<unsigned int>(time(nullptr)));
    int attackers = std::min(*armies, fromCountry->armies);
    int defenders = toCountry->armies;
    int defenderKills = 0;
    int attackerKills = 0;
    for (int i = 0; i < attackers; i++)
        if ((rand() % 100) < 60) attackerKills++;
    for (int i = 0; i < defenders; i++)
        if ((rand() % 100) < 70) defenderKills++;
    int survivingAttackers = std::max(0, attackers - defenderKills);
    int survivingDefenders = std::max(0, defenders - attackerKills);
    fromCountry->armies -= attackers;
    if (survivingDefenders == 0) {
        // Attacker captures the territory
        std::string previousOwner = toCountry->playerName ? *toCountry->playerName : "Neutral";
        delete toCountry->playerName;
        toCountry->playerName = new std::string(issuingPlayer->getName());
        toCountry->armies = survivingAttackers;
        issuingPlayer->addCountry(toCountry);
        // Award one card on first conquest only
        if (!issuingPlayer->hasConqueredThisTurn() && deck != nullptr) {
            Card* reward = deck->draw();
            if (reward != nullptr) issuingPlayer->getHand()->addCard(reward);
        }
        issuingPlayer->setConqueredThisTurn(true);
        setEffect("Battle at " + toCountry->name + ": " +
                  std::to_string(survivingAttackers) + " attackers survived, " +
                  "all defenders eliminated. " +
                  issuingPlayer->getName() + " captured " + toCountry->name +
                  " from " + previousOwner + ".");
    } else {
        // Defender holds, surviving attackers return to source
        toCountry->armies = survivingDefenders;
        fromCountry->armies += survivingAttackers;
        setEffect("Battle at " + toCountry->name + ": " +
        std::to_string(survivingDefenders) + " defenders survived, " +
        "attack repelled. "+
       std::to_string(survivingAttackers) + " attackers returned to " +
       fromCountry->name+ ".");
    }
}

Order* Advance::clone() const { return new Advance(*this); }
std::ostream& operator<<(std::ostream& os, const Advance& o) {
    os << static_cast<const Order&>(o);
    return os;
}

// Bomb

Bomb::Bomb(const std::string& targetName)
    : Order("Bomb " + targetName),
      issuingPlayer(nullptr),
      targetCountry(nullptr),
      map(nullptr) {}

Bomb::Bomb(Player* player, Country* target, Map* m)
    : Order("Bomb " + (target ? target->name : "?")),
      issuingPlayer(player),
      targetCountry(target),
      map(m) {}

// Copy constructor for game object pointers are not owned so they are shallow copied
Bomb::Bomb(const Bomb& other)
    : Order(other),
      issuingPlayer(other.issuingPlayer),
      targetCountry(other.targetCountry),
      map(other.map) {}

// Assignment operator with deep copy contents
Bomb& Bomb::operator=(const Bomb& other) {
    if (this != &other) {
        Order::operator=(other);
        issuingPlayer = other.issuingPlayer;
        targetCountry = other.targetCountry;
        map = other.map;
    }
    return *this;
}
Bomb::~Bomb() {}

// Validate Bomb:

bool Bomb::validate() const {
    if (issuingPlayer == nullptr || targetCountry == nullptr || map == nullptr)
        return true;
    if (targetCountry->playerName != nullptr &&
        *targetCountry->playerName == issuingPlayer->getName()) return false;
    if (targetCountry->playerName != nullptr &&
        issuingPlayer->isTruceWith(*targetCountry->playerName)) return false;
    for (Country* owned: issuingPlayer->toDefend()) {
        if (map->areAdjacent(owned->id, targetCountry->id)) return true;
    }
    return false;
}

// Execute Bomb
void Bomb::execute() {
    if (!validate()) {
        setEffect("Invalid Bomb: target belongs to issuing player, "
                  "not adjacent or truce is active.");
        return;
    }
    if (targetCountry != nullptr) {
        int removed = targetCountry->armies / 2;
        targetCountry->armies -= removed;
        setEffect("Bombed " + targetCountry->name + ". Removed " +
        std::to_string(removed) + " armies. " +
        std::to_string(targetCountry->armies) + " armies remain.");
    } else {
        setEffect("Bombed target (convenience mode).");
    }
}

Order* Bomb::clone() const { return new Bomb(*this); }
std::ostream& operator<<(std::ostream& os, const Bomb& o) {
    os << static_cast<const Order&>(o);
    return os;
}

// Blockade

Blockade::Blockade(const std::string& targetName)
    : Order("Blockade " + targetName),
      issuingPlayer(nullptr),
      targetCountry(nullptr),
      neutralPlayer(nullptr) {}

Blockade::Blockade(Player* player, Country* target, Player* neutral)
    : Order("Blockade " + (target ? target->name : "?")),
      issuingPlayer(player),
      targetCountry(target),
      neutralPlayer(neutral) {}

// Copy constructor since game object pointers are not owned so they are shallow copied
Blockade::Blockade(const Blockade& other)
    : Order(other),
      issuingPlayer(other.issuingPlayer),
      targetCountry(other.targetCountry),
      neutralPlayer(other.neutralPlayer) {
}

// Assignment operator with deep copy contents
Blockade& Blockade::operator=(const Blockade& other) {
    if (this != &other) {
        Order::operator=(other);
        issuingPlayer = other.issuingPlayer;
        targetCountry = other.targetCountry;
        neutralPlayer = other.neutralPlayer;
    }
    return *this;
}

Blockade::~Blockade() {}

// Validate Blockade:
bool Blockade::validate() const {
    if (issuingPlayer == nullptr || targetCountry == nullptr) return true;
    if (targetCountry->playerName == nullptr) return false;
    return (*targetCountry->playerName == issuingPlayer->getName());
}

// Execute Blockade:
void Blockade::execute() {
    if (!validate()) {
        setEffect("Invalid Blockade: target does not belong to issuing player.");
        return;
    }
    if (targetCountry != nullptr) {
        targetCountry->armies *= 2;
        if (issuingPlayer != nullptr)
            issuingPlayer->removeCountry(targetCountry);
        // Transfer to Neutral player if provided, otherwise mark as Neutral
        if (neutralPlayer != nullptr) {
            neutralPlayer->addCountry(targetCountry);
            delete targetCountry->playerName;
            targetCountry->playerName = new std::string(neutralPlayer->getName());
        } else {
            delete targetCountry->playerName;
            targetCountry->playerName = new std::string("Neutral");
        }
        setEffect("Blockaded " + targetCountry->name +
                  ". Armies doubled to " + std::to_string(targetCountry->armies) +
                  ". Ownership transferred to Neutral.");
    } else {
        setEffect("Blockaded target (convenience mode).");
    }
}

Order* Blockade::clone() const { return new Blockade(*this); }
std::ostream& operator<<(std::ostream& os, const Blockade& o) {
    os << static_cast<const Order&>(o);
    return os;
}

// Airlift

Airlift::Airlift(const std::string& from, const std::string& to, int a)
    : Order("Airlift " + from + " -> " + to + " (" + std::to_string(a) + ")"),
      issuingPlayer(nullptr),
      fromCountry(nullptr),
      toCountry(nullptr),
      armies(new int(a)) {}

Airlift::Airlift(Player* player, Country* from, Country* to, int a)
    : Order("Airlift " + (from ? from->name : "?") +
            " -> " + (to ? to->name : "?") +
            " (" + std::to_string(a) + ")"),
      issuingPlayer(player),
      fromCountry(from),
      toCountry(to),
      armies(new int(a)) {}

// Copy constructor for game object pointers are not owned so they are shallow copied
Airlift::Airlift(const Airlift& other)
    : Order(other),
      issuingPlayer(other.issuingPlayer),
      fromCountry(other.fromCountry),
      toCountry(other.toCountry),
      armies(new int(*other.armies)) {}

// Assignment operator with deep copy contents
Airlift& Airlift::operator=(const Airlift& other) {
    if (this != &other) {
        Order::operator=(other);
        issuingPlayer = other.issuingPlayer;
        fromCountry = other.fromCountry;
        toCountry = other.toCountry;
        *armies = *other.armies;
    }
    return *this;
}

Airlift::~Airlift() {
    delete armies;
    armies = nullptr;
}

// Validate Airlift:

bool Airlift::validate() const {
    if (armies == nullptr || *armies <= 0) return false;
    if (issuingPlayer == nullptr || fromCountry == nullptr || toCountry == nullptr)
        return true;
    if (fromCountry == toCountry) return false;
    if (fromCountry->playerName == nullptr || toCountry->playerName == nullptr)
        return false;
    return (*fromCountry->playerName == issuingPlayer->getName() &&
            *toCountry->playerName   == issuingPlayer->getName());
}

// Execute Airlift:
void Airlift::execute() {
    if (!validate()) {
        setEffect("Invalid Airlift: source or target does not belong to issuing player.");
        return;
    }
    if (fromCountry != nullptr && toCountry != nullptr) {
        int moved = std::min(*armies, fromCountry->armies);
        fromCountry->armies -= moved;
        toCountry->armies+= moved;
        setEffect("Airlifted " + std::to_string(moved) +
                  " armies from " + fromCountry->name +
                  " to " + toCountry->name + ".");
    } else {
        setEffect("Airlifted " + std::to_string(*armies) + " armies (convenience mode).");
    }
}

Order* Airlift::clone() const { return new Airlift(*this); }
std::ostream& operator<<(std::ostream& os, const Airlift& o) {
    os << static_cast<const Order&>(o);
    return os;
}

// Negotiate

Negotiate::Negotiate(const std::string& playerAName, const std::string& playerBName)
    : Order("Negotiate " + playerAName + " <-> " + playerBName),
      issuingPlayer(nullptr),
      targetPlayer(nullptr) {}

Negotiate::Negotiate(Player* issuer, Player* target)
    : Order("Negotiate " + (issuer ? issuer->getName() : "?") +
            " <-> " + (target ? target->getName() : "?")),
      issuingPlayer(issuer),
      targetPlayer(target) {}

// Copy constructor since player pointers are not owned so they are shallow copied
Negotiate::Negotiate(const Negotiate& other)
    : Order(other),
      issuingPlayer(other.issuingPlayer),
      targetPlayer(other.targetPlayer) {}

// Assignment operator with deep copy contents
Negotiate& Negotiate::operator=(const Negotiate& other) {
    if (this != &other) {
        Order::operator=(other);
        issuingPlayer = other.issuingPlayer;
        targetPlayer = other.targetPlayer;
    }
    return *this;
}

Negotiate::~Negotiate() {}

// Validate Negotiate:
bool Negotiate::validate() const {
    if (issuingPlayer == nullptr || targetPlayer == nullptr) return true;
    return (issuingPlayer != targetPlayer);
}

// Execute Negotiate
void Negotiate::execute() {
    if (!validate()) {
        setEffect("Invalid Negotiate: cannot negotiate with yourself.");
        return;
    }
    if (issuingPlayer != nullptr && targetPlayer != nullptr) {
        issuingPlayer->addTruce(targetPlayer->getName());
        targetPlayer->addTruce(issuingPlayer->getName());
        setEffect("Truce established between " + issuingPlayer->getName() +
                  " and " + targetPlayer->getName() +
                  ". Neither can attack the other this turn.");
    } else {
        setEffect("Negotiate executed (convenience mode).");
    }
}

Order* Negotiate::clone() const { return new Negotiate(*this); }
std::ostream& operator<<(std::ostream& os, const Negotiate& o) {
    os << static_cast<const Order&>(o);
    return os;
}

// OrdersList

OrdersList::OrdersList()
    : orders(new std::vector<Order*>()),
      lastAddedOrder("") {}
OrdersList::OrdersList(const OrdersList& other)
    : Subject(other),
      orders(new std::vector<Order*>()),
      lastAddedOrder(other.lastAddedOrder) {
    orders->reserve(other.orders->size());
    for (Order* o : *other.orders) {
        orders->push_back(o ? o->clone() : nullptr);
    }
}

OrdersList& OrdersList::operator=(const OrdersList& other) {
    if (this != &other) {
        Subject::operator=(other);
        lastAddedOrder = other.lastAddedOrder;
        for (Order* o : *orders) delete o;
        orders->clear();
        orders->reserve(other.orders->size());
        for (Order* o : *other.orders) {
        orders->push_back(o ? o->clone() : nullptr);
        }
    }
    return *this;
}

// Destructor to delete every Order* then delete the vector itself
OrdersList::~OrdersList() {
    for (Order* o : *orders) delete o;
    delete orders;
    orders = nullptr;
}

void OrdersList::addOrder(Order* order) {
    orders->push_back(order);
    lastAddedOrder = (order != nullptr) ? order->getDescription() : "(null order)";
    Notify(this);
}

void OrdersList::add(Order* order) {
    addOrder(order);
}

// remove(index) to delete the order at index and removes it from the vector
bool OrdersList::remove(int index) {
    if (index < 0 || index >= static_cast<int>(orders->size())) return false;
    delete (*orders)[index];
    orders->erase(orders->begin() + index);
    return true;
}

// Reorders the list
bool OrdersList::move(int fromIndex, int toIndex) {
    int n = static_cast<int>(orders->size());
    if (fromIndex < 0 || fromIndex >= n || toIndex < 0 || toIndex >= n) return false;
    if (fromIndex == toIndex) return true;
    Order* temp = (*orders)[fromIndex];
    orders->erase(orders->begin() + fromIndex);
    orders->insert(orders->begin() + toIndex, temp);
    return true;
}

int OrdersList::size() const {
    return static_cast<int>(orders->size());
}

Order* OrdersList::at(int index) const {
    if (index < 0 || index >= static_cast<int>(orders->size())) return nullptr;
    return (*orders)[index];
}

std::string OrdersList::stringToLog() const {
    return "[OrdersList] Added order: " + lastAddedOrder;
}

// Prints the full OrdersList
std::ostream& operator<<(std::ostream& os, const OrdersList& ol) {
    os << "OrdersList (" << ol.size() << " orders)\n";
    for (int i = 0; i < ol.size(); ++i) {
        os << "  [" << i << "] ";
        Order* o = ol.at(i);
        if (o) os << *o;
        else os << "(null)";
        os << "\n";
    }
    return os;
}
