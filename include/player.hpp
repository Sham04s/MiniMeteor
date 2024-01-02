#ifndef __PLAYER_H__
#define __PLAYER_H__

#include "character.hpp"
#include "asteroid.hpp"
#include "power_up.hpp"

#include <vector>

#define BOOST_TIME 0.8f                                      // seconds
#define BOOST_RECHARGE_COOLDOWN 1.5f                         // seconds
#define BOOST_BAR_HIDE_TIME (BOOST_RECHARGE_COOLDOWN + 1.0f) // seconds
#define BOOST_BAR_FADE_TIME 0.5f                             // seconds
#define MAX_UPGRADES_PER_TYPE 5

#define FIRE_RATE_MULTIPLIER 0.7f
#define BULLETS_PER_SHOT_MULTIPLIER 1.2f
#define BULLETS_SPEED_MULTIPLIER 1.2f
#define BULLETS_SPREAD_MULTIPLIER 0.82f

// TODO: implement turret mode

class Player : public Character
{
private:
    Vector2 initialOrigin;
    Camera2D camera;
    bool invincible;
    bool hasMoved;
    bool usingBoost;
    float boostTime;
    float lastBoostUsedTime;
    std::vector<PowerUp *> powerups;
    size_t powerupsCount[NUM_POWER_UP_TYPES];

    Sound *powerupPickupSound;

protected:
    void SetDefaultHitBox();

public:
    Player() : Player({0, 0}) {}
    Player(Vector2 origin);
    ~Player();

    void Update();
    void UpdatePowerups();
    void Draw();
    void DrawDebug();
    void HandleInput();
    void HandleCollision(GameObject *other, Vector2 *pushVector);

    PowerUp *GetPowerup(PowerUpType type);
    bool AddPowerup(PowerUp *powerup);
    bool RemovePowerup(PowerUpType type);
    bool HasPowerup(PowerUpType type);
    bool CanBeKilled();
    bool CanBeHit();
    bool HasMoved();
    bool Kill();
    void Respawn();
    void Reset();

    Rectangle GetFrameRec();

    std::vector<PowerUp *> GetPowerups() { return powerups; }
    float GetPowerupMultiplier(PowerUpType type);
    size_t GetPowerupCount(PowerUpType type) { return powerupsCount[type]; }
    Camera2D GetCamera() { return camera; }
    void UpdateCamera();

    void SetLives(int lives) { this->lives = lives; }
};

#endif // __PLAYER_H__