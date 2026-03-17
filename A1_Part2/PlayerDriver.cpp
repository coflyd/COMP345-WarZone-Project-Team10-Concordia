#include "Player.h"
#include <iostream>

int main() {
    std::cout << "--- Player Driver ---\n";

    Player p1("Amir");
    Player p2 = p1;

    std::cout << p1 << std::endl;
    std::cout << p2 << std::endl;

    p1.issueOrder();
    p1.issueOrder();

    std::cout << "Orders count: " << p1.getOrders()->size() << std::endl;

    auto defend = p1.toDefend();
    std::cout << "toDefend returned " << defend.size() << " countries\n";

    auto attack = p1.toAttack();
    std::cout << "toAttack returned " << attack.size() << " countries\n";

    std::cout << "Driver done.\n";
    return 0;
}