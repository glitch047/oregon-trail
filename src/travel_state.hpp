#ifndef TRAVEL_STATE_HPP
#define TRAVEL_STATE_HPP

#include "game_state.hpp"
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <string>
#include <vector>
#include <random>
#include <memory>

class Game;

// Forward declarations
class MenuState;

// Represents a party member on the journey
struct PartyMember {
    std::string name;
    int health = 100;       // 0-100 where 100 is perfect health
    bool isAlive = true;
    std::string ailment = "";
    
    PartyMember(const std::string& name) : name(name) {}
};

// Represents the player's resources
struct Resources {
    int money = 0;          // Cash on hand
    int food = 0;           // Pounds of food
    int ammunition = 0;     // Bullets for hunting
    int clothing = 0;       // Sets of clothing
    int wagonParts = 0;     // Spare parts for the wagon
    int medicines = 0;      // Medical supplies

    Resources(int startingMoney = 1600) : money(startingMoney) {}
};

// Weather conditions
enum class Weather {
    Fair,
    Cloudy,
    Rainy,
    Stormy,
    Snowy
};

// Represents a location on the trail
struct Location {
    std::string name;
    int distance;       // Miles from start
    std::string description;
    bool isLandmark;
    bool isRiver;
    int riverDepth;     // If isRiver is true

    Location(const std::string& name, int distance, const std::string& description, 
             bool isLandmark = false, bool isRiver = false, int riverDepth = 0)
        : name(name), distance(distance), description(description), 
          isLandmark(isLandmark), isRiver(isRiver), riverDepth(riverDepth) {}
};

class TravelState : public GameState {
public:
    TravelState(Game* game, const std::string& profession = "Banker");
    virtual ~TravelState();
    
    // GameState interface implementation
    virtual void enter() override;
    virtual void exit() override;
    virtual void handleEvent(const SDL_Event& event) override;
    virtual void update(float deltaTime) override;
    virtual void render() override;
    virtual std::string getName() const override { return "TravelState"; }
    
private:
    // States within the travel state
    enum class TravelSubState {
        Setup,          // Initial setup (naming party, etc)
        Traveling,      // Normal traveling
        Location,       // At a landmark or fort
        River,          // At a river crossing
        Hunting,        // In hunting mini-game
        Trading,        // Trading with others
        Event,          // Random event occurring
        Resting,        // Resting to recover health
        GameOver        // End of game (death or reached Oregon)
    };
    
    // Helper methods
    void renderText(const std::string& text, int x, int y);
    void renderTextCentered(const std::string& text, int y);
    
    // Game mechanics
    void advanceDay();
    void consumeResources();
    void updateWeather();
    Weather getWeatherForMonth(int month);
    void updateHealth();
    int calculateDailyMiles();
    void checkForLandmark();
    void checkForRivers();
    void triggerRandomEvent();
    void handleRiverCrossing();
    void startHunting();
    void handleTrading();
    void restForDays(int days);
    void setupInitialJourney();
    void returnToMenu();
    
    // User interface methods
    void renderTravelScreen();
    void renderSetupScreen();
    void renderLocationScreen();
    void renderRiverScreen();
    void renderHuntingScreen();
    void renderTradingScreen();
    void renderEventScreen();
    void renderRestingScreen();
    void renderGameOverScreen();
    void handleTravelInput(SDL_Keycode key);
    void handleSetupInput(SDL_Keycode key);
    
    // Random event generation
    void generateRandomEvents();
    std::string getRandomEvent();
    
    // Resource management
    void setupStartingResources();
    
    // Member variables
    std::string m_profession;
    std::vector<PartyMember> m_party;
    Resources m_resources;
    
    int m_currentDay = 1;          // Game starts on day 1
    int m_month = 3;               // Start in March
    int m_year = 1848;
    int m_milesTraveled = 0;
    int m_nextLandmarkIndex = 0;
    std::vector<Location> m_landmarks;
    Weather m_currentWeather = Weather::Fair;
    TravelSubState m_subState = TravelSubState::Setup;
    
    // For random events
    std::mt19937 m_rng;
    
    // For event handling
    std::string m_currentEvent;
    std::string m_eventMessage;
    
    // For UI
    TTF_Font* m_font = nullptr;
    SDL_Color m_textColor = {144, 238, 144, 255}; // Light green
    
    // Navigation help
    std::string m_helpText;
    
    // Setup state
    int m_setupStage = 0;
    std::vector<std::string> m_setupInputs;
    std::string m_currentInput;
    
    // Flags
    bool m_needsUpdate = true;
    bool m_gameOver = false;
    bool m_reachedOregon = false;
};

