/*
 * TournamentDriver.cpp
 *
 * Driver for Assignment 3 Part 2: Tournament Mode.
 *
 * Demonstrates:
 *  1) The tournament command is parsed and validated by parseTournamentCommand().
 *  2) Invalid commands (wrong ranges, Human strategy) are rejected.
 *  3) A valid tournament command triggers a fully automated tournament via
 *     GameEngine::tournamentMode().
 *  4) Results are printed in a formatted table at the end.
 *
 * Compile this as its own executable (separate from the other drivers).
 * Update the map path string to match your local data directory.
 */

#include <iostream>
#include <vector>
#include <string>
#include "GameEngine.h"
#include "CommandProcessing.h"
#include "PlayerStrategy.h"

using namespace std;

int main() {
    cout << "=== Assignment 3 Part 2: Tournament Mode Driver ===\n\n";

    vector<string> maps, strategies;
    int numGames, maxTurns;

    // ── Test 1: Valid command ──
    cout << "--- Test 1: Valid tournament command ---\n";
    string validCmd =
        "tournament -M data/europe.map data/asia.map "
        "-P Aggressive Benevolent Neutral "
        "-G 3 -D 20";
    bool ok = parseTournamentCommand(validCmd, maps, strategies,
                                     numGames, maxTurns);
    if (ok) {
        cout << "[PASS] Parsed OK. Maps: " << maps.size()
             << "  Strategies: " << strategies.size()
             << "  G=" << numGames << "  D=" << maxTurns << "\n";
    } else {
        cout << "[FAIL] Valid command was rejected.\n";
    }

    // ── Test 2: Invalid — only 1 strategy ──
    cout << "\n--- Test 2: Invalid - only 1 strategy (need 2-4) ---\n";
    ok = parseTournamentCommand(
        "tournament -M data/europe.map -P Aggressive -G 2 -D 15",
        maps, strategies, numGames, maxTurns);
    cout << (ok ? "[FAIL] Should have been rejected.\n"
                : "[PASS] Correctly rejected.\n");

    // ── Test 3: Invalid — Human strategy ──
    cout << "\n--- Test 3: Invalid - Human strategy not allowed ---\n";
    ok = parseTournamentCommand(
        "tournament -M data/europe.map -P Aggressive Human -G 2 -D 15",
        maps, strategies, numGames, maxTurns);
    cout << (ok ? "[FAIL] Should have been rejected.\n"
                : "[PASS] Correctly rejected.\n");

    // ── Test 4: Invalid — D out of range ──
    cout << "\n--- Test 4: Invalid - maxTurns=5 (need 10-50) ---\n";
    ok = parseTournamentCommand(
        "tournament -M data/europe.map -P Aggressive Benevolent -G 2 -D 5",
        maps, strategies, numGames, maxTurns);
    cout << (ok ? "[FAIL] Should have been rejected.\n"
                : "[PASS] Correctly rejected.\n");

    // ── Test 5: Run actual tournament via GameEngine ──
    // Update the map path(s) to match your local data directory.
    cout << "\n--- Test 5: Run tournament via GameEngine::tournamentMode() ---\n";
    string tournamentCmd =
        "tournament "
        "-M src/data/europe_map/europe.map "
        "-P Aggressive Benevolent Neutral Cheater "
        "-G 3 "
        "-D 15";

    ok = parseTournamentCommand(tournamentCmd, maps, strategies,
                                numGames, maxTurns);
    if (ok) {
        cout << "[INFO] Command validated. Starting tournament...\n";
        TournamentConfig cfg;
        cfg.maps       = maps;
        cfg.strategies = strategies;
        cfg.numGames   = numGames;
        cfg.maxTurns   = maxTurns;

        GameEngine engine;
        engine.tournamentMode(cfg);
    } else {
        cout << "[WARN] Could not parse tournament command.\n";
        cout << "       Update the map path in TournamentDriver.cpp and retry.\n";
    }

    cout << "\n=== Tournament driver complete ===\n";
    return 0;
}
