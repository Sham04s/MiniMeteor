#ifndef __CHARACTER_H__
#define __CHARACTER_H__

#include "game/objects/game_object.hpp"
#include "game/objects/bullet.hpp"

#define CHARACTER_SIZE 80             // pixels
#define CHARACTER_ACCELERATION 200    // pixels per second
#define CHARACTER_DECELERATION 300    // pixels per second
#define CHARACTER_TURN_SPEED 200      // degrees per second
#define CHARACTER_MAX_SPEED 200       // pixels per second
#define CHARACTER_MAX_LIVES 4         // lives
#define CHARACTER_SHOOT_COOLDOWN 0.8f // seconds
#define CHARACTER_DYING_TIME 0.4f     // seconds
#define CHARACTER_RESPAWN_TIME 1.0f   // second
#define THRUST_MIN_PITCH 0.5f         // max pitch is 1.0f
#define THRUST_MIN_VOLUME 0.5f        // max volume is 1.0f
// equal pan is 0.5f
#define THRUST_MIN_PAN 0.25f // min is right channel
#define THRUST_MAX_PAN 0.75f // max is left channel
#define THRUST_PITCH_DECAYING_TIME 4.0f

// we declare the enum as a bit mask so we can combine turning states with other states
enum CharacterState
{
    IDLE = 1 << 0,          // 000001 in binary
    ACCELERATING = 1 << 1,  // 000010 in binary
    TURNING_LEFT = 1 << 2,  // 000100 in binary
    TURNING_RIGHT = 1 << 3, // 001000 in binary
    DYING = 1 << 4,         // 010000 in binary
    DEAD = 1 << 5,          // 100000 in binary
};

/**
 * @brief The Character class is the base class for all characters in the game.
 * It handles the movement, shooting, and collision (these methods can be overriden)
 */
class Character : public GameObject
{
protected:
    int lives;

    /**
     * @brief A bit mask of the current state of the character.
     * See the CharacterState enum for more details.
     */
    int state;
    std::vector<Bullet> bullets;

    Vector2 accelDir;

    float lastShotTime;
    float lastDeathTime;
    float timeAccelerating;
    bool exploded;

    // Character stats
    float maxSpeed;
    float acceleration;
    float deceleration;
    float turnSpeed;
    float shootCooldown;
    int bulletsPerShot;
    float bulletsSpeed;
    float bulletsSpread;

    // Sound Aliases
    Sound shootSound;
    Sound thrustSound;
    Sound explosionSound;

    float pitchAndVolumeScale;

    /**
     * @brief Set the Default Hit Box object. This is called in the constructor.
     * This method can be overriden to set a custom hit box.
     */
    virtual void SetDefaultHitBox();

public:
    /**
     * @brief Construct a new Character object with the given origin.
     *
     * @param origin The origin position of the character.
     */
    Character(Vector2 origin);

    /**
     * @brief Destroy the Character object unloading all Sound Aliases.
     */
    virtual ~Character();

    /**
     * @brief Updates the state, movement, actions, sounds, and bullets of the character.
     */
    virtual void Update();

    /**
     * @brief Draws the bullets and the character with it's current state.
     */
    virtual void Draw();
    virtual void DrawDebug();
    virtual bool CheckCollision(GameObject *other, Vector2 *pushVector);
    virtual void HandleCollision(GameObject *other, Vector2 *pushVector);
    virtual void Shoot();
    /**
     * @brief Cleans the bullets that are out of bounds or destroyed.
     */
    virtual void CleanBullets();
    virtual bool CanBeKilled();
    virtual bool CanBeHit();
    virtual bool Kill();
    virtual void Respawn();

    virtual void PauseSounds();
    virtual void ResumeSounds();

    void AddLife();

    /**
     * @brief Accelerates the character in the direction it is facing (accelDir).
     *
     * @param acceleration The acceleration to apply.
     */
    virtual void Accelerate(float acceleration);

    /**
     * @brief Gets the source rectangle of the character's texture based on it's current state.
     * This is used to get the correct frame in a sprite sheet.
     * 
     * @return The source rectangle of the character's texture.
     */
    virtual Rectangle GetFrameRec();

    bool IsTurning() { return state & (TURNING_LEFT | TURNING_RIGHT); }
    bool IsDying() { return state & DYING; }
    bool IsDead() { return state & DEAD; }
    bool IsAlive() { return state & ~(DEAD | DYING); }
    bool CanShoot() { return GetTime() - lastShotTime > shootCooldown; }
    int GetLives() { return lives; }

    /**
     * @brief Get a pointer to the vector of bullets.
     * This is used to handle collisions in the game loop.
     * 
     * @return A pointer to the vector of bullets.
     */
    std::vector<Bullet> *GetBullets() { return &bullets; }
};
#endif // __CHARACTER_H__