#ifndef __PULSER_H__
#define __PULSER_H__

#include "game/objects/enemy.hpp"

#define PULSER_SHOOT_COOLDOWN 6.0f // seconds
#define PULSER_CHANGE_DIR_COOLDOWN 5.0f // seconds
#define PULSER_CHANGE_DIR_PROB 0.5f

class Pulser : public Enemy
{
private:
    Vector2 newAccelDir;
    float lastChangeDirTime;
    
protected:
    void SetDefaultHitBox();

public:
    Pulser(Player *player, EnemyAttributes attributes);
    ~Pulser();

    void Update();
    void DrawDebug();

    void ChangeDir();
    void Shoot();

    Rectangle GetFrameRec();
};

#endif // __PULSER_H__