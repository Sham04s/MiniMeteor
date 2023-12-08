#include "game_object.hpp"

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
        Vector2 point = {center.x + hitbox[i].x, center.y + hitbox[i].y};
        Vector2 nextPoint = {center.x + hitbox[(i + 1) % hitbox.size()].x, center.y + hitbox[(i + 1) % hitbox.size()].y};
        DrawCircleV(point, 2, GREEN);
        DrawLineV(point, nextPoint, GREEN);
    }
    Vector2 forwardEndPoint = {center.x + forwardDir.x, center.y + forwardDir.y};
    DrawLineEx(center, forwardEndPoint, 2, BLUE);
}
