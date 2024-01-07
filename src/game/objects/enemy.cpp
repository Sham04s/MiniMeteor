#include "game/objects/enemy.hpp"
#include <math.h>
#include <string>

BasicEnemy::BasicEnemy(Vector2 origin, Player *player) : Character(origin)
{
    this->state = IDLE;
    this->lookingForPlayer = false;
    this->lastTryToShootTime = 0;
    this->probOfShootingToPlayer = 0.5f; // 50% every ENEMY_TRY_TO_SHOOT_COOLDOWN seconds
    this->player = player;
    this->type = ENEMY;
    this->accelerateStartTime = 0;
    this->accelerateTime = INFINITY;
    this->rotateStartTime = 0;
    this->rotateTime = INFINITY;

    // nerf enemy
    this->turnSpeed = CHARACTER_TURN_SPEED / 3;
    this->maxSpeed = CHARACTER_MAX_SPEED / 3;
    this->acceleration = CHARACTER_ACCELERATION / 3;
    this->deceleration = CHARACTER_DECELERATION / 3;
    this->lookingAtPointAngleThreshold = 3.0f; // degrees
    this->lives = 1;

    this->texture = ResourceManager::GetSpriteTexture(ENEMY_BASIC_SPRITES);
    this->shootSound = ResourceManager::CreateSoundAlias(ENEMY_BULLET_SOUND);
    this->thrustSound = ResourceManager::CreateSoundAlias(ENEMY_THRUST_SOUND);
    this->explosionSound = ResourceManager::CreateSoundAlias(ENEMY_EXPLOSION_SOUND);

    SetDefaultHitBox();

    Rotate(GetRandomValue(0, 360));
    this->velocity = Vector2Scale(forwardDir, GetRandomValue(20, CHARACTER_SIZE));
}

BasicEnemy::BasicEnemy(Vector2 origin, Player *player, EnemyAttributes attributes)
    : BasicEnemy(origin, player)
{
    const float frMultiplier = attributes.fireRateMultiplier <= 0.0f ? 0.001f : attributes.fireRateMultiplier;
    
    this->velocity = Vector2Scale(this->velocity, attributes.velocityMultiplier);
    this->turnSpeed *= attributes.precisionMultiplier;
    this->shootCooldown /= frMultiplier;
    this->bulletsSpeed *= attributes.bulletSpeedMultiplier;
    this->lookingAtPointAngleThreshold /= attributes.precisionMultiplier;
}

BasicEnemy::~BasicEnemy()
{
    Kill();
}

void BasicEnemy::Update()
{
    Character::Update();

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

    // if enemy is looking at player's last seen position, shoot and accelerate
    if (lookingForPlayer && IsLookingAt(player->GetOrigin()))
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
                                 (ENEMY_ACCELERATE_MAX_TIME - ENEMY_ACCELERATE_MIN_TIME) +
                             ENEMY_ACCELERATE_MIN_TIME;
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
                             (ENEMY_ROTATE_MAX_TIME - ENEMY_ROTATE_MIN_TIME) +
                         ENEMY_ROTATE_MIN_TIME;
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

void BasicEnemy::DrawDebug()
{
    Character::DrawDebug();

    // draw enemy information below bounds
    static const char *stateStrings[] = {"IDLE", "ACCELERATING", "TURNING_LEFT", "TURNING_RIGHT", "DYING", "DEAD"};

    std::string stateString;
    for (int i = 0; i < 6; i++)
    {
        if (state & (1 << i))
        {
            stateString += stateStrings[i];
            stateString += " ";
        }
    }

    DrawText(TextFormat("State: %s", stateString.c_str()), origin.x - CHARACTER_SIZE / 2, origin.y + CHARACTER_SIZE / 2 + 10, 10, WHITE);
    DrawText(TextFormat("Looking for player: %s", lookingForPlayer ? "true" : "false"), origin.x - CHARACTER_SIZE / 2, origin.y + CHARACTER_SIZE / 2 + 20, 10, lookingForPlayer ? GREEN : RED);
    DrawText(TextFormat("Turn speed: %f", turnSpeed), origin.x - CHARACTER_SIZE / 2, origin.y + CHARACTER_SIZE / 2 + 30, 10, WHITE);
    if (lookingForPlayer)
    {
        DrawLineEx(origin, player->GetOrigin(), 2, RED);
        DrawText(TextFormat("Angle to player: %f", Vector2Angle(forwardDir, Vector2Subtract(player->GetOrigin(), origin)) * RAD2DEG), origin.x - CHARACTER_SIZE / 2, origin.y + CHARACTER_SIZE / 2 + 40, 10, WHITE);
    }
}

void BasicEnemy::HandleCollision(GameObject *other, Vector2 *pushVector)
{
    if (other->GetType() == BULLET)
    {
        if (((Bullet *)other)->IsPlayerBullet())
        {
            Kill();
        }
        return;
    }
    if (other->GetType() == ASTEROID || other->GetType() == ENEMY)
    {
        Push(other, *pushVector);
    }
}

Rectangle BasicEnemy::GetFrameRec()
{
    int frame = 0;
    if (state & ACCELERATING)
    {
        frame = 1;
    }

    return ResourceManager::GetSpriteSrcRect(ENEMY_BASIC_SPRITES, frame);
}

void BasicEnemy::ShootAtPlayer()
{
    if (lookingForPlayer)
    {
        return;
    }
    lookingForPlayer = true;
}

void BasicEnemy::TryToShootAtPlayer()
{
    if (!this->IsAlive() || player->IsDead() || lookingForPlayer || !player->HasMoved() || GetTime() - lastTryToShootTime < ENEMY_TRY_TO_SHOOT_COOLDOWN)
    {
        return;
    }

    if (GetRandomValue(0, 100) < probOfShootingToPlayer * 100)
    {
        ShootAtPlayer();
    }
    lastTryToShootTime = GetTime();
}

bool BasicEnemy::IsLookingAtPlayer()
{
    if (!lookingForPlayer)
    {
        return false;
    }
    return IsLookingAt(player->GetOrigin());
}

bool BasicEnemy::IsLookingAt(Vector2 position)
{
    return fabsf(Vector2Angle(forwardDir, Vector2Subtract(position, origin)) * RAD2DEG) < lookingAtPointAngleThreshold;
}

void BasicEnemy::SetDefaultHitBox()
{
    this->hitbox = {{0.0f, -0.4f}, {0.4f, 0.35f}, {-0.4f, 0.35f}};
    for (size_t i = 0; i < hitbox.size(); i++)
    {
        hitbox[i] = Vector2Add(Vector2Scale(hitbox[i], CHARACTER_SIZE / 2), origin);
    }
}
