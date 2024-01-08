#include "game/objects/shooter.hpp"
#include <math.h>
#include <string>

Shooter::Shooter(Player *player, EnemyAttributes attributes)
    : Enemy(player, attributes, SHOOTER)
{
    this->probOfShootingAtPlayer = attributes.probOfShootingAtPlayer;
    this->lookingForPlayer = false;
    this->lastTryToShootTime = 0;
    this->accelerateStartTime = 0;
    this->accelerateTime = INFINITY;
    this->rotateStartTime = 0;
    this->rotateTime = INFINITY;

    this->texture = ResourceManager::GetSpriteTexture(ENEMY_SHOOTER_SPRITES);

    SetDefaultHitBox();
}

Shooter::~Shooter()
{
}

void Shooter::Update()
{
    Enemy::Update();
    
    // if is not alive, do nothing else
    if (!this->IsAlive())
    {
        return;
    }
    
    TryToShootAtPlayer();

    // if enemy is looking for player, rotate towards player
    if (this->IsAlive() && lookingForPlayer)
    {
        // reset rotate time
        rotateStartTime = 0;
        rotateTime = INFINITY;

        const float rotationSpeed = turnSpeed * GetFrameTime();
        const float angleToPlayer = Vector2Angle(forwardDir, Vector2Subtract(player->GetOrigin(), origin)) * RAD2DEG;
        if (angleToPlayer > rotationSpeed)
        {
            state &= ~TURNING_LEFT;
            state |= TURNING_RIGHT;
        }
        else if (angleToPlayer < -rotationSpeed)
        {
            state &= ~TURNING_RIGHT;
            state |= TURNING_LEFT;
        }
        else
        {
            state &= ~(TURNING_LEFT | TURNING_RIGHT);
            Rotate(angleToPlayer);
        }
    }

    // if enemy is looking at player's position, shoot and accelerate
    if (lookingForPlayer && IsLookingAtPlayer())
    {
        Shoot();
        state |= ACCELERATING;
        accelerateStartTime = GetTime();
        lookingForPlayer = false;
    }

    // accelerate randomly
    if (state & (IDLE | TURNING_LEFT | TURNING_RIGHT))
    {
        const float probOfAccelerating = 0.01f;
        if (GetRandomValue(0, 100) < probOfAccelerating * 100)
        {
            state |= ACCELERATING;
            accelerateTime = ((float)GetRandomValue(0, 100) / 100.0f) *
                                 (SHOOTER_ACCELERATE_MAX_TIME - SHOOTER_ACCELERATE_MIN_TIME) +
                             SHOOTER_ACCELERATE_MIN_TIME;
            accelerateStartTime = GetTime();
        }
    }

    // stop accelerating after a while
    if (state & ACCELERATING)
    {
        if (GetTime() - accelerateStartTime > accelerateTime)
        {
            state &= ~ACCELERATING;
            accelerateTime = INFINITY;
        }
    }

    // if not looking for player, rotate randomly
    if (!lookingForPlayer)
    {
        const float probOfTurning = 0.01f;
        if (GetRandomValue(0, 100) < probOfTurning * 100)
        {
            state |= GetRandomValue(0, 1) ? TURNING_LEFT : TURNING_RIGHT;
            rotateTime = ((float)GetRandomValue(0, 100) / 100.0f) *
                             (SHOOTER_ROTATE_MAX_TIME - SHOOTER_ROTATE_MIN_TIME) +
                         SHOOTER_ROTATE_MIN_TIME;
            rotateStartTime = GetTime();
        }
    }

    // stop turning after a while
    if (state & (TURNING_LEFT | TURNING_RIGHT))
    {
        if (GetTime() - rotateStartTime > rotateTime)
        {
            state &= ~(TURNING_LEFT | TURNING_RIGHT);
            rotateTime = INFINITY;
        }
    }
}

void Shooter::DrawDebug()
{
    Enemy::DrawDebug();
    if (lookingForPlayer)
    {
        DrawLineEx(origin, player->GetOrigin(), 2, RED);
        DrawText(TextFormat("Angle to player: %f", Vector2Angle(forwardDir, Vector2Subtract(player->GetOrigin(), origin)) * RAD2DEG), origin.x - CHARACTER_SIZE / 2, origin.y + CHARACTER_SIZE / 2 + 40, 10, WHITE);
    }
}

Rectangle Shooter::GetFrameRec()
{
    int frame = 0;
    if (state & ACCELERATING)
    {
        frame = 1;
    }

    return ResourceManager::GetSpriteSrcRect(ENEMY_SHOOTER_SPRITES, frame);
}

void Shooter::ShootAtPlayer()
{
    if (lookingForPlayer)
    {
        return;
    }
    lookingForPlayer = true;
}

void Shooter::TryToShootAtPlayer()
{
    if (!this->IsAlive() || player->IsDead() || lookingForPlayer || !player->HasMoved() || GetTime() - lastTryToShootTime < ENEMY_SHOOT_COOLDOWN)
    {
        return;
    }

    if (GetRandomValue(0, 100) < probOfShootingAtPlayer * 100)
    {
        ShootAtPlayer();
    }
    lastTryToShootTime = GetTime();
}

void Shooter::SetDefaultHitBox()
{
    this->hitbox = {{0.0f, -0.4f}, {0.4f, 0.35f}, {-0.4f, 0.35f}};
    for (size_t i = 0; i < hitbox.size(); i++)
    {
        hitbox[i] = Vector2Add(Vector2Scale(Vector2Rotate(hitbox[i], rotation * DEG2RAD), CHARACTER_SIZE / 2), origin);
    }
}
