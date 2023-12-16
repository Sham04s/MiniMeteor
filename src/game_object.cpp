#include "game_object.hpp"

#include <math.h>

GameObject::GameObject(Rectangle bounds, float rotation, Vector2 forwardDir, std::vector<Vector2> hitbox, GameObjectType type)
{
    this->bounds = bounds;
    this->origin = {bounds.x + bounds.width / 2, bounds.y + bounds.height / 2};
    this->rotation = rotation;
    this->forwardDir = forwardDir;
    this->hitbox = hitbox;
    this->velocity = {0, 0};
    this->angularVelocity = 0;
    this->type = type;
    this->texture = nullptr;
}

GameObject::~GameObject()
{
}

void GameObject::Update() // for overriding
{
}

void GameObject::Draw()
{
    if (texture == nullptr)
    {
        Rectangle dst = {bounds.x, bounds.y, bounds.width / 2, bounds.height / 2};
        DrawRectanglePro(dst, {dst.width / 2, dst.height / 2}, rotation, WHITE);
        DrawRectanglePro({dst.x + dst.width / 2, dst.y + dst.height / 2, dst.width, dst.height}, {dst.width / 2, dst.height / 2}, rotation, WHITE);
        dst.y += dst.height;
        DrawRectanglePro(dst, {dst.width / 2, dst.height / 2}, rotation, PURPLE);
        DrawRectanglePro({dst.x + dst.width / 2, dst.y - dst.height / 2, dst.width, dst.height}, {dst.width / 2, dst.height / 2}, rotation, PURPLE);
        return;
    }
    Rectangle dst = {origin.x, origin.y, bounds.width, bounds.height};
    DrawTexturePro(*texture, {0, 0, (float)texture->width, (float)texture->height}, dst, {bounds.width / 2, bounds.height / 2}, rotation, WHITE);
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

void GameObject::PauseSounds()
{
    // base class does not have any sounds
}

void GameObject::ResumeSounds()
{
    // base class does not have any sounds
}

std::vector<Vector2> getAxes(std::vector<Vector2> hitbox)
{
    std::vector<Vector2> axes = {};

    for (size_t i = 0; i < hitbox.size(); i++)
    {
        // get the current vertex
        Vector2 p1 = hitbox[i];
        // get the next vertex
        Vector2 p2 = hitbox[i + 1 == hitbox.size() ? 0 : i + 1];
        // subtract the two to get the edge vector
        Vector2 edge = Vector2Subtract(p1, p2);
        // get either perpendicular vector
        Vector2 normal = Vector2Normalize({-edge.y, edge.x});
        // the perp method is just (x, y) =&gt; (-y, x) or (y, -x)
        axes.push_back(normal);
    }

    return axes;
}

Vector2 project(Vector2 axis, std::vector<Vector2> hitbox)
{
    float min = 0;
    float max = 0;

    for (size_t i = 0; i < hitbox.size(); i++)
    {
        float dotProduct = Vector2DotProduct(axis, hitbox[i]);
        if (i == 0 || dotProduct < min)
        {
            min = dotProduct;
        }
        if (i == 0 || dotProduct > max)
        {
            max = dotProduct;
        }
    }

    return {min, max};
}

float getOverlap(Vector2 a, Vector2 b, float *overlap)
{
    float minA = a.x;
    float maxA = a.y;
    float minB = b.x;
    float maxB = b.y;

    if (maxA >= minB && maxB >= minA)
    {
        *overlap = fmin(maxA, maxB) - fmax(minA, minB);
        return true;
    }
    else
    {
        return false;
    }
}

bool GameObject::CheckCollision(GameObject *other, Vector2 *pushVector)
{
    // if the hitbox is a single point then just check if the point is inside the other hitbox
    if (hitbox.size() == 1)
    {
        pushVector = {0};
        return CheckCollisionPointPoly(hitbox[0], other->GetHitbox().data(), other->GetHitbox().size());
    }

    // same as above but for the other object
    if (other->GetHitbox().size() == 1)
    {
        pushVector = {0};
        return CheckCollisionPointPoly(other->GetHitbox()[0], hitbox.data(), hitbox.size());
    }

    // object are not colliding if their bounding boxes are not colliding
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
        Vector2 p1 = project(axis, hitbox);
        Vector2 p2 = project(axis, other->GetHitbox());
        // do the projections overlap?
        if (!getOverlap(p1, p2, &o))
        {
            // then we can guarantee that the shapes do not overlap
            return false;
        }
        else
        {
            // get the overlap
            getOverlap(p1, p2, &o);
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
        Vector2 p1 = project(axis, hitbox);
        Vector2 p2 = project(axis, other->GetHitbox());
        // do the projections overlap?
        if (!getOverlap(p1, p2, &o))
        {
            // then we can guarantee that the shapes do not overlap
            return false;
        }
        else
        {
            // get the overlap
            getOverlap(p1, p2, &o);
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

    Vector2 c1c2 = Vector2Subtract(this->origin, other->GetOrigin());
    // check if the normal is in the direction of the center to center vector
    if (Vector2DotProduct(c1c2, mtv) < 0)
    {
        // if its in the opposite direction then flip it
        mtv = Vector2Negate(mtv);
    }
    *pushVector = mtv;
    return true;
}

void GameObject::Push(GameObject *other, Vector2 pushVector)
{
    float e = 0.85f;

    Vector2 thisVelocity = this->velocity;
    this->velocity = Vector2Scale(Vector2Normalize(pushVector), e * Vector2Length(other->GetVelocity()));
    other->SetVelocity(Vector2Scale(Vector2Normalize(pushVector), -e * Vector2Length(thisVelocity)));

    float thisAngularVelocity = this->angularVelocity;
    this->angularVelocity = e * other->GetAngularVelocity();
    other->SetAngularVelocity(-e * thisAngularVelocity);

    Translate(Vector2Scale(this->velocity, GetFrameTime()));
    other->Translate(Vector2Scale(other->GetVelocity(), GetFrameTime()));
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