#include "game/objects/enemy.hpp"
#include <string>

Enemy::Enemy(Vector2 origin, Player *player, EnemyAttributes attributes, EnemyType type)
    : Character(origin)
{
    this->state = IDLE;
    this->player = player;
    this->type = ENEMY;
    this->enemyType = type;

    // nerf enemy
    this->turnSpeed = CHARACTER_TURN_SPEED / 3;
    this->maxSpeed = CHARACTER_MAX_SPEED / 3;
    this->acceleration = CHARACTER_ACCELERATION / 3;
    this->deceleration = CHARACTER_DECELERATION / 3;
    this->lives = 1;

    this->texture = ResourceManager::GetInvalidTexture(); // This class is abstract
    this->shootSound = ResourceManager::CreateSoundAlias(ENEMY_BULLET_SOUND);
    this->thrustSound = ResourceManager::CreateSoundAlias(ENEMY_THRUST_SOUND);
    this->explosionSound = ResourceManager::CreateSoundAlias(ENEMY_EXPLOSION_SOUND);

    SetDefaultHitBox();

    Rotate(GetRandomValue(0, 360));
    this->velocity = Vector2Scale(forwardDir, GetRandomValue(20, CHARACTER_SIZE));

    const float frMultiplier = attributes.fireRateMultiplier <= 0.0f ? 0.001f : attributes.fireRateMultiplier;

    this->velocity = Vector2Scale(this->velocity, attributes.velocityMultiplier);
    this->turnSpeed *= attributes.precision; // increase turn speed with precision
    this->shootCooldown /= frMultiplier;
    this->bulletsSpeed *= attributes.bulletSpeedMultiplier;
    this->precision = attributes.precision;
}

Enemy::~Enemy()
{
}

void Enemy::SetDefaultHitBox()
{
    Character::SetDefaultHitBox();
}

void Enemy::Update()
{
    Character::Update();
}

void Enemy::DrawDebug()
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
    DrawText(TextFormat("Turn speed: %f", turnSpeed), origin.x - CHARACTER_SIZE / 2, origin.y + CHARACTER_SIZE / 2 + 20, 10, WHITE);
}

void Enemy::HandleCollision(GameObject *other, Vector2 *pushVector)
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
    };
}

Rectangle Enemy::GetFrameRec()
{
    return Character::GetFrameRec();
}

bool Enemy::IsLookingAtPlayer()
{
    return IsLookingAt(player->GetOrigin());
}

bool Enemy::IsLookingAt(Vector2 position)
{
    const float angleThreshold = 3.0f; // degrees
    const float angle = fabsf(Vector2Angle(forwardDir, Vector2Subtract(position, origin)) * RAD2DEG);
    return angle < angleThreshold / precision;
}
