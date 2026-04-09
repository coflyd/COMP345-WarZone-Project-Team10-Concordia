/*
 * PlayerStrategy.cpp
 *
 * Implements the Strategy design pattern for Warzone player behavior.
 * Each ConcreteStrategy provides its own issueOrder(), toAttack(), toDefend().
 * Player delegates those calls here rather than implementing behavior itself.
 */

#include <algorithm>
#include <iostream>
#include <limits>
#include "PlayerStrategy.h"
#include "Player.h"
#include "Orders.h"
#include "Cards.h"

// =============================================================================
// HumanPlayerStrategy
// =============================================================================

HumanPlayerStrategy::HumanPlayerStrategy(Player *p, Map *m) :
        PlayerStrategy(p, m) {
}

// Returns all owned territories sorted weakest-first (fewest armies first)
std::vector<Country*> HumanPlayerStrategy::toDefend() {
    std::vector<Country*> defend = player->getOwnedCountries();
    std::sort(defend.begin(), defend.end(), [](Country *a, Country *b) {
        return a->armies < b->armies;
    });
    return defend;
}

// Returns all adjacent enemy territories sorted weakest-first
std::vector<Country*> HumanPlayerStrategy::toAttack(
        std::vector<Player*> &allPlayers) {
    std::vector<Country*> targets;
    for (Player *other : allPlayers) {
        if (other == player) continue;
        for (Country *c : other->getOwnedCountries()) {
            if (map->isAdjacentToAnyOwnedCountry(c, player->getOwnedCountries()))
                targets.push_back(c);
        }
    }
    std::sort(targets.begin(), targets.end(), [](Country *a, Country *b) {
        return a->armies < b->armies;
    });
    return targets;
}

/*
 * issueOrder: interactive.
 * While the reinforcement pool is > 0, forces the player to deploy before
 * anything else (game rule). After deploying, a menu is shown each call.
 * Choosing "Done" sets doneIssuing = true so the engine stops asking.
 */
