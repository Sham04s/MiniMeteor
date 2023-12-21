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
    this->shootSound = ResourceManager::CreateSoundAlias(SHOOT_SOUND);
    this->thrustSound = ResourceManager::CreateSoundAlias(THRUST_SOUND);
    this->explosionSound = ResourceManager::CreateSoundAlias(SHIP_EXPLOSION_SOUND);

    SetDefaultHitBox();
}

Player::~Player()
{
}

void Player::Update()
{
    Character::Update();

    // invincibility ends when player atarts moving or shooting
    if (invincible && (state == ACCELERATING || usingBoost || lastShotTime > 0.0f))
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
        maxSpeed = CHARACTER_MAX_SPEED * 2;
        pitchAndVolumeScale = 1.0f;
    }
    else
    {
        acceleration = CHARACTER_ACCELERATION;
        maxSpeed = CHARACTER_MAX_SPEED;
        pitchAndVolumeScale = 0.7f;
    }

    // handle input if alive
    if (this->IsAlive())
    {
        HandleInput();
    }

    if (state == DEAD && lives > 0 && GetTime() - lastDeathTime > CHARACTER_RESPAWN_TIME)
    {
        Respawn();
    }

    UpdatePowerups();
}

void Player::UpdatePowerups()
{
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
            usingBoost = powerups[i]->CanBeApplied();
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
    powerups.shrink_to_fit();
}

void Player::Draw()
{
    Character::Draw();

    // draw boost bar
    const float timeSinceBoost = GetTime() - lastBoostUsedTime;
    if (IsAlive() && timeSinceBoost < BOOST_BAR_HIDE_TIME + BOOST_BAR_FADE_TIME)
    {
        Rectangle boostBar = {bounds.x + 10, bounds.y + bounds.height - 10, bounds.width - 20, 5};
        boostBar.width = boostBar.width * boostTime / BOOST_TIME;
        float alpha = 1.0f;
        if (timeSinceBoost >= BOOST_BAR_HIDE_TIME)
        {
            alpha -= (timeSinceBoost - BOOST_BAR_HIDE_TIME) / BOOST_BAR_FADE_TIME;
        }
        DrawRectangleRounded(boostBar, 0.5f, 0, Fade(GREEN, alpha));
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
            DrawRectangleRounded(shieldTimer, 0.5f, 0, BLUE);
        }
    }
}

void Player::DrawDebug()
{
    Character::DrawDebug();

    std::string powerupsText = "Powerups: [";
    for (size_t i = 0; i < powerups.size(); i++)
    {
        const char *typeText = "";
        switch (powerups[i]->GetType())
        {
        case SHIELD:
            typeText = "SHIELD";
            break;
        case TEMPORARY_SHIELD:
            typeText = "TEMPORARY_SHIELD";
            break;
        case TEMPORARY_INFINITE_BOOST:
            typeText = "TEMPORARY_INFINITE_BOOST";
            break;
        case FIRE_RATE_UPGRADE:
            typeText = "FIRE_RATE_UPGRADE";
            break;
        case LIFE:
            typeText = "LIFE";
            break;
        default:
            break;
        }

        if (i > 0)
        {
            powerupsText += ", ";
        }
        powerupsText += typeText;

        powerups[i]->DrawDebug();
    }
    powerupsText += "]";

    DrawText(powerupsText.c_str(), 10, 60, 16, WHITE);

    Rectangle belowBounds = {bounds.x, bounds.y + bounds.height, bounds.width, 20};
    DrawText(TextFormat("Invincible: %s", invincible ? "true" : "false"), belowBounds.x, belowBounds.y, 16, invincible ? GREEN : RED);
}

void Player::HandleInput()
{
    if (IsKeyPressed(KEY_W))
    {
        this->state = ACCELERATING;
    }
    if (IsKeyReleased(KEY_W))
    {
        this->state = IDLE;
        usingBoost = false;
    }
    if (IsKeyDown(KEY_LEFT_SHIFT) && this->state == ACCELERATING)
    {
        usingBoost = true;
    }
    if (IsKeyUp(KEY_LEFT_SHIFT) && usingBoost)
    {
        usingBoost = false;
    }

    if (IsKeyDown(KEY_A))
    {
        this->angularVelocity = -turnSpeed;
    }
    else if (IsKeyDown(KEY_D))
    {
        this->angularVelocity = turnSpeed;
    }
    else {
        this->angularVelocity = 0;
    }
    Rotate(angularVelocity * GetFrameTime());
    if (IsKeyPressed(KEY_SPACE))
    {
        Shoot();
    }
}

void Player::HandleCollision(GameObject *other, Vector2 *pushVector)
{
    if (other->GetType() == BASIC_ENEMY || other->GetType() == ASTEROID)
    {
        if (CanBeKilled())
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
        // powerup->PickUp();
        lives++;
        return true;
    }
    if (powerup->GetType() == TEMPORARY_SHIELD)
    {
        powerup->PickUp();
        PowerUp *temporaryShield = GetPowerup(TEMPORARY_SHIELD);
        if (temporaryShield != nullptr)
        {
            temporaryShield->ResetUseTime();
        }
        else
        {
            powerup->UpdateBounds(this->bounds);
            invincible = true;
            powerups.push_back(powerup);
        }
        return true;
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
    if (!CanBeKilled())
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
    switch (state)
    {
    case ACCELERATING:
        frame = (usingBoost && boostTime > 0) ? 2 : 1;
        break;

    case DYING:
        // frame = (int)((GetTime() - lastDeathTime) / CHARACTER_DYING_TIME * 4) + 3; // for sprites
        frame = 0; // idle
        break;

    default:
        frame = 0;
        break;
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
