#include "travel_state.hpp"
#include "game.hpp"
#include "menu_state.hpp"
#include <iostream>
#include <algorithm>
#include <sstream>
#include <ctime>
#include <random>
#include <SDL2/SDL.h>

// Constructor
TravelState::TravelState(Game* game, const std::string& profession)
    : GameState(game)
    , m_profession(profession)
    , m_rng(std::random_device{}())
{
    std::cout << "TravelState initialized with profession: " << profession << std::endl;
    
    // Setup starting resources based on profession
    if (profession == "Banker") {
        m_resources = Resources(1600);
    } else if (profession == "Carpenter") {
        m_resources = Resources(800);
    } else if (profession == "Farmer") {
        m_resources = Resources(400);
    } else {
        // Default
        m_resources = Resources(1000);
    }
    
    // Initialize landmarks
    m_landmarks = {
        Location("Independence, Missouri", 0, "Starting point of the Oregon Trail", true),
        Location("Kansas River Crossing", 102, "The wide Kansas River needs to be crossed.", true, true, 4),
        Location("Big Blue River Crossing", 185, "The Big Blue River is normally easy to cross, but recent rains have made it challenging.", true, true, 3),
        Location("Fort Kearney", 304, "Fort Kearney is a military post and emigrant supply point.", true),
        Location("Chimney Rock", 554, "Chimney Rock is a famous landmark on the trail, visible from miles away.", true),
        Location("Fort Laramie", 640, "Fort Laramie is an important supply and rest point.", true),
        Location("Independence Rock", 830, "Pioneers try to reach Independence Rock by July 4th to stay on schedule.", true),
        Location("South Pass", 932, "South Pass is a relatively easy passage through the Rocky Mountains.", true),
        Location("Green River Crossing", 989, "The Green River is deep and dangerous to cross.", true, true, 6),
        Location("Fort Bridger", 1085, "Fort Bridger is a trading post founded by Jim Bridger.", true),
        Location("Snake River Crossing", 1256, "The Snake River is treacherous and difficult to cross.", true, true, 5),
        Location("Fort Hall", 1288, "Fort Hall is an important trading post and supply point.", true),
        Location("Fort Boise", 1410, "Fort Boise is your last major stop before the Blue Mountains.", true),
        Location("The Dalles", 1920, "The Dalles is the end of the overland portion of the trail for many emigrants.", true),
        Location("Oregon City, Oregon", 2040, "Oregon City is the end of the Oregon Trail and your final destination.", true)
    };
    
    // Set up help text
    m_helpText = "SPACE: Continue | 1: Rest | 2: Hunt | 3: Trade | 4: Check Supplies | ESC: Return to Menu";
}

TravelState::~TravelState() {
    if (m_font) {
        TTF_CloseFont(m_font);
        m_font = nullptr;
    }
}

void TravelState::enter() {
    std::cout << "Entering TravelState" << std::endl;
    
    // Load font if not already loaded
    if (!m_font) {
        std::cout << "Attempting to load font for TravelState" << std::endl;
        m_font = TTF_OpenFont("assets/fonts/apple2.ttf", 16);
        if (!m_font) {
            std::cerr << "Failed to load font: " << TTF_GetError() << std::endl;
            
            // Try fallback fonts
            const char* fallbackFonts[] = {
                "/usr/share/fonts/truetype/dejavu/DejaVuSansMono.ttf",
                "/usr/share/fonts/TTF/DejaVuSansMono.ttf",
                "/usr/share/fonts/truetype/liberation/LiberationMono-Regular.ttf",
                nullptr
            };
            
            for (int i = 0; fallbackFonts[i] != nullptr; i++) {
                std::cout << "Trying fallback font: " << fallbackFonts[i] << std::endl;
                m_font = TTF_OpenFont(fallbackFonts[i], 16);
                if (m_font) {
                    std::cout << "Successfully loaded fallback font: " << fallbackFonts[i] << std::endl;
                    break;
                }
            }
            
            if (!m_font) {
                std::cerr << "Failed to load any fallback font: " << TTF_GetError() << std::endl;
            }
        } else {
            std::cout << "Successfully loaded font for TravelState" << std::endl;
        }
    }
    
    // Start in setup state
    m_subState = TravelSubState::Setup;
    setupInitialJourney();
}

void TravelState::exit() {
    std::cout << "Exiting TravelState" << std::endl;
}