void HumanPlayerStrategy::issueOrder(std::vector<Player*> &allPlayers,
        Deck *deck) {

    std::cout << "\n--- " << player->getName() << "'s turn (Human) ---\n";
    std::cout << "Reinforcement pool: " << player->getReinforcementPool() << "\n";
    std::cout << "Hand size: " << player->getHand()->size() << "\n";

    // Phase A: forced deploy while armies remain
    if (player->getReinforcementPool() > 0) {
        std::vector<Country*> defend = toDefend();
        std::cout << "Your territories (weakest first):\n";
        for (int i = 0; i < (int)defend.size(); i++)
            std::cout << "  " << i << ": " << defend[i]->name
                      << " (" << defend[i]->armies << " armies)\n";

        int idx = -1, armies = 0;
        while (idx < 0 || idx >= (int)defend.size()) {
            std::cout << "Deploy to territory (0-" << defend.size()-1 << "): ";
            std::cin >> idx;
            if (std::cin.fail()) { std::cin.clear(); idx = -1; }
        }
        while (armies <= 0 || armies > player->getReinforcementPool()) {
            std::cout << "How many armies (1-" << player->getReinforcementPool() << ")? ";
            std::cin >> armies;
            if (std::cin.fail()) { std::cin.clear(); armies = 0; }
        }
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

        player->getOrders()->add(new Deploy(player, defend[idx], armies));
        player->setReinforcementPool(player->getReinforcementPool() - armies);
        std::cout << "[Human] Deploy " << armies << " -> " << defend[idx]->name << "\n";
        return;
    }

    // Phase B-D: action menu
    std::cout << "Choose an action:\n"
              << "  1: Advance (defend - move armies between own territories)\n"
              << "  2: Advance (attack - move armies to enemy territory)\n"
              << "  3: Play a card\n"
              << "  4: Done issuing orders\n"
              << "Choice: ";

    int choice = 0;
    std::cin >> choice;
    if (std::cin.fail()) { std::cin.clear(); choice = 4; }
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    switch (choice) {

    case 1: { // Advance to defend
        std::vector<Country*> defend = toDefend();
        if (defend.size() < 2) {
            std::cout << "Need at least 2 own territories to advance between.\n";
            break;
        }
        std::cout << "Your territories:\n";
        for (int i = 0; i < (int)defend.size(); i++)
            std::cout << "  " << i << ": " << defend[i]->name
                      << " (" << defend[i]->armies << " armies)\n";
        int src = -1, dst = -1, armies = 0;
        while (src < 0 || src >= (int)defend.size()) {
            std::cout << "Source index: ";
            std::cin >> src;
            if (std::cin.fail()) { std::cin.clear(); src = -1; }
        }
        while (dst < 0 || dst >= (int)defend.size() || dst == src) {
            std::cout << "Destination index: ";
            std::cin >> dst;
            if (std::cin.fail()) { std::cin.clear(); dst = -1; }
        }
        while (armies <= 0) {
            std::cout << "Armies to move: ";
            std::cin >> armies;
            if (std::cin.fail()) { std::cin.clear(); armies = 0; }
        }
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        player->getOrders()->add(
            new Advance(player, defend[src], defend[dst], map, deck, armies));
        std::cout << "[Human] Advance (defend) " << armies << " "
                  << defend[src]->name << " -> " << defend[dst]->name << "\n";
        break;
    }

    case 2: { // Advance to attack
        std::vector<Country*> targets = toAttack(allPlayers);
        std::vector<Country*> defend  = toDefend();
        if (targets.empty()) {
            std::cout << "No adjacent enemy territories to attack.\n";
            break;
        }
        std::cout << "Attack targets (weakest first):\n";
        for (int i = 0; i < (int)targets.size(); i++)
            std::cout << "  " << i << ": " << targets[i]->name
                      << " (" << targets[i]->armies << " armies)\n";
        std::cout << "Your territories (source):\n";
        for (int i = 0; i < (int)defend.size(); i++)
            std::cout << "  " << i << ": " << defend[i]->name
                      << " (" << defend[i]->armies << " armies)\n";
        int tgt = -1, src = -1, armies = 0;
        while (tgt < 0 || tgt >= (int)targets.size()) {
            std::cout << "Target index: ";
            std::cin >> tgt;
            if (std::cin.fail()) { std::cin.clear(); tgt = -1; }
        }
        while (src < 0 || src >= (int)defend.size()) {
            std::cout << "Source index: ";
            std::cin >> src;
            if (std::cin.fail()) { std::cin.clear(); src = -1; }
        }
        while (armies <= 0) {
            std::cout << "Armies to send: ";
            std::cin >> armies;
            if (std::cin.fail()) { std::cin.clear(); armies = 0; }
        }
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        player->getOrders()->add(
            new Advance(player, defend[src], targets[tgt], map, deck, armies));
        std::cout << "[Human] Advance (attack) " << armies << " "
                  << defend[src]->name << " -> " << targets[tgt]->name << "\n";
        break;
    }

    case 3: { // Play a card
        const std::vector<Card*> *cards = player->getHand()->getCards();
        if (cards == nullptr || cards->empty()) {
            std::cout << "No cards in hand.\n";
            break;
        }
        std::cout << "Cards in hand:\n";
        for (int i = 0; i < (int)cards->size(); i++)
            std::cout << "  " << i << ": " << *(*cards)[i] << "\n";
        int idx = -1;
        while (idx < 0 || idx >= (int)cards->size()) {
            std::cout << "Card index to play: ";
            std::cin >> idx;
            if (std::cin.fail()) { std::cin.clear(); idx = -1; }
        }
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        Card *card = (*cards)[idx];
        std::cout << "[Human] Playing card: " << *card << "\n";
        card->play(player, deck);
        break;
    }

    default:
    case 4:
        player->setDoneIssuing(true);
        std::cout << "[Human] Done issuing orders.\n";
        break;
    }
}

// =============================================================================
// AggressivePlayerStrategy
// =============================================================================

AggressivePlayerStrategy::AggressivePlayerStrategy(Player *p, Map *m) :
        PlayerStrategy(p, m) {
}

// Returns territories sorted strongest-first (most armies first)
std::vector<Country*> AggressivePlayerStrategy::toDefend() {
    std::vector<Country*> defend = player->getOwnedCountries();
    std::sort(defend.begin(), defend.end(), [](Country *a, Country *b) {
        return a->armies > b->armies;
    });
    return defend;
}

// Returns all adjacent enemy territories
std::vector<Country*> AggressivePlayerStrategy::toAttack(
        std::vector<Player*> &allPlayers) {
    std::vector<Country*> targets;
    for (Player *other : allPlayers) {
        if (other == player) continue;
        for (Country *c : other->getOwnedCountries()) {
            if (map->isAdjacentToAnyOwnedCountry(c, player->getOwnedCountries()))
                targets.push_back(c);
        }
    }
    return targets;
}

