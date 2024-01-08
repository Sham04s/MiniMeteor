#include "game/objects/stalker.hpp"

Stalker::Stalker(Player *player, EnemyAttributes attributes)
    : Enemy(player, attributes, STALKER)
{
    this->texture = ResourceManager::GetSpriteTexture(ENEMY_STALKER_SPRITES);
    this->turnSpeed = 90;
    SetSoundVolume(thrustSound, 0.0f); // disable thrust sound
    state |= ACCELERATING;
    state |= TURNING_LEFT;

    SetDefaultHitBox();
}

Stalker::~Stalker()
{
}

void Stalker::Update()
{
    Enemy::Update();

    Vector2 playerPos = player->GetOrigin();
    Vector2 playerDir = Vector2Normalize(Vector2Subtract(playerPos, origin));

    this->accelDir = playerDir;
}

void Stalker::DrawDebug()
{
    Enemy::DrawDebug();
}

Rectangle Stalker::GetFrameRec()
{
    return Enemy::GetFrameRec();
}

void Stalker::SetDefaultHitBox()
{
    hitbox = {
        {-0.25f, -0.25f},
        {0.25f, -0.25f},
        {0.25f, 0.25f},
        {-0.25f, 0.25f},
        {-0.25f, -0.25f},
    };

    for (size_t i = 0; i < hitbox.size(); i++)
    {
        hitbox[i] = Vector2Add(Vector2Scale(Vector2Rotate(hitbox[i], rotation * DEG2RAD), CHARACTER_SIZE), origin);
    }
}