void TravelState::handleEvent(const SDL_Event& event) {
    if (event.type == SDL_KEYDOWN) {
        std::cout << "TravelState: Key pressed: " << SDL_GetKeyName(event.key.keysym.sym) << std::endl;
        
        // Handle differently based on sub-state
        switch (m_subState) {
            case TravelSubState::Setup:
                handleSetupInput(event.key.keysym.sym);
                break;
                
            case TravelSubState::Traveling:
                handleTravelInput(event.key.keysym.sym);
                break;
                
            case TravelSubState::Location:
                if (event.key.keysym.sym == SDLK_SPACE) {
                    m_subState = TravelSubState::Traveling;
                } else if (event.key.keysym.sym == SDLK_ESCAPE) {
                    returnToMenu();
                }
                break;
                
            case TravelSubState::River:
                if (event.key.keysym.sym == SDLK_1) {
                    // Ford the river
                    std::cout << "Fording the river" << std::endl;
                    // Simple chance of success based on river depth
                    int currentLandmarkIndex = std::max(0, m_nextLandmarkIndex - 1);
                    if (currentLandmarkIndex < static_cast<int>(m_landmarks.size()) && 
                        m_landmarks[currentLandmarkIndex].isRiver) {
                        
                        int depth = m_landmarks[currentLandmarkIndex].riverDepth;
                        std::uniform_int_distribution<> dis(1, 10);
                        int roll = dis(m_rng);
                        
                        if (roll > depth) {
                            m_eventMessage = "You successfully forded the river!";
                        } else {
                            m_eventMessage = "Disaster! Your wagon tipped while crossing!";
                            // Lose some supplies
                            int foodLoss = std::min(m_resources.food / 4, 50);
                            m_resources.food -= foodLoss;
                            
                            // Possible injury to party member
                            std::uniform_int_distribution<> partyDis(0, m_party.size() - 1);
                            int memberIndex = partyDis(m_rng);
                            m_party[memberIndex].health -= 20;
                            if (m_party[memberIndex].health <= 0) {
                                m_party[memberIndex].isAlive = false;
                                m_eventMessage += " " + m_party[memberIndex].name + " has drowned.";
                            } else {
                                m_eventMessage += " " + m_party[memberIndex].name + " was injured.";
                            }
                        }
                    }
                    m_subState = TravelSubState::Event;
                } else if (event.key.keysym.sym == SDLK_2) {
                    // Caulk the wagon
                    std::cout << "Caulking the wagon" << std::endl;
                    // Higher chance of success but uses resources
                    if (m_resources.wagonParts >= 1) {
                        m_resources.wagonParts--;
                        std::uniform_int_distribution<> dis(1, 10);
                        int roll = dis(m_rng);
                        
                        if (roll > 2) {
                            m_eventMessage = "You successfully caulked and floated the wagon across!";
                        } else {
                            m_eventMessage = "The river was too deep! Your wagon and supplies were damaged.";
                            // Lose more supplies
                            int foodLoss = std::min(m_resources.food / 3, 75);
                            m_resources.food -= foodLoss;
                            m_resources.clothing = std::max(0, m_resources.clothing - 1);
                        }
                    } else {
                        m_eventMessage = "You don't have enough wagon parts to caulk the wagon.";
                    }
                    m_subState = TravelSubState::Event;
                } else if (event.key.keysym.sym == SDLK_3) {
                    // Hire a guide
                    std::cout << "Hiring a guide" << std::endl;
                    if (m_resources.money >= 40) {
                        m_resources.money -= 40;
                        m_eventMessage = "You hired a guide to help you cross the river safely.";
                        m_subState = TravelSubState::Event;
                    } else {
                        m_eventMessage = "You don't have enough money to hire a guide.";
                        m_subState = TravelSubState::Event;
                    }
                } else if (event.key.keysym.sym == SDLK_4) {
                    // Wait for conditions to improve
                    std::cout << "Waiting for conditions to improve" << std::endl;
                    std::uniform_int_distribution<> dis(1, 5);
                    int daysToWait = dis(m_rng);
                    for (int i = 0; i < daysToWait; i++) {
                        advanceDay();
                    }
                    m_eventMessage = "You waited " + std::to_string(daysToWait) + 
                                     " days for river conditions to improve. The river seems a bit calmer now.";
                    m_subState = TravelSubState::Event;
                } else if (event.key.keysym.sym == SDLK_ESCAPE) {
                    returnToMenu();
                }
                break;
                
            case TravelSubState::Event:
                if (event.key.keysym.sym == SDLK_SPACE || 
                    event.key.keysym.sym == SDLK_RETURN) {
                    // Continue after the event
                    m_subState = TravelSubState::Traveling;
                } else if (event.key.keysym.sym == SDLK_ESCAPE) {
                    returnToMenu();
                }
                break;
                
            case TravelSubState::Hunting:
                // Simple hunting mechanics
                if (event.key.keysym.sym == SDLK_SPACE) {
                    if (m_resources.ammunition > 0) {
                        m_resources.ammunition--;
                        std::uniform_int_distribution<> dis(1, 10);
                        int roll = dis(m_rng);
                        if (roll > 3) { // 70% chance of success
                            int foodGained = roll * 10; // 40-100 pounds of food
                            m_resources.food += foodGained;
                            m_eventMessage = "Successful hunt! You gained " + 
                                            std::to_string(foodGained) + " pounds of food.";
                        } else {
                            m_eventMessage = "The hunt was unsuccessful. You wasted ammunition.";
                        }
                        m_subState = TravelSubState::Event;
                    } else {
                        m_eventMessage = "You're out of ammunition. You cannot hunt.";
                        m_subState = TravelSubState::Event;
                    }
                } else if (event.key.keysym.sym == SDLK_ESCAPE) {
                    // Return to travel
                    m_subState = TravelSubState::Traveling;
                }
                break;
                
            case TravelSubState::Trading:
                // Simple trading interface
                if (event.key.keysym.sym == SDLK_1 && m_resources.money >= 20) {
                    // Buy food
                    m_resources.money -= 20;
                    m_resources.food += 50;
                    m_eventMessage = "You purchased 50 pounds of food for $20.";
                    m_subState = TravelSubState::Event;
                } else if (event.key.keysym.sym == SDLK_2 && m_resources.money >= 10) {
                    // Buy ammunition
                    m_resources.money -= 10;
                    m_resources.ammunition += 20;
                    m_eventMessage = "You purchased 20 bullets for $10.";
                    m_subState = TravelSubState::Event;
                } else if (event.key.keysym.sym == SDLK_3 && m_resources.money >= 15) {
                    // Buy clothing
                    m_resources.money -= 15;
                    m_resources.clothing += 1;
                    m_eventMessage = "You purchased 1 set of clothing for $15.";
                    m_subState = TravelSubState::Event;
                } else if (event.key.keysym.sym == SDLK_4 && m_resources.money >= 35) {
                    // Buy wagon parts
                    m_resources.money -= 35;
                    m_resources.wagonParts += 1;
                    m_eventMessage = "You purchased 1 wagon part for $35.";
                    m_subState = TravelSubState::Event;
                } else if (event.key.keysym.sym == SDLK_5 && m_resources.money >= 25) {
                    // Buy medicine
                    m_resources.money -= 25;
                    m_resources.medicines += 1;
                    m_eventMessage = "You purchased 1 medicine kit for $25.";
                    m_subState = TravelSubState::Event;
                } else if (event.key.keysym.sym == SDLK_ESCAPE) {
                    // Exit trading
                    m_subState = TravelSubState::Traveling;
                }
                break;
                
            case TravelSubState::Resting:
                if (event.key.keysym.sym == SDLK_1) {
                    // Rest for 1 day
                    restForDays(1);
                    m_subState = TravelSubState::Traveling;
                } else if (event.key.keysym.sym == SDLK_2) {
                    // Rest for 3 days
                    restForDays(3);
                    m_subState = TravelSubState::Traveling;
                } else if (event.key.keysym.sym == SDLK_3) {
                    // Rest for a week
                    restForDays(7);
                    m_subState = TravelSubState::Traveling;
                } else if (event.key.keysym.sym == SDLK_ESCAPE) {
                    // Don't rest
                    m_subState = TravelSubState::Traveling;
                }
                break;
                
            case TravelSubState::GameOver:
                if (event.key.keysym.sym == SDLK_SPACE || 
                    event.key.keysym.sym == SDLK_RETURN ||
                    event.key.keysym.sym == SDLK_ESCAPE) {
                    // Return to main menu
                    returnToMenu();
                }
                break;
        }
    }
}

