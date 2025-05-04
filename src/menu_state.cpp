#include "menu_state.hpp"
#include "game.hpp"
#include "info_state.hpp"
#include <fstream>
#include <iostream>
#include <SDL2/SDL.h>

#include <iostream>

MenuState::MenuState(Game* game)
    : GameState(game)
    , m_selectedOption(0)
    , m_font(nullptr)
{
    // Set Apple II style green text color
    m_textColor = {144, 238, 144, 255}; // Light green
    std::cout << "MenuState initialized" << std::endl;
}

MenuState::~MenuState() {
    if (m_font) {
        TTF_CloseFont(m_font);
        m_font = nullptr;
    }
}

void MenuState::enter() {
    std::cout << "Entering MenuState" << std::endl;
    // Load font if not already loaded
    if (!m_font) {
        std::cout << "Attempting to load font from assets/fonts/apple2.ttf" << std::endl;
        m_font = TTF_OpenFont("assets/fonts/apple2.ttf", 16);
        if (!m_font) {
            std::cerr << "Failed to load font: " << TTF_GetError() << std::endl;
            std::cout << "Trying fallback font..." << std::endl;
            
            // Try multiple fallback fonts
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
                // Continue without a font - we'll handle this in the render methods
            }
        } else {
            std::cout << "Successfully loaded apple2.ttf font" << std::endl;
        }
    }
    
    // Load menu text and intro
    loadMenuText();
    loadIntroText();
}

void MenuState::loadIntroText() {
    m_introText.clear();
    
    // Try to load from file
    std::ifstream introFile("resources/text/intro.txt");
    if (introFile.is_open()) {
        std::string line;
        while (std::getline(introFile, line)) {
            m_introText.push_back(line);
        }
        introFile.close();
    }
    
    // If file load failed or file was empty, use default intro text
    if (m_introText.empty()) {
        m_introText = {
            "Try your hand at a journey across the 2,000 mile Oregon Trail!",
            "As a pioneer in Independence, Missouri, you must outfit and guide a",
            "party of settlers to Oregon's Willamette Valley in 1848.",
            "",
            "Choose your profession and supplies carefully - your choices",
            "can be the difference between life and death!"
        };
    }
}

void MenuState::exit() {
    std::cout << "Exiting MenuState" << std::endl;
    // Clean up resources specific to this state if needed
}

// processInput method has been moved to handleEvent

void MenuState::update(float deltaTime) {
    // Not much to update in menu state, it's mostly static
    // Could add animations or effects later
}

void MenuState::render() {
    // Clear screen to black
    SDL_Renderer* renderer = m_game->getRenderer();
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);
    
    // Render title
    renderTextCentered("THE OREGON TRAIL", 50);
    
    // Render intro text
    int y = 100;
    for (const auto& line : m_introText) {
        renderText(line, 50, y);
        y += 20;
    }
    
    // Render menu options
    y = 300;
    for (size_t i = 0; i < m_menuOptions.size(); ++i) {
        std::string prefix = (i == static_cast<size_t>(m_selectedOption)) ? "> " : "  ";
        renderText(prefix + m_menuOptions[i], 200, y);
        y += 30;
    }
    
    // Instruction text at bottom
    renderTextCentered("Use arrow keys to select, Enter to choose", 550);
}

void MenuState::loadMenuText() {
    m_menuOptions.clear();
    
    // Try to load from file
    std::ifstream menuFile("resources/text/menu.txt");
    if (menuFile.is_open()) {
        std::string line;
        while (std::getline(menuFile, line)) {
            if (!line.empty()) {
                m_menuOptions.push_back(line);
            }
        }
        menuFile.close();
    }
    
    // If file load failed or file was empty, use default menu options
    if (m_menuOptions.empty()) {
        m_menuOptions = {
            "Travel the trail",
            "Learn about the trail",
            "See the high scores",
            "Choose your profession",
            "Exit"
        };
    }
}

