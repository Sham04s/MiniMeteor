#include "asteroid.hpp"

#include "raymath.h"
#include <math.h>

Asteroid::Asteroid(Vector2 origin, int zIndex) : GameObject({origin.x - 32, origin.y - 32, 64, 64}, 0, {0, -1}, zIndex, {}, ASTEROID)
{
    this->velocity = {(float)GetRandomValue(-100, 100), (float)GetRandomValue(-100, 100)};
    this->rotation = GetRandomValue(0, 360);
    this->angularVelocity = GetRandomValue(-10, 10) * 12;

    SpriteTextureID randomAsteroid = (SpriteTextureID)GetRandomValue(1, 8);
    bool extraBig = GetRandomValue(0, 1);
    this->texture = ResourceManager::GetSpriteTexture(randomAsteroid);
    if (extraBig)
    {
        this->bounds = {origin.x - 48, origin.y - 48, 96, 96};
    }

    const int sides = 8;
    const float radius = randomAsteroid % 2 == 1 ? extraBig ? (32) : (24) : 18;
    const float angleIncrement = 2 * PI / sides;

    for (int i = 0; i < sides; i++)
    {
        float angle = i * angleIncrement;
        float x = radius * cos(angle);
        float y = radius * sin(angle);
        this->hitbox.push_back(Vector2Add(origin, {x, y}));
    }
}

Asteroid::~Asteroid()
{
}

void Asteroid::Update()
{
    if (origin.x > GetScreenWidth() + 32)
    {
        origin.x = -32;
    }
    if (origin.x < -32)
    {
        origin.x = GetScreenWidth();
    }
    if (origin.y > GetScreenHeight() + 32)
    {
        origin.y = -32;
    }
    if (origin.y < -32)
    {
        origin.y = GetScreenHeight();
    }
    Rotate(angularVelocity * GetFrameTime());
    Translate(Vector2Scale(velocity, GetFrameTime()));

    if (GetOrigin().x > GetScreenWidth() + 32)
    {
        Translate({(float)-GetScreenWidth() - 64, 0});
    }
    if (GetOrigin().x < -32)
    {
        Translate({(float)GetScreenWidth() + 64, 0});
    }
    if (GetOrigin().y > GetScreenHeight() + 32)
    {
        Translate({0, (float)-GetScreenHeight() - 64});
    }
    if (GetOrigin().y < -32)
    {
        Translate({0, (float)GetScreenHeight() + 64});
    }
}

void Asteroid::Draw()
{
    GameObject::Draw();
}

void Asteroid::DrawDebug()
{
    GameObject::DrawDebug();
}

void Asteroid::SetVelocity(Vector2 velocity)
{
    this->velocity = velocity;
}

void Asteroid::SetRotation(float rotation)
{
    this->rotation = rotation;
}

void Asteroid::SetAngularVelocity(float angularVelocity)
{
    this->angularVelocity = angularVelocity;
}

Vector2 Asteroid::GetVelocity()
{
    return velocity;
}

float Asteroid::GetRotation()
{
    return rotation;
}

float Asteroid::GetAngularVelocity()
{
    return angularVelocity;
}
