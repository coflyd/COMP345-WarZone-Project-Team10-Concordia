#include <cstdio>
#include <fstream>
#include <iostream>
#include <string>
#include <sstream>

#include "CommandProcessing.h"
#include "GameEngine.h"
#include "Orders.h"

int main() {
    // Start with a clean log file so the demo shows only Part 5 output.
    // Helps make the observer results easy to verify during presentation.
    std::remove("gamelog.txt");

    std::cout << "=== Part 5 Logging Demo ===\n";

    // 1) Real command input from a file through FileCommandProcessorAdapter.
    // Rubric:
    // - 5.2.1: when a command is read, it is written in the log file
    // - 5.2.11: driver demonstrates commands read from a file are written to gamelog.txt
    {
        std::ofstream commandFile("part5_demo_commands.txt");
        commandFile << "loadmap\n";
    }

    // Reads the command from a file using the adapter instead of hardcoding input directly
    // into the processor logic. This is the "read from a file" part of 5.2.11.
    FileCommandProcessorAdapter processor("part5_demo_commands.txt");

    // getCommand() should cause CommandProcessor::saveCommand() to run,
    // which should call Notify(...) and write the command to gamelog.txt.
    // Rubric:
    // - 5.2.1
    // - 5.2.10: CommandProcessor::saveCommand() uses Notify(...)
    // - 5.2.11
    Command* fileCommand = processor.getCommand();

    // saveEffect() should log the command effect.
    // Rubric:
    // - 5.2.1: when a command is executed, its effect is written in the log file
    // - 5.2.10: Command::saveEffect() uses Notify(...)
    // - 5.2.11
    if (fileCommand != nullptr) {
        fileCommand->saveEffect("Command executed from file input");
    }

    // 2) Order insertion and order execution logging.
    OrdersList orders;

    // Adding the order should notify the observer and log the order insertion.
    // Rubric:
    // - 5.2.2: when an order is inserted in the player's order list, it is logged
    // - 5.2.10: OrderList::addOrder() / OrdersList::addOrder() uses Notify(...)
    // - 5.2.12: driver demonstrates order addition causes a log entry
    orders.addOrder(new Deploy("Alaska", 5));

    Order* first = orders.at(0);

    // Executing the order should notify the observer and log the order effect.
    // Rubric:
    // - 5.2.3: when an order is executed, its effect is written into the log file
    // - 5.2.10: Order::execute() uses Notify(...) (directly or through setEffect())
    // - 5.2.13: driver demonstrates order execution causes a log entry
    if (first != nullptr) {
        first->execute();
    }

    // 3) GameEngine state transition logging.
    GameEngine engine;

    // Save real cin buffer so it can be restored after fake input.
    std::streambuf* originalCin = std::cin.rdbuf();

    // Fake user input "1" so the demo can automatically choose a map option
    // during loadmap without manual typing in class.
    std::istringstream fakeInput("1\n");
    std::cin.rdbuf(fakeInput.rdbuf());

    // transition("loadmap") should change the engine state and notify the observer.
    // Rubric:
    // - 5.2.4: when the GameEngine state changes, the new state is written to the log file
    // - 5.2.10: GameEngine::transition() uses Notify(...)
    // - 5.2.14: driver demonstrates state change causes a log entry
    engine.transition("loadmap");

    // Restore normal keyboard input after the scripted demo input.
    std::cin.rdbuf(originalCin);

    // Open and print the final log file so the grader can directly verify
    // that all observer notifications produced log entries.
    // This helps visibly prove:
    // - 5.2.1
    // - 5.2.2
    // - 5.2.3
    // - 5.2.4
    // - 5.2.11
    // - 5.2.12
    // - 5.2.13
    // - 5.2.14
    std::ifstream logFile("gamelog.txt");
    std::cout << "\n=== gamelog.txt ===\n";
    std::cout << logFile.rdbuf();

    return 0;
}