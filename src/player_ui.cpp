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
}

void PlayerUI::DrawDebug()
{
    UIObject::DrawDebug();
}
