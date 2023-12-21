#ifndef __POWER_UP_H__
#define __POWER_UP_H__

#include "game_object.hpp"

#define POWER_UP_SIZE 40.0f
#define POWER_UP_TIME_TO_LIVE 10.0f                      // seconds
#define POWER_UP_BLINK_TIME 3.0f                         // seconds
#define POWER_UP_BLINK_PERIOD POWER_UP_BLINK_TIME / 5.0f // seconds

#define TEMPORARY_SHIELD_TIME 5.0f // seconds
#define TEMPORARY_BOOST_TIME 5.0f  // seconds
#define FIRE_RATE_UPGRADE_MULTIPLIER 2.0f

enum PowerUpType
{
    SHIELD,
    TEMPORARY_SHIELD,
    TEMPORARY_INFINITE_BOOST,
    FIRE_RATE_UPGRADE,
    LIFE,
    NUM_POWER_UP_TYPES
};

class PowerUp : public GameObject
{
private:
    PowerUpType powerupType;
    bool pickedUp;
    float timeToLive;
    float effectiveUseTime;

public:
    PowerUp() : PowerUp({0, 0}, SHIELD) {}
    PowerUp(Vector2 origin, PowerUpType type);
    virtual ~PowerUp();

    virtual void Update();
    virtual void Draw();
    virtual void DrawDebug();
    virtual void HandleCollision(GameObject *other, Vector2 *pushVector);

    void PickUp();
    void UpdateBounds(Rectangle playerBounds);
    void ResetUseTime();

    PowerUpType GetType() { return powerupType; }
    float GetTimeToLive() { return timeToLive; }
    float GetEffectiveUseTime() { return effectiveUseTime; }
    bool IsPickedUp() { return pickedUp; }
    bool IsExpired() { return timeToLive <= 0.0f && !pickedUp; }
    bool CanBeApplied() { return effectiveUseTime > 0.0f; }
};

#endif // __POWER_UP_H__