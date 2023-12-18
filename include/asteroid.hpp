#ifndef __ASTEROID_H__
#define __ASTEROID_H__

#include "game_object.hpp"

#define ASTEROID_SIZE_SMALL 64
#define ASTEROID_SIZE_LARGE 96
#define ASTEROID_EXPLOSION_TIME 0.2f

enum AsteroidVariant
{
    SMALL,
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
    AsteroidVariant variant;
    AsteroidState state;
    Sound explosionSound;
    float size;
    float lastExplosionTime;
    
public:
    Asteroid(Vector2 origin);
    ~Asteroid();

    void Update();
    void Draw();
    void DrawDebug();
    void Destroy();

    void SetVelocity(Vector2 velocity) { this->velocity = velocity; }
    void SetRotation(float rotation) { this->rotation = rotation; }
    void SetAngularVelocity(float angularVelocity) { this->angularVelocity = angularVelocity; }

    Vector2 GetVelocity() { return velocity; }
    float GetRotation() { return rotation; }
    float GetAngularVelocity() { return angularVelocity; }

    bool IsFloating() { return state == FLOATING; }
    bool IsExploding() { return state == EXPLODING; }
    bool IsDestroyed() { return state == DESTROYED; }
};


#endif // __ASTEROID_H__