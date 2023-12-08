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
    int zIndex;
    std::vector<Vector2> hitbox;
    GameObjectType type;
    Texture2D texture;
    bool wasColliding;
    bool firstCollision;
    Vector2 lastCollisionPoint;
    GameObject *lastCollisionObject;

public:
    GameObject() : bounds({0, 0, 0, 0}), origin({0, 0}), rotation(0), forwardDir({0, 0}), zIndex(0), hitbox({}), type(NONE){}; // TODO: change this!
    GameObject(Rectangle bounds, float rotation, Vector2 forwardDir, int zIndex, std::vector<Vector2> hitbox, GameObjectType type);
    ~GameObject();

    void Update();
    void Draw();
    void DrawDebug();

    bool CheckCollision(GameObject *other);
    void ResetCollisionChecks();
    void Translate(Vector2 translation);
    void Rotate(float angle);

    Rectangle GetBounds() { return bounds; }
    Vector2 GetOrigin() { return origin; }
    float GetRotation() { return rotation; }
    Vector2 GetForwardDir() { return forwardDir; }
    int GetZIndex() { return zIndex; }
    std::vector<Vector2> GetHitbox() { return hitbox; }
    GameObjectType GetType() { return type; }
    Texture2D GetTexture() { return texture; }
    Vector2 GetLastCollisionPoint() { return lastCollisionPoint; }
    GameObject *GetLastCollisionObject() { return lastCollisionObject; }
    bool IsFirstCollision() { return firstCollision; }

    void SetBounds(Rectangle bounds) { this->bounds = bounds; }
    void SetOrigin(Vector2 origin) { this->origin = origin; }
    void SetRotation(float rotation) { this->rotation = rotation; }
    void SetForwardDir(Vector2 forwardDir) { this->forwardDir = forwardDir; }
    void SetZIndex(int zIndex) { this->zIndex = zIndex; }
    void SetHitbox(std::vector<Vector2> hitbox) { this->hitbox = hitbox; }
    void SetType(GameObjectType type) { this->type = type; }
    void SetTexture(Texture2D texture) { this->texture = texture; }
};

#endif // __GAME_OBJECT_H__