#include "ui/components/hud/player_ui.hpp"

#include "raylib.h"
#include "utils/score_registry.hpp"
#include "utils/utils.hpp"
#include "ui/components/common/ui_object.hpp"

#include <stdio.h>

PlayerUI::PlayerUI(Player *player)
    : UIObject({0, 0, (float)GetScreenWidth(), (float)GetScreenHeight()}, nullptr, nullptr), player(player)
{
    this->player = player;
    this->livesBar = new LivesBar(Rectangle{(float)GetScreenWidth() / 2 + 200, 10, (float)GetScreenWidth() / 2 - 200, 50}, player);
    this->powerups = new PlayerPowerups(player, LEFT);
    this->scoreLabel = new Label({10, 10, 0, 0}, scoreText, WHITE, ALIGN_START, ALIGN_START, DEFAULT_FONT_SIZE, this);
    this->texture = ResourceManager::GetDefaultTexture();
    this->directionalShipMeterBounds = {(float)GetScreenWidth() - 200, livesBar->GetBounds().y + livesBar->GetBounds().height + 10, 200, 10};
    this->directionalShipIcon = ResourceManager::GetUITexture(DIRECTIONAL_SHIP_TEXTURE);
    this->directionalShipIconBounds = {directionalShipMeterBounds.x - DIRECTIONAL_SHIP_ICON_SIZE - 2,
                                       directionalShipMeterBounds.y + directionalShipMeterBounds.height / 2 - DIRECTIONAL_SHIP_ICON_SIZE / 2,
                                       DIRECTIONAL_SHIP_ICON_SIZE, DIRECTIONAL_SHIP_ICON_SIZE};

    AddChild(livesBar);
    AddChild(powerups);
    AddChild(scoreLabel);

    // initialize score text
    sprintf(scoreText, "%08d", GetTotalScore());
}

PlayerUI::~PlayerUI()
{
}

void PlayerUI::Update()
{
    UIObject::Update();

    // update score text
    sprintf(scoreText, "%08d", GetTotalScore());
}

void PlayerUI::Draw()
{
    UIObject::Draw();

    // calculate color alpha based on distance to player
    float alpha = 1.0f;
    Vector2 playerPos = GetWorldToScreen2D(player->GetOrigin(), player->GetCamera());
    float distanceToPlayer = Vector2Distance(playerPos, {directionalShipMeterBounds.x + directionalShipMeterBounds.width / 2, directionalShipMeterBounds.y + directionalShipMeterBounds.height / 2});
    if (distanceToPlayer < 100)
    {
        alpha = fmaxf(0.0f, distanceToPlayer / 100);
    }

    // draw directional ship meter
    const int padding = 2;

    Rectangle directionalShipMeter = {directionalShipMeterBounds.x + padding, directionalShipMeterBounds.y + padding,
                                      directionalShipMeterBounds.width - padding * 2, directionalShipMeterBounds.height - padding * 2};
    directionalShipMeter.width = directionalShipMeter.width * player->GetDirectionalShipMeter() / DIRECTIONAL_SHIP_METER_MAX;
    directionalShipMeter.x = directionalShipMeterBounds.x + directionalShipMeterBounds.width - directionalShipMeter.width - padding;

    // meter container
    DrawRectangleLinesEx(directionalShipMeterBounds, 1, Fade(WHITE, alpha));

    // meter
    DrawRectangleRec(directionalShipMeter, Fade(WHITE, alpha));

    // draw directional ship icon
    DrawTexturePro(*directionalShipIcon, {0, 0, (float)directionalShipIcon->width, (float)directionalShipIcon->height},
                   directionalShipIconBounds, {0, 0}, 0, Fade(WHITE, alpha));
}

void PlayerUI::DrawDebug()
{
    UIObject::DrawDebug();
    DrawRectangleLinesEx(directionalShipMeterBounds, 1, RED);
    DrawRectangleLinesEx(directionalShipIconBounds, 1, RED);
}

void PlayerUI::Resize(Vector2 prevScreenSize)
{
    UIObject::Resize(prevScreenSize);

    // resize meter
    directionalShipMeterBounds.width *= GetScreenWidth() / prevScreenSize.x;
    directionalShipMeterBounds.height *= GetScreenHeight() / prevScreenSize.y;
    directionalShipMeterBounds.x = GetScreenWidth() - directionalShipMeterBounds.width;
    directionalShipMeterBounds.y = livesBar->GetCenteredBounds().y + livesBar->GetCenteredBounds().height + 10;

    // resize meter icon
    directionalShipIconBounds.width *= GetScreenHeight() / prevScreenSize.y;
    directionalShipIconBounds.height *= GetScreenHeight() / prevScreenSize.y;
    directionalShipIconBounds.x = directionalShipMeterBounds.x - directionalShipIconBounds.width - 2;
    directionalShipIconBounds.y = directionalShipMeterBounds.y + directionalShipMeterBounds.height / 2 - directionalShipIconBounds.height / 2;
}
