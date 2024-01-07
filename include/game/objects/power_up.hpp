#ifndef __POWER_UP_H__
#define __POWER_UP_H__

#include "game/objects/game_object.hpp"
#include "utils/utils.hpp"

#define POWER_UP_SIZE 40.0f
#define POWER_UP_TIME_TO_LIVE 10.0f                      // seconds
#define POWER_UP_PULSE_PERIOD 1.0f                       // seconds
#define POWER_UP_BLINK_TIME 3.0f                         // seconds
#define POWER_UP_BLINK_PERIOD POWER_UP_BLINK_TIME / 5.0f // seconds
#define POWER_UP_SHAKE_TIME 0.5f                         // seconds

#define TEMPORARY_SHIELD_TIME 5.0f         // seconds
#define TEMPORARY_INFINITE_BOOST_TIME 5.0f // seconds

#define SHOOT_COOLDOWN_MULTIPLIER 0.79f
#define BULLETS_PER_SHOT_MULTIPLIER 1.2f
#define BULLETS_SPEED_MULTIPLIER 1.2f
#define BULLETS_SPREAD_MULTIPLIER 0.82f

enum PowerUpType
{
    SHIELD,
    TEMPORARY_SHIELD,
    TEMPORARY_INFINITE_BOOST,
    SHOOT_COOLDOWN_UPGRADE,
    BULLET_SPEED_UPGRADE,
    BULLET_SPREAD_UPGRADE,
    EXTRA_BULLET_UPGRADE,
    LIFE,
    NUM_POWER_UP_TYPES
};

class PowerUp : public GameObject
{
private:
    PowerUpType powerupType;
    bool pickedUp;
    bool drawable;

    // used when the powerup is not collected by the player
    bool shaking;
    float shakingTime;
    float lastShakeTime;

    /**
     * @brief Time to live of the powerup.
     * When this reaches 0, the powerup is destroyed.
     */
    float timeToLive;

    /**
     * @brief Time the powerup can be used by the player.
     */
    float effectiveUseTime;

    Sound *spawnSound;
    Sound *pickupSound;
    Sound *cantPickupSound;

public:
    /**
     * @brief Construct a new PowerUp object with a random type. The origin is randomly generated inside the screen.
     *
     */
    PowerUp() : PowerUp(RandomVecInsideScreen(POWER_UP_SIZE), (PowerUpType)GetRandomValue(0, NUM_POWER_UP_TYPES - 1)) {}

    /**
     * @brief Construct a new PowerUp object. The origin is randomly generated inside the screen.
     *
     */
    PowerUp(PowerUpType type) : PowerUp(RandomVecInsideScreen(POWER_UP_SIZE), type) {}

    /**
     * @brief Construct a new PowerUp object.
     *
     * @param origin The origin of the powerup.
     * @param type The type of the powerup.
     */
    PowerUp(Vector2 origin, PowerUpType type);
    virtual ~PowerUp();

    virtual void Update();
    virtual void Draw();
    virtual void DrawDebug();
    virtual void HandleCollision(GameObject *other, Vector2 *pushVector);
    virtual void PauseSounds();
    virtual void ResumeSounds();

    /**
     * @brief Picks up the powerup setting it's time to live to 0 and it's
     * effective use time according to the powerup type.
     */
    void PickUp();

    /**
     * @brief Shakes the powerup.
     */
    void Shake();

    /**
     * @brief Updates the bounds of the powerup according to the player's bounds.
     *
     * @param playerBounds The player's bounds.
     */
    void UpdateBounds(Rectangle playerBounds);

    /**
     * @brief Resets the effective use time of the powerup.
     */
    void ResetUseTime();

    PowerUpType GetType() { return powerupType; }
    float GetTimeToLive() { return timeToLive; }
    float GetEffectiveUseTime() { return effectiveUseTime; }
    bool IsPickedUp() { return pickedUp; }
    bool IsExpired() { return timeToLive <= 0.0f && !pickedUp; }
    bool CanBeApplied() { return effectiveUseTime > 0.0f; }

    /**
     * @brief Gets the name of the powerup.
     *
     * @param type The type of the powerup.
     * @return const char* The name of the powerup.
     */
    static const char *GetPowerUpName(PowerUpType type);
};

#endif // __POWER_UP_H__