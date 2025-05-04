#ifndef MENU_STATE_HPP
#define MENU_STATE_HPP

#include "game_state.hpp"
#include <vector>
#include <string>
#include <SDL2/SDL_ttf.h>

class MenuState : public GameState {
public:
    MenuState(Game* game);
    virtual ~MenuState();

    // GameState interface
    virtual void enter() override;
    virtual void exit() override;
    virtual void handleEvent(const SDL_Event& event) override; // Changed from processInput to match base class
    virtual void update(float deltaTime) override;
    virtual void render() override;
    virtual std::string getName() const override { return "MenuState"; }

private:
    // Menu data
    std::vector<std::string> m_menuOptions;
    std::vector<std::string> m_introText;
    int m_selectedOption;
    
    // Text rendering
    TTF_Font* m_font;
    SDL_Color m_textColor;
    
    // Helper methods
    void loadMenuText();
    void loadIntroText();
    void renderText(const std::string& text, int x, int y);
    void renderTextCentered(const std::string& text, int y);
    void handleMenuSelection();
};

#endif // MENU_STATE_HPP

