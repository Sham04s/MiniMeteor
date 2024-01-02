#include "player_powerups.hpp"
#include "resource_manager.hpp"

PlayerPowerups::PlayerPowerups(Player *player, CenteredPosition position)
    : UIObject()
{
    this->player = player;
    this->position = position;
    this->texture = ResourceManager::GetDefaultTexture();

    powerupTextures[SHIELD] = ResourceManager::GetSpriteTexture(POWERUP_SHIELD_ITEM_SPRITE);
    powerupTextures[TEMPORARY_SHIELD] = ResourceManager::GetSpriteTexture(POWERUP_TEMPORARY_SHIELD_ITEM_SPRITE);
    powerupTextures[TEMPORARY_INFINITE_BOOST] = ResourceManager::GetSpriteTexture(POWERUP_TEMPORARY_INFINITE_BOOST_ITEM_SPRITE);
    powerupTextures[FIRE_RATE_UPGRADE] = ResourceManager::GetSpriteTexture(POWERUP_FIRE_RATE_UPGRADE_ITEM_SPRITE);
    powerupTextures[BULLET_SPEED_UPGRADE] = ResourceManager::GetSpriteTexture(POWERUP_BULLET_SPEED_UPGRADE_ITEM_SPRITE);
    powerupTextures[BULLET_SPREAD_UPGRADE] = ResourceManager::GetSpriteTexture(POWERUP_BULLET_SPREAD_UPGRADE_ITEM_SPRITE);
    powerupTextures[EXTRA_BULLET_UPGRADE] = ResourceManager::GetSpriteTexture(POWERUP_EXTRA_BULLET_UPGRADE_ITEM_SPRITE);
    powerupTextures[LIFE] = ResourceManager::GetSpriteTexture(POWERUP_LIFE_ITEM_SPRITE); // this is not used
}

PlayerPowerups::~PlayerPowerups()
{
}

void PlayerPowerups::Update()
{
    if (player->IsDead())
    {
        return;
    }

    // unique powerups
    static size_t prevNumPowerups = 0;
    size_t numPowerups = 0;
    for (size_t i = 0; i < NUM_POWER_UP_TYPES; i++)
    {
        if (player->HasPowerup((PowerUpType)i))
        {
            numPowerups++;
        }
    }
    if (prevNumPowerups == numPowerups)
    {
        return;
    }
    prevNumPowerups = numPowerups;

    // calculate bounds based on number of powerups and position
    Vector2 topLeft = {0, 0};
    Vector2 size = {0, 0};
    Vector2 parentTopLeft = {parent == nullptr ? 0 : parent->GetBounds().x,
                             parent == nullptr ? 0 : parent->GetBounds().y};
    Vector2 parentSize = {parent == nullptr ? GetScreenWidth() : parent->GetBounds().width,
                          parent == nullptr ? GetScreenHeight() : parent->GetBounds().height};
    switch (position)
    {
    case TOP:
        topLeft.x = parentSize.x / 2.0f - (numPowerups * ICONS_SIZE + (numPowerups - 1) * ICONS_SPACING) / 2.0f;
        topLeft.y = ICONS_SPACING;
        size.x = numPowerups * ICONS_SIZE + (numPowerups - 1) * ICONS_SPACING;
        size.y = ICONS_SIZE;
        break;
    case BOTTOM:
        topLeft.x = parentSize.x / 2.0f - (numPowerups * ICONS_SIZE + (numPowerups - 1) * ICONS_SPACING) / 2.0f;
        topLeft.y = parentSize.y - ICONS_SIZE - ICONS_SPACING;
        size.x = numPowerups * ICONS_SIZE + (numPowerups - 1) * ICONS_SPACING;
        size.y = ICONS_SIZE;
        break;
    case LEFT:
        topLeft.x = ICONS_SPACING;
        topLeft.y = parentSize.y / 2.0f - (numPowerups * ICONS_SIZE + (numPowerups - 1) * ICONS_SPACING) / 2.0f;
        size.x = ICONS_SIZE;
        size.y = numPowerups * ICONS_SIZE + (numPowerups - 1) * ICONS_SPACING;
        break;
    case RIGHT:
        topLeft.x = parentSize.x - ICONS_SIZE - ICONS_SPACING;
        topLeft.y = parentSize.y / 2.0f - (numPowerups * ICONS_SIZE + (numPowerups - 1) * ICONS_SPACING) / 2.0f;
        size.x = ICONS_SIZE;
        size.y = numPowerups * ICONS_SIZE + (numPowerups - 1) * ICONS_SPACING;
        break;
    default:
        break;
    }

    this->relBounds = {topLeft.x, topLeft.y, size.x, size.y};
    if (this->parent != nullptr)
    {
        this->bounds.x = parentTopLeft.x + this->relBounds.x;
        this->bounds.y = parentTopLeft.y + this->relBounds.y;
        this->bounds.width = this->relBounds.width;
        this->bounds.height = this->relBounds.height;
    }
    else
    {
        this->bounds = this->relBounds;
    }
}

void PlayerPowerups::Draw()
{
    UIObject::Draw();

    Rectangle dstRect = {bounds.x, bounds.y, ICONS_SIZE, ICONS_SIZE};
    Texture2D *powerupTexture = ResourceManager::GetInvalidTexture();
    Vector2 textPos = {dstRect.x + ICONS_SIZE + ICONS_SPACING, dstRect.y + ICONS_SIZE / 2};

    Vector2 playerPos = GetWorldToScreen2D(player->GetOrigin(), player->GetCamera());
    float distanceToPlayer;
    if (position == TOP || position == BOTTOM)
    {
        distanceToPlayer = fabsf(bounds.y + bounds.height / 2 - playerPos.y) + bounds.height / 2;
    }
    else
    {
        distanceToPlayer = fabsf(bounds.x + bounds.width / 2 - playerPos.x) + bounds.width / 2;
    }
    Color tint = Fade(WHITE, 0.2f + 0.8f * fminf(distanceToPlayer / FADE_DISTANCE_THRESHOLD, 1.0f));

    for (size_t i = 0; i < NUM_POWER_UP_TYPES; i++)
    {   
        PowerUp *powerup = player->GetPowerup((PowerUpType)i);
        if (powerup == nullptr)
        {
            continue;
        }
        
        size_t count = player->GetPowerupCount((PowerUpType)i);
        powerupTexture = powerupTextures[(PowerUpType)i];

        const char *multiplierText = "";
        float multiplier = player->GetPowerupMultiplier(powerup->GetType());
        if (fmodf(multiplier, 1.0f) != 0.0f)
        {
            multiplierText = TextFormat("x%0.1f", multiplier);
        }
        else if (count > 1)
        {
            multiplierText = TextFormat("x%ld", count);
        }

        DrawTexturePro(*powerupTexture, {0, 0, (float)powerupTexture->width, (float)powerupTexture->height}, dstRect, {0, 0}, 0, tint);
        DrawTextEx(*ResourceManager::GetFont(), multiplierText, textPos, MULTIPLIER_FONT_SIZE, 0, tint);

        if (position == TOP || position == BOTTOM)
        {
            dstRect.x += ICONS_SIZE + ICONS_SPACING;
            textPos.x += ICONS_SIZE + ICONS_SPACING;
        }
        else
        {
            dstRect.y += ICONS_SIZE + ICONS_SPACING;
            textPos.y += ICONS_SIZE + ICONS_SPACING;
        }
    }
}

void PlayerPowerups::DrawDebug()
{
    UIObject::DrawDebug();
}
