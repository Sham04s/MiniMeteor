#include "game/objects/game_object.hpp"
#include "utils/utils.hpp"

#include <math.h>

GameObject::GameObject(Rectangle bounds, float rotation, Vector2 forwardDir, std::vector<Vector2> hitbox, GameObjectType type)
{
    this->bounds = bounds;
    this->origin = {bounds.x + bounds.width / 2, bounds.y + bounds.height / 2};
    this->rotation = rotation;
    this->forwardDir = forwardDir;
    this->hitbox = hitbox;
    this->previousVelocity = {0, 0};
    this->velocity = {0, 0};
    this->previousAngularVelocity = 0;
    this->angularVelocity = 0;
    this->type = type;
    this->texture = ResourceManager::GetInvalidTexture();
}

GameObject::~GameObject()
{
}

void GameObject::Update() // for overriding
{
    this->previousVelocity = this->velocity;
    this->previousAngularVelocity = this->angularVelocity;
    Translate(Vector2Scale(velocity, GetFrameTime()));
    Rotate(angularVelocity * GetFrameTime());
}

void GameObject::Draw()
{
    Rectangle dst = {origin.x, origin.y, bounds.width, bounds.height};
    DrawTexturePro(*texture, {0, 0, (float)texture->width, (float)texture->height}, dst, {bounds.width / 2, bounds.height / 2}, rotation, WHITE);
}

void GameObject::DrawDebug()
{
    // draw bounding box
    DrawRectangleLinesEx(bounds, 1, RED);

    size_t hitboxSize = hitbox.size();
    if (hitbox.size() > 1)
    {
        hitboxSize--;
    }

    // draw hitbox
    for (size_t i = 0; i < hitboxSize; i++)
    {
        Vector2 point = {hitbox[i].x, hitbox[i].y};
        Vector2 nextPoint = {hitbox[(i + 1) % hitboxSize].x, hitbox[(i + 1) % hitboxSize].y};
        DrawCircleV(point, 2, GREEN);
        DrawLineV(point, nextPoint, GREEN);
    }

    // draw forward direction
    Vector2 forwardEndPoint = {origin.x + forwardDir.x * 50, origin.y + forwardDir.y * 50};
    DrawLineEx(origin, forwardEndPoint, 2, BLUE);

    // draw velocity
    DrawLineEx(origin, Vector2Add(origin, Vector2Scale(velocity, 0.25f)), 2, GREEN);

    // Vector2 textPos = {bounds.x, bounds.y + bounds.height + 5};
    // DrawTextEx(*ResourceManager::GetFont(), TextFormat("pos: (%.2f, %.2f)", origin.x, origin.y), textPos, 16, 1, WHITE);
}

void GameObject::HandleCollision(GameObject *other, Vector2 *pushVector)
{
    // base class just pushes the objects apart
    Push(other, *pushVector);
}

void GameObject::PauseSounds()
{
    // base class does not have any sounds
}

void GameObject::ResumeSounds()
{
    // base class does not have any sounds
}

