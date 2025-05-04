#include <iostream>
#include <memory>
#include "game.hpp"
#include "menu_state.hpp"

int main(int argc, char* argv[]) {
    try {
        auto game = std::make_unique<Game>("Oregon Trail", 800, 600);
        
        if (!game->initialize()) {
            std::cerr << "Failed to initialize game." << std::endl;
            return 1;
        }
        
        // Create and push the initial menu state
        auto menuState = std::make_unique<MenuState>(game.get());
        game->pushState(std::move(menuState));
        
        game->run();
        
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
}
