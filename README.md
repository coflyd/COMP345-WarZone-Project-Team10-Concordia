# COMP 345 — Warzone Game Engine in C++
### Team 10 · Concordia University · Winter 2026

This is our C++ implementation of the Warzone strategy game, developed as a team project for COMP 345 at Concordia University. Over three assignments, we built a fully playable game engine from scratch — starting with the map and player structures, adding a real game loop with order execution, and finishing with AI strategies and tournament mode.

## Team Members

| Name | Component |
|------|-----------|
| Amir Hossein Shiri Deh Piri | Map & Map Loader |
| Constance Fleury | Player & Orders |
| Hamed Vasheghani Farahani | Game Engine & Startup Phase |
| Tony Yacoub | Cards & Logging Observer |
| Sitong Guo | Command Processing & Strategy |


## What This Project Is

Warzone is a turn-based strategy game where players compete to control territories across a world map. This project reimplements its core mechanics in C++: loading maps, managing players and armies, issuing and executing orders, and running full games automatically.

The codebase was built incrementally across three sprints, each introducing new systems while keeping everything integrated. Every component follows strict OOP principles — pointer-based data members, proper copy constructors and assignment operators, no memory leaks, and clean separation between `.h` and `.cpp` files.


## Sprint Breakdown

### Assignment 1 — Core Foundations

The first sprint was about getting the essentials right. We built the map as a connected graph (territories as nodes, adjacency as edges), implemented the player structure with territory ownership and an order list, defined all six order types, created the card deck and hand system, and wired up the game engine as a state machine that responds to console commands.

- `Map.cpp / Map.h` — Graph-based map with `validate()` checking connectivity and continent integrity. Reads `.map` files from the Domination format.
- `Player.cpp / Player.h` — Owns territories, a hand of cards, and a list of orders. Exposes `toAttack()`, `toDefend()`, and `issueOrder()`.
- `Orders.cpp / Orders.h` — Deploy, Advance, Bomb, Blockade, Airlift, Negotiate — each with `validate()` and `execute()`.
- `Cards.cpp / Cards.h` — Deck and hand with a `draw()` method and card `play()` that generates orders.
- `GameEngine.cpp / GameEngine.h` — State machine with console-driven transitions between game phases.

### Assignment 2 — Game Logic & Observers

The second sprint turned the skeleton into a working game. We added command processing (console and file-based via the Adapter pattern), a full startup phase, and the main game loop with all three phases: reinforcement, issuing orders, and executing orders. Orders now actually do something — battles are simulated, territories change hands, and cards are awarded. Everything notable that happens in the game gets written to a log file through the Observer pattern.

- `CommandProcessing.cpp / CommandProcessing.h` — Reads and validates commands from console or file. Invalid commands in the wrong game state are rejected with an error.
- Startup phase handles map loading, player registration, territory distribution, reinforcement pools, and initial card draws.
- Reinforcement gives each player armies based on territories owned (divided by 3, rounded down) plus continent bonuses, with a minimum of 3.
- Battle simulation: each attacking unit has a 60% chance of killing a defender; each defending unit has a 70% chance of killing an attacker.
- `LoggingObserver.cpp / LoggingObserver.h` — Every command, order, and state transition gets logged to `gamelog.txt` through `ILoggable` and the Subject/Observer hierarchy.

### Assignment 3 — Strategy Patterns & Tournament Mode

The third sprint added the ability for players to behave differently depending on their assigned strategy, and introduced a fully automated tournament mode.

- `PlayerStrategies.cpp / PlayerStrategies.h` — Strategy pattern with five concrete behaviors:
  - **Human** — Asks the user for input at every decision.
  - **Aggressive** — Dumps armies on its strongest territory and keeps attacking until it runs out of moves.
  - **Benevolent** — Reinforces its weakest territories and never attacks.
  - **Neutral** — Does nothing — until attacked, at which point it switches to Aggressive.
  - **Cheater** — Skips the formalities and just takes all adjacent enemy territories once per turn.
- Tournament mode runs a full series of games across multiple maps and strategies without any user interaction. A draw is declared if a game exceeds the turn limit. Results are printed as a table in `gamelog.txt`.

```
tournament -M <listofmapfiles> -P <listofplayerstrategies> -G <numberofgames> -D <maxnumberofturns>
```

## File Structure
```
COMP-345-WarZone-Team10/
|
+-- Map.cpp / Map.h
+-- Player.cpp / Player.h
+-- Orders.cpp / Orders.h
+-- Cards.cpp / Cards.h
+-- GameEngine.cpp / GameEngine.h
+-- CommandProcessing.cpp / CommandProcessing.h
+-- LoggingObserver.cpp / LoggingObserver.h
+-- PlayerStrategies.cpp / PlayerStrategies.h
|
+-- MapDriver.cpp
+-- PlayerDriver.cpp
+-- OrdersDriver.cpp
+-- CardsDriver.cpp
+-- GameEngineDriver.cpp
+-- CommandProcessingDriver.cpp
+-- LoggingObserverDriver.cpp
+-- PlayerStrategiesDriver.cpp
```

## How to Build & Run

```bash
g++ -std=c++17 -Wall *.cpp -o warzone

./warzone -console
./warzone -file commands.txt
```

Once in the start state, tournament mode can be launched with:

```
tournament -M map1.map map2.map -P Aggressive Benevolent -G 3 -D 20
```

## References
- Warzone: https://www.warzone.com/
- Domination map files: http://domination.sourceforge.net/getmaps.shtml
- Assignment 1 walkthrough: https://youtu.be/egx1HKxdF10
- Assignment 2 walkthrough: https://youtu.be/-4NGKisJ3jY
- Assignment 3 walkthrough: https://youtu.be/6pRY5CvgTtQ