bool GameObject::CheckCollision(GameObject *other, Vector2 *pushVector)
{
    *pushVector = {0};

    // if either object has no hitbox then they are not colliding
    if (hitbox.size() == 0 || other->GetHitbox().size() == 0)
    {
        return false;
    }

    // if both objects have a single point hitbox then just check if the points are colliding
    if (hitbox.size() == 1 && other->GetHitbox().size() == 1)
    {
        return CheckCollisionCircles(hitbox[0], 1, other->GetHitbox()[0], 1);
    }

    // if the hitbox is a single point then just check if the point is inside the other hitbox
    if (hitbox.size() == 1)
    {
        return CheckCollisionPointPoly(hitbox[0], other->GetHitbox().data(), other->GetHitbox().size());
    }

    // same as above but for the other object
    if (other->GetHitbox().size() == 1)
    {
        return CheckCollisionPointPoly(other->GetHitbox()[0], hitbox.data(), hitbox.size());
    }

    // objects are not colliding if their bounding boxes are not colliding
    if (this->bounds.x + this->bounds.width < other->GetBounds().x || this->bounds.x > other->GetBounds().x + other->GetBounds().width || this->bounds.y + this->bounds.height < other->GetBounds().y || this->bounds.y > other->GetBounds().y + other->GetBounds().height)
    {
        return false;
    }

    // SAT adapted from https://dyn4j.org/2010/01/sat/#sat-mtv
    float overlap = INFINITY;
    float o = overlap;
    Vector2 smallest = {0, 0};
    std::vector<Vector2> axes1 = getAxes(hitbox);
    std::vector<Vector2> axes2 = getAxes(other->GetHitbox());
    // loop over the axes1
    for (size_t i = 0; i < axes1.size(); i++)
    {
        Vector2 axis = axes1[i];
        // project both shapes onto the axis
        Vector2 p1 = Project(axis, hitbox);
        Vector2 p2 = Project(axis, other->GetHitbox());
        // do the projections overlap?
        if (!Overlaps(p1, p2, &o))
        {
            // then we can guarantee that the shapes do not overlap
            return false;
        }
        else
        {
            // get the overlap
            Overlaps(p1, p2, &o);
            // check for minimum
            if (o < overlap)
            {
                // then set this one as the smallest
                overlap = o;
                smallest = axis;
            }
        }
    }
    // loop over the axes2
    for (size_t i = 0; i < axes2.size(); i++)
    {
        Vector2 axis = axes2[i];
        // project both shapes onto the axis
        Vector2 p1 = Project(axis, hitbox);
        Vector2 p2 = Project(axis, other->GetHitbox());
        // do the projections overlap?
        if (!Overlaps(p1, p2, &o))
        {
            // then we can guarantee that the shapes do not overlap
            return false;
        }
        else
        {
            // get the overlap
            Overlaps(p1, p2, &o);
            // check for minimum
            if (o < overlap)
            {
                // then set this one as the smallest
                overlap = o;
                smallest = axis;
            }
        }
    }
    // minimum translation vector
    Vector2 mtv = Vector2Scale(smallest, overlap);

    Vector2 c2c1 = Vector2Subtract(other->GetOrigin(), this->origin);
    // check if the normal is in the direction of the center to center vector
    if (Vector2DotProduct(c2c1, mtv) < 0)
    {
        // if its in the opposite direction then flip it
        mtv = Vector2Negate(mtv);
    }
    *pushVector = mtv;
    return true;
}

void GameObject::Push(GameObject *other, Vector2 pushVector)
{
    // constant for reducing velocity after collision
    static const float e = 0.85f;
    static const float nullVelocityThreshold = 1e-9f;

    this->previousVelocity = this->velocity;

    // if both objects are not moving then just push the other object with a velocity of 100
    if (Vector2Length(this->previousVelocity) < nullVelocityThreshold && Vector2Length(other->previousVelocity) < nullVelocityThreshold)
    {
        other->velocity = Vector2Scale(Vector2Normalize(pushVector), e * 100.0f);
    }
    else if (Vector2Length(this->previousVelocity) < nullVelocityThreshold)
    {
        other->velocity = Vector2Scale(Vector2Normalize(pushVector), e * Vector2Length(other->previousVelocity));
    }
    else
    {
        other->velocity = Vector2Scale(Vector2Normalize(pushVector), e * Vector2Length(this->previousVelocity));
    }

    other->previousAngularVelocity = other->angularVelocity;
    other->angularVelocity = e * this->previousAngularVelocity;

    // translate both objects with their new velocities so they don't get stuck
    other->Translate(Vector2Scale(other->GetVelocity(), GetFrameTime()));
    this->Translate(Vector2Scale(this->GetVelocity(), GetFrameTime()));
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
