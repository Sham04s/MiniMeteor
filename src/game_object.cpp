#include "game_object.hpp"

GameObject::GameObject(Rectangle bounds, float rotation, Vector2 forwardDir, int zIndex, std::vector<Vector2> hitbox, GameObjectType type)
{
    this->bounds = bounds;
    this->rotation = rotation;
    this->origin = {bounds.x + bounds.width / 2, bounds.y + bounds.height / 2};
    this->forwardDir = forwardDir;
    this->zIndex = zIndex;
    this->hitbox = hitbox;
    this->type = type;

    this->wasColliding = false;
    this->firstCollision = false;
    this->lastCollisionPoint = {0, 0};
    this->lastCollisionObject = nullptr;

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
    Rectangle dst = {origin.x, origin.y, bounds.width, bounds.height};
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
    Vector2 forwardEndPoint = {origin.x + forwardDir.x * 50, origin.y + forwardDir.y * 50};
    DrawLineEx(origin, forwardEndPoint, 2, BLUE);
}

bool GameObject::CheckCollision(GameObject *other)
{
    bool collision = false;

    if (hitbox.size() < 2)
    {
        collision = CheckCollisionPointPoly({hitbox[0].x, hitbox[0].y}, other->GetHitbox().data(), other->GetHitbox().size());
        if (collision)
        {
            this->lastCollisionPoint = {hitbox[0].x, hitbox[0].y};
            this->lastCollisionObject = other;
        }
    }
    else
    {

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
                if (CheckCollisionLines(point, nextPoint, otherPoint, otherNextPoint, &this->lastCollisionPoint))
                {
                    this->lastCollisionObject = other;
                    collision = true;
                    break;
                }
            }
        }
    }
    if (!collision)
    {
        this->lastCollisionPoint = {0, 0};
        this->lastCollisionObject = nullptr;
    }
    if (collision && !wasColliding)
    {
        firstCollision = true;
    }
    else
    {
        firstCollision = false;
    }
    this->wasColliding = collision;
    return collision;
}

void GameObject::ResetCollisionChecks()
{
    this->wasColliding = false;
    this->firstCollision = false;
    this->lastCollisionPoint = {0, 0};
    this->lastCollisionObject = nullptr;
}

void GameObject::Translate(Vector2 translation)
{
    this->origin = Vector2Add(this->origin, translation);
    this->bounds.x = this->origin.x - this->bounds.width / 2;
    this->bounds.y = this->origin.y - this->bounds.height / 2;
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
        vertex = Vector2Subtract(this->hitbox[i], GetOrigin());
        vertex = Vector2Rotate(vertex, angle * DEG2RAD);
        this->hitbox[i] = Vector2Add(vertex, GetOrigin());
    }
}