#ifndef __SHOOTER_H__
#define __SHOOTER_H__

#include <vector>

#include "game/objects/enemy.hpp"
#include "game/objects/player.hpp"
#include "utils/utils.hpp"

#define SHOOTER_ACCELERATE_MIN_TIME 1.0f // seconds
#define SHOOTER_ACCELERATE_MAX_TIME 2.5f // seconds
#define SHOOTER_ROTATE_MIN_TIME 0.5f     // seconds
#define SHOOTER_ROTATE_MAX_TIME 1.5f     // seconds

class Shooter : public Enemy
{
private:
    bool lookingForPlayer;

    float probOfShootingAtPlayer;
    float lastTryToShootTime;

    float accelerateStartTime;
    float accelerateTime;

    float rotateStartTime;
    float rotateTime;

protected:
    void SetDefaultHitBox();

public:
    Shooter(Player *player, EnemyAttributes attributes);
    ~Shooter();

    void Update();
    void DrawDebug();
    Rectangle GetFrameRec();

    void ShootAtPlayer();
    void TryToShootAtPlayer();

    bool IsLookingForPlayer() { return lookingForPlayer; }
};
#endif // __SHOOTER_H__