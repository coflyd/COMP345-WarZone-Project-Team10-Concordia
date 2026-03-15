#include <iostream>
#include <vector>
#include <string>

#include "Cards.h"
#include "Player.h"
#include "Orders.h"
#include "Map.h"
#include "GameEngine.h"

using namespace std;

int main(int argc, char *argv[]) {

	/*

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

	 cout
	 << "-------------------------Part 3-----------------------------------------"
	 << endl;

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
	 Order *o = ol.at(i);
	 if (o)
	 o->execute();
	 }

	 // Print list after execution, now effects should show
	 std::cout << "\n--- Orders after execution (should print effect) ---\n";
	 std::cout << ol << "\n";

	 // Deep copy test to show Rule of 3 works
	 std::cout << "--- Copy OrdersList (deep copy demo) ---\n";
	 OrdersList copy = ol;
	 std::cout << copy << "\n";

	 std::cout << "=== Driver complete ===\n";

	 cout
	 << "-------------------------Part 4-----------------------------------------"
	 << endl;
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
	 Card *c = deck.draw();

	 //  "if (c)" makes sure that the c getting drawn is not a nullptr
	 if (c)
	 player.getHand()->addCard(c);
	 }

	 cout << "After draw deck size: " << deck.size() << "\n";
	 cout << "After draw hand size: " << player.getHand()->size() << "\n";
	 cout << "After draw orders size: " << player.getOrders()->size() << "\n\n";

	 cout << "Playing all cards\n";

	 while (player.getHand()->size() > 0) {

	 //  Returning the top card from the player's hand, which is the last card in the vector
	 Card *c = player.getHand()->getCards()->back();
	 c->play(&player, &deck);
	 }

	 cout << "After play deck size: " << deck.size() << "\n";
	 cout << "After play hand size: " << player.getHand()->size() << "\n";
	 cout << "After play orders size: " << player.getOrders()->size() << "\n\n";

	 cout << "OrdersList content:\n";
	 cout << *player.getOrders() << "\n";

	 cout
	 << "-------------------------Part 5-----------------------------------------"
	 << endl;

	 GameEngine *engine = new GameEngine();
	 CommandProcessor *processor = nullptr;
	 */

	cout
			<< "-------------------------Part 1-----------------------------------------"
			<< endl;
	GameEngine *engine = new GameEngine();

	std::string arg = "-console";

	// Vérifier les arguments de la ligne de commande
	if (argc > 1) {
		arg = argv[1];
		if (arg == "-console") {
			engine->commandProcessor = engine->getCommandProcessor();

		} else if (arg == "-file" && argc > 2) {
			std::string filename = argv[2];
			engine->commandProcessor = new FileCommandProcessorAdapter(
					filename);
			engine->setCommandProcessor(engine->commandProcessor);
		} else {
			std::cerr << "Usage: " << argv[0]
					<< " [-console | -file <filename>]" << std::endl;
			delete engine;
			return (1);
		}
	} else {
		engine->commandProcessor = engine->getCommandProcessor();
	}

	std::string input;
	std::cout << "=== Warzone Game Engine ===\n";

	bool fileMode = (arg == "-file" && argc > 2);
	bool endOfFile = false;

	cout
			<< "-------------------------Part 2-----------------------------------------"
			<< endl;
	engine->startupPhase();

	while (true) {
		std::cout << "\nCurrent State: " << engine->getCurrentState() << "\n";

		if (fileMode && !endOfFile) {
			input = engine->commandProcessor->getCommand()->getType();
			if (input.empty()) {
				std::cout
						<< "No more commands in file. Switching to console mode.\n";
				endOfFile = true;
			}
		} else {
			std::cout << "Enter command: ";
			std::getline(std::cin, input);
			if (input.empty()) {
				continue;
			}
		}

		// Valider la commande
		if (!engine->commandProcessor->validate(input,
				engine->getCurrentState())) {
			std::cout << "\"" << input
					<< "\" is an invalid command for current state. Ignoring...\n";
			continue;
		}

		// Exécuter la transition
		engine->transition(input);

		std::cout << "\n--- Engine Status ---\n";
		std::cout << *engine << "\n";

		if (engine->getCurrentState().getName() == "end") {
			std::cout << "\nGame ended.\n";
			break;
		}
	}

	if (arg == "-file" && argc > 2) {
		delete engine->commandProcessor;
	}
	delete engine;

	return (0);

}
