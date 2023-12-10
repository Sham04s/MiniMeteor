#include "game_object.hpp"

GameObject::GameObject(Rectangle bounds, float rotation, Vector2 forwardDir, std::vector<Vector2> hitbox, GameObjectType type)
{
    this->bounds = bounds;
    this->origin = {bounds.x + bounds.width / 2, bounds.y + bounds.height / 2};
    this->rotation = rotation;
    this->forwardDir = forwardDir;
    this->hitbox = hitbox;
    this->velocity = {0, 0};
    this->angularVelocity = 0;
    this->mass = 1;
    this->type = type;
    this->texture = {0};
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
    DrawLineEx(origin, Vector2Add(origin, Vector2Scale(velocity, 0.25f)), 2, GREEN);
}

bool GameObject::CheckCollision(GameObject *other, Vector2 *collisionPoint)
{
    bool collision = false;

    std::vector<Vector2> futureHitbox;
    for (size_t i = 0; i < hitbox.size(); i++)
    {
        futureHitbox.push_back(Vector2Add(hitbox[i], Vector2Scale(velocity, GetFrameTime())));
    }

    if (futureHitbox.size() < 2)
    {
        collision = CheckCollisionPointPoly({futureHitbox[0].x, futureHitbox[0].y}, other->GetHitbox().data(), other->GetHitbox().size());
        if (collision)
        {
            *collisionPoint = {futureHitbox[0].x, futureHitbox[0].y};
        }
    }
    else
    {
        for (size_t i = 0; i < futureHitbox.size() && !collision; i++)
        {
            Vector2 point = {futureHitbox[i].x, futureHitbox[i].y};
            // TODO: only check on spawn or prevent asteroid to spawn on top of the player
            // if (CheckCollisionPointPoly(point, other->hitbox.data(), other->hitbox.size()))
            // {
            //     collision = true;
            //     *collisionPoint = point;
            //     break;
            // }

            Vector2 nextPoint = {futureHitbox[(i + 1) % futureHitbox.size()].x, futureHitbox[(i + 1) % futureHitbox.size()].y};
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
    }
    return collision;
}

void GameObject::ApplyPhysics(GameObject *other, Vector2 collisionPoint)
{
    // apply collision response to both objects
    float u = Vector2Length(Vector2Subtract(this->GetVelocity(), other->GetVelocity()));                  // relative velocity
    float e = 0.2f;                                                                                  // coefficient of restitution
    float angle = Vector2Angle(Vector2Subtract(other->GetOrigin(), collisionPoint), this->velocity); // angle between other and player
    float q = (other->mass / this->mass);                                                                             // mass ratio

    float f = (1 + e) / (2 * q + 1 + pow(sin(angle), 2));

    float va = f * u;               // velocity of other after collision
    float vp = (1 - 2 * q * f) * u; // velocity of player after collision

    float w = (f * sin(angle) * u) / Vector2Length(Vector2Subtract(other->GetOrigin(), collisionPoint)); // angular velocity of other after collision

    other->SetVelocity(Vector2Scale(Vector2Normalize(Vector2Subtract(other->GetOrigin(), collisionPoint)), va));
    other->SetAngularVelocity(w * RAD2DEG);

    this->velocity = Vector2Scale(Vector2Normalize(Vector2Subtract(this->origin, collisionPoint)), vp);

    this->Translate(Vector2Scale(this->velocity, GetFrameTime()));
    other->Translate(Vector2Scale(other->GetVelocity(), GetFrameTime()));
}

// void GameObject::ResetCollisionChecks()
// {
//     this->wasColliding = this->isColliding;
//     this->enteredCollision = false;
//     // this->lastCollisionPoint = {0, 0};
//     // this->lastCollisionObject = nullptr;
// }

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

void GameObject::Scale(float scale)
{
    // scale relative to origin
    this->bounds.width *= scale;
    this->bounds.height *= scale;
    this->bounds.x = this->origin.x - this->bounds.width / 2;
    this->bounds.y = this->origin.y - this->bounds.height / 2;
    for (size_t i = 0; i < this->hitbox.size(); i++)
    {
        Vector2 vertex = Vector2Subtract(this->hitbox[i], GetOrigin());
        vertex = Vector2Scale(vertex, scale);
        this->hitbox[i] = Vector2Add(vertex, GetOrigin());
    }
}