void TravelState::update(float deltaTime) {
    // Only update game state when needed (after player input)
    if (!m_needsUpdate)
        return;
        
    m_needsUpdate = false;
    
    // Check if game is over
    if (m_gameOver)
        return;
        
    if (m_subState != TravelSubState::Traveling)
        return;
    
    // Check for landmarks and rivers
    checkForLandmark();
    
    // Generate random events
    if (m_subState == TravelSubState::Traveling) {
        // Small chance of random event each day
        std::uniform_int_distribution<> dis(1, 100);
        int roll = dis(m_rng);
        if (roll <= 15) { // 15% chance
            triggerRandomEvent();
        }
    }
}

void TravelState::render() {
    // Get renderer
    SDL_Renderer* renderer = m_game->getRenderer();
    
    // Clear screen to black
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);
    
    // Render different screens based on sub-state
    switch (m_subState) {
        case TravelSubState::Setup:
            renderSetupScreen();
            break;
            
        case TravelSubState::Traveling:
            renderTravelScreen();
            break;
            
        case TravelSubState::Location:
            renderLocationScreen();
            break;
            
        case TravelSubState::River:
            renderRiverScreen();
            break;
            
        case TravelSubState::Hunting:
            renderHuntingScreen();
            break;
            
        case TravelSubState::Trading:
            renderTradingScreen();
            break;
            
        case TravelSubState::Event:
            renderEventScreen();
            break;
            
        case TravelSubState::Resting:
            renderRestingScreen();
            break;
            
        case TravelSubState::GameOver:
            renderGameOverScreen();
            break;
    }
    
    // Always render help text at bottom
    renderTextCentered(m_helpText, m_game->getWindowHeight() - 30);
}

void TravelState::renderText(const std::string& text, int x, int y) {
    if (text.empty()) {
        return;
    }
    
    if (!m_font) {
        std::cerr << "Cannot render text: font not loaded" << std::endl;
        return;
    }
    
    SDL_Surface* textSurface = TTF_RenderText_Solid(m_font, text.c_str(), m_textColor);
    if (!textSurface) {
        std::cerr << "Unable to render text surface: " << TTF_GetError() << std::endl;
        return;
    }
    
    SDL_Texture* textTexture = SDL_CreateTextureFromSurface(m_game->getRenderer(), textSurface);
    if (!textTexture) {
        std::cerr << "Unable to create texture from rendered text: " << SDL_GetError() << std::endl;
        SDL_FreeSurface(textSurface);
        return;
    }
    
    SDL_Rect renderQuad = { x, y, textSurface->w, textSurface->h };
    SDL_RenderCopy(m_game->getRenderer(), textTexture, nullptr, &renderQuad);
    
    SDL_FreeSurface(textSurface);
    SDL_DestroyTexture(textTexture);
}

void TravelState::renderTextCentered(const std::string& text, int y) {
    if (text.empty()) {
        return;
    }
    
    if (!m_font) {
        std::cerr << "Cannot render centered text: font not loaded" << std::endl;
        return;
    }
    
    SDL_Surface* textSurface = TTF_RenderText_Solid(m_font, text.c_str(), m_textColor);
    if (!textSurface) {
        std::cerr << "Unable to render text surface: " << TTF_GetError() << std::endl;
        return;
    }
    
    SDL_Texture* textTexture = SDL_CreateTextureFromSurface(m_game->getRenderer(), textSurface);
    if (!textTexture) {
        std::cerr << "Unable to create texture from rendered text: " << SDL_GetError() << std::endl;
        SDL_FreeSurface(textSurface);
        return;
    }
    
    int x = (m_game->getWindowWidth() - textSurface->w) / 2;
    SDL_Rect renderQuad = { x, y, textSurface->w, textSurface->h };
    SDL_RenderCopy(m_game->getRenderer(), textTexture, nullptr, &renderQuad);
    
    SDL_FreeSurface(textSurface);
    SDL_DestroyTexture(textTexture);
}

// Core gameplay mechanics
void TravelState::setupInitialJourney() {
    m_setupStage = 0;
    m_setupInputs.clear();
    m_currentInput = "";
    
    // Default party setup with placeholder names
    m_party.clear();
    m_party.push_back(PartyMember("Player"));
    m_party.push_back(PartyMember("Companion 1"));
    m_party.push_back(PartyMember("Companion 2"));
    m_party.push_back(PartyMember("Companion 3"));
    m_party.push_back(PartyMember("Companion 4"));
    
    // Setup initial resources
    setupStartingResources();
}

void TravelState::setupStartingResources() {
    // Start with some supplies
    if (m_profession == "Banker") {
        m_resources.money = 1600;
        m_resources.food = 200;
        m_resources.ammunition = 100;
        m_resources.clothing = 3;
        m_resources.wagonParts = 3;
        m_resources.medicines = 2;
    } else if (m_profession == "Carpenter") {
        m_resources.money = 800;
        m_resources.food = 180;
        m_resources.ammunition = 80;
        m_resources.clothing = 2;
        m_resources.wagonParts = 2;
        m_resources.medicines = 1;
    } else if (m_profession == "Farmer") {
        m_resources.money = 400;
        m_resources.food = 160;
        m_resources.ammunition = 60;
        m_resources.clothing = 1;
        m_resources.wagonParts = 1;
        m_resources.medicines = 1;
    } else {
        // Default
        m_resources.money = 1000;
        m_resources.food = 180;
        m_resources.ammunition = 80;
        m_resources.clothing = 2;
        m_resources.wagonParts = 2;
        m_resources.medicines = 1;
    }
}

