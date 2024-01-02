#include "player.hpp"
#include "enemy.hpp"

#include <math.h>
#include <string>

Player::Player(Vector2 origin) : Character(origin)
{
    this->initialOrigin = {(float)GetScreenWidth() / 2, (float)GetScreenHeight() / 2};
    this->type = PLAYER;

    this->Reset();

    this->texture = ResourceManager::GetSpriteTexture(PLAYER_SPRITES);
    this->shootSound = ResourceManager::CreateSoundAlias(BULLET_SOUND);
    this->thrustSound = ResourceManager::CreateSoundAlias(THRUST_SOUND);
    this->explosionSound = ResourceManager::CreateSoundAlias(SHIP_EXPLOSION_SOUND);
    this->powerupPickupSound = ResourceManager::GetSound(POWERUP_PICKUP_SOUND);

    SetDefaultHitBox();
}

Player::~Player()
{
}

void Player::Update()
{
    // invincibility ends when player starts moving or shooting
    if (invincible && ((state & ACCELERATING) || usingBoost || lastShotTime > 0.0f))
    {
        invincible = false;
        hasMoved = true;
    }

    if (usingBoost)
    {
        boostTime = fmaxf(boostTime - GetFrameTime(), 0.0f);
        if (boostTime <= 0.0f)
        {
            usingBoost = false;
        }
        lastBoostUsedTime = GetTime();
    }
    else if (GetTime() - lastBoostUsedTime > BOOST_RECHARGE_COOLDOWN)
    {
        boostTime = fminf(boostTime + GetFrameTime(), BOOST_TIME);
    }

    if (usingBoost && boostTime > 0.0f)
    {
        acceleration = CHARACTER_ACCELERATION * 2;
        deceleration = CHARACTER_DECELERATION * 2;
        maxSpeed = CHARACTER_MAX_SPEED * 2;
        pitchAndVolumeScale = 1.0f;
    }
    else
    {
        acceleration = CHARACTER_ACCELERATION;
        deceleration = CHARACTER_DECELERATION;
        maxSpeed = CHARACTER_MAX_SPEED;
        pitchAndVolumeScale = 0.7f;
    }

    // handle input if alive
    if (this->IsAlive())
    {
        HandleInput();
    }

    if ((state & DEAD) && lives > 0 && GetTime() - lastDeathTime > CHARACTER_RESPAWN_TIME)
    {
        Respawn();
    }

    Character::Update();

    UpdatePowerups();
}

void Player::UpdatePowerups()
{
    for (size_t i = 0; i < powerups.size(); i++)
    {
        PowerUp *powerup = powerups[i];
        powerup->Update();
        powerup->UpdateBounds(this->bounds);
        
        if (powerup->GetType() == TEMPORARY_INFINITE_BOOST)
        {
            boostTime = BOOST_TIME;
        }
        
        if (!powerup->CanBeApplied())
        {
            powerupsCount[powerup->GetType()] -= 1;
            delete powerup;
            powerups.erase(powerups.begin() + i);
        }
    }

    bulletsPerShot = 1 + powerupsCount[EXTRA_BULLET_UPGRADE];
    shootCooldown = CHARACTER_SHOOT_COOLDOWN * GetPowerupMultiplier(FIRE_RATE_UPGRADE);
    bulletsSpeed = BULLET_SPEED * GetPowerupMultiplier(BULLET_SPEED_UPGRADE);
    bulletsSpread = BULLET_SPREAD * GetPowerupMultiplier(BULLET_SPREAD_UPGRADE);
}

void Player::Draw()
{
    Character::Draw();

    // draw boost bar
    PowerUp *temporaryInfiniteBoost = GetPowerup(TEMPORARY_INFINITE_BOOST);
    const float timeSinceBoost = GetTime() - lastBoostUsedTime;
    if (IsAlive() && (temporaryInfiniteBoost != nullptr || timeSinceBoost < BOOST_BAR_HIDE_TIME + BOOST_BAR_FADE_TIME))
    {
        Rectangle boostBar = {bounds.x + 10, bounds.y + bounds.height - 10, bounds.width - 20, 5};
        if (temporaryInfiniteBoost == nullptr)
        {
            boostBar.width = boostBar.width * boostTime / BOOST_TIME;
            float alpha = 1.0f;
            if (timeSinceBoost >= BOOST_BAR_HIDE_TIME)
            {
                alpha -= (timeSinceBoost - BOOST_BAR_HIDE_TIME) / BOOST_BAR_FADE_TIME;
            }
            DrawRectangleRounded(boostBar, 0.5f, 0, Fade(GREEN, alpha));
        }
        else
        {
            // draw boost bar with gradient acoording to powerup time left
            const float timeLeft = temporaryInfiniteBoost->GetEffectiveUseTime();
            const float gradientWidth = boostBar.width * timeLeft / TEMPORARY_INFINITE_BOOST_TIME;
            Rectangle gradientBar = {boostBar.x + 1, boostBar.y - 1, gradientWidth - 1, boostBar.height + 2};
            Rectangle roundedLeft = {boostBar.x, boostBar.y - 1, fminf(1, boostBar.width), boostBar.height + 2};

            // draw rounded rectangle with gradient
            DrawRectangleRounded(roundedLeft, 0.8f, 0, RED);
            DrawRectangleGradientH(gradientBar.x, gradientBar.y, gradientBar.width, gradientBar.height, RED, GREEN);
        }
    }

    if (IsAlive())
    {
        for (auto powerup : powerups)
        {
            powerup->Draw();
        }

        // draw temporary shield timer
        PowerUp *temporaryShield = GetPowerup(TEMPORARY_SHIELD);
        if (temporaryShield != nullptr)
        {
            const float timeLeft = temporaryShield->GetEffectiveUseTime();
            Rectangle shieldTimer = {bounds.x + 10, bounds.y + bounds.height - 20, bounds.width - 20, 5};
            shieldTimer.width = shieldTimer.width * timeLeft / TEMPORARY_SHIELD_TIME;
            DrawRectangleRounded(shieldTimer, 0.5f, 0, WHITE);
        }
    }
}

