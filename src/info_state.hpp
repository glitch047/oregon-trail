#ifndef INFO_STATE_HPP
#define INFO_STATE_HPP

#include "game_state.hpp"
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <string>
#include <vector>

class Game;

class InfoState : public GameState {
public:
    InfoState(Game* game, const std::string& title, const std::string& content);
    virtual ~InfoState();
    
    // GameState interface implementation
    virtual void enter() override;
    virtual void exit() override;
    virtual void handleEvent(const SDL_Event& event) override;
    virtual void update(float deltaTime) override;
    virtual void render() override;
    virtual std::string getName() const override { return "InfoState"; }
    
private:
    // Render text at specified position
    void renderText(const std::string& text, int x, int y);
    
    // Render text centered horizontally at specified y position
    void renderTextCentered(const std::string& text, int y);
    
    // Split content into lines for display
    void processContent();
    
    // Helper method to return to the menu state
    void returnToMenu();
    
    // Member variables
    std::string m_title;
    std::string m_rawContent;
    std::vector<std::string> m_contentLines;
    TTF_Font* m_font;
    SDL_Color m_textColor;
    int m_scrollOffset;
};

#endif // INFO_STATE_HPP

