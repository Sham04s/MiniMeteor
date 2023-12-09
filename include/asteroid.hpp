#ifndef __ASTEROID_H__
#define __ASTEROID_H__

#include "game_object.hpp"

#define ASTEROID_SIZE_SMALL 32
#define ASTEROID_SIZE_MEDIUM 64
#define ASTEROID_SIZE_LARGE 96
#define ASTEROID_EXPLOSION_TIME 0.2f

enum AsteroidVariant
{
    SMALL,
    MEDIUM,
    LARGE
};

enum AsteroidState
{
    FLOATING,
    EXPLODING,
    DESTROYED
};

class Asteroid : public GameObject
{
private:
    Vector2 velocity;
    float angularVelocity;
    AsteroidVariant variant;
    AsteroidState state;
    float lastExplosionTime;
    
public:
    Asteroid(Vector2 origin, int zIndex);
    ~Asteroid();

    void Update();
    void Draw();
    void DrawDebug();
    void Destroy();

    void SetVelocity(Vector2 velocity);
    void SetRotation(float rotation);
    void SetAngularVelocity(float angularVelocity);

    Vector2 GetVelocity();
    float GetRotation();
    float GetAngularVelocity();

    bool isDestroyed() { return state == DESTROYED; }
};


#endif // __ASTEROID_H__