void MenuState::renderText(const std::string& text, int x, int y) {
    if (text.empty()) {
        return;
    }
    
    if (!m_font) {
        std::cerr << "Cannot render text '" << text << "': font not loaded" << std::endl;
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


void MenuState::handleMenuSelection() {
    std::cout << "Selected option: " << m_selectedOption << " - " 
              << (m_selectedOption < static_cast<int>(m_menuOptions.size()) ? m_menuOptions[m_selectedOption] : "Unknown") 
              << std::endl;

    switch(m_selectedOption) {
        case 0: // Travel the trail
            std::cout << "Travel the trail selected - Creating placeholder TravelState" << std::endl;
            {
                // Create a placeholder state with "Not Implemented" message
                auto travelState = std::make_unique<InfoState>(
                    m_game, 
                    "Travel the Trail", 
                    "This feature is coming soon!\n\n"
                    "The full travel simulation will include:\n"
                    "- Daily travel progress\n"
                    "- Resource management\n"
                    "- Weather effects\n"
                    "- River crossings\n"
                    "- Health and illness\n"
                    "- Random events\n\n"
                    "Check back in a future update!"
                );
                m_game->changeState(std::move(travelState));
            }
            break;
            
        case 1: // Learn about the trail
            std::cout << "Learn about the trail selected - Creating LearnState" << std::endl;
            {
                // Create an info state with Oregon Trail information
                auto learnState = std::make_unique<InfoState>(
                    m_game, 
                    "Learn About The Trail", 
                    "The Oregon Trail was a 2,170-mile east-west, large-wheeled wagon route that\n"
                    "connected the Missouri River to valleys in Oregon. It was the primary path\n"
                    "for emigration to the Oregon Territory in the 1840s-1860s. Hundreds of thousands\n"
                    "of settlers used it to reach new opportunities in the West.\n\n"
                    "The journey typically took 4-6 months to complete, with travelers facing many\n"
                    "hardships including disease, accidents, hostile encounters, and harsh weather.\n\n"
                    "The trail began in Independence, Missouri and ended in Oregon City, Oregon.\n"
                    "Major landmarks along the way included:\n"
                    "- Fort Kearney, Nebraska\n"
                    "- Chimney Rock, Nebraska\n"
                    "- Fort Laramie, Wyoming\n"
                    "- Independence Rock, Wyoming\n"
                    "- South Pass, Wyoming\n"
                    "- Fort Hall, Idaho\n"
                    "- The Dalles, Oregon\n\n"
                    "Travelers faced many life-or-death decisions during their journey, including\n"
                    "river crossings, mountain passages, and interactions with Native Americans.\n\n"
                    "Though dangerous, the Oregon Trail represented opportunity and a chance for\n"
                    "a better life in the fertile Willamette Valley."
                );
                m_game->changeState(std::move(learnState));
            }
            break;
            
        case 2: // See the high scores
            std::cout << "See the high scores selected - Creating HighScoreState" << std::endl;
            {
                auto highScoreState = std::make_unique<InfoState>(
                    m_game, 
                    "High Scores", 
                    "No high scores available yet.\n\n"
                    "Complete a journey to record your score!\n\n"
                    "Scoring is based on:\n"
                    "- Number of party members who survive\n"
                    "- Resources remaining\n"
                    "- Cash remaining\n"
                    "- Profession difficulty\n"
                    "- Days taken to complete the journey"
                );
                m_game->changeState(std::move(highScoreState));
            }
            break;
            
        case 3: // Choose your profession
            std::cout << "Choose your profession selected - Creating ProfessionState" << std::endl;
            {
                auto professionState = std::make_unique<InfoState>(
                    m_game, 
                    "Choose Your Profession", 
                    "Available professions:\n\n"
                    "1. Banker from Boston - $1600 starting money\n"
                    "   The easiest profession for those new to the trail.\n\n"
                    "2. Carpenter from Ohio - $800 starting money\n"
                    "   Good for experienced travelers.\n\n"
                    "3. Farmer from Illinois - $400 starting money\n"
                    "   The most challenging start, but highest score potential.\n\n"
                    "Your profession affects your starting money, which you'll use to buy\n"
                    "supplies for your journey. Choose wisely!\n\n"
                    "Profession selection coming soon! Press ESC to return to the menu."
                );
                m_game->changeState(std::move(professionState));
            }
            break;
            
        case 4: // Exit
            std::cout << "Exit selected" << std::endl;
            m_game->quit();
            break;
            
        default:
            std::cout << "Unknown option selected" << std::endl;
            break;
    }
}
void MenuState::renderTextCentered(const std::string& text, int y) {
    if (text.empty()) {
        return;
    }
    
    if (!m_font) {
        std::cerr << "Cannot render centered text '" << text << "': font not loaded" << std::endl;
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

void MenuState::handleEvent(const SDL_Event& event) {
    std::cout << "MenuState handling event: " << event.type << std::endl;
    
    if (event.type == SDL_KEYDOWN) {
        std::cout << "Key pressed: " << SDL_GetKeyName(event.key.keysym.sym) << std::endl;
        
        switch (event.key.keysym.sym) {
            case SDLK_UP:
                // Move selection up
                m_selectedOption = (m_selectedOption > 0) ? 
                                  m_selectedOption - 1 : 
                                  static_cast<int>(m_menuOptions.size() - 1);
                std::cout << "Selected option moved up to: " << m_selectedOption << std::endl;
                break;
                
            case SDLK_DOWN:
                // Move selection down
                m_selectedOption = (m_selectedOption < static_cast<int>(m_menuOptions.size() - 1)) ? 
                                  m_selectedOption + 1 : 
                                  0;
                std::cout << "Selected option moved down to: " << m_selectedOption << std::endl;
                break;
                
            case SDLK_RETURN:
            case SDLK_SPACE:
                // Select current option
                handleMenuSelection();
                break;
                
            case SDLK_ESCAPE:
                // Exit the game
                std::cout << "Escape pressed, quitting game" << std::endl;
                m_game->quit();
                break;
                
            default:
                std::cout << "Unhandled key pressed" << std::endl;
                break;
        }
    }
}
