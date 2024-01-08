#include "game/objects/pulser.hpp"

Pulser::Pulser(Player *player, EnemyAttributes attributes)
    : Enemy(player, attributes)
{
    this->texture = ResourceManager::GetSpriteTexture(ENEMY_PULSER_SPRITES);
    this->turnSpeed = 360.0f / PULSER_SHOOT_COOLDOWN;
    this->bulletsPerShot = attributes.bulletsPerShot;
    this->bulletsSpread = 360.0f / bulletsPerShot;
    this->bulletsSpeed = attributes.bulletSpeedMultiplier * BULLET_SPEED / 2; // slow bullets
    SetSoundVolume(thrustSound, 0.0f); // disable thrust sound
    state |= ACCELERATING;
    state |= TURNING_RIGHT;
    this->lastChangeDirTime = 0.0f;
    this->lastShootTime = GetTime();

    SetDefaultHitBox();
}

Pulser::~Pulser()
{
}

void Pulser::Update()
{
    Enemy::Update();
    if (GetTime() - lastShootTime > PULSER_SHOOT_COOLDOWN)
    {
        Shoot();
    }

    if (GetTime() - lastChangeDirTime > PULSER_CHANGE_DIR_COOLDOWN)
    {
        if (GetRandomValue(0, 100) < PULSER_CHANGE_DIR_PROB * 100)
        {
            ChangeDir();
        }
    }

    accelDir = newAccelDir; // overrides character acce lDir
}

void Pulser::DrawDebug()
{
    Enemy::DrawDebug();
}

void Pulser::ChangeDir()
{
    newAccelDir = Vector2Normalize({(float)GetRandomValue(-100, 100), (float)GetRandomValue(-100, 100)});
    lastChangeDirTime = GetTime();
}

// same as Character::Shoot() but with center at origin and bullets spawning away from the center
void Pulser::Shoot()
{
    if (!CanShoot())
    {
        return;
    }

    Vector2 bulletDir = Vector2Rotate(forwardDir, (bulletsPerShot - 1) * bulletsSpread * DEG2RAD / 2);

    for (int i = 0; i < bulletsPerShot; i++)
    {
        bullets.push_back(Bullet(Vector2Add(this->origin, Vector2Scale(bulletDir, CHARACTER_SIZE / 2)),
                                 bulletDir, bulletsSpeed, this->type == PLAYER));

        bulletDir = Vector2Rotate(bulletDir, bulletsSpread * DEG2RAD);
    }
    lastShootTime = GetTime();
    PlaySound(shootSound);
}

Rectangle Pulser::GetFrameRec()
{
    return Enemy::GetFrameRec();
}

void Pulser::SetDefaultHitBox()
{

    // TODO: check if this is the correct hitbox
    hitbox = {
        {-0.25f, -0.25f},
        {0.25f, -0.25f},
        {0.25f, 0.25f},
        {-0.25f, 0.25f},
    };

    for (size_t i = 0; i < hitbox.size(); i++)
    {
        hitbox[i] = Vector2Add(Vector2Scale(Vector2Rotate(hitbox[i], rotation * DEG2RAD), CHARACTER_SIZE), origin);
    }
}