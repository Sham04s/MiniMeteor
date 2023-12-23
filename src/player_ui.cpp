#include "player_ui.hpp"
#include "raylib.h"
#include "score_registry.hpp"

PlayerUI::PlayerUI(Player *player)
    : UIObject({0, 0, (float)GetScreenWidth(), (float)GetScreenHeight()}, nullptr, nullptr), player(player)
{
    this->player = player;
    livesBar = new LivesBar(Rectangle{(float)GetScreenWidth() / 2 + 100, 10, (float)GetScreenWidth() / 2 - 100, 50}, player);
    AddChild(livesBar);

    scoreRect = {10, 10, (float)MeasureText("Score: 0000000", SCORE_FONT_SIZE), SCORE_FONT_SIZE};
}

PlayerUI::~PlayerUI()
{
}

void PlayerUI::Draw()
{
    DrawText(TextFormat("%07i", GetTotalScore()), scoreRect.x, scoreRect.y, SCORE_FONT_SIZE, WHITE);

    livesBar->Draw();
}

void PlayerUI::DrawDebug()
{
    UIObject::DrawDebug();
}
