#include "info_state.hpp"
#include "game.hpp"
#include "menu_state.hpp"
#include <iostream>
#include <sstream>
#include <algorithm>

InfoState::InfoState(Game* game, const std::string& title, const std::string& content)
    : GameState(game)
    , m_title(title)
    , m_rawContent(content)
    , m_font(nullptr)
    , m_scrollOffset(0)
{
    // Set Apple II style green text color
    m_textColor = {144, 238, 144, 255}; // Light green
    
    // Process content into displayable lines
    processContent();
    
    std::cout << "InfoState initialized with title: " << title << std::endl;
}

InfoState::~InfoState() {
    if (m_font) {
        TTF_CloseFont(m_font);
        m_font = nullptr;
    }
}

void InfoState::enter() {
    std::cout << "Entering InfoState: " << m_title << std::endl;
    
    // Load font if not already loaded
    if (!m_font) {
        std::cout << "Attempting to load font for InfoState" << std::endl;
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
            std::cout << "Successfully loaded font for InfoState" << std::endl;
        }
    }
    
    // Reset scroll position
    m_scrollOffset = 0;
}

void InfoState::exit() {
    std::cout << "Exiting InfoState: " << m_title << std::endl;
}

// Helper method to return to the menu state
void InfoState::returnToMenu() {
    std::cout << "Returning to menu" << std::endl;
    auto menuState = std::make_unique<MenuState>(m_game);
    m_game->changeState(std::move(menuState));
}

void InfoState::handleEvent(const SDL_Event& event) {
    if (event.type == SDL_KEYDOWN) {
        std::cout << "InfoState: Key pressed: " << SDL_GetKeyName(event.key.keysym.sym) << std::endl;
        
        switch (event.key.keysym.sym) {
            case SDLK_ESCAPE:
                // Return to menu
                std::cout << "Escape pressed, returning to menu" << std::endl;
                returnToMenu();
                break;
                
            case SDLK_UP:
                // Scroll up
                if (m_scrollOffset > 0) {
                    m_scrollOffset -= 1;
                }
                break;
                
            case SDLK_DOWN:
                // Scroll down (with limit)
                if (m_scrollOffset < static_cast<int>(m_contentLines.size()) - 10) {
                    m_scrollOffset += 1;
                }
                break;
                
            case SDLK_PAGEUP:
                // Scroll up by page
                m_scrollOffset = std::max(0, m_scrollOffset - 10);
                break;
                
            case SDLK_PAGEDOWN:
                // Scroll down by page
                m_scrollOffset = std::min(static_cast<int>(m_contentLines.size()) - 10, 
                                        m_scrollOffset + 10);
                break;
                
            case SDLK_HOME:
                // Scroll to top
                m_scrollOffset = 0;
                break;
                
            case SDLK_END:
                // Scroll to end
                m_scrollOffset = std::max(0, static_cast<int>(m_contentLines.size()) - 10);
                break;
                
            default:
                // No action for other keys
                break;
        }
    }
}

void InfoState::update(float deltaTime) {
    // Not much to update in this state
    // Could add animations or effects later
}

void InfoState::render() {
    // Get renderer
    SDL_Renderer* renderer = m_game->getRenderer();
    
    // Clear screen to black
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);
    
    // Render title
    renderTextCentered(m_title, 50);
    
    // Render horizontal line under title
    SDL_SetRenderDrawColor(renderer, 144, 238, 144, 255); // Light green
    SDL_RenderDrawLine(renderer, 50, 80, m_game->getWindowWidth() - 50, 80);
    
    // Render content with scrolling
    int y = 100;
    int visibleLines = (m_game->getWindowHeight() - 150) / 20; // Approximate lines that can fit
    
    for (int i = m_scrollOffset; 
         i < m_scrollOffset + visibleLines && i < static_cast<int>(m_contentLines.size()); 
         ++i) {
        renderText(m_contentLines[i], 50, y);
        y += 20;
    }
    
    // Render instruction text at bottom
    SDL_SetRenderDrawColor(renderer, 144, 238, 144, 255); // Light green
    SDL_RenderDrawLine(renderer, 50, m_game->getWindowHeight() - 50, 
                      m_game->getWindowWidth() - 50, m_game->getWindowHeight() - 50);
    renderTextCentered("Press ESC to return to menu, Up/Down to scroll", 
                      m_game->getWindowHeight() - 30);
    
    // Render scroll indicators if needed
    if (m_scrollOffset > 0) {
        renderTextCentered("▲", 85);
    }
    if (m_scrollOffset + visibleLines < static_cast<int>(m_contentLines.size())) {
        renderTextCentered("▼", m_game->getWindowHeight() - 65);
    }
}

void InfoState::renderText(const std::string& text, int x, int y) {
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

void InfoState::renderTextCentered(const std::string& text, int y) {
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

void InfoState::processContent() {
    m_contentLines.clear();
    
    // Split the content by newlines first
    std::istringstream stream(m_rawContent);
    std::string line;
    
    while (std::getline(stream, line)) {
        // If the line is empty, add it as is
        if (line.empty()) {
            m_contentLines.push_back("");
            continue;
        }
        
        // Check if line needs to be wrapped (assume ~70 chars fit on screen)
        const int maxLineLength = 70;
        
        while (line.length() > maxLineLength) {
            // Find a good breaking point (space)
            size_t breakPos = line.rfind(' ', maxLineLength);
            if (breakPos == std::string::npos) {
                // No space found, force break at maxLineLength
                breakPos = maxLineLength;
            }
            
            // Add the line segment
            m_contentLines.push_back(line.substr(0, breakPos));
            
            // Continue with remainder, removing leading spaces
            line = line.substr(breakPos);
            line.erase(0, line.find_first_not_of(' '));
        }
        
        // Add the remaining part of the line
        if (!line.empty()) {
            m_contentLines.push_back(line);
        }
    }
    
    std::cout << "Processed content into " << m_contentLines.size() << " lines" << std::endl;
}

