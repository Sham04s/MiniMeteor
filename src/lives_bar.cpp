#include "lives_bar.hpp"
#include <math.h>

LivesBar::LivesBar(Rectangle bounds, Player *player)
{
    this->bounds = bounds;
    this->relBounds = bounds;
    this->player = player;
    this->lifeTexture = ResourceManager::GetUITexture(LIFE_TEXTURE);
    Resize({(float)GetScreenWidth(), (float)GetScreenHeight()});
}

LivesBar::~LivesBar()
{
}

void LivesBar::Draw()
{
    Rectangle lifeBounds = {centeredBounds.x + lifeSize * (MAX_LIVES - 1), centeredBounds.y, lifeSize, lifeSize};

    for (int i = 0; i < player->GetLives() - 1; i++)
    {
        DrawTexturePro(*lifeTexture, {0, 0, (float)lifeTexture->width, (float)lifeTexture->height}, lifeBounds, {0}, 0, WHITE);
        lifeBounds.x -= lifeSize;
    }
    if (player->HasPowerup(SHIELD))
    {
        DrawTexturePro(*lifeTexture, {0, 0, (float)lifeTexture->width, (float)lifeTexture->height}, lifeBounds, {0}, 0, BLUE);
    }
    else
    {
        DrawTexturePro(*lifeTexture, {0, 0, (float)lifeTexture->width, (float)lifeTexture->height}, lifeBounds, {0}, 0, WHITE);
    }
}

void LivesBar::DrawDebug()
{
    UIObject::DrawDebug();
}

void LivesBar::Resize(Vector2 prevScreenSize)
{
    UIObject::Resize(prevScreenSize);
    this->lifeSize = fminf(bounds.width / MAX_LIVES, bounds.height);
    this->centeredBounds = {bounds.x + bounds.width / 2 - lifeSize * MAX_LIVES / 2,
                            bounds.y + bounds.height / 2 - lifeSize / 2,
                            lifeSize * MAX_LIVES, lifeSize};
}
