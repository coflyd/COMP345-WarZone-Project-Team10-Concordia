#include <iostream>
#include <string>
#include "GameEngine.h"

void automatedDemo() {

    std::cout << "Automated demonstration of states\n";
    GameEngine* engine = new GameEngine();

    std::string commands[] = {
        // Startup phase
        "loadmap",         
        "loadmap",         
        "validatemap",     
        "addplayer",       
        "addplayer",       
        "assigncountries", 
        
        // Play phase - first round
        "issueorder",      
        "issueorder",      
        "endissueorders",  
        "execorder",       
        "endexecorders",   
        
        // Play phase - second round
        "issueorder",      
        "endissueorders",  
        "execorder",       
        "win",             
        
        // Win phase
        "play",            
        "loadmap",         
        "end"              
    };
    
    // Execute all commands
    for (const std::string& cmd : commands) {
        std::cout << "Current State: " << engine->getCurrentState()->getName() << "\n";
        std::cout << "Executing command: '" << cmd << "'\n";
        engine->transition(cmd);
        std::cout << "Result: " << engine->getCurrentState()->getName() << "\n";
        std::cout << "---\n";
    }
    
   
    std::cout << "Testing invalid commands\n";
    
    delete engine;
    engine = new GameEngine();
    
    // Test invalid commands
    std::string invalidTests[][2] = {
        {"start", "validatemap"},        // Can't validate without loading
        {"start", "addplayer"},          // Can't add player from start
        {"maploaded", "assigncountries"},// Can't assign from maploaded
        {"start", "invalidcommand"}      // Completely invalid command
    };
    
    for (auto& test : invalidTests) {
        if (test[0] == "maploaded") {
            engine->transition("loadmap");
        } 
        std::cout << "Current State: " << engine->getCurrentState()->getName() << "\n";
        std::cout << "Trying invalid command: '" << test[1] << "'\n";
        engine->transition(test[1]);
        std::cout << "State remains: " << engine->getCurrentState()->getName() << "\n";
        std::cout << "---\n";
    } 
    std::cout << "\n=== Full Engine Status ===\n";
    std::cout << *engine << "\n";
    delete engine;
}


void interactiveMode() {
    std::cout << "Interactive mode\n";
    
    GameEngine* engine = new GameEngine();
    std::string input;
    
    std::cout << "Available commands depend on current state:\n";
    std::cout << "  Startup: loadmap, validatemap, addplayer, assigncountries\n";
    std::cout << "  Play: issueorder, endissueorders, execorder, endexecorders, win\n";
    std::cout << "  Win: play, end\n";
    std::cout << "  Any state: end\n\n";
    
    while (true) {
        std::cout << "\n>>> Current State: " << engine->getCurrentState()->getName() << "\n";
        std::cout << ">>> Enter command (or 'quit' to exit): ";
        std::getline(std::cin, input);
        
        if (input.empty()) {
            std::cout << "Empty command. Try again.\n";
            continue;
        }
        
        if (input == "quit") {
            std::cout << "Exiting interactive mode.\n";
            break;
        }
        
        engine->transition(input);
        
        if (engine->getCurrentState()->getName() == "end") {
            std::cout << "\nGame ended. Final state reached.\n";
            std::cout << "\n=== Full Engine Status ===\n";
            std::cout << *engine << "\n";
            break;
        }
    }
    
    delete engine;
}


int main() {
    std::cout << "Warzone Game Engine - Part 5\n";

    automatedDemo();
    
    std::cout << "\n\nWould you like to try interactive mode? (y/n): ";
    std::string response;
    std::getline(std::cin, response);
    
    if (response == "y" || response == "Y" || response == "yes") {
        interactiveMode();
    }    
    return 0;
}
