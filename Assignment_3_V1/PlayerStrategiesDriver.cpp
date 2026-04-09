/*
 * PlayerStrategiesDriver.cpp
 *
 * Driver for Assignment 3 Part 1: Player Strategy Pattern.
 *
 * Demonstrates:
 *  1) Different players can be assigned different strategies that lead to
 *     different behavior using the Strategy design pattern.
 *  2) The strategy adopted by a player can be changed dynamically during play.
 *  3) The human player makes decisions based on user interaction.
 *  4) Computer players make decisions automatically.
 *
 * Compile this as its own executable (separate from the other drivers).
 * Update the map path to match your local data directory.
 */

#include <iostream>
#include <vector>
#include "Player.h"
#include "PlayerStrategy.h"
#include "Map.h"
#include "Cards.h"
#include "Orders.h"

using namespace std;

static Country* makeCountry(int id, const string &name, int continent,
                             const string &owner, int armies) {
    Country *c = new Country();
    c->id = id;
    c->name = name;
    c->continentNumber = continent;
    c->playerName = new string(owner);
    c->armies = armies;
    c->x = c->y = 0;
    return c;
}

static void section(const string &title) {
    cout << "\n========================================\n";
    cout << "  " << title << "\n";
    cout << "========================================\n";
}

int main() {
    cout << "=== Assignment 3 Part 1: Player Strategy Pattern Driver ===\n";

    // Load a real map so adjacency checks work
    auto mapLoader = MapLoader("src/data/europe_map/europe.map");
    Map *gameMap = new Map(&mapLoader);

    // ── Create players and assign strategies ──
    Player *aggressive = new Player("AggressivePlayer");
    Player *benevolent = new Player("BenevolentPlayer");
    Player *neutral    = new Player("NeutralPlayer");
    Player *cheater    = new Player("CheaterPlayer");

    // setPlayerStrategy transfers ownership — Player destructor will delete these
    aggressive->setPlayerStrategy(new AggressivePlayerStrategy(aggressive, gameMap));
    benevolent->setPlayerStrategy(new BenevolentPlayerStrategy(benevolent, gameMap));
    neutral->setPlayerStrategy(   new NeutralPlayerStrategy(neutral, gameMap));
    cheater->setPlayerStrategy(   new CheaterPlayerStrategy(cheater, gameMap));

    // Give each player some territories
    Country *c1 = makeCountry(1, "Alpha",   1, "AggressivePlayer", 10);
    Country *c2 = makeCountry(2, "Beta",    1, "AggressivePlayer",  3);
    Country *c3 = makeCountry(3, "Gamma",   1, "BenevolentPlayer",  2);
    Country *c4 = makeCountry(4, "Delta",   1, "BenevolentPlayer",  8);
    Country *c5 = makeCountry(5, "Epsilon", 1, "NeutralPlayer",     5);
    Country *c6 = makeCountry(6, "Zeta",    1, "CheaterPlayer",     4);

    aggressive->addCountry(c1);
    aggressive->addCountry(c2);
    benevolent->addCountry(c3);
    benevolent->addCountry(c4);
    neutral->addCountry(c5);
    cheater->addCountry(c6);

    aggressive->setReinforcementPool(6);
    benevolent->setReinforcementPool(6);
    neutral->setReinforcementPool(6);
    cheater->setReinforcementPool(6);

    // Shared deck and initial cards
    Deck *deck = new Deck();
    for (int i = 0; i < 3; i++) deck->returnCard(new Card(CardType::bomb));
    for (int i = 0; i < 3; i++) deck->returnCard(new Card(CardType::airlift));
    for (int i = 0; i < 3; i++) deck->returnCard(new Card(CardType::diplomacy));
    deck->shuffle();
    for (Player *p : {aggressive, benevolent, neutral, cheater}) {
        Card *c = deck->draw();
        if (c) p->getHand()->addCard(c);
    }

    vector<Player*> allPlayers = {aggressive, benevolent, neutral, cheater};

    // ─────────────────────────────────────────────
    section("(1) Aggressive Strategy");
    cout << "Deploys all armies onto its strongest territory,\n"
         << "then attacks every adjacent enemy territory.\n";
    aggressive->setDoneIssuing(false);
    aggressive->issueOrder(allPlayers, deck);  // deploy phase (returns early)
    aggressive->issueOrder(allPlayers, deck);  // attack + done
    cout << "Aggressive orders queued: " << aggressive->getOrders()->size() << "\n";

    // ─────────────────────────────────────────────
    section("(2) Benevolent Strategy");
    cout << "Deploys onto its weakest territory, reinforces from strongest,\n"
         << "never issues attack orders.\n";
    benevolent->setDoneIssuing(false);
    benevolent->issueOrder(allPlayers, deck);  // deploy phase
    benevolent->issueOrder(allPlayers, deck);  // reinforce + done
    cout << "Benevolent orders queued: " << benevolent->getOrders()->size() << "\n";

    // ─────────────────────────────────────────────
    section("(3) Neutral Strategy - not attacked");
    cout << "Neutral player issues no orders at all.\n";
    neutral->setDoneIssuing(false);
    neutral->setAttacked(false);
    neutral->issueOrder(allPlayers, deck);
    cout << "Neutral done issuing: " << (neutral->isDoneIssuing() ? "yes" : "no") << "\n";
    cout << "Neutral orders queued: " << neutral->getOrders()->size() << "\n";

    // ─────────────────────────────────────────────
    section("(4) Dynamic Strategy Change - Neutral becomes Aggressive when attacked");
    cout << "Setting attacked=true, then calling issueOrder.\n"
         << "The strategy should switch to Aggressive mid-game.\n";
    // Note: after this call, neutral->playerStrategy points to AggressivePlayerStrategy
    neutral->setDoneIssuing(false);
    neutral->setReinforcementPool(5); // give armies so we can see deploy happen
    neutral->setAttacked(true);
    neutral->issueOrder(allPlayers, deck); // triggers switch + immediate aggressive turn
    cout << "Orders queued after switch: " << neutral->getOrders()->size() << "\n";

    // ─────────────────────────────────────────────
    section("(5) Cheater Strategy");
    cout << "Directly conquers all adjacent enemy territories without battle.\n";
    cheater->setDoneIssuing(false);
    cheater->issueOrder(allPlayers, deck);  // deploy phase
    cheater->issueOrder(allPlayers, deck);  // conquer phase + done
    cout << "Cheater now owns: " << cheater->getOwnedCountries().size()
         << " territories.\n";

    // ─────────────────────────────────────────────
    section("(6) Human Strategy - interactive");
    cout << "You will be prompted to enter orders interactively.\n";
    cout << "(Enter '4' at the action menu to mark done.)\n";

    Player *human = new Player("HumanPlayer");
    human->setPlayerStrategy(new HumanPlayerStrategy(human, gameMap));
    Country *hc = makeCountry(7, "Theta", 1, "HumanPlayer", 5);
    human->addCountry(hc);
    human->setReinforcementPool(4);
    Card *hcard = deck->draw();
    if (hcard) human->getHand()->addCard(hcard);

    allPlayers.push_back(human);
    human->setDoneIssuing(false);

    // Pool > 0 so this call goes straight to the deploy prompt
    human->issueOrder(allPlayers, deck);
    // Pool should now be 0 — action menu appears
    human->issueOrder(allPlayers, deck);

    cout << "Human orders queued: " << human->getOrders()->size() << "\n";

    // ── Cleanup ──
    // Countries were created with new — delete them
    // (Players do NOT delete owned countries in their destructor since normally
    //  the Map owns them. Here we created them manually so we delete them here.)
    delete deck;
    delete aggressive; // destructor deletes its playerStrategy
    delete benevolent;
    delete neutral;    // now holds AggressivePlayerStrategy after the switch
    delete cheater;
    delete human;
    // Delete manually created countries (not owned by a Map)
    delete c1; delete c2; delete c3; delete c4;
    delete c5; delete c6; delete hc;
    delete gameMap;

    cout << "\n=== Part 1 driver complete ===\n";
    return 0;
}
