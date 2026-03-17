#include <iostream>
#include <vector>
#include <string>
#include "Cards.h"
#include "Player.h"
#include "Orders.h"
#include "Map.h"
#include "GameEngine.h"


using namespace std;


int main() {
    
    cout<< "-------------------------Part 1-----------------------------------------"<<endl;
    auto mapLoader = std::make_unique<MapLoader>("data/europe_map/europe.map");
    Map europe(std::move(mapLoader));
    europe.initialCountryDistribution(4);
    europe.display();
    cout << europe << endl;
    europe.validate();

    
    cout<< "-------------------------Part 2-----------------------------------------"<<endl;

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

    
    
    cout<< "-------------------------Part 3-----------------------------------------"<<endl;

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

    
    cout<< "-------------------------Part 4-----------------------------------------"<<endl;
    Deck deck;
    Player player("Joe");

    // Fill deck with 10 cards, 2 of each type
    deck.returnCard(new Card(CardType::bomb));
    deck.returnCard(new Card(CardType::reinforcement));
    deck.returnCard(new Card(CardType::blockade));
    deck.returnCard(new Card(CardType::airlift));
    deck.returnCard(new Card(CardType::diplomacy));
    deck.returnCard(new Card(CardType::bomb));
    deck.returnCard(new Card(CardType::reinforcement));
    deck.returnCard(new Card(CardType::blockade));
    deck.returnCard(new Card(CardType::airlift));
    deck.returnCard(new Card(CardType::diplomacy));
    
    cout << "Initial deck size: " << deck.size() << "\n";
    cout << "Initial hand size: " << player.getHand()->size() << "\n";
    cout << "Initial orders size: " << player.getOrders()->size() << "\n\n";

    cout << "Drawing 5 cards into player's hand\n";
    for (int i = 0; i < 5; i++) {
        Card* c = deck.draw();
        
        //  "if (c)" makes sure that the c getting drawn is not a nullptr
        if (c) player.getHand()->addCard(c);
    }

    cout << "After draw deck size: " << deck.size() << "\n";
    cout << "After draw hand size: " << player.getHand()->size() << "\n";
    cout << "After draw orders size: " << player.getOrders()->size() << "\n\n";

    cout << "Playing all cards\n";
    
    while (player.getHand()->size() > 0) {
        
        //  Returning the top card from the player's hand, which is the last card in the vector
        Card* c = player.getHand()->getCards()->back();
        c->play(&player, &deck);
    }

    cout << "After play deck size: " << deck.size() << "\n";
    cout << "After play hand size: " << player.getHand()->size() << "\n";
    cout << "After play orders size: " << player.getOrders()->size() << "\n\n";

    cout << "OrdersList content:\n";
    cout << *player.getOrders() << "\n";
    
    
    
    cout<< "-------------------------Part 5-----------------------------------------"<<endl;
#include <iostream>


    GameEngine* engine = new GameEngine();
    std::string input;
    std::cout << "=== Warzone Game Engine ===\n";
    while (true) {
        std::cout << "\nCurrent State: "
                  << engine->getCurrentState()->getName()
                  << "\n";
        std::cout << "Enter command: ";
        std::getline(std::cin, input);
        if (input.empty()) {
            std::cout << "Empty command. Try again.\n";
            continue;
        }
        engine->transition(input);
        std::cout << "\n--- Engine Status ---\n";
        std::cout << *engine << "\n";
        if (engine->getCurrentState()->getName() == "end") {
            std::cout << "\nGame ended.\n";
            break;
        }
    }
    delete engine;
    return 0;
}
