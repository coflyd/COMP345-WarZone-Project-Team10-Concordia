#include "Player.h"

void Player::issueOrder() {
    orders->add(new Deploy("Default", 5));
    std::cout << "Order issued for " << *name << std::endl;
}

Player::Player(const std::string& n) {
    name = new std::string(n);
    owned = new std::vector<Country*>();
    hand = new Hand();
    orders = new OrdersList();
}

Player::Player(const Player& other) {
    name = new std::string(*other.name);
    owned = new std::vector<Country*>(*other.owned);
    hand = new Hand(*other.hand);
    orders = new OrdersList(*other.orders);
}

Player& Player::operator=(const Player& other) {
    if (this == &other) return *this;
    delete name;
    delete owned;
    delete hand;
    delete orders;

    name = new std::string(*other.name);
    owned = new std::vector<Country*>(*other.owned);
    hand = new Hand(*other.hand);
    orders = new OrdersList(*other.orders);
    return *this;
}

Player::~Player() {
    delete name;
    delete owned;
    delete hand;
    delete orders;
}

void Player::addCountry(Country* c) {
    owned->push_back(c);
}

std::vector<Country*> Player::toDefend() {
    return *owned;
}

std::vector<Country*> Player::toAttack() {
    return std::vector<Country*>();
}

OrdersList* Player::getOrders() const {
    return orders;
}

std::string Player::getName() const {
    return *name;
}

Hand* Player::getHand() const {
    return hand;
}

std::ostream& operator<<(std::ostream& out, const Player& p) {
    out << "Player: " << *p.name << " | Countries: " << p.owned->size();
    return out;
}