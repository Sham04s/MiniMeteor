#include "player.hpp"
#include "enemy.hpp"
#include <math.h>
#include <string>

Player::Player(Vector2 origin) : Character(origin)
{
    this->lives = 4;
    this->initialOrigin = {(float)GetScreenWidth() / 2, (float)GetScreenHeight() / 2};
    this->state = IDLE;
    this->invincible = true; // invincible when spawns
    this->usingBoost = false;
    this->boostTime = BOOST_TIME;
    this->lastBoostUsedTime = 0;
    this->bullets = {};
    this->lastShotTime = 0;
    this->lastDeathTime = 0;
    this->type = PLAYER;
    this->texture = ResourceManager::GetSpriteTexture(PLAYER_SPRITES);
    this->shootSound = ResourceManager::CreateSoundAlias(BULLET_SOUND);
    this->thrustSound = ResourceManager::CreateSoundAlias(THRUST_SOUND);
    this->explosionSound = ResourceManager::CreateSoundAlias(SHIP_EXPLOSION_SOUND);

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
    int fireRateUpgradeCount = 0;
    int bulletSpeedUpgradeCount = 0;
    int bulletSpreadUpgradeCount = 0;
    bulletsPerShot = 1;

    for (size_t i = 0; i < powerups.size(); i++)
    {
        powerups[i]->Update();
        powerups[i]->UpdateBounds(this->bounds);
        switch (powerups[i]->GetType())
        {
        case TEMPORARY_SHIELD:
            invincible = powerups[i]->CanBeApplied();
            break;
        case TEMPORARY_INFINITE_BOOST:
            boostTime = powerups[i]->CanBeApplied() ? BOOST_TIME : boostTime;
            break;
        case FIRE_RATE_UPGRADE:
            fireRateUpgradeCount += powerups[i]->CanBeApplied() ? 1 : 0;
            break;
        case BULLET_SPEED_UPGRADE:
            bulletSpeedUpgradeCount += powerups[i]->CanBeApplied() ? 1 : 0;
            break;
        case BULLET_SPREAD_UPGRADE:
            bulletSpreadUpgradeCount += powerups[i]->CanBeApplied() ? 1 : 0;
            break;
        case EXTRA_BULLET_UPGRADE:
            bulletsPerShot += powerups[i]->CanBeApplied() ? 1 : 0;
            break;
        default:
            break;
        }
        if (!powerups[i]->CanBeApplied())
        {
            delete powerups[i];
            powerups.erase(powerups.begin() + i);
        }
    }
    shootCooldown = CHARACTER_SHOOT_COOLDOWN * powf(0.7f, fireRateUpgradeCount) * powf(1.2f, bulletsPerShot - 1);
    bulletsSpeed = BULLET_SPEED * powf(1.2f, bulletSpeedUpgradeCount);
    bulletsSpread = BULLET_SPREAD * powf(0.82f, bulletSpreadUpgradeCount);
    powerups.shrink_to_fit();
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
        for (size_t i = 0; i < powerups.size(); i++)
        {
            powerups[i]->Draw();
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

    std::string powerupsText = "Powerups: [";
    for (size_t i = 0; i < powerups.size(); i++)
    {
        const char *typeText = PowerUp::GetPowerUpName(powerups[i]->GetType());
        if (i > 0)
        {
            powerupsText += ", ";
        }
        powerupsText += typeText;

        powerups[i]->DrawDebug();
    }
    powerupsText += "]";

    DrawText(powerupsText.c_str(), 10, 60, 16, WHITE);
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
        lives++;
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
        powerups.push_back(powerup);
        return true;
    }

    if (powerup->GetType() == TEMPORARY_SHIELD || powerup->GetType() == TEMPORARY_INFINITE_BOOST)
    {
        powerup->PickUp();
        PowerUp *temporaryPowerUp = GetPowerup(powerup->GetType());
        if (temporaryPowerUp != nullptr)
        {
            temporaryPowerUp->ResetUseTime();
        }
        else
        {
            powerup->UpdateBounds(this->bounds);
            powerups.push_back(powerup);
        }
        return true;
    }
    if (powerup->GetType() == FIRE_RATE_UPGRADE)
    {
        int fireRateUpgradeCount = CountPowerup(FIRE_RATE_UPGRADE);
        if (fireRateUpgradeCount >= 3)
        {
            return false;
        }
        powerup->UpdateBounds(this->bounds);
        powerups.push_back(powerup);
        return true;
    }
    if (powerup->GetType() == BULLET_SPEED_UPGRADE || powerup->GetType() == BULLET_SPREAD_UPGRADE || powerup->GetType() == EXTRA_BULLET_UPGRADE)
    {
        int bulletSpeedUpgradeCount = CountPowerup(powerup->GetType());
        if (bulletSpeedUpgradeCount >= MAX_UPGRADES_PER_TYPE)
        {
            return false;
        }
        powerup->UpdateBounds(this->bounds);
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
            delete powerups[i];
            powerups.erase(powerups.begin() + i);
            return true;
        }
    }
    return false;
}

bool Player::HasPowerup(PowerUpType type)
{
    for (size_t i = 0; i < powerups.size(); i++)
    {
        if (powerups[i]->GetType() == type)
        {
            return true;
        }
    }
    return false;
}

int Player::CountPowerup(PowerUpType type)
{
    int count = 0;
    for (size_t i = 0; i < powerups.size(); i++)
    {
        if (powerups[i]->GetType() == type)
        {
            count++;
        }
    }
    return count;
}

bool Player::CanBeKilled()
{
    return !invincible;
}

bool Player::CanBeHit()
{
    return !invincible || HasPowerup(SHIELD) || HasPowerup(TEMPORARY_SHIELD);
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
    this->forwardDir = {0, -1};
    this->velocity = {0, 0};
    this->angularVelocity = 0;
    this->state = IDLE;
    this->lastDeathTime = 0;
    this->lastShotTime = 0;
    this->lastBoostUsedTime = 0;
    this->usingBoost = false;
    this->boostTime = BOOST_TIME;
    this->shootCooldown = CHARACTER_SHOOT_COOLDOWN;
    this->bulletsPerShot = 1;
    this->bulletsSpeed = BULLET_SPEED;
    this->bulletsSpread = BULLET_SPREAD;
    // leave bullets live
    SetDefaultHitBox();

    for (size_t i = 0; i < powerups.size(); i++) // TODO: maybe don't delete powerups when respawning
    {
        delete powerups[i];
    }
    powerups.clear();
}

void Player::Reset()
{
    Respawn();
    this->lives = 4;
    this->bullets.clear();
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

void Player::SetDefaultHitBox()
{
    this->hitbox = {{0.0f, -0.4f}, {0.4f, 0.35f}, {-0.4f, 0.35f}}; // up, right-down, left-down

    for (size_t i = 0; i < hitbox.size(); i++)
    {
        this->hitbox[i] = Vector2Add(Vector2Scale(this->hitbox[i], CHARACTER_SIZE / 2), GetOrigin());
    }
}

PowerUp *Player::GetPowerup(PowerUpType type)
{
    for (size_t i = 0; i < powerups.size(); i++)
    {
        if (powerups[i]->GetType() == type)
        {
            return powerups[i];
        }
    }
    return nullptr;
}