void TravelState::advanceDay() {
    // Increment day counter
    m_currentDay++;
    
    // Update month/year if necessary
    if (m_currentDay > 30) { // Simplified month length
        m_currentDay = 1;
        m_month++;
        if (m_month > 12) {
            m_month = 1;
            m_year++;
        }
    }
    
    // Update weather based on month
    updateWeather();
    
    // Consume daily resources
    consumeResources();
    
    // Update health of party members
    updateHealth();
    
    // Travel distance for the day
    int milesForDay = calculateDailyMiles();
    m_milesTraveled += milesForDay;
    
    std::cout << "Day " << m_currentDay << " of month " << m_month << ": traveled " 
              << milesForDay << " miles. Total: " << m_milesTraveled << std::endl;
    
    // Check if reached Oregon
    if (m_milesTraveled >= 2040) {
        m_reachedOregon = true;
        m_gameOver = true;
        m_eventMessage = "Congratulations! You have reached Oregon City!";
        m_subState = TravelSubState::GameOver;
    }
    
    // Check if all party members are dead
    bool anyoneAlive = false;
    for (const auto& member : m_party) {
        if (member.isAlive) {
            anyoneAlive = true;
            break;
        }
    }
    
    if (!anyoneAlive) {
        m_gameOver = true;
        m_eventMessage = "Game Over. All members of your party have died.";
        m_subState = TravelSubState::GameOver;
    }
    
    // Check if out of food
    if (m_resources.food <= 0) {
        // Reduce health of party members
        for (auto& member : m_party) {
            if (member.isAlive) {
                member.health -= 15;
                if (member.health <= 0) {
                    member.isAlive = false;
                }
            }
        }
    }
    
    // Mark that we need to redraw
    m_needsUpdate = true;
}

void TravelState::consumeResources() {
    // Count alive party members
    int aliveMembers = 0;
    for (const auto& member : m_party) {
        if (member.isAlive) {
            aliveMembers++;
        }
    }
    
    // Each person consumes 2 pounds of food per day
    int foodConsumed = aliveMembers * 2;
    m_resources.food = std::max(0, m_resources.food - foodConsumed);
    
    // Clothing deteriorates based on weather
    if (m_currentWeather == Weather::Rainy || m_currentWeather == Weather::Stormy) {
        // More wear on clothing in bad weather
        std::uniform_int_distribution<> dis(1, 100);
        int roll = dis(m_rng);
        if (roll <= 10) { // 10% chance of clothing wear in bad weather
            if (m_resources.clothing > 0) {
                m_resources.clothing--;
                std::cout << "Some clothing has worn out due to bad weather." << std::endl;
            }
        }
    }
    
    // Wagon parts can break on rough terrain
    if (m_currentWeather == Weather::Stormy) {
        std::uniform_int_distribution<> dis(1, 100);
        int roll = dis(m_rng);
        if (roll <= 5) { // 5% chance of wagon damage in stormy weather
            if (m_resources.wagonParts > 0) {
                m_resources.wagonParts--;
                std::cout << "A wagon part broke during the storm." << std::endl;
            } else {
                // If no spare parts, reduce travel pace
                std::cout << "Your wagon is damaged and slowing you down." << std::endl;
            }
        }
    }
}

void TravelState::updateWeather() {
    Weather baseSeason = getWeatherForMonth(m_month);
    
    // Slight randomization of weather
    std::uniform_int_distribution<> dis(1, 100);
    int roll = dis(m_rng);
    
    if (roll <= 70) {
        // 70% chance of seasonal weather
        m_currentWeather = baseSeason;
    } else if (roll <= 85) {
        // 15% chance of better weather than expected
        switch (baseSeason) {
            case Weather::Snowy:
                m_currentWeather = Weather::Cloudy;
                break;
            case Weather::Rainy:
                m_currentWeather = Weather::Cloudy;
                break;
            case Weather::Stormy:
                m_currentWeather = Weather::Rainy;
                break;
            case Weather::Cloudy:
                m_currentWeather = Weather::Fair;
                break;
            default:
                m_currentWeather = Weather::Fair;
                break;
        }
    } else {
        // 15% chance of worse weather than expected
        switch (baseSeason) {
            case Weather::Fair:
                m_currentWeather = Weather::Cloudy;
                break;
            case Weather::Cloudy:
                m_currentWeather = Weather::Rainy;
                break;
            case Weather::Rainy:
                m_currentWeather = Weather::Stormy;
                break;
            default:
                m_currentWeather = Weather::Stormy;
                break;
        }
    }
}

Weather TravelState::getWeatherForMonth(int month) {
    // Simplified seasonal weather patterns
    switch (month) {
        case 12:
        case 1:
        case 2:
            return Weather::Snowy; // Winter
        case 3:
        case 4:
        case 5:
            return Weather::Rainy; // Spring
        case 6:
        case 7:
        case 8:
            return Weather::Fair; // Summer
        case 9:
        case 10:
        case 11:
            return Weather::Cloudy; // Fall
        default:
            return Weather::Fair;
    }
}