void Player::DrawDebug()
{
    Character::DrawDebug();

    DrawText(TextFormat("Speed: %.2f", Vector2Length(velocity)), 10, 80, 16, WHITE);
    DrawText(TextFormat("MAX Speed: %.2f", maxSpeed), 10, 100, 16, WHITE);

    Rectangle belowBounds = {bounds.x, bounds.y + bounds.height, bounds.width, 20};
    DrawText(TextFormat("Invincible: %s", invincible ? "true" : "false"), belowBounds.x, belowBounds.y, 16, invincible ? GREEN : RED);
}

void Player::HandleInput()
{
    if (IsKeyPressed(KEY_W))
    {
        this->state |= ACCELERATING;
    }
    if (IsKeyReleased(KEY_W))
    {
        this->state &= ~ACCELERATING;
        usingBoost = false;
    }
    if (IsKeyDown(KEY_LEFT_SHIFT) && this->state & ACCELERATING)
    {
        usingBoost = true;
    }
    if (IsKeyUp(KEY_LEFT_SHIFT) && usingBoost)
    {
        usingBoost = false;
    }

    if (IsKeyDown(KEY_A))
    {
        state &= ~TURNING_RIGHT;
        state |= TURNING_LEFT;
    }
    else if (IsKeyDown(KEY_D))
    {
        state &= ~TURNING_LEFT;
        state |= TURNING_RIGHT;
    }
    else
    {
        state &= ~(TURNING_LEFT | TURNING_RIGHT);
    }
    if (IsKeyDown(KEY_SPACE))
    {
        Shoot();
    }
}

void Player::HandleCollision(GameObject *other, Vector2 *pushVector)
{
    if (!IsAlive())
    {
        return;
    }
    if (other->GetType() == BASIC_ENEMY)
    {
        BasicEnemy *enemy = (BasicEnemy *)other;
        Push(enemy, *pushVector);
        if (this->CanBeHit() && !this->HasPowerup(SHIELD))
        {
            enemy->Push(this, Vector2Negate(*pushVector));
        }
        if (CanBeKilled())
        {
            Kill();
        }
        if (this->CanBeKilled() || this->HasPowerup(SHIELD))
        {
            enemy->Kill();
        }
        return;
    }
    if (other->GetType() == ASTEROID)
    {
        Asteroid *asteroid = (Asteroid *)other;
        if (this->CanBeHit() && !this->HasPowerup(SHIELD))
        {
            other->Push(this, Vector2Negate(*pushVector));
        }
        if (this->HasPowerup(SHIELD))
        {
            asteroid->Destroy();
        }
        if (this->CanBeKilled())
        {
            Kill();
        }
        Push(other, *pushVector);
        return;
    }
    if (other->GetType() == ENEMY_BULLET)
    {
        if (CanBeHit())
        {
            ((Bullet *)other)->Destroy();
        }
        if (CanBeKilled())
        {
            Kill();
        }
        return;
    }
    if (other->GetType() == POWER_UP)
    {
        PowerUp *powerup = (PowerUp *)other;
        if (AddPowerup(powerup))
        {
            PlaySound(*powerupPickupSound);
            powerup->PickUp();
        }
        return;
    }
}

bool Player::AddPowerup(PowerUp *powerup)
{
    if (powerup->IsPickedUp() || powerup->IsExpired())
    {
        return false;
    }
    if (powerup->GetType() == LIFE)
    {
        if (lives >= CHARACTER_MAX_LIVES)
        {
            return false;
        }
        AddLife();
        return true;
    }
    if (powerup->GetType() == SHIELD)
    {
        // if already has shield, don't pick up
        if (HasPowerup(SHIELD))
        {
            return false;
        }
        powerup->UpdateBounds(this->bounds);
        powerupsCount[SHIELD] += 1;
        powerups.push_back(powerup);
        return true;
    }

    if (powerup->GetType() == TEMPORARY_SHIELD || powerup->GetType() == TEMPORARY_INFINITE_BOOST)
    {
        PowerUp *temporaryPowerUp = GetPowerup(powerup->GetType());
        if (temporaryPowerUp != nullptr)
        {
            temporaryPowerUp->ResetUseTime();
            return true;
        }
        powerup->UpdateBounds(this->bounds);
        powerupsCount[powerup->GetType()] = 1;
        powerups.push_back(powerup);
        return true;
    }
    if (powerup->GetType() == FIRE_RATE_UPGRADE || powerup->GetType() == BULLET_SPEED_UPGRADE || powerup->GetType() == BULLET_SPREAD_UPGRADE || powerup->GetType() == EXTRA_BULLET_UPGRADE)
    {
        int upgradeCount = powerupsCount[powerup->GetType()];
        if (upgradeCount >= MAX_UPGRADES_PER_TYPE)
        {
            return false;
        }

        powerup->UpdateBounds(this->bounds);
        powerupsCount[powerup->GetType()] += 1;
        powerups.push_back(powerup);

        return true;
    }

    return false;
}