/*
 * issueOrder:
 *   A - Deploy all armies onto the strongest territory.
 *   B - Consolidate: move half the armies from strongest to weakest.
 *   C - Attack every adjacent enemy from the strongest adjacent source.
 *   D - Play one card.
 *   Then mark done.
 */
void AggressivePlayerStrategy::issueOrder(std::vector<Player*> &allPlayers,
        Deck *deck) {
    if (player->getReinforcementPool() > 0) {
        std::vector<Country*> defend = toDefend();
        if (!defend.empty()) {
            Country *target = defend.front();
            int armies = player->getReinforcementPool();
            player->getOrders()->add(new Deploy(player, target, armies));
            std::cout << "[" << player->getName() << "] Deploy " << armies
                      << " -> " << target->name << "\n";
            player->setReinforcementPool(0);
        }
        return;
    }

    std::vector<Country*> defend = toDefend();
    if (defend.size() >= 2) {
        Country *strongest = defend.front();
        Country *weakest   = defend.back();
        if (strongest->armies > 1) {
            int toMove = strongest->armies / 2;
            player->getOrders()->add(
                new Advance(player, strongest, weakest, map, deck, toMove));
            std::cout << "[" << player->getName() << "] Advance (consolidate) "
                      << toMove << " " << strongest->name << " -> "
                      << weakest->name << "\n";
        }
    }

    std::vector<Country*> attackTargets = toAttack(allPlayers);
    for (Country *target : attackTargets) {
        Country *source = map->findStrongestAdjacentCountry(
                target, player->getOwnedCountries());
        if (source != nullptr && source->armies > 1) {
            int toSend = source->armies - 1;
            player->getOrders()->add(
                new Advance(player, source, target, map, deck, toSend));
            std::cout << "[" << player->getName() << "] Advance (attack) "
                      << toSend << " " << source->name << " -> "
                      << target->name << "\n";
        }
    }

    const std::vector<Card*> *cards = player->getHand()->getCards();
    if (cards != nullptr && !cards->empty() && deck != nullptr) {
        Card *card = (*cards)[0];
        std::cout << "[" << player->getName() << "] Playing card: " << *card << "\n";
        card->play(player, deck);
    }

    player->setDoneIssuing(true);
    std::cout << "[" << player->getName() << "] Done issuing orders.\n";
}

// =============================================================================
// BenevolentPlayerStrategy
// =============================================================================

BenevolentPlayerStrategy::BenevolentPlayerStrategy(Player *p, Map *m) :
        PlayerStrategy(p, m) {
}

// Returns territories sorted weakest-first (fewest armies first)
std::vector<Country*> BenevolentPlayerStrategy::toDefend() {
    std::vector<Country*> defend = player->getOwnedCountries();
    std::sort(defend.begin(), defend.end(), [](Country *a, Country *b) {
        return a->armies < b->armies;
    });
    return defend;
}

// Benevolent player never attacks
std::vector<Country*> BenevolentPlayerStrategy::toAttack(
        std::vector<Player*> &allPlayers) {
    return {};
}

/*
 * issueOrder:
 *   A - Deploy onto the weakest territory.
 *   B - Reinforce weakest from strongest.
 *   D - Play one card.
 *   Never attacks. Then mark done.
 */
void BenevolentPlayerStrategy::issueOrder(std::vector<Player*> &allPlayers,
        Deck *deck) {
    if (player->getReinforcementPool() > 0) {
        std::vector<Country*> defend = toDefend();
        if (!defend.empty()) {
            Country *target = defend.front();
            int armies = player->getReinforcementPool();
            player->getOrders()->add(new Deploy(player, target, armies));
            std::cout << "[" << player->getName() << "] Deploy " << armies
                      << " -> " << target->name << "\n";
            player->setReinforcementPool(0);
        }
        return;
    }

    std::vector<Country*> defend = toDefend();
    if (defend.size() >= 2) {
        Country *weakest   = defend.front();
        Country *strongest = defend.back();
        if (strongest->armies > 1) {
            int toMove = strongest->armies / 2;
            player->getOrders()->add(
                new Advance(player, strongest, weakest, map, deck, toMove));
            std::cout << "[" << player->getName() << "] Advance (reinforce) "
                      << toMove << " " << strongest->name << " -> "
                      << weakest->name << "\n";
        }
    }

    const std::vector<Card*> *cards = player->getHand()->getCards();
    if (cards != nullptr && !cards->empty() && deck != nullptr) {
        Card *card = (*cards)[0];
        std::cout << "[" << player->getName() << "] Playing card: " << *card << "\n";
        card->play(player, deck);
    }

    player->setDoneIssuing(true);
    std::cout << "[" << player->getName() << "] Done issuing orders.\n";
}

