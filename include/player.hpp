#ifndef __PLAYER_H__
#define __PLAYER_H__

#include "game_object.hpp"
#include "bullet.hpp"
#include "asteroid.hpp"

#define PLAYER_SIZE 80 // pixels
#define PLAYER_ACCELERATION 200 // pixels per second
#define PLAYER_DECELERATION 100 // pixels per second
#define PLAYER_TURN_SPEED 200 // degrees per second
#define PLAYER_MAX_SPEED 200 // pixels per second
#define PLAYER_MAX_LIVES 4 // lives
#define SHOOT_COOLDOWN 0.4f // seconds

#define PLAYER_DYING_TIME 0.4f // seconds 
#define PLAYER_RESPAWN_TIME 1.0f // seconds

enum PlayerState
{
    IDLE,
    ACCELERATING,
    EXTRA_ACCELERATING,
    TURNING_LEFT,
    TURNING_RIGHT,
    SHOOTING,
    DYING,
    DEAD,
};

class Player : public GameObject
{
private:

    int lives;
    Vector2 initialOrigin;
    PlayerState playerState;
    std::vector<int> powerups;
    std::vector<Bullet> bullets;
    float lastShotTime;
    float lastDeathTime;

    void SetDefaultHitBox();

public:
    Player() : GameObject(){};
    Player(Vector2 origin);
    ~Player();

    void Update();
    bool CheckCollision(GameObject *other, Vector2 *collisionPoint);
    void HandleInput();
    void Draw();
    void DrawDebug();
    void AddLive();
    void AddPowerUp(/*PowerUp powerup*/);
    void Kill();
    void Respawn();

    int GetLives() { return lives; }
    Vector2 GetOriginalCenter() { return initialOrigin; }
    PlayerState GetState() { return playerState; }
    Vector2 GetVelocity() { return velocity; }
    std::vector<int> GetPowerups() { return powerups; }
    std::vector<Bullet> *GetBullets() { return &bullets; }

    void SetOriginalCenter(Vector2 initialOrigin) { this->initialOrigin = initialOrigin; }
    void SetLives(int lives) { this->lives = lives; }
    void SetVelocity(Vector2 velocity) { this->velocity = velocity; }
    void SetPowerups(std::vector<int> powerups) { this->powerups = powerups; }
    void SetShots(std::vector<Bullet> bullets) { this->bullets = bullets; } // TODO: consider removing some setters

private:
    void Accelerate(float acceleration);
    void Shoot();
    void PurgeBullets();
};

#endif // __PLAYER_H__