void TravelState::updateHealth() {
    for (auto& member : m_party) {
        if (!member.isAlive)
            continue;
            
        // Base health change
        int healthChange = 0;
        
        // Health boost if resting
        if (m_subState == TravelSubState::Resting) {
            healthChange += 5;
        }
        
        // Health penalty if no food
        if (m_resources.food <= 0) {
            healthChange -= 10;
        }
        
        // Health penalty for bad weather without clothing
        if ((m_currentWeather == Weather::Rainy || m_currentWeather == Weather::Snowy) && 
            m_resources.clothing <= 0) {
            healthChange -= 5;
        }
        
        // Random chance of illness
        std::uniform_int_distribution<> dis(1, 100);
        int roll = dis(m_rng);
        if (roll <= 5) { // 5% chance of illness
            member.health -= 15;
            member.ailment = "sick";
            
            // Medicine can help
            if (m_resources.medicines > 0) {
                m_resources.medicines--;
                member.health += 10; // Medicine alleviates some health loss
                member.ailment = "recovering";
            }
        }
        
        // Apply health change
        member.health += healthChange;
        
        // Cap health at 0-100
        member.health = std::max(0, std::min(100, member.health));
        
        // Check if died
        if (member.health <= 0) {
            member.isAlive = false;
            m_eventMessage = member.name + " has died.";
            m_subState = TravelSubState::Event;
        }
    }
}

int TravelState::calculateDailyMiles() {
    // Base travel rate
    int baseMiles = 10;
    
    // Weather modifiers
    switch (m_currentWeather) {
        case Weather::Fair:
            baseMiles += 5;
            break;
        case Weather::Cloudy:
            // No modifier
            break;
        case Weather::Rainy:
            baseMiles -= 3;
            break;
        case Weather::Stormy:
            baseMiles -= 7;
            break;
        case Weather::Snowy:
            baseMiles -= 10;
            break;
    }
    
    // Wagon damage modifier
    if (m_resources.wagonParts <= 0) {
        baseMiles = std::max(1, baseMiles - 5); // Damaged wagon slows travel
    }
    
    // Ensure minimum travel rate
    return std::max(1, baseMiles);
}

void TravelState::checkForLandmark() {
    if (m_nextLandmarkIndex >= static_cast<int>(m_landmarks.size()))
        return;
        
    if (m_milesTraveled >= m_landmarks[m_nextLandmarkIndex].distance) {
        // Reached a landmark
        Location landmark = m_landmarks[m_nextLandmarkIndex];
        m_eventMessage = "You have reached " + landmark.name + "!\n" + landmark.description;
        
        // Special handling for river crossings
        if (landmark.isRiver) {
            m_subState = TravelSubState::River;
        } else {
            m_subState = TravelSubState::Location;
        }
        
        m_nextLandmarkIndex++;
    }
}

void TravelState::triggerRandomEvent() {
    std::vector<std::string> events = {
        "One of your oxen is sick. It needs to rest for a day.",
        "A wheel on your wagon is damaged. You lose a wagon part.",
        "Heavy rains have washed out part of the trail ahead.",
        "You found wild berries and gathered some extra food!",
        "A friendly Native American group shows you a shortcut.",
        "Your wagon axle breaks! You must repair it to continue.",
        "Bandits attack your party! You lose some supplies.",
        "A friendly settler shares some food with your party.",
        "A snowstorm forces you to take shelter for a day.",
        "One of your party members has come down with dysentery."
    };
    
    std::uniform_int_distribution<> dis(0, events.size() - 1);
    int eventIndex = dis(m_rng);
    m_currentEvent = "Random Event";
    m_eventMessage = events[eventIndex];
    
    // Handle event effects
    switch (eventIndex) {
        case 0: // Sick oxen
            // Lose a day
            advanceDay();
            break;
            
        case 1: // Wagon wheel damaged
            if (m_resources.wagonParts > 0) {
                m_resources.wagonParts--;
            } else {
                // No spare parts slows you down
                m_eventMessage += " Without spare parts, this will slow your journey.";
            }
            break;
            
        case 2: // Heavy rains
            // Will affect travel speed through weather
            m_currentWeather = Weather::Rainy;
            break;
            
        case 3: // Found wild berries
            m_resources.food += 20;
            break;
            
        case 4: // Shortcut
            m_milesTraveled += 20;
            break;
            
        case 5: // Broken axle
            if (m_resources.wagonParts > 0) {
                m_resources.wagonParts--;
                m_eventMessage += " You used a spare part to fix it.";
            } else {
                // More serious breakdown
                m_eventMessage += " Without spare parts, your wagon is severely damaged. This will greatly slow your journey.";
            }
            break;
            
        case 6: // Bandits attack
            // Lose some resources
            m_resources.food = std::max(0, m_resources.food - 30);
            m_resources.ammunition = std::max(0, m_resources.ammunition - 20);
            m_resources.money = std::max(0, m_resources.money - 25);
            break;
            
        case 7: // Friendly settler shares food
            m_resources.food += 30;
            break;
            
        case 8: // Snowstorm
            // Force rest for a few days
            m_currentWeather = Weather::Snowy;
            restForDays(2);
            break;
            
        case 9: // Dysentery
            {
                // Random party member gets sick
                std::vector<size_t> aliveIndices;
                for (size_t i = 0; i < m_party.size(); i++) {
                    if (m_party[i].isAlive) {
                        aliveIndices.push_back(i);
                    }
                }
                
                if (!aliveIndices.empty()) {
                    std::uniform_int_distribution<> memberDis(0, aliveIndices.size() - 1);
                    int victimIndex = aliveIndices[memberDis(m_rng)];
                    m_party[victimIndex].health -= 25;
                    m_party[victimIndex].ailment = "dysentery";
                    m_eventMessage += " " + m_party[victimIndex].name + " has caught it.";
                    
                    // Medicine can help
                    if (m_resources.medicines > 0) {
                        m_resources.medicines--;
                        m_party[victimIndex].health += 15;
                        m_eventMessage += " You used medicine to treat them.";
                    }
                    
                    // Check if died
                    if (m_party[victimIndex].health <= 0) {
                        m_party[victimIndex].isAlive = false;
                        m_eventMessage += " Unfortunately, " + m_party[victimIndex].name + " has died.";
                    }
                }
            }
            break;
    }
    
    // Switch to event state to display the result
    m_subState = TravelSubState::Event;
}

void TravelState::handleSetupInput(SDL_Keycode key) {
    if (key == SDLK_SPACE || key == SDLK_RETURN) {
        // Complete setup and start journey
        m_subState = TravelSubState::Traveling;
    } else if (key == SDLK_ESCAPE) {
        returnToMenu();
    }
}

