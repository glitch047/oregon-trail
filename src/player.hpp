#ifndef PLAYER_HPP
#define PLAYER_HPP

#include <string>
#include <vector>

// Forward declarations
class Game;

enum class Profession {
    BANKER,
    CARPENTER,
    FARMER
};

struct PartyMember {
    std::string name;
    int health;
    bool isAlive;
    
    PartyMember(const std::string& memberName) 
        : name(memberName), health(100), isAlive(true) {}
};

class Player {
public:
    Player(const std::string& name);
    ~Player();
    
    void update(float deltaTime);
    
    // Getters
    const std::string& getName() const { return m_name; }
    Profession getProfession() const { return m_profession; }
    int getMoney() const { return m_money; }
    int getFood() const { return m_food; }
    int getAmmunition() const { return m_ammunition; }
    
    // Setters
    void setProfession(Profession profession);
    void addPartyMember(const std::string& name);
    void setMoney(int amount) { m_money = amount; }
    void addMoney(int amount) { m_money += amount; }
    void setFood(int amount) { m_food = amount; }
    void addFood(int amount) { m_food += amount; }
    void setAmmunition(int amount) { m_ammunition = amount; }
    void addAmmunition(int amount) { m_ammunition += amount; }
    
private:
    std::string m_name;
    Profession m_profession;
    int m_money;
    int m_food;
    int m_ammunition;
    int m_clothing;
    int m_spareWagonParts;
    
    std::vector<PartyMember> m_partyMembers;
};

#endif // PLAYER_HPP

