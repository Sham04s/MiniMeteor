#include "player.hpp"
#include "raymath.h"

#include <math.h>

Player::Player(Vector2 center, int zIndex) : GameObject({center.x - 16, center.y - 16, 32, 32}, 0, {0, -1}, zIndex, {}, PLAYER)
{
    this->originalCenter = center;
    this->lives = 4;
    this->velocity = {0, 0};
    this->powerups = {};
    this->shots = {};
    this->hitbox = {{0.0f, -0.4f}, {0.4, 0.4}, {-0.4, 0.4}};
    SetTexture(ResourceManager::GetSpriteTexture(PLAYER_SPRITE));
}

Player::~Player()
{
}

void Player::Update()
{
    if (IsKeyPressed(KEY_W))
    {
        this->isAccelerating = true;
    }
    if (IsKeyReleased(KEY_W))
    {
        this->isAccelerating = false;
    }
    if (IsKeyPressed(KEY_S))
    {
        this->isBraking = true;
    }
    if (IsKeyReleased(KEY_S))
    {
        this->isBraking = false;
    }
    
    if (this->isAccelerating)
    {
        Accelerate(200);
    }
    if (this->isBraking)
    {
        Decelerate(200);
    }
    if (IsKeyDown(KEY_A))
    {
        Rotate(-180); // 180 degrees per second
    }
    if (IsKeyDown(KEY_D))
    {
        Rotate(180);
    }
    if (IsKeyPressed(KEY_SPACE))
    {
        Shoot();
    }

    // apply velocity
    SetCenter(Vector2Add(GetCenter(), Vector2Scale(GetVelocity(), GetFrameTime())));

    // for wrapping around the screen
    SetCenter({fmodf(GetCenter().x + GetScreenWidth(), GetScreenWidth()), fmodf(GetCenter().y + GetScreenHeight(), GetScreenHeight())});

    SetBounds({GetCenter().x - 16, GetCenter().y - 16, 32, 32});
}

void Player::AddLive()
{
    this->lives++;
}

void Player::AddPowerUp(/*PowerUp powerup*/)
{
}

void Player::DrawDebug()
{
    GameObject::DrawDebug();

    DrawText(TextFormat("Velocity: (%.2f, %.2f)", GetVelocity().x, GetVelocity().y), 10, 30, 20, GREEN);
}
void Player::Kill()
{
    this->lives--;
}

void Player::Reset()
{
    this->center = this->originalCenter;
    this->rotation = 0;
    this->forwardDir = {0, -1};
    this->velocity = {0, 0};
    this->powerups = {};
    this->shots = {}; // maybe not?
    this->hitbox = {{0.0f, -0.4f}, {0.4, 0.4}, {-0.4, 0.4}};
}

void Player::Rotate(float angle) // in degrees
{
    angle = angle * GetFrameTime();
    SetRotation(GetRotation() + angle);
    SetForwardDir(Vector2Rotate(GetForwardDir(), angle * DEG2RAD));
    for (size_t i = 0; i < GetHitbox().size(); i++)
    {
        this->hitbox[i] = Vector2Rotate(this->hitbox[i], angle * DEG2RAD);
    }
}

void Player::Accelerate(float acceleration)
{
    SetVelocity(Vector2Add(GetVelocity(), Vector2Scale(GetForwardDir(), acceleration * GetFrameTime())));
    if (Vector2Length(GetVelocity()) > MAX_SPEED)
    {
        SetVelocity(Vector2Scale(Vector2Normalize(GetVelocity()), MAX_SPEED));
    }
}

void Player::Decelerate(float deceleration)
{
    Vector2 newVelocity = Vector2Subtract(GetVelocity(), Vector2Scale(GetForwardDir(), deceleration * GetFrameTime()));
    if (Vector2DotProduct(GetVelocity(), GetForwardDir()) < 0 && Vector2Length(newVelocity) > 80)
    {
        return;
    }
    SetVelocity(newVelocity);
}

void Player::Shoot()
{
}
