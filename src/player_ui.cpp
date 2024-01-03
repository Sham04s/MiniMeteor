#include "player_ui.hpp"
#include "raylib.h"
#include "score_registry.hpp"

#include <stdio.h>

PlayerUI::PlayerUI(Player *player)
    : UIObject({0, 0, (float)GetScreenWidth(), (float)GetScreenHeight()}, nullptr, nullptr), player(player)
{
    this->player = player;
    this->livesBar = new LivesBar(Rectangle{(float)GetScreenWidth() / 2 + 100, 10, (float)GetScreenWidth() / 2 - 100, 50}, player);
    this->powerups = new PlayerPowerups(player, LEFT);
    this->scoreLabel = new Label({10, 10, 0, 0}, scoreText, WHITE, ALIGN_START, ALIGN_START, SCORE_FONT_SIZE, this);
    this->texture = ResourceManager::GetDefaultTexture();
    this->directionalShipMeterBounds = {(float)GetScreenWidth() - 200, livesBar->GetBounds().y + livesBar->GetBounds().height + 10, 200, 10};
    this->directionalShipIcon = ResourceManager::GetUITexture(DIRECTIONAL_SHIP_TEXTURE);

    AddChild(livesBar);
    AddChild(powerups);
    AddChild(scoreLabel);

    sprintf(scoreText, "%08d", GetTotalScore());
}

PlayerUI::~PlayerUI()
{
}

void PlayerUI::Update()
{
    UIObject::Update();
    sprintf(scoreText, "%08d", GetTotalScore());
}

void PlayerUI::Draw()
{
    UIObject::Draw();
    DrawRectangleLinesEx(directionalShipMeterBounds, 1, WHITE);
    const int padding = 2;

    Rectangle iconBounds = {directionalShipMeterBounds.x - DIRECTIONAL_SHIP_ICON_SIZE - padding,
                            directionalShipMeterBounds.y + directionalShipMeterBounds.height / 2 - DIRECTIONAL_SHIP_ICON_SIZE / 2,
                            DIRECTIONAL_SHIP_ICON_SIZE, DIRECTIONAL_SHIP_ICON_SIZE};

    Rectangle directionalShipMeter = {directionalShipMeterBounds.x + padding, directionalShipMeterBounds.y + padding,
                                      directionalShipMeterBounds.width - padding * 2, directionalShipMeterBounds.height - padding * 2};
    directionalShipMeter.width = directionalShipMeter.width * player->GetDirectionalShipMeter() / DIRECTIONAL_SHIP_METER_MAX;
    directionalShipMeter.x = directionalShipMeterBounds.x + directionalShipMeterBounds.width - directionalShipMeter.width - padding;

    DrawTexturePro(*directionalShipIcon, {0, 0, (float)directionalShipIcon->width, (float)directionalShipIcon->height}, iconBounds, {0, 0}, 0, WHITE);
    DrawRectangleRec(directionalShipMeter, WHITE);

}

void PlayerUI::DrawDebug()
{
    UIObject::DrawDebug();
}

void PlayerUI::Resize(Vector2 prevScreenSize)
{
    UIObject::Resize(prevScreenSize);
    directionalShipMeterBounds.width = 200 * GetScreenWidth() / prevScreenSize.x;
    directionalShipMeterBounds.x = GetScreenWidth() - directionalShipMeterBounds.width;
    directionalShipMeterBounds.y = livesBar->GetBounds().y + livesBar->GetBounds().height + 10;
    directionalShipMeterBounds.height = 10 * GetScreenHeight() / prevScreenSize.y;
}