bool Player::RemovePowerup(PowerUpType type)
{
    for (size_t i = 0; i < powerups.size(); i++)
    {
        if (powerups[i]->GetType() == type)
        {
            powerupsCount[type] -= 1;
            delete powerups[i];
            powerups.erase(powerups.begin() + i);
            return true;
        }
    }
    return false;
}

bool Player::HasPowerup(PowerUpType type)
{
    return GetPowerupCount(type) > 0;
}

PowerUp *Player::GetPowerup(PowerUpType type)
{
    for (auto powerup : powerups)
    {
        if (powerup->GetType() == type)
        {
            return powerup;
        }
    }
    return nullptr;
}

bool Player::CanBeKilled()
{
    return !invincible;
}

bool Player::CanBeHit()
{
    return !invincible || HasPowerup(SHIELD) || HasPowerup(TEMPORARY_SHIELD);
}

bool Player::HasMoved()
{
    return hasMoved;
}

bool Player::Kill()
{
    if (HasPowerup(SHIELD))
    {
        RemovePowerup(SHIELD);
        return false;
    }
    if (HasPowerup(TEMPORARY_SHIELD))
    {
        return false;
    }
    return Character::Kill();
}

void Player::Respawn()
{
    this->origin = this->initialOrigin;
    this->bounds = {origin.x - CHARACTER_SIZE / 2, origin.y - CHARACTER_SIZE / 2, CHARACTER_SIZE, CHARACTER_SIZE};
    this->rotation = 0;
    this->invincible = true; // invincible when spawns
    this->hasMoved = false;
    this->usingBoost = false;
    this->boostTime = BOOST_TIME;
    this->lastBoostUsedTime = 0;
    this->forwardDir = {0, -1};
    this->velocity = {0, 0};
    this->angularVelocity = 0;
    this->state = IDLE;
    this->lastDeathTime = 0;
    this->lastShotTime = 0;
    this->shootCooldown = CHARACTER_SHOOT_COOLDOWN;
    this->bulletsPerShot = 1;
    this->bulletsSpeed = BULLET_SPEED;
    this->bulletsSpread = BULLET_SPREAD;
    // leave bullets live
    SetDefaultHitBox();

    // remove one powerup of each type
    for (size_t i = 0; i < NUM_POWER_UP_TYPES; i++)
    {
        RemovePowerup((PowerUpType)i);
    }
}

void Player::Reset()
{
    Respawn();
    this->lives = 3;
    this->bullets.clear();
    
    // remove all powerups
    for (auto powerup : powerups)
    {
        delete powerup;
    }
    powerups.clear();
    for (size_t i = 0; i < NUM_POWER_UP_TYPES; i++)
    {
        powerupsCount[i] = 0;
    }
}

Rectangle Player::GetFrameRec()
{
    int frame = 0;
    if (state & ACCELERATING)
    {
        frame = (usingBoost && boostTime > 0) ? 2 : 1;
    }
    else if (state & DYING)
    {
        // frame = (int)((GetTime() - lastDeathTime) / CHARACTER_DYING_TIME * 4) + 3; // for sprites
        frame = 0; // idle
    }

    return ResourceManager::GetSpriteSrcRect(PLAYER_SPRITES, frame);
}

float Player::GetPowerupMultiplier(PowerUpType type)
{
    int count = powerupsCount[type];

    switch (type)
    {
    case FIRE_RATE_UPGRADE:
        return powf(FIRE_RATE_MULTIPLIER, count) * powf(BULLETS_PER_SHOT_MULTIPLIER, bulletsPerShot - 1);
    case BULLET_SPEED_UPGRADE:
        return powf(BULLETS_SPEED_MULTIPLIER, count);
    case BULLET_SPREAD_UPGRADE:
        return powf(BULLETS_SPREAD_MULTIPLIER, count);
    default:
        return count;
    }
}

void Player::SetDefaultHitBox()
{
    this->hitbox = {{0.0f, -0.4f}, {0.4f, 0.35f}, {-0.4f, 0.35f}}; // up, right-down, left-down

    for (size_t i = 0; i < hitbox.size(); i++)
    {
        this->hitbox[i] = Vector2Add(Vector2Scale(this->hitbox[i], CHARACTER_SIZE / 2), GetOrigin());
    }
}
