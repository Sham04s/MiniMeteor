#ifndef __BULLET_H__
#define __BULLET_H__

#include "game/objects/game_object.hpp"

#define BULLET_SIZE 40
#define BULLET_SPEED 250
#define BULLET_SPREAD 10.0f // degrees

class Bullet : public GameObject
{
private:
    bool isPlayerBullet; /** Whether the bullet is a player bullet or not */
    bool isAlive;        /** Whether the bullet is still alive or not */

public:
    /**
     * @brief Construct a new Bullet object with the given origin, forward direction, speed, and whether it is a player bullet or not.
     *
     * @param origin Where to spawn the bullet.
     * @param forwardDir The forward direction of the bullet.
     * @param speed The speed of the bullet.
     * @param isPlayerBullet Whether the bullet is a player bullet or not.
     */
    Bullet(Vector2 origin, Vector2 forwardDir, float speed, bool isPlayerBullet);
    ~Bullet();

    void Update();
    void Draw();
    void DrawDebug();
    void HandleCollision(GameObject *other, Vector2 *pushVector);

    /**
     * @brief Destroys the bullet. (the bullet will be removed in the next update)
     */
    void Destroy() { isAlive = false; }
    bool IsOutOfBounds();
    bool IsPlayerBullet() { return isPlayerBullet; }
    bool IsAlive() { return isAlive; }
};
#endif // __BULLET_H__