void TravelState::handleTravelInput(SDL_Keycode key) {
    switch (key) {
        case SDLK_SPACE:
            // Continue on the trail (advance one day)
            advanceDay();
            break;
            
        case SDLK_1:
            // Rest
            m_subState = TravelSubState::Resting;
            break;
            
        case SDLK_2:
            // Hunt
            m_subState = TravelSubState::Hunting;
            break;
            
        case SDLK_3:
            // Trade
            m_subState = TravelSubState::Trading;
            break;
            
        case SDLK_4:
            // Check supplies
            m_eventMessage = "Current Supplies:\n"
                            "Money: $" + std::to_string(m_resources.money) + "\n"
                            "Food: " + std::to_string(m_resources.food) + " pounds\n"
                            "Ammunition: " + std::to_string(m_resources.ammunition) + " bullets\n"
                            "Clothing: " + std::to_string(m_resources.clothing) + " sets\n"
                            "Wagon Parts: " + std::to_string(m_resources.wagonParts) + "\n"
                            "Medicines: " + std::to_string(m_resources.medicines);
            m_subState = TravelSubState::Event;
            break;
            
        case SDLK_ESCAPE:
            returnToMenu();
            break;
            
        default:
            break;
    }
}

void TravelState::restForDays(int days) {
    for (int i = 0; i < days; i++) {
        // Rest mode improves health but still consumes resources
        advanceDay();
    }
    
    m_eventMessage = "You rested for " + std::to_string(days) + " days. Your party's health has improved.";
    m_subState = TravelSubState::Event;
}

void TravelState::returnToMenu() {
    std::cout << "Returning to menu from TravelState" << std::endl;
    auto menuState = std::make_unique<MenuState>(m_game);
    m_game->changeState(std::move(menuState));
}

// Rendering methods for different sub-states
void TravelState::renderSetupScreen() {
    int y = 100;
    
    renderTextCentered("THE OREGON TRAIL", 50);
    
    renderTextCentered("You are about to embark on the Oregon Trail as a " + m_profession, y);
    y += 30;
    
    renderTextCentered("Your party:", y);
    y += 30;
    
    for (const auto& member : m_party) {
        renderTextCentered(member.name, y);
        y += 20;
    }
    
    y += 20;
    renderTextCentered("Your supplies:", y);
    y += 30;
    
    renderTextCentered("Money: $" + std::to_string(m_resources.money), y); y += 20;
    renderTextCentered("Food: " + std::to_string(m_resources.food) + " pounds", y); y += 20;
    renderTextCentered("Ammunition: " + std::to_string(m_resources.ammunition) + " bullets", y); y += 20;
    renderTextCentered("Clothing: " + std::to_string(m_resources.clothing) + " sets", y); y += 20;
    renderTextCentered("Wagon Parts: " + std::to_string(m_resources.wagonParts), y); y += 20;
    renderTextCentered("Medicines: " + std::to_string(m_resources.medicines), y); y += 30;
    
    renderTextCentered("Press SPACE to begin your journey", y + 20);
    renderTextCentered("Press ESC to return to menu", y + 40);
}

void TravelState::renderTravelScreen() {
    int y = 50;
    
    // Title
    renderTextCentered("OREGON TRAIL - ON THE TRAIL", y);
    y += 40;
    
    // Date and weather
    std::string monthNames[] = {"", "January", "February", "March", "April", "May", "June", 
                              "July", "August", "September", "October", "November", "December"};
    std::string weatherNames[] = {"Fair", "Cloudy", "Rainy", "Stormy", "Snowy"};
    
    renderText("Date: " + monthNames[m_month] + " " + std::to_string(m_currentDay) + ", " + std::to_string(m_year), 50, y);
    y += 20;
    
    renderText("Weather: " + weatherNames[static_cast<int>(m_currentWeather)], 50, y);
    y += 20;
    
    renderText("Miles Traveled: " + std::to_string(m_milesTraveled), 50, y);
    y += 20;
    
    // Next landmark
    if (m_nextLandmarkIndex < static_cast<int>(m_landmarks.size())) {
        int milesTo = m_landmarks[m_nextLandmarkIndex].distance - m_milesTraveled;
        renderText("Next Landmark: " + m_landmarks[m_nextLandmarkIndex].name + 
                   " (" + std::to_string(milesTo) + " miles)", 50, y);
    } else {
        renderText("You are nearing your destination!", 50, y);
    }
    y += 40;
    
    // Party status
    renderText("Party Status:", 50, y);
    y += 20;
    
    for (const auto& member : m_party) {
        std::string status = member.name + ": ";
        if (!member.isAlive) {
            status += "Dead";
        } else if (member.health < 20) {
            status += "Critical (" + std::to_string(member.health) + "%)";
        } else if (member.health < 50) {
            status += "Poor (" + std::to_string(member.health) + "%)";
        } else if (member.health < 80) {
            status += "Fair (" + std::to_string(member.health) + "%)";
        } else {
            status += "Good (" + std::to_string(member.health) + "%)";
        }
        
        if (!member.ailment.empty() && member.isAlive) {
            status += " - " + member.ailment;
        }
        
        renderText(status, 70, y);
        y += 20;
    }
    y += 20;
    
    // Supplies
    renderText("Supplies:", 50, y);
    y += 20;
    
    renderText("Food: " + std::to_string(m_resources.food) + " pounds", 70, y); y += 20;
    renderText("Ammunition: " + std::to_string(m_resources.ammunition) + " bullets", 70, y); y += 20;
    renderText("Money: $" + std::to_string(m_resources.money), 70, y); y += 20;
    renderText("Wagon Parts: " + std::to_string(m_resources.wagonParts), 70, y); y += 20;
    renderText("Clothing: " + std::to_string(m_resources.clothing) + " sets", 70, y); y += 20;
    renderText("Medicines: " + std::to_string(m_resources.medicines), 70, y);
    
    // Options
    y = m_game->getWindowHeight() - 100;
    renderTextCentered("What would you like to do?", y);
    y += 30;
    
    renderText("SPACE - Continue on trail", 200, y); y += 20;
    renderText("1 - Stop to rest", 200, y); y += 20;
    renderText("2 - Hunt for food", 200, y); y += 20;
    renderText("3 - Trade supplies", 200, y);
}

