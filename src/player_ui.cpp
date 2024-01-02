#include "player_ui.hpp"
#include "raylib.h"
#include "score_registry.hpp"

PlayerUI::PlayerUI(Player *player)
    : UIObject({0, 0, (float)GetScreenWidth(), (float)GetScreenHeight()}, nullptr, nullptr), player(player)
{
    this->player = player;
    this->livesBar = new LivesBar(Rectangle{(float)GetScreenWidth() / 2 + 100, 10, (float)GetScreenWidth() / 2 - 100, 50}, player);
    this->powerups = new PlayerPowerups(player, LEFT);
    
    AddChild(livesBar);
    AddChild(powerups);

    scoreRect = {10, 10, (float)MeasureText("Score: 0000000", SCORE_FONT_SIZE), SCORE_FONT_SIZE};
}

PlayerUI::~PlayerUI()
{
}

void PlayerUI::Draw()
{
    const char *scoreText = TextFormat("%08i", GetTotalScore());
    DrawTextEx(*ResourceManager::GetFont(), scoreText, {scoreRect.x, scoreRect.y}, SCORE_FONT_SIZE, 1, WHITE);

    for (size_t i = 0; i < children.size(); i++)
    {
        children[i]->Draw();
    }
    
}

void PlayerUI::DrawDebug()
{
    UIObject::DrawDebug();
}
