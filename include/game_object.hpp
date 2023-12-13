#ifndef __GAME_OBJECT_H__
#define __GAME_OBJECT_H__

#include "raylib.h"

#include <vector>
#include "raymath.h"
#include <math.h>

#include "resource_manager.hpp"

enum GameObjectType
{
    PLAYER,
    ENEMY,
    BULLET,
    ASTEROID,
    EXPLOSION,
    NUM_GAME_OBJECT_TYPES,
    NONE
};

class GameObject
{
protected:
    Rectangle bounds;
    Vector2 origin;
    float rotation; // TODO: maybe this is not needed
    Vector2 forwardDir;
    std::vector<Vector2> hitbox;
    Vector2 velocity;
    float angularVelocity; // in degrees per second
    float mass;
    GameObjectType type;
    Texture2D *texture;
    

public:
    GameObject() : bounds({0, 0, 0, 0}), origin({0, 0}), rotation(0), forwardDir({0, 0}), hitbox({}), type(NONE){}; // TODO: change this!
    GameObject(Rectangle bounds, float rotation, Vector2 forwardDir, std::vector<Vector2> hitbox, GameObjectType type);
    ~GameObject();

    void Update();
    void Draw();
    void DrawDebug();

    bool CheckCollision(GameObject *other, Vector2 *collisionPoint);
    void Push(GameObject *other, Vector2 collisionPoint);
    void Translate(Vector2 translation);
    void Rotate(float angle);
    void Scale(float scale);

    Rectangle GetBounds() { return bounds; }
    Vector2 GetOrigin() { return origin; }
    float GetRotation() { return rotation; }
    Vector2 GetVelocity() { return velocity; }
    float GetAngularVelocity() { return angularVelocity; }
    float GetMass() { return mass; }
    Vector2 GetForwardDir() { return forwardDir; }
    std::vector<Vector2> GetHitbox() { return hitbox; }
    GameObjectType GetType() { return type; }

    void SetBounds(Rectangle bounds) { this->bounds = bounds; }
    void SetOrigin(Vector2 origin) { this->origin = origin; }
    void SetRotation(float rotation) { this->rotation = rotation; }
    void SetVelocity(Vector2 velocity) { this->velocity = velocity; }
    void SetAngularVelocity(float angularVelocity) { this->angularVelocity = angularVelocity; }
    void SetForwardDir(Vector2 forwardDir) { this->forwardDir = forwardDir; }
    void SetHitbox(std::vector<Vector2> hitbox) { this->hitbox = hitbox; }
};

#endif // __GAME_OBJECT_H__