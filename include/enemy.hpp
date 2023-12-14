#ifndef __ENEMY_H__
#define __ENEMY_H__

#include <vector>

#include "character.hpp"
#include "player.hpp"

#define ENEMY_ACCELERATE_TIME 1.5f // seconds
#define ENEMY_TRY_TO_SHOOT_COOLDOWN 2.0f      // seconds

enum EnemyState
{
    ENEMY_MOVING,
    ENEMY_LOOKING_FOR_PLAYER,
    // ENEMY_SHOOTING,
    ENEMY_DYING,
    ENEMY_DEAD
};

class BasicEnemy : public Character
{
private:
    bool lookingForPlayer;
    float lastTryToShootTime;
    float accelerateStartTime;
    Player *player;

protected:
    void SetDefaultHitBox();

public:
    BasicEnemy(Vector2 origin);
    ~BasicEnemy();

    void Update();
    void DrawDebug();

    void ShootAtPlayer(Player &player);
    void TryToShootAtPlayer(Player &player);

    bool IsLookingForPlayer() { return lookingForPlayer; }
    bool IsLookingAtPlayer(Player &player);
    bool IsLookingAt(Vector2 position);
};
#endif // __ENEMY_H__