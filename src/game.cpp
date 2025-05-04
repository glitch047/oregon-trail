#include "game.hpp"
#include "game_state.hpp"
#include "player.hpp"
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_mixer.h>
#include <iostream>
#include <stdexcept>
#include <memory>

Game::Game(const std::string& title, int width, int height)
    : m_windowTitle(title)
    , m_windowWidth(width)
    , m_windowHeight(height)
    , m_window(nullptr)
    , m_renderer(nullptr)
    , m_isRunning(false)
    , m_player(nullptr)
{
}

void Game::quit() {
    std::cout << "Quit requested, closing game..." << std::endl;
    
    // Ensure clean state transition before ending the game loop
    // This prevents potential segfaults during cleanup
    if (!m_states.empty()) {
        // We don't need to explicitly call exit() on states here 
        // since shutdown() will handle that
        std::cout << "Game has " << m_states.size() << " active states to clean up" << std::endl;
    }
    
    // Set the running flag to false to exit the game loop
    m_isRunning = false;
    std::cout << "Game will exit on next frame" << std::endl;
}

Game::~Game() {
    // Clear all states before shutdown
    while (!m_states.empty()) {
        m_states.pop();
    }
    
    shutdown();
}

bool Game::initialize() {
    if (!initSDL()) {
        return false;
    }
    
    m_player = std::make_unique<Player>("Player");
    m_isRunning = true;
    return true;
}

bool Game::initSDL() {
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0) {
        std::cerr << "SDL initialization failed: " << SDL_GetError() << std::endl;
        return false;
    }
    
    // Create window
    m_window = SDL_CreateWindow(
        m_windowTitle.c_str(),
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        m_windowWidth,
        m_windowHeight,
        SDL_WINDOW_SHOWN
    );
    
    if (!m_window) {
        std::cerr << "Window creation failed: " << SDL_GetError() << std::endl;
        return false;
    }
    
    // Create renderer
    m_renderer = SDL_CreateRenderer(
        m_window,
        -1,
        SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC
    );
    
    if (!m_renderer) {
        std::cerr << "Renderer creation failed: " << SDL_GetError() << std::endl;
        return false;
    }
    
    // Initialize SDL_image
    int imgFlags = IMG_INIT_PNG;
    if (!(IMG_Init(imgFlags) & imgFlags)) {
        std::cerr << "SDL_image initialization failed: " << IMG_GetError() << std::endl;
        return false;
    }
    
    // Initialize SDL_ttf
    if (TTF_Init() != 0) {
        std::cerr << "SDL_ttf initialization failed: " << TTF_GetError() << std::endl;
        return false;
    }
    
    // Initialize SDL_mixer
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
        std::cerr << "SDL_mixer initialization failed: " << Mix_GetError() << std::endl;
        return false;
    }
    
    return true;
}

void Game::run() {
    if (!m_isRunning) {
        throw std::runtime_error("Game not initialized");
    }
    
    Uint32 lastTime = SDL_GetTicks();
    
    // Game loop
    while (m_isRunning) {
        // Calculate delta time
        Uint32 currentTime = SDL_GetTicks();
        float deltaTime = (currentTime - lastTime) / 1000.0f;
        lastTime = currentTime;
        
        processInput();
        update(deltaTime);
        render();
    }
}

void Game::processInput() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
            m_isRunning = false;
        }
        
        // Let the current state handle any other input
        if (hasStates()) {
            currentState()->handleEvent(event);
        }
    }
}

void Game::update(float deltaTime) {
    // Update the current state
    if (hasStates()) {
        currentState()->update(deltaTime);
    }
    
    // Update player data if needed
    if (m_player) {
        m_player->update(deltaTime);
    }
}

void Game::render() {
    // Clear screen
    SDL_SetRenderDrawColor(m_renderer, 0, 0, 0, 255);
    SDL_RenderClear(m_renderer);
    
    // Render the current state
    if (hasStates()) {
        currentState()->render();
    }
    
    // Present render
    SDL_RenderPresent(m_renderer);
}

void Game::pushState(std::unique_ptr<GameState> state) {
    if (state) {
        if (!m_states.empty()) {
            m_states.top()->exit();
        }
        m_states.push(std::move(state));
        m_states.top()->enter();
    }
}

void Game::popState() {
    if (!m_states.empty()) {
        m_states.top()->exit();
        m_states.pop();
        
        if (!m_states.empty()) {
            m_states.top()->enter();
        } else {
            // No more states, exit the game
            m_isRunning = false;
        }
    }
}

void Game::changeState(std::unique_ptr<GameState> state) {
    if (state) {
        if (!m_states.empty()) {
            m_states.top()->exit();
            m_states.pop();
        }
        m_states.push(std::move(state));
        m_states.top()->enter();
    }
}

GameState* Game::currentState() {
    return m_states.empty() ? nullptr : m_states.top().get();
}

bool Game::hasStates() const {
    return !m_states.empty();
}

void Game::shutdown() {
    std::cout << "Game shutting down..." << std::endl;
    
    // Clear any remaining game states
    while (!m_states.empty()) {
        try {
            if (m_states.top()) {
                std::cout << "Exiting state during shutdown" << std::endl;
                m_states.top()->exit();
            }
            m_states.pop();
        } catch (const std::exception& e) {
            std::cerr << "Error during state cleanup: " << e.what() << std::endl;
        }
    }
    
    // Clean up player
    m_player.reset();
    
    // Clean up SDL resources
    if (m_renderer) {
        SDL_DestroyRenderer(m_renderer);
        m_renderer = nullptr;
    }
    if (m_window) {
        SDL_DestroyWindow(m_window);
        m_window = nullptr;
    }
    
    // Quit SDL subsystems
    Mix_Quit();
    TTF_Quit();
    IMG_Quit();
    SDL_Quit();
    
    std::cout << "Game shutdown complete" << std::endl;
}
