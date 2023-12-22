#ifndef __BULLET_H__
#define __BULLET_H__

#include "game_object.hpp"

#define BULLET_SIZE 40
#define BULLET_SPEED 250
#define BULLET_SPREAD 10.0f // degrees

class Bullet : public GameObject
{
private:
    bool isPlayerBullet;
    bool isAlive;

public:
    Bullet(Vector2 origin, Vector2 forwardDir, float speed, bool isPlayerBullet);
    ~Bullet();

    void Update();
    void Draw();
    void DrawDebug();
    void HandleCollision(GameObject *other, Vector2 *pushVector);
    void Destroy() { isAlive = false; }

    bool IsOutOfBounds();
    bool IsPlayerBullet() { return isPlayerBullet; }
    bool IsAlive() { return isAlive; }
};
#endif // __BULLET_H__