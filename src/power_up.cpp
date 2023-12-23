#include "power_up.hpp"
#include "player.hpp"

#include <map>

// TODO: make a consumable item class and make powerups inherit from it

const std::map<PowerUpType, SpriteTextureID> powerUpSpriteItemMap = {
    {SHIELD, POWERUP_SHIELD_ITEM_SPRITE},
    {TEMPORARY_SHIELD, POWERUP_TEMPORARY_SHIELD_ITEM_SPRITE},
    {FIRE_RATE_UPGRADE, POWERUP_FIRE_RATE_UPGRADE_ITEM_SPRITE},
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
    this->timeToLive = POWER_UP_TIME_TO_LIVE;
    this->effectiveUseTime = 0.0f;
    this->texture = ResourceManager::GetInvalidTexture();
    if (powerUpSpriteItemMap.find(type) != powerUpSpriteItemMap.end())
    {
        this->texture = ResourceManager::GetSpriteTexture(powerUpSpriteItemMap.at(type));
    }
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

    if (timeToLive > POWER_UP_BLINK_TIME || fmodf(timeToLive, POWER_UP_BLINK_PERIOD) > POWER_UP_BLINK_PERIOD / 2)
    {
        GameObject::Draw();
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

void PowerUp::PickUp()
{
    // TODO: add sound effect
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
        this->texture = ResourceManager::GetSpriteTexture(POWERUP_TEMPORARY_SHIELD_SPRITE);
        drawable = true;
        break;
    case TEMPORARY_INFINITE_BOOST:
        break;
    case FIRE_RATE_UPGRADE:
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
    case FIRE_RATE_UPGRADE:
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
    case FIRE_RATE_UPGRADE:
        return "FIRE_RATE_UPGRADE";
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