#endif // TRAVEL_STATE_HPP

#ifndef TRAVEL_STATE_HPP
#define TRAVEL_STATE_HPP

#include "game_state.hpp"
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <string>
#include <vector>
#include <random>
#include <memory>

class Game;

// Forward declarations
class MenuState;

// Represents a party member on the journey
struct PartyMember {
    std::string name;
    int health = 100;       // 0-100 where 100 is perfect health
    bool isAlive = true;
    std::string ailment = "";
    
    PartyMember(const std::string& name) : name(name) {}
};

// Represents the player's resources
struct Resources {
    int money = 0;          // Cash on hand
    int food = 0;           // Pounds of food
    int ammunition = 0;     // Bullets for hunting
    int clothing = 0;       // Sets of clothing
    int wagonParts = 0;     // Spare parts for the wagon
    int medicines = 0;      // Medical supplies

    Resources(int startingMoney = 1600) : money(startingMoney) {}
};

// Weather conditions
enum class Weather {
    Fair,
    Cloudy,
    Rainy,
    Stormy,
    Snowy
};

// Represents a location on the trail
struct Location {
    std::string name;
    int distance;       // Miles from start
    std::string description;
    bool isLandmark;
    bool isRiver;
    int riverDepth;     // If isRiver is true

    Location(const std::string& name, int distance, const std::string& description, 
             bool isLandmark = false, bool isRiver = false, int riverDepth = 0)
        : name(name), distance(distance), description(description), 
          isLandmark(isLandmark), isRiver(isRiver), riverDepth(riverDepth) {}
};

class TravelState : public GameState {
public:
    TravelState(Game* game, const std::string& profession = "Banker");
    virtual ~TravelState();
    
    // GameState interface implementation
    virtual void enter() override;
    virtual void exit() override;
    virtual void handleEvent(const SDL_Event& event) override;
    virtual void update(float deltaTime) override;
    virtual void render() override;
    virtual std::string getName() const override { return "TravelState"; }
    
private:
    // States within the travel state
    enum class TravelSubState {
        Setup,          // Initial setup (naming party, etc)
        Traveling,      // Normal traveling
        Location,       // At a landmark or fort
        River,          // At a river crossing
        Hunting,        // In hunting mini-game
        Trading,        // Trading with others
        Event,          // Random event occurring
        Resting,        // Resting to recover health
        GameOver        // End of game (death or reached Oregon)
    };
    
    // Helper methods
    void renderText(const std::string& text, int x, int y);
    void renderTextCentered(const std::string& text, int y);
    
    // Game mechanics
    void advanceDay();
    void consumeResources();
    void updateWeather();
    Weather getWeatherForMonth(int month);
    void updateHealth();
    int calculateDailyMiles();
    void checkForLandmark();
    void checkForRivers();
    void triggerRandomEvent();
    void handleRiverCrossing();
    void startHunting();
    void handleTrading();
    void restForDays(int days);
    void setupInitialJourney();
    void returnToMenu();
    
    // User interface methods
    void renderTravelScreen();
    void renderSetupScreen();
    void renderLocationScreen();
    void renderRiverScreen();
    void renderHuntingScreen();
    void renderTradingScreen();
    void renderEventScreen();
    void renderRestingScreen();
    void renderGameOverScreen();
    void handleTravelInput(SDL_Keycode key);
    void handleSetupInput(SDL_Keycode key);
    
    // Random event generation
    void generateRandomEvents();
    std::string getRandomEvent();
    
    // Resource management
    void setupStartingResources();
    
    // Member variables
    std::string m_profession;
    std::vector<PartyMember> m_party;
    Resources m_resources;
    
    int m_currentDay = 1;          // Game starts on day 1
    int m_month = 3;               // Start in March
    int m_year = 1848;
    int m_milesTraveled = 0;
    int m_nextLandmarkIndex = 0;
    std::vector<Location> m_landmarks;
    Weather m_currentWeather = Weather::Fair;
    TravelSubState m_subState = TravelSubState::Setup;
    
    // For random events
    std::mt19937 m_rng;
    
    // For event handling
    std::string m_currentEvent;
    std::string m_eventMessage;
    
    // For UI
    TTF_Font* m_font = nullptr;
    SDL_Color m_textColor = {144, 238, 144, 255}; // Light green
    
    // Navigation help
    std::string m_helpText;
    
    // Setup state
    int m_setupStage = 0;
    std::vector<std::string> m_setupInputs;
    std::string m_currentInput;
    
    // Flags
    bool m_needsUpdate = true;
    bool m_gameOver = false;
    bool m_reachedOregon = false;
};

#endif // TRAVEL_STATE_HPP

