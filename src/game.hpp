#ifndef GAME_HPP
#define GAME_HPP

#include <string>
#include <memory>
#include <vector>
#include <stack>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_mixer.h>
#include "player.hpp"

// Forward declarations
class GameState;

class Game {
public:
    Game(const std::string& title, int width, int height);
    ~Game();
    
    // Delete copy constructor and assignment operator
    Game(const Game&) = delete;
    Game& operator=(const Game&) = delete;
    
    bool initialize();
    void run();
    void shutdown();
    
    // State management
    void pushState(std::unique_ptr<GameState> state);
    void popState();
    void changeState(std::unique_ptr<GameState> state);
    GameState* currentState();
    bool hasStates() const;
    
    // Accessor methods
    SDL_Renderer* getRenderer() const { return m_renderer; }
    SDL_Window* getWindow() const { return m_window; }
    int getWindowWidth() const { return m_windowWidth; }
    int getWindowHeight() const { return m_windowHeight; }
    
    // Game control
    void quit();

private:
    bool initSDL();
    void processInput();
    void update(float deltaTime);
    void render();
    
    // Window properties
    std::string m_windowTitle;
    int m_windowWidth;
    int m_windowHeight;
    
    // SDL components
    SDL_Window* m_window;
    SDL_Renderer* m_renderer;
    
    // Game state
    bool m_isRunning;
    std::stack<std::unique_ptr<GameState>> m_states;
    
    // Game objects
    std::unique_ptr<Player> m_player;
};

#endif // GAME_HPP
