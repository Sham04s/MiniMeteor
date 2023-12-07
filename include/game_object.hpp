#ifndef __GAME_OBJECT_H__
#define __GAME_OBJECT_H__

#include "raylib.h"
#include <vector>

#include "resource_manager.hpp"

enum GameObjectType
{
    PLAYER = PLAYER_SPRITE,
    ENEMY = ENEMY_SPRITE,
    BULLET = BULLET_SPRITE,
    ASTEROID = ASTEROID_SPRITE,
    EXPLOSION = EXPLOSION_SPRITE,
    NUM_GAME_OBJECT_TYPES = NUM_SPRITE_TEXTURES,
    NONE
};

class GameObject
{
protected:
    Rectangle bounds;
    Vector2 center;
    float rotation; // TODO: maybe this is not needed
    Vector2 forwardDir;
    int zIndex;
    std::vector<Vector2> hitbox; // relative to center and to size
    GameObjectType type;
    Texture2D texture;

public:
    GameObject(): bounds({0, 0, 0, 0}), center({0, 0}), rotation(0), forwardDir({0, 0}), zIndex(0), hitbox({}), type(NONE){}; // TODO: change this!
    GameObject(Rectangle bounds, float rotation, Vector2 forwardDir, int zIndex, std::vector<Vector2> hitbox, GameObjectType type);
    ~GameObject();

    void Update();
    void Draw();
    void DrawDebug();

    Rectangle GetBounds() { return bounds; }
    Vector2 GetCenter() { return center; }
    float GetRotation() { return rotation; }
    Vector2 GetForwardDir() { return forwardDir; }
    int GetZIndex() { return zIndex; }
    std::vector<Vector2> GetHitbox() { return hitbox; }
    GameObjectType GetType() { return type; }
    Texture2D GetTexture() { return texture; }

    void SetBounds(Rectangle bounds) { this->bounds = bounds; }
    void SetCenter(Vector2 center) { this->center = center; }
    void SetRotation(float rotation) { this->rotation = rotation; }
    void SetForwardDir(Vector2 forwardDir) { this->forwardDir = forwardDir; }
    void SetZIndex(int zIndex) { this->zIndex = zIndex; }
    void SetHitbox(std::vector<Vector2> hitbox) { this->hitbox = hitbox; }
    void SetType(GameObjectType type) { this->type = type; }
    void SetTexture(Texture2D texture) { this->texture = texture; }
    
};

#endif // __GAME_OBJECT_H__