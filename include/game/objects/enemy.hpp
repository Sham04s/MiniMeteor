#ifndef __ENEMY_H__
#define __ENEMY_H__

#include "game/objects/character.hpp"
#include "game/objects/player.hpp"
#include "utils/utils.hpp"

#define ENEMY_SHOOT_COOLDOWN CHARACTER_SHOOT_COOLDOWN * 2.5f // seconds

typedef struct EnemyAttributes
{
    float velocityMultiplier;
    float fireRateMultiplier;
    float bulletSpeedMultiplier;
    float precision;
    float probOfShootingAtPlayer; // only for shooters
    float bulletsPerShot; // only for pulsers
} EnemySettings;

class Enemy : public Character
{
protected:
    Player *player;
    float precision;
    virtual void SetDefaultHitBox();

public:
    Enemy(Player *player, EnemyAttributes attributes)
        : Enemy(RandomVecOutsideScreen(CHARACTER_SIZE), player, attributes){};
    Enemy(Vector2 origin, Player *player, EnemyAttributes attributes);
    ~Enemy();

    virtual void Update();
    virtual void DrawDebug();

    virtual void HandleCollision(GameObject *other, Vector2 *pushVector);
    virtual Rectangle GetFrameRec();

    bool IsLookingAtPlayer();
    bool IsLookingAt(Vector2 position);
};

#endif // __ENEMY_H__