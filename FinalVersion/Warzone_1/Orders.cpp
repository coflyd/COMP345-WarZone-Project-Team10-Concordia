#include "Orders.h"
#include <algorithm>

/*
    Small helper function (only in this cpp file):
    Returns true if the string contains at least one non-whitespace character.
*/
static bool nonEmpty(const std::string& s) {
    for (char c : s) {
        if (c != ' ' && c != '\t' && c != '\n' && c != '\r') return true;
    }
    return false;
}

// =======================
// Order
// =======================
Order::Order()
    : description(new std::string("")),
    effect(new std::string("")),
    executed(new bool(false)) {
}

Order::Order(const std::string& desc)
    : description(new std::string(desc)),
    effect(new std::string("")),
    executed(new bool(false)) {
}

/*
    Copy constructor: deep copy pointer members.
*/
Order::Order(const Order& other)
    : description(new std::string(*other.description)),
    effect(new std::string(*other.effect)),
    executed(new bool(*other.executed)) {
}

/*
    Assignment operator: deep copy contents (we reuse allocated pointers).
*/
Order& Order::operator=(const Order& other) {
    if (this != &other) {
        *description = *other.description;
        *effect = *other.effect;
        *executed = *other.executed;
    }
    return *this;
}

/*
    Destructor: delete dynamically allocated members.
*/
Order::~Order() {
    delete description;
    delete effect;
    delete executed;
    description = nullptr;
    effect = nullptr;
    executed = nullptr;
}

/*
    Used in derived execute() methods to store what happened and set executed = true.
*/
void Order::setEffect(const std::string& newEffect) {
    *effect = newEffect;
    *executed = true;
}

// Simple getters
std::string Order::getDescription() const { return *description; }
std::string Order::getEffect() const { return *effect; }
bool Order::wasExecuted() const { return *executed; }

/*
    Prints the order description.
    If the order was executed, also prints the effect.
*/
std::ostream& operator<<(std::ostream& os, const Order& o) {
    os << "Order: " << o.getDescription();
    if (o.wasExecuted()) {
        os << " | Effect: " << o.getEffect();
    }
    return os;
}

// =======================
// Deploy
// =======================
Deploy::Deploy()
    : Order("Deploy"),
    targetTerritory(new std::string("")),
    armies(new int(0)) {
}

Deploy::Deploy(const std::string& target, int a)
    : Order("Deploy to " + target + " (" + std::to_string(a) + ")"),
    targetTerritory(new std::string(target)),
    armies(new int(a)) {
}

Deploy::Deploy(const Deploy& other)
    : Order(other),
    targetTerritory(new std::string(*other.targetTerritory)),
    armies(new int(*other.armies)) {
}

Deploy& Deploy::operator=(const Deploy& other) {
    if (this != &other) {
        Order::operator=(other);
        *targetTerritory = *other.targetTerritory;
        *armies = *other.armies;
    }
    return *this;
}

Deploy::~Deploy() {
    delete targetTerritory;
    delete armies;
    targetTerritory = nullptr;
    armies = nullptr;
}

/*
    For A1 we do simple validation:
    - territory name is not empty
    - armies > 0
*/
bool Deploy::validate() const {
    return targetTerritory != nullptr && armies != nullptr
        && nonEmpty(*targetTerritory) && *armies > 0;
}

/*
    execute() MUST validate first (assignment requirement).
    If invalid: do nothing (but we store an "invalid" effect message for the demo).
*/
void Deploy::execute() {
    if (!validate()) {
        setEffect("Invalid Deploy (missing target or armies <= 0). No action taken.");
        return;
    }
    setEffect("Deployed " + std::to_string(*armies) + " armies to " + *targetTerritory + ".");
}

Order* Deploy::clone() const { return new Deploy(*this); }

std::ostream& operator<<(std::ostream& os, const Deploy& o) {
    os << static_cast<const Order&>(o);
    return os;
}

// =======================
// Advance
// =======================
Advance::Advance()
    : Order("Advance"),
    fromTerritory(new std::string("")),
    toTerritory(new std::string("")),
    armies(new int(0)) {
}

Advance::Advance(const std::string& from, const std::string& to, int a)
    : Order("Advance " + from + " -> " + to + " (" + std::to_string(a) + ")"),
    fromTerritory(new std::string(from)),
    toTerritory(new std::string(to)),
    armies(new int(a)) {
}

Advance::Advance(const Advance& other)
    : Order(other),
    fromTerritory(new std::string(*other.fromTerritory)),
    toTerritory(new std::string(*other.toTerritory)),
    armies(new int(*other.armies)) {
}

