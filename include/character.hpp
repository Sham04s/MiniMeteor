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
#define THRUST_MIN_PITCH 0.5f  // max pitch is 1.0f
#define THRUST_MIN_VOLUME 0.5f // max volume is 1.0f
// equal pan is 0.5f
#define TRHUST_MIN_PAN 0.4f // min is right channel
#define THRUST_MAX_PAN 0.7f // max is left channel
#define THRUST_PITCH_DECAYING_TIME 4.0f

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
    Sound thrustSound;
    float timeAccelerating;

    virtual void SetDefaultHitBox();

public:
    Character(Vector2 origin);
    virtual ~Character();

    virtual void Update();
    virtual void Draw();
    virtual void Shoot();
    virtual void CleanBullets();
    virtual bool Kill();
    virtual void Respawn();

    virtual void PauseSounds();
    virtual void ResumeSounds();

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