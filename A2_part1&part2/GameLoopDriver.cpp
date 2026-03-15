// GameLoopDriver.cpp Part 3 Driver

// 1 correct reinforcement army count (different cases)
// 2 only Deploy orders while reinforcement pool > 0
// 3 Advance orders to defend and attack
// 4 card play produces orders
// 5 player with no territories is removed
// 6 game ends when one player owns all territories

#include <iostream>
#include "GameEngine.h"
#include "Player.h"
#include "Map.h"
#include "Orders.h"
#include "Cards.h"

int main() {
	std::cout << "============================================\n";
	std::cout << "			Part 3: Main Game Loop			   \n";
	std::cout << "============================================\n\n";


	// 1 and 2: Reinforcement and different cases

	std::cout << "=== 1 and 2: Reinforcement Phase ===\n\n";
	{
		// Load map
		MapLoader loader("data/europe.map");
		Map map(&loader);

		Player *alice = new Player("Alice");  // will get 3 territories -> 3 armies (min)
		Player *bob   = new Player("Bob");    // will get 9 territories -> 3 armies
		Player *carol = new Player("Carol");  // will get 12 territories -> 4 armies

		int total = map.getCountriesNumber();
		int i = 1;
		// Give Alice 3, Bob 9, Carol the rest
		while (i <= 3 && i <= total)  { alice->addCountry(map.getCountryById(i)); i++; }
		while (i <= 12 && i <= total) { bob->addCountry(map.getCountryById(i));   i++; }
		while (i <= total)            { carol->addCountry(map.getCountryById(i));  i++; }

		std::vector<Player*> players = {alice, bob, carol};

		// Give each territory 10 armies so issueOrder can move them
		for (Player *p : players) {
			for (Country *c : p->getOwnedCountries()) {
				c->armies = 10;
			}
		}

		// Compute and display reinforcements
		for (Player *p : players) {
			int numT   = (int)p->getOwnedCountries().size();
			int armies = std::max(3, numT / 3);
			p->setReinforcementPool(armies);
			std::cout << p->getName() << " owns " << numT
					  << " territories -> " << armies << " reinforcement armies.\n";
		}

		// 2: Alice has 3 armies in pool -> only Deploy issued
		std::cout << "\n--- 2: Alice issues only Deploy while pool > 0 ---\n";
		alice->issueOrder(&map, players, nullptr);
		std::cout << "Alice orders: " << *alice->getOrders() << "\n";
		std::cout << "Alice pool after: " << alice->getReinforcementPool() << " (should be 0)\n";

		delete alice;
		delete bob;
		delete carol;
	}


	// 3 and 4: Advance + Card play
 
	std::cout << "\n=== 3 and 4: Advance orders + Card play ===\n\n";
	{
		MapLoader loader("data/europe.map");
		Map map(&loader);

		Player *alice = new Player("Alice");
		Player *bob   = new Player("Bob");

		// Alice owns territories 1 and 2
		Country *t1 = map.getCountryById(1);
		Country *t2 = map.getCountryById(2);
		Country *t3 = map.getCountryById(3);

		if (t1) { t1->armies = 10; alice->addCountry(t1); }
		if (t2) { t2->armies = 2;  alice->addCountry(t2); }
		if (t3) { t3->armies = 5;  bob->addCountry(t3); }

		// Give Alice a bomb card
		Deck deck;
		alice->getHand()->addCard(new Card(CardType::bomb));

		std::vector<Player*> players = {alice, bob};

		// Pool is 0 -> should issue Advance (defend + attack) + play card
		alice->setReinforcementPool(0);
		alice->setDoneIssuing(false);
		alice->issueOrder(&map, players, &deck);

		std::cout << "\nAlice orders:\n" << *alice->getOrders() << "\n";
		std::cout << "Alice hand size (should be 0 after playing): "
				  << alice->getHand()->size() << "\n";

		delete alice;
		delete bob;
	}

	// 5 and 6: Full game loop — elimination + win

	std::cout << "\n=== 5 and 6: Elimination & Win condition ===\n\n";
	std::cout << "Starting full game. Follow prompts to set up.\n\n";

	GameEngine engine;
	engine.startupPhase();  // loadmap, validatemap, addplayer, gamestart
	engine.mainGameLoop();  // runs until winner

	return 0;
}
