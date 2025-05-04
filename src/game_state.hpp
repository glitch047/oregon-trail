#ifndef GAME_STATE_HPP
#define GAME_STATE_HPP

#include <SDL2/SDL.h>
#include <string>
#include <memory>

// Forward declaration
class Game;

class GameState {
public:
    GameState(Game* game) : m_game(game) {}
    virtual ~GameState() = default;
    
    // State interface - all derived states must implement these
    virtual void enter() = 0;
    virtual void exit() = 0;
    virtual void handleEvent(const SDL_Event& event) = 0;  // Changed from processInput to match game.cpp
    virtual void update(float deltaTime) = 0;
    virtual void render() = 0;
    
    // Common utility methods
    virtual std::string getName() const = 0;

protected:
    Game* m_game; // Reference to the game object
};

#endif // GAME_STATE_HPP

