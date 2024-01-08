#include "game/objects/player.hpp"
#include "game/objects/shooter.hpp"

#include <math.h>
#include <string>

Player::Player(Vector2 origin) : Character(origin)
{
    this->initialOrigin = origin;
    this->type = PLAYER; // Add braces here
    this->camera = {
        .offset = {GetScreenWidth() / 2.0f, GetScreenHeight() / 2.0f},
        .target = {0, 0},
        .rotation = 0.0f,
        .zoom = 1.0f};

    this->texture = ResourceManager::GetSpriteTexture(PLAYER_SPRITES);
    this->crosshair = ResourceManager::GetSpriteTexture(CROSSHAIR_SPRITE);
    this->shootSound = ResourceManager::CreateSoundAlias(BULLET_SOUND);
    this->thrustSound = ResourceManager::CreateSoundAlias(THRUST_SOUND);
    this->explosionSound = ResourceManager::CreateSoundAlias(SHIP_EXPLOSION_SOUND);

    this->changeToDirShipSound = ResourceManager::GetSound(CHANGE_TO_DIR_SHIP_SOUND);
    this->changeToShipSound = ResourceManager::GetSound(CHANGE_TO_NORMAL_SHIP_SOUND);

    Reset();
    Hide();
}

Player::~Player()
{
}

