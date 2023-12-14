#include "enemy.hpp"
#include <math.h>

BasicEnemy::BasicEnemy(Vector2 origin) : Character(origin)
{
    this->state = CHARACTER_IDLE;
    this->texture = ResourceManager::GetSpriteTexture(ENEMY_BASIC_SPRITE);
    this->lookingForPlayer = false;
    this->lastTryToShootTime = 0;
    this->player = nullptr;
    this->accelerateStartTime = 0;
    // nerf enemy
    this->turnSpeed = CHARACTER_TURN_SPEED / 3;
    this->maxSpeed = CHARACTER_MAX_SPEED / 3;
    this->acceleration = CHARACTER_ACCELERATION / 3;
    this->deceleration = CHARACTER_DECELERATION / 3;
    this->lives = 1;

    SetDefaultHitBox();

    Rotate(GetRandomValue(0, 360));
    this->velocity = Vector2Scale(forwardDir, GetRandomValue(20, CHARACTER_SIZE));
}

BasicEnemy::~BasicEnemy()
{
}

void BasicEnemy::Update()
{
    Character::Update();

    // if enemy is looking for player, rotate towards player
    if (lookingForPlayer)
    {
        const float rotationSpeed = turnSpeed * GetFrameTime();
        const float angleToPlayer = Vector2Angle(forwardDir, Vector2Subtract(player->GetOrigin(), origin)) * RAD2DEG;
        if (angleToPlayer > rotationSpeed)
        {
            Rotate(rotationSpeed);
        }
        else if (angleToPlayer < -rotationSpeed)
        {
            Rotate(-rotationSpeed);
        }
        else
        {
            Rotate(angleToPlayer);
        }
    }

    // if enemy is looking at player's last seen position, shoot and accelerate
    if (lookingForPlayer && IsLookingAt(player->GetOrigin()))
    {
        Shoot();
        state = CHARACTER_ACCELERATING;
        accelerateStartTime = GetTime();
        lookingForPlayer = false;
    }

    // accelerate randomly
    if (state == CHARACTER_IDLE)
    {
        const float probOfAccelerating = 0.01f;
        if (GetRandomValue(0, 100) < probOfAccelerating * 100)
        {
            state = CHARACTER_ACCELERATING;
            accelerateStartTime = GetTime();
        }
    }

    // stop accelerating after a while
    if (state == CHARACTER_EXTRA_ACCELERATING || state == CHARACTER_ACCELERATING)
    {
        if (GetTime() - accelerateStartTime > ENEMY_ACCELERATE_TIME)
        {
            state = CHARACTER_IDLE;
        }
    }
}

void BasicEnemy::DrawDebug()
{
    Character::DrawDebug();

    // draw enemy information below bounds
    const char *stateStrings[] = {"IDLE", "ACCELERATING", "EXTRA_ACCELERATING", "TURNING_LEFT", "TURNING_RIGHT", "DYING", "DEAD"};

    DrawText(TextFormat("State: %s", stateStrings[state]), origin.x - CHARACTER_SIZE / 2, origin.y + CHARACTER_SIZE / 2 + 10, 10, WHITE);
    DrawText(TextFormat("Looking for player: %s", lookingForPlayer ? "true" : "false"), origin.x - CHARACTER_SIZE / 2, origin.y + CHARACTER_SIZE / 2 + 20, 10, lookingForPlayer ? GREEN : RED);
    DrawText(TextFormat("Turn speed: %f", turnSpeed), origin.x - CHARACTER_SIZE / 2, origin.y + CHARACTER_SIZE / 2 + 30, 10, WHITE);
    if (lookingForPlayer)
    {
        DrawLineEx(origin, player->GetOrigin(), 2, RED);
        DrawText(TextFormat("Angle to player: %f", Vector2Angle(forwardDir, Vector2Subtract(player->GetOrigin(), origin)) * RAD2DEG), origin.x - CHARACTER_SIZE / 2, origin.y + CHARACTER_SIZE / 2 + 40, 10, WHITE);
    }
}

void BasicEnemy::ShootAtPlayer(Player &player)
{
    if (lookingForPlayer)
    {
        return;
    }
    lookingForPlayer = true;
    this->player = &player;
}

void BasicEnemy::TryToShootAtPlayer(Player &player)
{
    if (!this->IsAlive() || player.IsDead() || lookingForPlayer || GetTime() - lastTryToShootTime < ENEMY_TRY_TO_SHOOT_COOLDOWN)
    {
        return;
    }

    // 50% chance of shooting every 2 seconds
    const double probOfShooting = 0.5f;
    if (GetRandomValue(0, 100) < probOfShooting * 100)
    {
        ShootAtPlayer(player);
    }
    lastTryToShootTime = GetTime();
}

bool BasicEnemy::IsLookingAtPlayer(Player &player)
{
    if (!lookingForPlayer)
    {
        return false;
    }
    return IsLookingAt(player.GetOrigin());
}

bool BasicEnemy::IsLookingAt(Vector2 position)
{
    return fabsf(Vector2Angle(forwardDir, Vector2Subtract(position, origin)) * RAD2DEG) < 3; // 3 degrees
}

void BasicEnemy::SetDefaultHitBox()
{
    this->hitbox = {{0.0f, -0.4f}, {0.4f, 0.35f}, {-0.4f, 0.35f}};
    for (size_t i = 0; i < hitbox.size(); i++)
    {
        hitbox[i] = Vector2Add(Vector2Scale(hitbox[i], CHARACTER_SIZE), origin);
    }
}
