#pragma once

#include <iostream>
#include <string>
#include <vector>

/*
    ===========================
    Order (Abstract Base Class)
    ===========================
    - Parent class for all orders.
    - Has a description string and an effect string.
    - validate() checks if an order is allowed/valid.
    - execute() must validate first, then sets an effect.
    - clone() is used for deep copying (Rule of 3 in OrdersList).
*/
class Order {
protected:
    // All data members are pointers (as required by the assignment)
    std::string* description;  // what the order is (ex: "Deploy to Alaska (5)")
    std::string* effect;       // what happened after executing (ex: "Deployed 5 armies...")
    bool* executed;            // tracks if execute() was called successfully

    // Helper used by derived classes to store the effect and mark executed = true
    void setEffect(const std::string& newEffect);

public:
    // Constructors / Rule of 3
    Order();
    explicit Order(const std::string& desc);
    Order(const Order& other);
    Order& operator=(const Order& other);
    virtual ~Order();

    // Pure virtual functions -> every derived class MUST implement them
    virtual bool validate() const = 0;
    virtual void execute() = 0;

    // Used for deep copying through base pointers
    virtual Order* clone() const = 0;

    // Getters used in operator<<
    std::string getDescription() const;
    std::string getEffect() const;
    bool wasExecuted() const;

    // Stream insertion to print order info
    friend std::ostream& operator<<(std::ostream& os, const Order& o);
};

/*
    ===========================
    Deploy Order
    ===========================
    Meaning: Add armies to a target territory (placeholder in A1)
*/
class Deploy : public Order {
private:
    std::string* targetTerritory;
    int* armies;

public:
    Deploy();
    Deploy(const std::string& target, int armies);
    Deploy(const Deploy& other);
    Deploy& operator=(const Deploy& other);
    ~Deploy() override;

    bool validate() const override;
    void execute() override;
    Order* clone() const override;

    friend std::ostream& operator<<(std::ostream& os, const Deploy& o);
};

/*
    ===========================
    Advance Order
    ===========================
    Meaning: Move armies from one territory to another (placeholder in A1)
*/
class Advance : public Order {
private:
    std::string* fromTerritory;
    std::string* toTerritory;
    int* armies;

public:
    Advance();
    Advance(const std::string& from, const std::string& to, int armies);
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
    Meaning: Bomb a territory (placeholder in A1)
*/
class Bomb : public Order {
private:
    std::string* targetTerritory;

public:
    Bomb();
    explicit Bomb(const std::string& target);
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
    Meaning: Blockade a territory (placeholder in A1)
*/
class Blockade : public Order {
private:
    std::string* targetTerritory;

public:
    Blockade();
    explicit Blockade(const std::string& target);
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
    Meaning: Move armies between any two territories (placeholder in A1)
*/
class Airlift : public Order {
private:
    std::string* fromTerritory;
    std::string* toTerritory;
    int* armies;

public:
    Airlift();
    Airlift(const std::string& from, const std::string& to, int armies);
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
    Meaning: Two players cannot attack each other (placeholder in A1)
*/
class Negotiate : public Order {
private:
    std::string* playerA;
    std::string* playerB;

public:
    Negotiate();
    Negotiate(const std::string& a, const std::string& b);
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
    - Stores a list of Order pointers (polymorphism).
    - remove(index): deletes the order at index and removes it from the list.
    - move(from,to): reorders the list.
    - Implements deep copy using Order::clone().
*/
class OrdersList {
private:
    std::vector<Order*>* orders;

public:
    OrdersList();
    OrdersList(const OrdersList& other);
    OrdersList& operator=(const OrdersList& other);
    ~OrdersList();

    void add(Order* order);
    bool remove(int index);
    bool move(int fromIndex, int toIndex);

    int size() const;
    Order* at(int index) const;

    friend std::ostream& operator<<(std::ostream& os, const OrdersList& ol);
};
