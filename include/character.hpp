#ifndef __CHARACTER_H__
#define __CHARACTER_H__

#include "game_object.hpp"
#include "bullet.hpp"

#define CHARACTER_SIZE 80          // pixels
#define CHARACTER_ACCELERATION 200 // pixels per second
#define CHARACTER_DECELERATION 300 // pixels per second
#define CHARACTER_TURN_SPEED 200   // degrees per second
#define CHARACTER_MAX_SPEED 200    // pixels per second
#define CHARACTER_MAX_LIVES 4      // lives
#define SHOOT_COOLDOWN 0.4f        // seconds
#define CHARACTER_DYING_TIME 0.4f  // seconds
#define CHARACTER_RESPAWN_TIME 1.0f // second

enum CharacterState
{
    CHARACTER_IDLE,
    CHARACTER_ACCELERATING,
    CHARACTER_EXTRA_ACCELERATING,
    CHARACTER_TURNING_LEFT,
    CHARACTER_TURNING_RIGHT,
    CHARACTER_DYING,
    CHARACTER_DEAD,
};

class Character : public GameObject
{
protected:
    int lives;
    CharacterState state;
    std::vector<Bullet> bullets;
    float lastShotTime;
    float lastDeathTime;
    float maxSpeed;
    float acceleration;
    float deceleration;
    float turnSpeed;

    virtual void SetDefaultHitBox();

public:
    Character(Vector2 origin);
    virtual ~Character();

    virtual void Update();
    virtual void Draw();
    virtual void Shoot();
    virtual void CleanBullets();
    virtual void Kill();
    virtual void Respawn();

    void AddLife();

    virtual void Accelerate(float acceleration);

    virtual Rectangle GetFrameRec();
    
    bool IsMoving() { return state == CHARACTER_ACCELERATING || state == CHARACTER_EXTRA_ACCELERATING; }
    bool IsTurning() { return state == CHARACTER_TURNING_LEFT || state == CHARACTER_TURNING_RIGHT; }
    bool IsDying() { return state == CHARACTER_DYING; }
    bool IsDead() { return state == CHARACTER_DEAD; }
    bool IsAlive() { return state != CHARACTER_DEAD && state != CHARACTER_DYING; }
    bool CanShoot() { return GetTime() - lastShotTime > SHOOT_COOLDOWN; }
    int GetLives() { return lives; }
    std::vector<Bullet> *GetBullets() { return &bullets; }

    
};
#endif // __CHARACTER_H__