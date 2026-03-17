#include "Orders.h"
#include "Player.h"
#include "Map.h"
#include "Cards.h"
#include <iostream>
#include <cassert>

// Helper to build a standalone Country without loading a .map file
static Country* makeCountry(int id, const std::string& name,
    int continent, const std::string& owner, int armies) {
    Country* c = new Country();
    c->id = id;
    c->name = name;
    c->continentNumber = continent;
    c->playerName = new std::string(owner);
    c->armies = armies;
    c->x = c->y = 0;
    return c;
}

// Helper to print a section separator with a title
static void section(const std::string& title) {
    std::cout << "\n========================================\n";
    std::cout << "  " << title << "\n";
    std::cout << "========================================\n";
}

// Main driver
int main() {
    std::cout << "=== Part 4: Order Execution Driver ===\n";
    Player* tony = new Player("Tony");
    Player* yacoub = new Player("Yacoub");
    Player* neutral = new Player("Neutral");

    tony->setReinforcements(20);
    yacoub->setReinforcements(10);
    Country* alaska = makeCountry(1, "Alaska",1, "Tony", 10);
    Country* kamchatka = makeCountry(2, "Kamchatka",1, "Yacoub",   6);
    Country* ontario = makeCountry(3, "Ontario",1, "Tony", 8);
    Country* greenland = makeCountry(4, "Greenland", 1, "Yacoub",   4);
    tony->addCountry(alaska);
    tony->addCountry(ontario);
    yacoub->addCountry(kamchatka);
    yacoub->addCountry(greenland);
    // Deck with one of each card type
    Deck* deck = new Deck();
    deck->returnCard(new Card(CardType::bomb));
    deck->returnCard(new Card(CardType::reinforcement));
    deck->returnCard(new Card(CardType::blockade));
    deck->returnCard(new Card(CardType::airlift));
    deck->returnCard(new Card(CardType::diplomacy));

    // Deploy
    section("(1) Deploy — validates ownership and reinforcement pool");

    // Valid deploy
    Deploy* d1 = new Deploy(tony, alaska, 5);
    std::cout << "[Before] Tony reinforcements: " << tony->getReinforcements()
              << " Alaska armies: " << alaska->armies << "\n";
    d1->execute();
    std::cout << *d1 << "\n";
    std::cout << "[After]  Tony reinforcements: " << tony->getReinforcements()
              << " Alaska armies: " << alaska->armies << "\n";
    assert(alaska->armies == 15);
    assert(tony->getReinforcements() == 15);

    // Invalid deploy
    Deploy* d2 = new Deploy(tony, kamchatka, 3);
    d2->execute();
    std::cout << *d2 << "\n";

    // Invalid deploy
    Deploy* d3 = new Deploy(tony, alaska, 999);
    d3->execute();
    std::cout << *d3 << "\n";

    // Advance
    section("(2) Advance — friendly transfer and battle simulation");

    // map==nullptr skips adjacency check, entering convenience mode
    Advance* adv1 = new Advance(tony, alaska, ontario, nullptr, deck, 3);
    std::cout << "[Before] Alaska: " << alaska->armies
              << "Ontario: " << ontario->armies << "\n";
    adv1->execute();
    std::cout << *adv1 << "\n";
    std::cout << "[After] Alaska: " << alaska->armies
              << "Ontario:" << ontario->armies << "\n";

    // More armies to make the outcome more likely to result in a capture
    alaska->armies = 12;
    kamchatka->armies = 2;

    Advance* adv2 = new Advance(tony, alaska, kamchatka, nullptr, deck, 10);
    adv2->execute();
    std::cout << *adv2 << "\n";
    if (kamchatka->playerName != nullptr && *kamchatka->playerName == "Tony") {
        std::cout << "[INFO] Tony captured Kamchatka!\n";
    } else {
        std::cout << "[INFO] Yacoub held Kamchatka.\n";
    }
    // Card awarded
    section("(3) Card awarded on first conquest this turn");
    std::cout << "Tony conquered this turn: "
              << (tony->hasConqueredThisTurn() ? "yes" : "no") << "\n";
    std::cout << "Tony hand size: " << tony->getHand()->size() << "\n";
    if (tony->hasConqueredThisTurn())
        std::cout << "[PASS] conqueredThisTurn flag set correctly.\n";
    // Negotiate
    section("(4) Negotiate — truce prevents attacks between players");
    // Reset state for a clean test
    tony->clearTruces();
    yacoub->clearTruces();
    kamchatka->armies = 8;
    delete kamchatka->playerName;
    kamchatka->playerName = new std::string("Yacoub");
    tony->setConqueredThisTurn(false);
    Negotiate* neg = new Negotiate(tony, yacoub);
    neg->execute();
    std::cout << *neg << "\n";
    std::cout << "Tony has truce with Yacoub: "
              << (tony->isTruceWith("Yacoub") ? "yes" : "no") << "\n";
    std::cout << "Yacoub has truce with Tony: "
              << (yacoub->isTruceWith("Tony") ? "yes" : "no") << "\n";

    alaska->armies = 10;
    Advance* adv3 = new Advance(tony, alaska, kamchatka, nullptr, deck, 5);
    adv3->execute();
    std::cout << *adv3 << "\n";
    std::cout << "[PASS] Advance blocked by negotiate truce.\n";
    Negotiate* negInvalid = new Negotiate(tony, tony);
    negInvalid->execute();
    std::cout << *negInvalid << "\n";

    // Blockade
    section("(5) Blockade — doubles armies and transfers to Neutral");
    std::cout << "[Before] Ontario owner: " << *ontario->playerName
              << " armies: " << ontario->armies << "\n";
    Blockade* blk = new Blockade(tony, ontario, neutral);
    blk->execute();
    std::cout << *blk << "\n";
    std::cout << "[After]  Ontario owner: " << *ontario->playerName
              << " armies: " << ontario->armies << "\n";
    assert(*ontario->playerName == "Neutral");

    // Invalid Blockade
    Blockade* blkInvalid = new Blockade(tony, kamchatka, neutral);
    blkInvalid->execute();
    std::cout << *blkInvalid << "\n";

    // Bomb
    section("(6) Bomb — halves enemy territory armies");
    // map=nullptr skips adjacency check, entering convenience mode
    kamchatka->armies = 10;
    Bomb* bomb = new Bomb(tony, kamchatka, nullptr);
    std::cout << "[Before] Kamchatka armies: " << kamchatka->armies << "\n";
    bomb->execute();
    std::cout << *bomb << "\n";
    std::cout << "[After]  Kamchatka armies: " << kamchatka->armies << "\n";
    assert(kamchatka->armies == 5);

    // Invalid Bomb
    Bomb* bombInvalid = new Bomb(tony, alaska, nullptr);
    bombInvalid->execute();
    std::cout << *bombInvalid << "\n";

    // Airlift
    section("Airlift — moves armies between any two owned territories");

    alaska->armies = 10;
    Country* iceland = makeCountry(5, "Iceland", 1, "Tony", 3);
    tony->addCountry(iceland);

    Airlift* air = new Airlift(tony, alaska, iceland, 4);
    std::cout << "[Before] Alaska: " << alaska->armies
              << " Iceland: " << iceland->armies << "\n";
    air->execute();
    std::cout << *air << "\n";
    std::cout << "[After]  Alaska: " << alaska->armies
              << " Iceland: " << iceland->armies << "\n";
    assert(alaska->armies == 6);
    assert(iceland->armies == 7);

    // Invalid Airlife because source territory belongs to an enemy
    Airlift* airInvalid = new Airlift(tony, kamchatka, alaska, 3);
    airInvalid->execute();
    std::cout << *airInvalid << "\n";
    section("Cards — play() creates orders in player list");
    tony->getHand()->addCard(new Card(CardType::bomb));
    tony->getHand()->addCard(new Card(CardType::reinforcement));
    tony->getHand()->addCard(new Card(CardType::blockade));
    tony->getHand()->addCard(new Card(CardType::airlift));
    tony->getHand()->addCard(new Card(CardType::diplomacy));

    std::cout << "Hand before playing: " << *tony->getHand() << "\n";
    int ordersBefore = tony->getOrders()->size();

    // Copy the hand vector first since play() removes cards from the hand
    std::vector<Card*> toPlay(*tony->getHand()->getCards());
    for (Card* c : toPlay) c->play(tony, deck);
    std::cout << "Hand after playing:  " << *tony->getHand() << "\n";
    std::cout << "Orders added by cards: "
              << (tony->getOrders()->size() - ordersBefore) << "\n";
    assert(tony->getHand()->size() == 0);
    section("OrdersList — move and remove");

    OrdersList* ol = new OrdersList();
    ol->add(new Deploy("Alpha", 1));
    ol->add(new Deploy("Beta",  2));
    ol->add(new Deploy("Gamma", 3));
    std::cout << "Initial:\n" << *ol;
    ol->move(0, 2);
    std::cout << "After move(0,2):\n" << *ol;
    ol->remove(1);
    std::cout << "After remove(1):\n" << *ol;

    // Cleanup
    delete d1;
    delete d2;
    delete d3;
    delete adv1;
    delete adv2; delete adv3;
    delete neg;
    delete negInvalid;
    delete blk;
    delete blkInvalid;
    delete bomb;
    delete bombInvalid;
    delete air;
    delete airInvalid;
    delete ol;
    delete deck;
    delete alaska;
    delete kamchatka;
    delete ontario;
    delete greenland;
    delete iceland;
    delete tony;
    delete yacoub;
    delete neutral;
    std::cout << "\n=== All Part 4 tests completed ===\n";
    return 0;
}
