#ifndef __ENEMY_H__
#define __ENEMY_H__

#include <vector>

#include "character.hpp"
#include "player.hpp"

#define ENEMY_ACCELERATE_MIN_TIME 1.0f // seconds
#define ENEMY_ACCELERATE_MAX_TIME 2.5f // seconds
#define ENEMY_ROTATE_MIN_TIME 0.5f     // seconds
#define ENEMY_ROTATE_MAX_TIME 1.5f     // seconds
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
    float accelerateTime;
    float rotateStartTime;
    float rotateTime;
    Player *player;

protected:
    void SetDefaultHitBox();
    
public:
    BasicEnemy(Vector2 origin);
    ~BasicEnemy();

    void Update();
    void DrawDebug();
    Rectangle GetFrameRec();

    void ShootAtPlayer(Player &player);
    void TryToShootAtPlayer(Player &player);

    bool IsLookingForPlayer() { return lookingForPlayer; }
    bool IsLookingAtPlayer(Player &player);
    bool IsLookingAt(Vector2 position);
};
#endif // __ENEMY_H__