void TravelState::renderLocationScreen() {
    int y = 50;
    
    // Get current landmark
    int currentLandmarkIndex = std::max(0, m_nextLandmarkIndex - 1);
    Location landmark = m_landmarks[currentLandmarkIndex];
    
    // Title
    renderTextCentered("LANDMARK: " + landmark.name, y);
    y += 40;
    
    // Date and miles
    std::string monthNames[] = {"", "January", "February", "March", "April", "May", "June", 
                              "July", "August", "September", "October", "November", "December"};
    
    renderText("Date: " + monthNames[m_month] + " " + std::to_string(m_currentDay) + ", " + std::to_string(m_year), 50, y);
    y += 20;
    
    renderText("Miles Traveled: " + std::to_string(m_milesTraveled), 50, y);
    y += 40;
    
    // Landmark description
    renderTextCentered(landmark.description, y);
    y += 40;
    
    // Options that may be available at landmark
    if (landmark.name.find("Fort") != std::string::npos) {
        renderTextCentered("This fort offers trading opportunities and a chance to rest.", y);
        y += 30;
        renderText("Press 1 to Trade", 200, y); y += 20;
        renderText("Press 2 to Rest", 200, y); y += 20;
    }
    
    // Continue journey
    y = m_game->getWindowHeight() - 100;
    renderTextCentered("Press SPACE to continue your journey", y);
}

void TravelState::renderRiverScreen() {
    int y = 50;
    
    // Get current river
    int currentLandmarkIndex = std::max(0, m_nextLandmarkIndex - 1);
    Location river = m_landmarks[currentLandmarkIndex];
    
    // Title
    renderTextCentered("RIVER CROSSING: " + river.name, y);
    y += 40;
    
    // River info
    renderTextCentered(river.description, y);
    y += 30;
    
    std::string weatherNames[] = {"Fair", "Cloudy", "Rainy", "Stormy", "Snowy"};
    
    renderText("Weather: " + weatherNames[static_cast<int>(m_currentWeather)], 50, y);
    y += 20;
    
    std::string depthDescription;
    if (river.riverDepth <= 2) depthDescription = "shallow";
    else if (river.riverDepth <= 4) depthDescription = "moderate";
    else depthDescription = "deep";
    
    renderText("River depth: " + depthDescription, 50, y);
    y += 40;
    
    // Options
    renderTextCentered("How will you cross the river?", y);
    y += 30;
    
    renderText("1 - Attempt to ford the river", 200, y); y += 20;
    renderText("2 - Caulk the wagon and float across", 200, y); y += 20;
    renderText("3 - Hire a local guide ($40)", 200, y); y += 20;
    renderText("4 - Wait for conditions to improve", 200, y);
    
    // Risk levels
    y = m_game->getWindowHeight() - 120;
    if (m_currentWeather == Weather::Rainy || m_currentWeather == Weather::Stormy) {
        renderTextCentered("WARNING: The river is running high due to recent rains.", y);
    } else if (river.riverDepth >= 5) {
        renderTextCentered("WARNING: This river is very deep and dangerous.", y);
    }
    y += 30;
    
    renderTextCentered("Your decision may risk lives and supplies.", y);
}

void TravelState::renderHuntingScreen() {
    int y = 50;
    
    // Title
    renderTextCentered("HUNTING", y);
    y += 40;
    
    // Hunting info
    renderTextCentered("You're hunting for food to feed your party.", y);
    y += 30;
    
    renderText("Ammunition: " + std::to_string(m_resources.ammunition) + " bullets", 50, y);
    y += 40;
    
    if (m_resources.ammunition <= 0) {
        renderTextCentered("You don't have any ammunition for hunting!", y);
        y += 30;
        renderTextCentered("Press ESC to return to travel", y);
    } else {
        renderTextCentered("Press SPACE to fire your rifle", y);
        y += 30;
        renderTextCentered("Each shot uses 1 bullet", y);
        y += 30;
        renderTextCentered("Press ESC to cancel hunting and return to travel", y);
        
        // Draw a simple target
        SDL_Renderer* renderer = m_game->getRenderer();
        int centerX = m_game->getWindowWidth() / 2;
        int centerY = y + 80;
        
        SDL_SetRenderDrawColor(renderer, 144, 238, 144, 255); // Light green
        
        // Draw concentric circles
        for (int radius = 50; radius > 10; radius -= 10) {
            for (int i = 0; i < 360; i++) {
                double angle = i * M_PI / 180.0;
                int x = centerX + static_cast<int>(radius * cos(angle));
                int y = centerY + static_cast<int>(radius * sin(angle));
                SDL_RenderDrawPoint(renderer, x, y);
            }
        }
        
        // Draw crosshairs
        SDL_RenderDrawLine(renderer, centerX - 60, centerY, centerX + 60, centerY);
        SDL_RenderDrawLine(renderer, centerX, centerY - 60, centerX, centerY + 60);
    }
}

