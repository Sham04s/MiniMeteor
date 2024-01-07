#include "game/objects/power_up.hpp"
#include "game/objects/player.hpp"

#include <map>

const std::map<PowerUpType, SpriteTextureID> powerUpSpriteItemMap = {
    {SHIELD, POWERUP_SHIELD_ITEM_SPRITE},
    {TEMPORARY_SHIELD, POWERUP_TEMPORARY_SHIELD_ITEM_SPRITE},
    {SHOOT_COOLDOWN_UPGRADE, POWERUP_SHOOT_COOLDOWN_UPGRADE_ITEM_SPRITE},
    {BULLET_SPREAD_UPGRADE, POWERUP_BULLET_SPREAD_UPGRADE_ITEM_SPRITE},
    {EXTRA_BULLET_UPGRADE, POWERUP_EXTRA_BULLET_UPGRADE_ITEM_SPRITE},
    {BULLET_SPEED_UPGRADE, POWERUP_BULLET_SPEED_UPGRADE_ITEM_SPRITE},
    {TEMPORARY_INFINITE_BOOST, POWERUP_TEMPORARY_INFINITE_BOOST_ITEM_SPRITE},
    {LIFE, POWERUP_LIFE_ITEM_SPRITE},
};

PowerUp::PowerUp(Vector2 origin, PowerUpType type) : GameObject()
{
    this->origin = origin;
    this->bounds = {origin.x - POWER_UP_SIZE / 2, origin.y - POWER_UP_SIZE / 2, POWER_UP_SIZE, POWER_UP_SIZE};
    this->hitbox = {{bounds.x, bounds.y}, {bounds.x + bounds.width, bounds.y}, {bounds.x + bounds.width, bounds.y + bounds.height}, {bounds.x, bounds.y + bounds.height}};
    this->powerupType = type;
    this->type = POWER_UP;
    this->pickedUp = false;
    this->drawable = true;
    this->shaking = false;
    this->shakingTime = 0.0f;
    this->lastShakeTime = 0.0f;
    this->timeToLive = POWER_UP_TIME_TO_LIVE;
    this->effectiveUseTime = 0.0f;

    this->spawnSound = ResourceManager::GetSound(POWERUP_SPAWN_SOUND);
    this->pickupSound = ResourceManager::GetSound(POWERUP_PICKUP_SOUND);
    this->cantPickupSound = ResourceManager::GetSound(POWERUP_CANT_PICKUP_SOUND);
    this->texture = ResourceManager::GetInvalidTexture();
    if (powerUpSpriteItemMap.find(type) != powerUpSpriteItemMap.end())
    {
        this->texture = ResourceManager::GetSpriteTexture(powerUpSpriteItemMap.at(type));
    }
    PlaySound(*spawnSound);
}

PowerUp::~PowerUp()
{
}

void PowerUp::Update()
{
    if (pickedUp)
    {
        effectiveUseTime = fmaxf(effectiveUseTime - GetFrameTime(), 0.0f);
        return;
    }
    if (shaking)
    {
        Translate({(float)GetRandomValue(-1, 1), 0});
        shakingTime += GetFrameTime();
        if (shakingTime >= POWER_UP_SHAKE_TIME)
        {
            shaking = false;
            shakingTime = 0.0f;
        }
    }
    GameObject::Update();
    timeToLive = fmaxf(timeToLive - GetFrameTime(), 0.0f);
}

void PowerUp::Draw()
{
    if ((!pickedUp && timeToLive <= 0.0f) || !drawable)
    {
        return;
    }
    if (pickedUp)
    {
        GameObject::Draw();
        return;
    }

    Color colorTint = WHITE;
    Rectangle dst = {origin.x, origin.y, bounds.width, bounds.height};

    if (timeToLive > POWER_UP_BLINK_TIME) // is still good
    {
        float alpha = 0.75f + 0.25f * sinf(2 * PI * (timeToLive / POWER_UP_PULSE_PERIOD));
        colorTint = Fade(WHITE, alpha);
        DrawTexturePro(*texture, {0, 0, (float)texture->width, (float)texture->height}, dst, {bounds.width / 2, bounds.height / 2}, rotation, colorTint);
        return;
    }

    if (fmodf(timeToLive, POWER_UP_BLINK_PERIOD) < POWER_UP_BLINK_PERIOD / 2.0f) // is blinking
    {
        DrawTexturePro(*texture, {0, 0, (float)texture->width, (float)texture->height}, dst, {bounds.width / 2, bounds.height / 2}, rotation, colorTint);
        return;
    }
}

