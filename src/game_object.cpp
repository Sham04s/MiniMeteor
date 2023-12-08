#include "game_object.hpp"
#include "raymath.h"

GameObject::GameObject(Rectangle bounds, float rotation, Vector2 forwardDir, int zIndex, std::vector<Vector2> hitbox, GameObjectType type)
{
    this->bounds = bounds;
    this->rotation = rotation;
    this->center = {bounds.x + bounds.width / 2, bounds.y + bounds.height / 2};
    this->forwardDir = forwardDir;
    this->zIndex = zIndex;
    this->hitbox = hitbox;
    this->type = type;
    if (type != NONE)
    {
        this->texture = {0, 0, 0, 0, 0};
    }
}

GameObject::~GameObject()
{
}

void GameObject::Update() // for overriding
{
}

void GameObject::Draw()
{
    Rectangle dst = {center.x, center.y, bounds.width, bounds.height};
    DrawTexturePro(texture, {0, 0, (float)texture.width, (float)texture.height}, dst, {bounds.width / 2, bounds.height / 2}, rotation, WHITE);
}

void GameObject::DrawDebug()
{
    DrawRectangleLinesEx(bounds, 1, RED);
    for (size_t i = 0; i < hitbox.size(); i++)
    {
        Vector2 point = {hitbox[i].x, hitbox[i].y};
        Vector2 nextPoint = {hitbox[(i + 1) % hitbox.size()].x, hitbox[(i + 1) % hitbox.size()].y};
        DrawCircleV(point, 2, GREEN);
        DrawLineV(point, nextPoint, GREEN);
    }
    Vector2 forwardEndPoint = {center.x + forwardDir.x * 50, center.y + forwardDir.y * 50};
    DrawLineEx(center, forwardEndPoint, 2, BLUE);
}

bool GameObject::CheckCollision(GameObject *other, Vector2 *collisionPoint)
{
    bool collision = false;
    for (size_t i = 0; i < hitbox.size() && !collision; i++)
    {
        Vector2 point = {hitbox[i].x, hitbox[i].y};
        // TODO: only check on spawn or prevent asteroid to spawn on top of the player
        // if (CheckCollisionPointPoly(point, other->hitbox.data(), other->hitbox.size()))
        // {
        //     collision = true;
        //     *collisionPoint = point;
        //     break;
        // }

        Vector2 nextPoint = {hitbox[(i + 1) % hitbox.size()].x, hitbox[(i + 1) % hitbox.size()].y};
        for (size_t j = 0; j < other->GetHitbox().size() && !collision; j++)
        {
            Vector2 otherPoint = {other->GetHitbox()[j].x, other->GetHitbox()[j].y};
            Vector2 otherNextPoint = {other->GetHitbox()[(j + 1) % other->GetHitbox().size()].x, other->GetHitbox()[(j + 1) % other->GetHitbox().size()].y};
            if (CheckCollisionLines(point, nextPoint, otherPoint, otherNextPoint, collisionPoint))
            {
                collision = true;
                break;
            }
        }
    }
    return collision;
}

void GameObject::Translate(Vector2 translation)
{
    this->center = Vector2Add(this->center, translation);
    this->bounds.x = this->center.x - this->bounds.width / 2;
    this->bounds.y = this->center.y - this->bounds.height / 2;
    for (size_t i = 0; i < this->hitbox.size(); i++)
    {
        this->hitbox[i] = Vector2Add(this->hitbox[i], translation);
    }
}

void GameObject::Rotate(float angle) // in degrees
{
    ;
    this->rotation = fmod((GetRotation() + angle), 360);
    SetForwardDir(Vector2Rotate(GetForwardDir(), angle * DEG2RAD));
    Vector2 vertex;
    for (size_t i = 0; i < this->hitbox.size(); i++)
    {
        vertex = Vector2Subtract(this->hitbox[i], GetCenter());
        vertex = Vector2Rotate(vertex, angle * DEG2RAD);
        this->hitbox[i] = Vector2Add(vertex, GetCenter());
    }
}