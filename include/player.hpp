#include "game_object.hpp"

enum PlayerState
{
    IDLE,
    ACCELERATING,
    BRAKING,
    TURNING_LEFT,
    TURNING_RIGHT,
    SHOOTING,
    DYING
};

class Player : public GameObject
{
private:
    static constexpr float MAX_SPEED = 200;

    Vector2 originalCenter;
    int lives;
    Vector2 velocity;
    PlayerState playerState;
    std::vector<int> powerups;
    std::vector<int> shots;

public:
    Player() : GameObject(){};
    Player(Vector2 center, int zIndex);
    ~Player();

    void Update();
    void Draw();
    void DrawDebug();
    void AddLive();
    void AddPowerUp(/*PowerUp powerup*/);
    void Kill();
    void Reset();

    Vector2 GetOriginalCenter() { return originalCenter; }
    int GetLives() { return lives; }
    Vector2 GetVelocity() { return velocity; }
    std::vector<int> GetPowerups() { return powerups; }
    std::vector<int> GetShots() { return shots; }

    void SetOriginalCenter(Vector2 originalCenter) { this->originalCenter = originalCenter; }
    void SetLives(int lives) { this->lives = lives; }
    void SetVelocity(Vector2 velocity) { this->velocity = velocity; }
    void SetPowerups(std::vector<int> powerups) { this->powerups = powerups; }
    void SetShots(std::vector<int> shots) { this->shots = shots; }

private:
    void Rotate(float angle);
    void Accelerate(float acceleration);
    void Decelerate(float deceleration);
    void Shoot();
};
