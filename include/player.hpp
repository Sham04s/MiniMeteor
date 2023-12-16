#ifndef __PLAYER_H__
#define __PLAYER_H__

#include "character.hpp"
#include "asteroid.hpp"

class Player : public Character
{
private:
    Vector2 initialOrigin;
    bool invincible;
    std::vector<int> powerups;

protected:
    void SetDefaultHitBox();

public:
    Player() : Player({0, 0}) {}
    Player(Vector2 origin);
    ~Player();

    void Update();
    void HandleInput();

    void AddPowerup();
    bool CanBeKilled();
    bool Kill();
    void Respawn();
    void Reset();

    Rectangle GetFrameRec();

    std::vector<int> GetPowerups() { return powerups; }

    void SetLives(int lives) { this->lives = lives; }
};

#endif // __PLAYER_H__