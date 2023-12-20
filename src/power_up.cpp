#include "power_up.hpp"
#include "player.hpp"

#include <map>

// TODO: make a consumable item class and make powerups inherit from it

const std::map<PowerUpType, SpriteTextureID> powerUpSpriteItemMap = {
    {SHIELD, POWERUP_SHIELD_ITEM_SPRITE},
    {TEMPORARY_SHIELD, POWERUP_TEMPORARY_SHIELD_ITEM_SPRITE},
    // {TEMPORARY_INFINITE_BOOST, POWERUP_SHIELD_SPRITE},
    // {FIRE_RATE_UPGRADE, POWERUP_SHIELD_SPRITE},
    {LIFE, POWERUP_LIFE_ITEM_SPRITE},
};

PowerUp::PowerUp(Vector2 origin, PowerUpType type) : GameObject()
{
    this->origin = origin;
    this->bounds = {origin.x - POWER_UP_SIZE / 2, origin.y - POWER_UP_SIZE / 2, POWER_UP_SIZE, POWER_UP_SIZE};
    this->hitbox = {{bounds.x, bounds.y}, {bounds.x + bounds.width, bounds.y}, {bounds.x + bounds.width, bounds.y + bounds.height}, {bounds.x, bounds.y + bounds.height}};
    this->type = type;
    GameObject::type = POWER_UP;
    this->pickedUp = false;
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
    // TODO: draw power up
    if (!pickedUp && timeToLive <= 0.0f)
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
    GameObject::DrawDebug();
    const char *typeText = "";
    switch (type)
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
    DrawText(typeText, bounds.x, bounds.y + bounds.height + 5, 16, WHITE);
}

void PowerUp::PickUp()
{
    pickedUp = true;
    timeToLive = 0.0f;
    hitbox.clear();
    switch (type)
    {
    case SHIELD:
        this->texture = ResourceManager::GetSpriteTexture(POWERUP_SHIELD_SPRITE);
        break;
    case TEMPORARY_SHIELD:
        this->texture = ResourceManager::GetSpriteTexture(POWERUP_TEMPORARY_SHIELD_SPRITE);
        effectiveUseTime = TEMPORARY_SHIELD_TIME;
        break;
    case TEMPORARY_INFINITE_BOOST:
        effectiveUseTime = TEMPORARY_BOOST_TIME;
        break;
    case FIRE_RATE_UPGRADE:
        break;
    case LIFE:
        break;
    default:
        break;
    }
}

void PowerUp::UpdateBounds(Rectangle playerBounds)
{
    this->bounds = {playerBounds.x + playerBounds.width / 2 - POWER_UP_SIZE / 2, playerBounds.y + playerBounds.height / 2 - POWER_UP_SIZE / 2, POWER_UP_SIZE, POWER_UP_SIZE};
    this->origin = {bounds.x + bounds.width / 2, bounds.y + bounds.height / 2};
}

void PowerUp::ResetUseTime()
{
    switch (type)
    {
    case SHIELD:
        break;
    case TEMPORARY_SHIELD:
        effectiveUseTime = TEMPORARY_SHIELD_TIME;
        break;
    case TEMPORARY_INFINITE_BOOST:
        effectiveUseTime = TEMPORARY_BOOST_TIME;
        break;
    case FIRE_RATE_UPGRADE:
        break;
    case LIFE:
        break;
    default:
        break;
    }
}