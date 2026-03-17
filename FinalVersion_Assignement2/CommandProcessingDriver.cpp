#include <iostream>
#include <string>
#include "CommandProcessing.h"
#include "GameEngine.h"

using namespace std;

// ============================================================
// COMP 345 A2 – Part 1 Driver
// Demonstrates:
//   (1) Commands read from console
//   (2) Commands read from file
//   (3) Invalid commands rejected; valid ones accepted
// ============================================================

int main(int argc, char* argv[]) {

    string mode = "-console";
    string filename = "";
    if (argc >= 2) mode = argv[1];
    if (argc >= 3) filename = argv[2];

    if (mode != "-console" && mode != "-file") {
        cerr << "Usage: " << argv[0] << " [-console | -file <filename>]\n";
        return 1;
    }
    if (mode == "-file" && filename.empty()) {
        cerr << "Usage: " << argv[0] << " -file <filename>\n";
        return 1;
    }

    cout << "==============================================\n";
    cout << " COMP345 A2 Part 1: Command Processor Demo\n";
    cout << "==============================================\n\n";

    GameEngine* engine = new GameEngine();

    // ── Demo (3): validate() – no transition() called ────────
    cout << "== Demo (3): Invalid command rejection ==\n";
    cout << "Current state: " << engine->getCurrentState() << "\n\n";

    bool ok = engine->commandProcessor->validate(
                  "validatemap", engine->getCurrentState());
    cout << "validate(\"validatemap\") in \"start\": "
         << (ok ? "ACCEPTED" : "REJECTED") << "\n";

    ok = engine->commandProcessor->validate(
             "addplayer", engine->getCurrentState());
    cout << "validate(\"addplayer\")   in \"start\": "
         << (ok ? "ACCEPTED" : "REJECTED") << "\n";

    ok = engine->commandProcessor->validate(
             "loadmap", engine->getCurrentState());
    cout << "validate(\"loadmap\")     in \"start\": "
         << (ok ? "ACCEPTED" : "REJECTED") << "\n\n";

    // ── Demo (1) or (2): read + save commands ────────────────
    // We read commands and validate them, but do NOT call
    // transition() to avoid triggering actual game logic
    // (file loading, player creation, etc.).
    // State is advanced manually so validation is meaningful.

    if (mode == "-file") {
        cout << "== Demo (2): Reading from file: \""
             << filename << "\" ==\n\n";
        FileCommandProcessorAdapter* adapter =
            new FileCommandProcessorAdapter(filename);
        adapter->setGameEngine(engine);
        engine->setCommandProcessor(adapter);
    } else {
        cout << "== Demo (1): Reading from console ==\n";
        cout << "Enter commands one by one (Ctrl+D to stop).\n";
        cout << "Tokens: loadmap validatemap addplayer gamestart\n";
        cout << "        issueorder endissueorders execorder\n";
        cout << "        endexecorders win play end\n\n";
    }

    // Transition table (mirrors GameEngine) for manual state advance
    map<pair<string,string>, string> table = {
        {{"start",              "loadmap"},        "maploaded"},
        {{"maploaded",          "loadmap"},        "maploaded"},
        {{"maploaded",          "validatemap"},    "mapvalidated"},
        {{"mapvalidated",       "addplayer"},      "playersadded"},
        {{"playersadded",       "addplayer"},      "playersadded"},
        {{"playersadded",       "gamestart"},      "assignreinforcement"},
        {{"assignreinforcement","issueorder"},     "issueorders"},
        {{"issueorders",        "issueorder"},     "issueorders"},
        {{"issueorders",        "endissueorders"}, "executeorders"},
        {{"executeorders",      "execorder"},      "executeorders"},
        {{"executeorders",      "endexecorders"},  "assignreinforcement"},
        {{"executeorders",      "win"},            "win"},
        {{"win",                "play"},           "start"},
        {{"win",                "end"},            "end"}
    };

    State currentState = engine->getCurrentState();

    while (true) {
        cout << "\nCurrent state: " << currentState << "\n";

        if (mode == "-file") {
            FileCommandProcessorAdapter* fa =
                dynamic_cast<FileCommandProcessorAdapter*>(
                    engine->commandProcessor);
            if (fa && !fa->hasMoreCommands()) {
                cout << "[File] No more commands.\n";
                break;
            }
        }

        Command* cmd = engine->commandProcessor->getCommand();
        if (!cmd) break;

        string t = cmd->getType();
        cout << "[Saved]  " << *cmd << "\n";

        // Validate against current state
        bool valid = engine->commandProcessor->validate(t, currentState);
        if (!valid) {
            cmd->saveEffect("ERROR: \"" + t + "\" invalid in state \""
                            + currentState.getName() + "\"");
            cout << "[Rejected] " << *cmd << "\n";
            continue;
        }

        // Advance state manually
        auto it = table.find({currentState.getName(), t});
        if (it != table.end()) {
            currentState = State(it->second);
            cmd->saveEffect("Transitioned to " + it->second);
        }

        cout << "  Effect : " << cmd->getEffect() << "\n";
        cout << "  State  : " << currentState << "\n";

        if (t == "end") break;
    }

    // ── Print full history ───────────────────────────────────
    cout << "\n== All stored commands ==\n";
    for (const Command* c : engine->commandProcessor->getCommands()) {
        cout << "  " << *c << "\n";
    }

    delete engine;
    cout << "\nPart 1 demo complete.\n";
    return 0;
}
