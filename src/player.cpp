#include "player.hpp"

Player::Player(const std::string& name)
    : m_name(name)
    , m_profession(Profession::BANKER)
    , m_money(0)
    , m_food(0)
    , m_ammunition(0)
    , m_clothing(0)
    , m_spareWagonParts(0)
{
    // Add player as first party member
    m_partyMembers.emplace_back(name);
    
    // Set starting resources based on profession
    setProfession(Profession::BANKER);
}

Player::~Player() {
    // Nothing to clean up
}

void Player::setProfession(Profession profession) {
    m_profession = profession;
    
    // Set starting money based on profession
    switch (m_profession) {
        case Profession::BANKER:
            m_money = 1600;
            break;
        case Profession::CARPENTER:
            m_money = 800;
            break;
        case Profession::FARMER:
            m_money = 400;
            break;
    }
}

void Player::addPartyMember(const std::string& name) {
    // Maximum party size is 5 (player + 4 others)
    if (m_partyMembers.size() < 5) {
        m_partyMembers.emplace_back(name);
    }
}

void Player::update(float deltaTime) {
    // Update player and party members state (health, etc.)
    // This would handle resource consumption over time
}

