#include "enemy.hpp"
#include <math.h>

BasicEnemy::BasicEnemy(Vector2 origin) : Character(origin)
{
    this->state = CHARACTER_IDLE;
    this->texture = ResourceManager::GetSpriteTexture(ENEMY_BASIC_SPRITES);
    this->thrustSound = LoadSoundAlias(*ResourceManager::GetSound(ENEMY_THRUST_SOUND));
    this->lookingForPlayer = false;
    this->lastTryToShootTime = 0;
    this->player = nullptr;
    this->type = BASIC_ENEMY;
    this->accelerateStartTime = 0;
    this->accelerateTime = INFINITY;
    this->rotateStartTime = 0;
    this->rotateTime = INFINITY;
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

    // if is not alive, do nothing else
    if (!this->IsAlive())
    {
        return;
    }

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
            state = CHARACTER_TURNING_RIGHT;
        }
        else if (angleToPlayer < -rotationSpeed)
        {
            state = CHARACTER_TURNING_LEFT;
        }
        else
        {
            state = CHARACTER_IDLE;
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
            accelerateTime = ((float)GetRandomValue(0, 100) / 100.0f) *
                                 (ENEMY_ACCELERATE_MAX_TIME - ENEMY_ACCELERATE_MIN_TIME) +
                             ENEMY_ACCELERATE_MIN_TIME;
            accelerateStartTime = GetTime();
        }
    }

    // stop accelerating after a while
    if (state == CHARACTER_EXTRA_ACCELERATING || state == CHARACTER_ACCELERATING)
    {
        if (GetTime() - accelerateStartTime > accelerateTime)
        {
            state = CHARACTER_IDLE;
            accelerateTime = INFINITY;
        }
    }

    // if not looking for player, rotate randomly
    if (!lookingForPlayer)
    {
        const float probOfTurning = 0.01f;
        if (GetRandomValue(0, 100) < probOfTurning * 100)
        {
            state = GetRandomValue(0, 1) ? CHARACTER_TURNING_LEFT : CHARACTER_TURNING_RIGHT;
            rotateTime = ((float)GetRandomValue(0, 100) / 100.0f) *
                             (ENEMY_ROTATE_MAX_TIME - ENEMY_ROTATE_MIN_TIME) +
                         ENEMY_ROTATE_MIN_TIME;
            rotateStartTime = GetTime();
        }
    }

    // stop turning after a while
    if (state == CHARACTER_TURNING_LEFT || state == CHARACTER_TURNING_RIGHT)
    {
        if (GetTime() - rotateStartTime > rotateTime)
        {
            state = CHARACTER_IDLE;
            rotateTime = INFINITY;
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

Rectangle BasicEnemy::GetFrameRec()
{
    int frame;
    switch (state)
    {
    case CHARACTER_ACCELERATING:
        frame = 1;
        break;

    case CHARACTER_EXTRA_ACCELERATING:
        frame = 2;
        break;

    default:
        frame = 0; // idle
        break;
    }

    return ResourceManager::GetSpriteSrcRect(ENEMY_BASIC_SPRITES, frame);
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
        hitbox[i] = Vector2Add(Vector2Scale(hitbox[i], CHARACTER_SIZE / 2), origin);
    }
}