void PowerUp::DrawDebug()
{
    if (pickedUp && !drawable)
    {
        return;
    }
    GameObject::DrawDebug();
    const char *typeText = GetPowerUpName(powerupType);
    DrawText(typeText, bounds.x, bounds.y + bounds.height + 5, 16, WHITE);
}

void PowerUp::HandleCollision(GameObject *other, Vector2 *pushVector)
{
    // do nothing
    (void)other;
    *pushVector = {0};
}

void PowerUp::PauseSounds()
{
    PauseSound(*spawnSound);
    PauseSound(*pickupSound);
    PauseSound(*cantPickupSound);
}

void PowerUp::ResumeSounds()
{
    ResumeSound(*spawnSound);
    ResumeSound(*pickupSound);
    ResumeSound(*cantPickupSound);
}

void PowerUp::PickUp()
{
    PlaySound(*pickupSound);
    pickedUp = true;
    drawable = false;
    timeToLive = 0.0f;
    hitbox.clear();
    switch (powerupType)
    {
    case SHIELD:
        this->texture = ResourceManager::GetSpriteTexture(POWERUP_SHIELD_SPRITE);
        drawable = true;
        break;
    case TEMPORARY_SHIELD:
        this->texture = ResourceManager::GetSpriteTexture(POWERUP_SHIELD_SPRITE);
        drawable = true;
        break;
    case TEMPORARY_INFINITE_BOOST:
        break;
    case SHOOT_COOLDOWN_UPGRADE:
        break;
    case BULLET_SPEED_UPGRADE:
        break;
    case EXTRA_BULLET_UPGRADE:
        break;
    case BULLET_SPREAD_UPGRADE:
        break;
    case LIFE:
        break;
    default:
        break;
    }

    ResetUseTime();
}

void PowerUp::AnimateCantPickup()
{
    if (shaking || pickedUp || GetTime() - lastShakeTime < POWER_UP_SHAKE_TIME * 6)
    {
        return;
    }
    PlaySound(*cantPickupSound);
    this->shaking = true;
    this->lastShakeTime = GetTime();
}

void PowerUp::UpdateBounds(Rectangle playerBounds)
{
    this->bounds = {playerBounds.x + playerBounds.width / 2 - POWER_UP_SIZE / 2, playerBounds.y + playerBounds.height / 2 - POWER_UP_SIZE / 2, POWER_UP_SIZE, POWER_UP_SIZE};
    this->origin = {bounds.x + bounds.width / 2, bounds.y + bounds.height / 2};
}

void PowerUp::ResetUseTime()
{
    effectiveUseTime = INFINITY;
    switch (powerupType)
    {
    case SHIELD:
        break;
    case TEMPORARY_SHIELD:
        effectiveUseTime = TEMPORARY_SHIELD_TIME;
        break;
    case TEMPORARY_INFINITE_BOOST:
        effectiveUseTime = TEMPORARY_INFINITE_BOOST_TIME;
        break;
    case SHOOT_COOLDOWN_UPGRADE:
        break;
    case LIFE:
        break;
    default:
        break;
    }
}

const char *PowerUp::GetPowerUpName(PowerUpType type)
{
    switch (type)
    {
    case SHIELD:
        return "SHIELD";
    case TEMPORARY_SHIELD:
        return "TEMPORARY_SHIELD";
    case TEMPORARY_INFINITE_BOOST:
        return "TEMPORARY_INFINITE_BOOST";
    case SHOOT_COOLDOWN_UPGRADE:
        return "SHOOT_COOLDOWN_UPGRADE";
    case BULLET_SPEED_UPGRADE:
        return "BULLET_SPEED_UPGRADE";
    case BULLET_SPREAD_UPGRADE:
        return "BULLET_SPREAD_UPGRADE";
    case EXTRA_BULLET_UPGRADE:
        return "EXTRA_BULLET_UPGRADE";
    case LIFE:
        return "LIFE";
    default:
        return "UNKNOWN";
    }
}