Advance& Advance::operator=(const Advance& other) {
    if (this != &other) {
        Order::operator=(other);
        *fromTerritory = *other.fromTerritory;
        *toTerritory = *other.toTerritory;
        *armies = *other.armies;
    }
    return *this;
}

Advance::~Advance() {
    delete fromTerritory;
    delete toTerritory;
    delete armies;
    fromTerritory = nullptr;
    toTerritory = nullptr;
    armies = nullptr;
}

bool Advance::validate() const {
    return fromTerritory != nullptr && toTerritory != nullptr && armies != nullptr
        && nonEmpty(*fromTerritory) && nonEmpty(*toTerritory)
        && *armies > 0 && (*fromTerritory != *toTerritory);
}

void Advance::execute() {
    if (!validate()) {
        setEffect("Invalid Advance (bad territories or armies <= 0). No action taken.");
        return;
    }
    setEffect("Advanced " + std::to_string(*armies) + " armies from " + *fromTerritory +
        " to " + *toTerritory + ".");
}

Order* Advance::clone() const { return new Advance(*this); }

std::ostream& operator<<(std::ostream& os, const Advance& o) {
    os << static_cast<const Order&>(o);
    return os;
}

// =======================
// Bomb
// =======================
Bomb::Bomb()
    : Order("Bomb"),
    targetTerritory(new std::string("")) {
}

Bomb::Bomb(const std::string& target)
    : Order("Bomb " + target),
    targetTerritory(new std::string(target)) {
}

Bomb::Bomb(const Bomb& other)
    : Order(other),
    targetTerritory(new std::string(*other.targetTerritory)) {
}

Bomb& Bomb::operator=(const Bomb& other) {
    if (this != &other) {
        Order::operator=(other);
        *targetTerritory = *other.targetTerritory;
    }
    return *this;
}

Bomb::~Bomb() {
    delete targetTerritory;
    targetTerritory = nullptr;
}

bool Bomb::validate() const {
    return targetTerritory != nullptr && nonEmpty(*targetTerritory);
}

void Bomb::execute() {
    if (!validate()) {
        setEffect("Invalid Bomb (missing target). No action taken.");
        return;
    }
    setEffect("Bombed " + *targetTerritory + " (placeholder effect).");
}

Order* Bomb::clone() const { return new Bomb(*this); }

std::ostream& operator<<(std::ostream& os, const Bomb& o) {
    os << static_cast<const Order&>(o);
    return os;
}

// =======================
// Blockade
// =======================
Blockade::Blockade()
    : Order("Blockade"),
    targetTerritory(new std::string("")) {
}

Blockade::Blockade(const std::string& target)
    : Order("Blockade " + target),
    targetTerritory(new std::string(target)) {
}

Blockade::Blockade(const Blockade& other)
    : Order(other),
    targetTerritory(new std::string(*other.targetTerritory)) {
}

Blockade& Blockade::operator=(const Blockade& other) {
    if (this != &other) {
        Order::operator=(other);
        *targetTerritory = *other.targetTerritory;
    }
    return *this;
}

Blockade::~Blockade() {
    delete targetTerritory;
    targetTerritory = nullptr;
}

bool Blockade::validate() const {
    return targetTerritory != nullptr && nonEmpty(*targetTerritory);
}

void Blockade::execute() {
    if (!validate()) {
        setEffect("Invalid Blockade (missing target). No action taken.");
        return;
    }
    setEffect("Blockaded " + *targetTerritory + " (placeholder effect).");
}

Order* Blockade::clone() const { return new Blockade(*this); }

std::ostream& operator<<(std::ostream& os, const Blockade& o) {
    os << static_cast<const Order&>(o);
    return os;
}

// =======================
// Airlift
// =======================
Airlift::Airlift()
    : Order("Airlift"),
    fromTerritory(new std::string("")),
    toTerritory(new std::string("")),
    armies(new int(0)) {
}

Airlift::Airlift(const std::string& from, const std::string& to, int a)
    : Order("Airlift " + from + " -> " + to + " (" + std::to_string(a) + ")"),
    fromTerritory(new std::string(from)),
    toTerritory(new std::string(to)),
    armies(new int(a)) {
}

Airlift::Airlift(const Airlift& other)
    : Order(other),
    fromTerritory(new std::string(*other.fromTerritory)),
    toTerritory(new std::string(*other.toTerritory)),
    armies(new int(*other.armies)) {
}