// =============================================================================
// NeutralPlayerStrategy
// =============================================================================

NeutralPlayerStrategy::NeutralPlayerStrategy(Player *p, Map *m) :
        PlayerStrategy(p, m) {
}

// Returns territories sorted weakest-first
std::vector<Country*> NeutralPlayerStrategy::toDefend() {
    std::vector<Country*> defend = player->getOwnedCountries();
    std::sort(defend.begin(), defend.end(), [](Country *a, Country *b) {
        return a->armies < b->armies;
    });
    return defend;
}

// Neutral player never attacks
std::vector<Country*> NeutralPlayerStrategy::toAttack(
        std::vector<Player*> &allPlayers) {
    return {};
}

/*
 * issueOrder: issues no orders.
 * If this player was attacked this turn it switches to Aggressive dynamically
 * and delegates to the new strategy for the remainder of this call.
 * IMPORTANT: after setPlayerStrategy() is called, 'this' is deleted.
 *            No member access is allowed after that point.
 */
void NeutralPlayerStrategy::issueOrder(std::vector<Player*> &allPlayers,
        Deck *deck) {
    if (player->getAttacked()) {
        std::cout << "[" << player->getName()
                  << "] Was attacked! Switching to Aggressive strategy.\n";
        player->setPlayerStrategy(new AggressivePlayerStrategy(player, map));
        player->setAttacked(false);
        player->issueOrder(allPlayers, deck); // 'this' is now deleted — return immediately
        return;
    }

    player->setDoneIssuing(true);
    std::cout << "[" << player->getName() << "] Neutral - issuing no orders.\n";
}

// =============================================================================
// CheaterPlayerStrategy
// =============================================================================

CheaterPlayerStrategy::CheaterPlayerStrategy(Player *p, Map *m) :
        PlayerStrategy(p, m) {
}

// Returns territories sorted weakest-first
std::vector<Country*> CheaterPlayerStrategy::toDefend() {
    std::vector<Country*> defend = player->getOwnedCountries();
    std::sort(defend.begin(), defend.end(), [](Country *a, Country *b) {
        return a->armies < b->armies;
    });
    return defend;
}

// Returns all adjacent enemy territories
std::vector<Country*> CheaterPlayerStrategy::toAttack(
        std::vector<Player*> &allPlayers) {
    std::vector<Country*> targets;
    for (Player *other : allPlayers) {
        if (other == player) continue;
        for (Country *c : other->getOwnedCountries()) {
            if (map->isAdjacentToAnyOwnedCountry(c, player->getOwnedCountries()))
                targets.push_back(c);
        }
    }
    return targets;
}

/*
 * issueOrder:
 *   Deploy reinforcements first (required by game rules).
 *   Then directly conquer all adjacent enemy territories without battle
 *   by transferring ownership. Then mark done.
 */
void CheaterPlayerStrategy::issueOrder(std::vector<Player*> &allPlayers,
        Deck *deck) {
    if (player->getReinforcementPool() > 0) {
        std::vector<Country*> defend = toDefend();
        if (!defend.empty()) {
            Country *target = defend.front();
            int armies = player->getReinforcementPool();
            player->getOrders()->add(new Deploy(player, target, armies));
            std::cout << "[" << player->getName() << "] Deploy " << armies
                      << " -> " << target->name << "\n";
            player->setReinforcementPool(0);
        }
        return;
    }

    // Collect targets first to avoid modifying the list while iterating
    std::vector<Country*> targets = toAttack(allPlayers);
    for (Country *target : targets) {
        for (Player *other : allPlayers) {
            if (other == player) continue;
            bool owns = false;
            for (Country *c : other->getOwnedCountries())
                if (c == target) { owns = true; break; }
            if (owns) {
                other->removeCountry(target);
                player->addCountry(target);
                delete target->playerName;
                target->playerName = new std::string(player->getName());
                std::cout << "[" << player->getName() << "] Cheat conquered: "
                          << target->name << "\n";
                break;
            }
        }
    }

    player->setDoneIssuing(true);
    std::cout << "[" << player->getName() << "] Done issuing orders.\n";
}