void Player::Update()
{
    // invincibility ends when player starts moving or shooting
    if (invincible && ((state & ACCELERATING) || usingBoost || lastShootTime > 0.0f))
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

    if (directionalShip && IsAlive())
    {
        directionalShipMeter -= 10.0f * GetFrameTime(); // (MAX / 10) seconds

        // disable directional ship if meter is empty
        if (directionalShipMeter <= 0.0f)
        {
            directionalShipMeter = 0.0f;
            ToggleDirectionalShip();
        }
    }

    if (changingShip)
    {
        static bool changed = false;
        changingShipTime += GetFrameTime();
        if (changingShipTime >= CHANGING_SHIP_TIME && !changed)
        {
            directionalShip = !directionalShip;
            changed = true;
        }
        if (changingShipTime >= CHANGING_SHIP_TIME * 2)
        {
            changingShip = false;
            changingShipTime = 0.0f;
            changed = false;
            if (!directionalShip)
            {
                accelDir = forwardDir;
                SetDefaultHitBox();
            }
            else
            {
                SetDirectionalShipHitBox();
            }
            state &= ~ACCELERATING;
        }
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
    shootCooldown = CHARACTER_SHOOT_COOLDOWN * GetPowerupMultiplier(SHOOT_COOLDOWN_UPGRADE);
    bulletsSpeed = BULLET_SPEED * GetPowerupMultiplier(BULLET_SPEED_UPGRADE);
    bulletsSpread = BULLET_SPREAD * GetPowerupMultiplier(BULLET_SPREAD_UPGRADE);
}

void Player::Draw()
{
    if (hidden)
    {
        return;
    }

    Character::Draw();

    const float widthScaleFactor = 0.8f;
    Rectangle bar = {bounds.x + bounds.width * (1.0f - widthScaleFactor) / 2, bounds.y + bounds.width - 20,
                     bounds.width * widthScaleFactor, 5};

    // draw boost bar
    PowerUp *temporaryInfiniteBoost = GetPowerup(TEMPORARY_INFINITE_BOOST);
    const float timeSinceBoost = GetTime() - lastBoostUsedTime;
    if (IsAlive() && (temporaryInfiniteBoost != nullptr || timeSinceBoost < BOOST_BAR_HIDE_TIME + BOOST_BAR_FADE_TIME))
    {
        Rectangle boostBar = bar;
        bar.y += bar.height + 5;
        if (temporaryInfiniteBoost == nullptr)
        {
            // draw temporary infinite boost bar
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

    // draw directional ship meter
    if (IsAlive() && directionalShip)
    {
        Rectangle directionalShipMeterRect = bar;
        bar.y += bar.height + 5;
        directionalShipMeterRect.width = directionalShipMeterRect.width * directionalShipMeter / DIRECTIONAL_SHIP_METER_MAX;
        DrawRectangleRounded(directionalShipMeterRect, 0.5f, 0, WHITE);
    }

    if (IsAlive())
    {
        if (changingShip)
        {
            static Vector2 originalSize = {0, 0};
            if (originalSize.x == 0 && originalSize.y == 0)
            {
                originalSize = {bounds.width, bounds.height};
            }

            // changingShipTime <= CHANGING_SHIP_TIME -> shrinking
            // changingShipTime > CHANGING_SHIP_TIME -> growing
            float scaleFactor = 1 + (changingShipTime <= CHANGING_SHIP_TIME ? -1.0f : 1.0f) * GetFrameTime() / (CHANGING_SHIP_TIME);

            this->Scale(scaleFactor);
        }

        if (directionalShip)
        {
            // draw mouse crosshair
            Vector2 mousePos = GetScreenToWorld2D(GetMousePosition(), camera);
            Vector2 size = {32, 32};
            DrawTexturePro(*crosshair, {0, 0, (float)crosshair->width, (float)crosshair->height},
                           {mousePos.x - size.x / 2, mousePos.y - size.y / 2, size.x, size.y}, {0, 0}, 0, WHITE);
        }

        for (auto powerup : powerups)
        {
            powerup->Draw();
        }

        // draw temporary shield timer
        PowerUp *temporaryShield = GetPowerup(TEMPORARY_SHIELD);
        if (temporaryShield != nullptr)
        {
            const float timeLeft = temporaryShield->GetEffectiveUseTime();
            Rectangle shieldTimer = bar;
            bar.y += bar.height + 5;
            shieldTimer.width = shieldTimer.width * timeLeft / TEMPORARY_SHIELD_TIME;
            DrawRectangleRounded(shieldTimer, 0.5f, 0, SKYBLUE);
        }
    }
}

void Player::DrawDebug()
{
    if (hidden)
    {
        auto originalHitbox = hitbox;
        hitbox.clear();
        Character::DrawDebug();
        hitbox = originalHitbox;
        return;
    }
    Character::DrawDebug();

    Vector2 belowPlayer = GetWorldToScreen2D({origin.x, origin.y + CHARACTER_SIZE + 10}, camera);
    DrawText(TextFormat("Invincible: %s", invincible ? "true" : "false"), belowPlayer.x, belowPlayer.y, 20, invincible ? GREEN : RED);
}

void Player::HandleInput()
{
    if (!IsAlive() || hidden)
    {
        return;
    }
#ifdef _DEBUG
    if (IsMouseButtonPressed(MOUSE_BUTTON_SIDE))
    {
        directionalShipMeter = DIRECTIONAL_SHIP_METER_MAX;
    }
#endif

    if (directionalShip)
    {
        int wasdMask = IsKeyDown(KEY_D) << 0 | IsKeyDown(KEY_S) << 1 | IsKeyDown(KEY_A) << 2 | IsKeyDown(KEY_W) << 3;
        accelDir = {0, 0};
        if (wasdMask)
        {
            accelDir.x = (bool)(wasdMask & 1) - (bool)(wasdMask & 4); // 1 if D, -1 if A, 0 otherwise
            accelDir.y = (bool)(wasdMask & 2) - (bool)(wasdMask & 8); // 1 if S, -1 if W, 0 otherwise
        }
        if (accelDir.x != 0 || accelDir.y != 0)
        {
            state |= ACCELERATING;
        }
        else
        {
            state &= ~ACCELERATING;
            usingBoost = false;
        }

        Vector2 mousePos = GetScreenToWorld2D(GetMousePosition(), camera);
        float angle = Vector2Angle(forwardDir, Vector2Subtract(mousePos, origin));
        Rotate(angle * RAD2DEG);
    }
    else
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
    }

    if (IsKeyDown(KEY_LEFT_SHIFT) && this->state & ACCELERATING)
    {
        usingBoost = true;
    }
    if (IsKeyUp(KEY_LEFT_SHIFT) && usingBoost)
    {
        usingBoost = false;
    }

    if (IsKeyDown(KEY_SPACE) || IsMouseButtonDown(MOUSE_LEFT_BUTTON))
    {
        Shoot();
    }
    if (IsKeyPressed(KEY_Q))
    {
        ToggleDirectionalShip();
    }
}

bool Player::CheckCollision(GameObject *other, Vector2 *pushVector)
{
    if (hidden)
    {
        return false;
    }
    return Character::CheckCollision(other, pushVector);
}

void Player::HandleCollision(GameObject *other, Vector2 *pushVector)
{
    if (!IsAlive())
    {
        return;
    }
    if (other->GetType() == ENEMY)
    {
        Shooter *enemy = (Shooter *)other;
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
            IncreaseDirectionalShipMeter(ENEMY_SHOOTER_KILLED);
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
            IncreaseDirectionalShipMeter(asteroid->GetVariant() == LARGE ? LARGE_ASTEROID_DESTROYED : SMALL_ASTEROID_DESTROYED);
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
        else
        {
            powerup->Shake();
        }
        return;
    }
}

void Player::HandleBulletCollision(Bullet *bullet, GameObject *other, Vector2 *pushVector)
{
    bullet->HandleCollision(other, pushVector);
    if (other->GetType() == ASTEROID)
    {
        Asteroid *asteroid = (Asteroid *)other;
        IncreaseDirectionalShipMeter(asteroid->GetVariant() == LARGE ? LARGE_ASTEROID_DESTROYED : SMALL_ASTEROID_DESTROYED);
    }
    if (other->GetType() == ENEMY)
    {
        IncreaseDirectionalShipMeter(ENEMY_SHOOTER_KILLED);
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
        if (HasPowerup(SHIELD) || HasPowerup(TEMPORARY_SHIELD))
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
        if (powerup->GetType() == TEMPORARY_SHIELD && HasPowerup(SHIELD))
        {
            return false;
        }

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
    if (powerup->GetType() == SHOOT_COOLDOWN_UPGRADE || powerup->GetType() == BULLET_SPEED_UPGRADE || powerup->GetType() == BULLET_SPREAD_UPGRADE || powerup->GetType() == EXTRA_BULLET_UPGRADE)
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
    this->invincible = true; // invincible when respawns
    this->hasMoved = false;
    this->usingBoost = false;
    this->boostTime = BOOST_TIME;
    this->lastBoostUsedTime = 0;
    this->forwardDir = {0, -1};
    this->accelDir = forwardDir;
    this->directionalShip = false;
    this->velocity = {0, 0};
    this->angularVelocity = 0;
    this->state = IDLE;
    this->lastDeathTime = 0;
    this->lastShootTime = 0;
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
    // Reset all variables to default values
    Respawn();
    this->lives = PLAYER_INITIAL_LIVES;
    this->directionalShipMeter = DIRECTIONAL_SHIP_METER_MAX / 3;
    this->directionalShip = false;
    this->changingShip = false;
    this->changingShipTime = 0.0f;

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

void Player::ToggleDirectionalShip()
{
    if (changingShip || !IsAlive())
    {
        return;
    }
    if (!directionalShip && directionalShipMeter <= 0.0f)
    {
        return;
    }

    if (directionalShip)
    {
        PlaySound(*changeToShipSound);
    }
    else
    {
        PlaySound(*changeToDirShipSound);
    }
    changingShip = true;
}

void Player::IncreaseDirectionalShipMeter(ScoreType scoreType)
{
    switch (scoreType)
    {
    case ENEMY_SHOOTER_KILLED:
        directionalShipMeter += 10.0f;
        break;
    case LARGE_ASTEROID_DESTROYED:
        directionalShipMeter += 6.0f;
        break;
    case SMALL_ASTEROID_DESTROYED:
        directionalShipMeter += 3.0f;
        break;
    default:
        return;
    }

    directionalShipMeter = fminf(directionalShipMeter, DIRECTIONAL_SHIP_METER_MAX);
}

Rectangle Player::GetFrameRec()
{
    // see resources/characters/player/player.png  [0..11]
    // [0..2] -> default ship -> idle, accelerating, boost
    // [3..11] -> directional ship -> idle, accelerating * 4 dirs, boost * 4 dirs
    // dir -> up, left, down, right (the thrust is in the opposite direction)
    // angle ranges -> [-45, 45], [45, 135], [135..180, -180..-135], [-135, -45]

    int frame = 0;

    if (!directionalShip)
    {
        if (state & ACCELERATING)
        {
            frame = (usingBoost && boostTime > 0) ? 2 : 1;
        }
    }
    else
    {
        frame = 3; // idle
        if (state & ACCELERATING)
        {
            float angle = -Vector2Angle(forwardDir, accelDir) * RAD2DEG;
            int dir = 0;
            if (angle >= -45 && angle < 45)
            {
                dir = 0;
            }
            else if (angle >= 45 && angle < 135)
            {
                dir = 1;
            }
            else if (angle >= 135 || angle < -135)
            {
                dir = 2;
            }
            else if (angle >= -135 && angle < -45)
            {
                dir = 3;
            }
            else
            {
                dir = -1; // error
            }
            frame = 4 + dir + (usingBoost && boostTime > 0 ? 4 : 0);
        }
    }

    return ResourceManager::GetSpriteSrcRect(PLAYER_SPRITES, frame);
}

float Player::GetPowerupMultiplier(PowerUpType type)
{
    int count = powerupsCount[type];
    count += directionalShip * 2; // directional ship slightly increases powerup multiplier but not bullet per shot

    switch (type)
    {
    case SHOOT_COOLDOWN_UPGRADE:
        return powf(SHOOT_COOLDOWN_MULTIPLIER, count) * powf(BULLETS_PER_SHOT_MULTIPLIER, bulletsPerShot - 1);
    case BULLET_SPEED_UPGRADE:
        return powf(BULLETS_SPEED_MULTIPLIER, count);
    case BULLET_SPREAD_UPGRADE:
        return powf(BULLETS_SPREAD_MULTIPLIER, count);
    default:
        return count;
    }
}

void Player::UpdateCamera()
{
    camera.offset = {GetScreenWidth() / 2.0f, GetScreenHeight() / 2.0f};
}

void Player::SetDefaultHitBox()
{
    this->hitbox = {{0.0f, -0.4f}, {0.4f, 0.35f}, {-0.4f, 0.35f}, {0.0f, -0.4f}}; // up, right-down, left-down, close polygon

    // scale to size -> rotate -> translate
    for (size_t i = 0; i < hitbox.size(); i++)
    {
        this->hitbox[i] = Vector2Add(Vector2Rotate(Vector2Scale(this->hitbox[i], CHARACTER_SIZE / 2), rotation * DEG2RAD), GetOrigin());
    }
}

void Player::SetDirectionalShipHitBox()
{
    this->hitbox = {
        {-0.25f, -0.4f},
        {0.25f, -0.4f},
        {0.4f, -0.1f},
        {0.4f, 0.25},
        {0.1f, 0.4f},
        {-0.122f, 0.4f},
        {-0.4f, 0.25f},
        {-0.4f, -0.1f},
        {-0.25f, -0.4f}, // close polygon
    };

    // scale to size -> rotate -> translate
    for (size_t i = 0; i < hitbox.size(); i++)
    {
        this->hitbox[i] = Vector2Add(Vector2Rotate(Vector2Scale(this->hitbox[i], CHARACTER_SIZE / 2), rotation * DEG2RAD), GetOrigin());
    }
}
