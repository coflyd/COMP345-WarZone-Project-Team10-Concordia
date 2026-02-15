#include "Orders.h"
#include <iostream>

int main() {
    std::cout << "=== Part 3: Orders List Driver ===\n\n";

    // Create the OrdersList
    OrdersList ol;

    // Create at least one of each order type and add to list
    ol.add(new Deploy("Alaska", 5));
    ol.add(new Advance("Alaska", "Ontario", 3));
    ol.add(new Bomb("Quebec"));
    ol.add(new Blockade("Brazil"));
    ol.add(new Airlift("Brazil", "Japan", 4));
    ol.add(new Negotiate("Player1", "Player2"));

    // Add an invalid order to show it can exist (but will not execute)
    ol.add(new Deploy("", -10)); // invalid on purpose

    // Print list before doing anything
    std::cout << "--- Initial Orders ---\n";
    std::cout << ol << "\n";

    // Demonstrate move()
    std::cout << "--- Move order 0 to index 3 ---\n";
    ol.move(0, 3);
    std::cout << ol << "\n";

    // Demonstrate remove()
    std::cout << "--- Remove order at index 2 ---\n";
    ol.remove(2);
    std::cout << ol << "\n";

    // Execute all orders (execute validates first)
    std::cout << "--- Execute all orders ---\n";
    for (int i = 0; i < ol.size(); ++i) {
        Order* o = ol.at(i);
        if (o) o->execute();
    }

    // Print list after execution, now effects should show
    std::cout << "\n--- Orders after execution (should print effect) ---\n";
    std::cout << ol << "\n";

    // Deep copy test to show Rule of 3 works
    std::cout << "--- Copy OrdersList (deep copy demo) ---\n";
    OrdersList copy = ol;
    std::cout << copy << "\n";

    std::cout << "=== Driver complete ===\n";
    return 0;
}
