#ifndef __STALKER_H__
#define __STALKER_H__

#include "game/objects/enemy.hpp"

class Stalker : public Enemy
{
private:
protected:
    void SetDefaultHitBox();

public:
    Stalker(Player *player, EnemyAttributes attributes);
    ~Stalker();

    void Update();
    void DrawDebug();
    
    Rectangle GetFrameRec();
};

#endif // __STALKER_H__