Airlift& Airlift::operator=(const Airlift& other) {
    if (this != &other) {
        Order::operator=(other);
        *fromTerritory = *other.fromTerritory;
        *toTerritory = *other.toTerritory;
        *armies = *other.armies;
    }
    return *this;
}

Airlift::~Airlift() {
    delete fromTerritory;
    delete toTerritory;
    delete armies;
    fromTerritory = nullptr;
    toTerritory = nullptr;
    armies = nullptr;
}

bool Airlift::validate() const {
    return fromTerritory != nullptr && toTerritory != nullptr && armies != nullptr
        && nonEmpty(*fromTerritory) && nonEmpty(*toTerritory)
        && *armies > 0 && (*fromTerritory != *toTerritory);
}

void Airlift::execute() {
    if (!validate()) {
        setEffect("Invalid Airlift (bad territories or armies <= 0). No action taken.");
        return;
    }
    setEffect("Airlifted " + std::to_string(*armies) + " armies from " + *fromTerritory +
        " to " + *toTerritory + ".");
}

Order* Airlift::clone() const { return new Airlift(*this); }

std::ostream& operator<<(std::ostream& os, const Airlift& o) {
    os << static_cast<const Order&>(o);
    return os;
}

// =======================
// Negotiate
// =======================
Negotiate::Negotiate()
    : Order("Negotiate"),
    playerA(new std::string("")),
    playerB(new std::string("")) {
}

Negotiate::Negotiate(const std::string& a, const std::string& b)
    : Order("Negotiate " + a + " <-> " + b),
    playerA(new std::string(a)),
    playerB(new std::string(b)) {
}

Negotiate::Negotiate(const Negotiate& other)
    : Order(other),
    playerA(new std::string(*other.playerA)),
    playerB(new std::string(*other.playerB)) {
}

Negotiate& Negotiate::operator=(const Negotiate& other) {
    if (this != &other) {
        Order::operator=(other);
        *playerA = *other.playerA;
        *playerB = *other.playerB;
    }
    return *this;
}

Negotiate::~Negotiate() {
    delete playerA;
    delete playerB;
    playerA = nullptr;
    playerB = nullptr;
}

bool Negotiate::validate() const {
    return playerA != nullptr && playerB != nullptr
        && nonEmpty(*playerA) && nonEmpty(*playerB)
        && (*playerA != *playerB);
}

void Negotiate::execute() {
    if (!validate()) {
        setEffect("Invalid Negotiate (players missing or same). No action taken.");
        return;
    }
    setEffect("Negotiation set between " + *playerA + " and " + *playerB +
        " (placeholder effect).");
}

Order* Negotiate::clone() const { return new Negotiate(*this); }

std::ostream& operator<<(std::ostream& os, const Negotiate& o) {
    os << static_cast<const Order&>(o);
    return os;
}

// =======================
// OrdersList
// =======================
OrdersList::OrdersList()
    : orders(new std::vector<Order*>()) {
}

/*
    Copy constructor (deep copy):
    - We clone each order so the new list owns its own copies.
*/
OrdersList::OrdersList(const OrdersList& other)
    : orders(new std::vector<Order*>()) {
    orders->reserve(other.orders->size());
    for (Order* o : *other.orders) {
        orders->push_back(o ? o->clone() : nullptr);
    }
}

/*
    Assignment operator (deep copy):
    - First delete current orders to prevent memory leaks.
    - Then clone from other.
*/
OrdersList& OrdersList::operator=(const OrdersList& other) {
    if (this != &other) {
        for (Order* o : *orders) delete o;
        orders->clear();

        orders->reserve(other.orders->size());
        for (Order* o : *other.orders) {
            orders->push_back(o ? o->clone() : nullptr);
        }
    }
    return *this;
}

/*
    Destructor:
    - delete every Order* inside the vector
    - delete the vector pointer itself
*/
OrdersList::~OrdersList() {
    for (Order* o : *orders) delete o;
    delete orders;
    orders = nullptr;
}

void OrdersList::add(Order* order) {
    orders->push_back(order);
}

/*
    remove(index):
    - deletes the order at index (important to avoid leaks)
    - removes it from the vector
*/
bool OrdersList::remove(int index) {
    if (index < 0 || index >= static_cast<int>(orders->size())) return false;
    delete (*orders)[index];
    orders->erase(orders->begin() + index);
    return true;
}

/*
    move(fromIndex, toIndex):
    - reorders the orders list
*/
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

/*
    Prints the full OrdersList with indices.
*/
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