void TravelState::renderTradingScreen() {
    int y = 50;
    
    // Title
    renderTextCentered("TRADING POST", y);
    y += 40;
    
    // Trading info
    renderTextCentered("You can trade for supplies here.", y);
    y += 30;
    
    renderText("Money: $" + std::to_string(m_resources.money), 50, y);
    y += 40;
    
    // Items for sale
    renderTextCentered("Items for Sale:", y);
    y += 30;
    
    renderText("1 - Food (50 pounds) - $20", 200, y); y += 20;
    renderText("2 - Ammunition (20 bullets) - $10", 200, y); y += 20;
    renderText("3 - Clothing (1 set) - $15", 200, y); y += 20;
    renderText("4 - Wagon Parts (1) - $35", 200, y); y += 20;
    renderText("5 - Medicine (1) - $25", 200, y); y += 20;
    
    // Current supplies
    y += 20;
    renderTextCentered("Current Supplies:", y);
    y += 30;
    
    renderText("Food: " + std::to_string(m_resources.food) + " pounds", 200, y); y += 20;
    renderText("Ammunition: " + std::to_string(m_resources.ammunition) + " bullets", 200, y); y += 20;
    renderText("Clothing: " + std::to_string(m_resources.clothing) + " sets", 200, y); y += 20;
    renderText("Wagon Parts: " + std::to_string(m_resources.wagonParts), 200, y); y += 20;
    renderText("Medicines: " + std::to_string(m_resources.medicines), 200, y);
    
    // Exit
    y = m_game->getWindowHeight() - 70;
    renderTextCentered("Press ESC to exit trading", y);
}

void TravelState::renderEventScreen() {
    int y = 100;
    
    // Title - event type
    renderTextCentered(m_currentEvent.empty() ? "EVENT" : m_currentEvent, y);
    y += 40;
    
    // Break message into lines for better readability
    std::istringstream stream(m_eventMessage);
    std::string line;
    while (std::getline(stream, line)) {
        renderTextCentered(line, y);
        y += 25;
    }
    
    // Exit
    y = m_game->getWindowHeight() - 70;
    renderTextCentered("Press SPACE to continue", y);
}

void TravelState::renderRestingScreen() {
    int y = 50;
    
    // Title
    renderTextCentered("REST", y);
    y += 40;
    
    // Resting info
    renderTextCentered("Resting will improve your party's health but consume supplies.", y);
    y += 30;
    
    // Party health info
    renderTextCentered("Party Health:", y);
    y += 30;
    
    for (const auto& member : m_party) {
        if (member.isAlive) {
            std::string status = member.name + ": ";
            
            if (member.health < 20) {
                status += "Critical (" + std::to_string(member.health) + "%)";
            } else if (member.health < 50) {
                status += "Poor (" + std::to_string(member.health) + "%)";
            } else if (member.health < 80) {
                status += "Fair (" + std::to_string(member.health) + "%)";
            } else {
                status += "Good (" + std::to_string(member.health) + "%)";
            }
            
            if (!member.ailment.empty()) {
                status += " - " + member.ailment;
            }
            
            renderTextCentered(status, y);
            y += 20;
        }
    }
    
    y += 30;
    
    // Rest options
    renderTextCentered("How long would you like to rest?", y);
    y += 30;
    
    renderText("1 - Rest for 1 day", 200, y); y += 20;
    renderText("2 - Rest for 3 days", 200, y); y += 20;
    renderText("3 - Rest for a week (7 days)", 200, y); y += 20;
    renderText("ESC - Cancel resting", 200, y);
    
    // Warning for long rests
    if (m_resources.food < 50) {
        y += 40;
        renderTextCentered("WARNING: You have limited food supplies!", y);
    }
}

void TravelState::renderGameOverScreen() {
    int y = 100;
    
    if (m_reachedOregon) {
        // Victory screen
        renderTextCentered("CONGRATULATIONS!", y);
        y += 40;
        
        renderTextCentered("You have successfully completed the Oregon Trail!", y);
        y += 30;
        
        // Calculate final score
        int partyScore = 0;
        int aliveCount = 0;
        for (const auto& member : m_party) {
            if (member.isAlive) {
                aliveCount++;
                partyScore += member.health;
            }
        }
        
        int resourceScore = m_resources.food / 5 + m_resources.money / 5 + 
                           m_resources.ammunition / 10 + 
                           m_resources.clothing * 10 + 
                           m_resources.wagonParts * 15 + 
                           m_resources.medicines * 20;
        
        // Final score calculation
        int professionMultiplier = 1;
        if (m_profession == "Farmer") {
            professionMultiplier = 3;  // Harder start, better score
        } else if (m_profession == "Carpenter") {
            professionMultiplier = 2;  // Medium difficulty
        }
        
        int totalScore = (partyScore + resourceScore) * professionMultiplier;
        
        // Display party status
        renderTextCentered("Party Members Who Survived: " + std::to_string(aliveCount) + 
                           " out of " + std::to_string(m_party.size()), y);
        y += 30;
        
        // Display resources
        renderTextCentered("Remaining Resources:", y);
        y += 30;
        
        renderText("Food: " + std::to_string(m_resources.food) + " pounds", 200, y); y += 20;
        renderText("Money: $" + std::to_string(m_resources.money), 200, y); y += 20;
        renderText("Other supplies value: " + std::to_string(resourceScore - m_resources.food/5 - m_resources.money/5), 200, y);
        y += 30;
        
        // Display score
        renderTextCentered("Your final score: " + std::to_string(totalScore), y);
        y += 30;
        
        // Rating based on score
        std::string rating;
        if (totalScore > 1000) {
            rating = "Trail Guide - Outstanding!";
        } else if (totalScore > 750) {
            rating = "Seasoned Pioneer - Great job!";
        } else if (totalScore > 500) {
            rating = "Determined Traveler - Good work!";
        } else if (totalScore > 250) {
            rating = "Lucky Greenhorn - You made it!";
        } else {
            rating = "Surviving Pioneer - At least you're alive!";
        }
        
        renderTextCentered("Rating: " + rating, y);
    } else {
        // Game over screen
        renderTextCentered("GAME OVER", y);
        y += 40;
        
        renderTextCentered(m_eventMessage, y);
        y += 40;
        
        // Calculate how far they got
        double percentComplete = static_cast<double>(m_milesTraveled) / 2040.0 * 100.0;
        renderTextCentered("You traveled " + std::to_string(m_milesTraveled) + " miles.", y);
        y += 30;
        renderTextCentered("Journey completion: " + 
                           std::to_string(static_cast<int>(percentComplete)) + "%", y);
    }
    
    // Button to return to main menu
    y = m_game->getWindowHeight() - 100;
    renderTextCentered("Press SPACE or ESC to return to the main menu", y);
}
