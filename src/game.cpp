#include "raylib.h"
#include "game.hpp"

void InitGame()
{
    SetTargetFPS(60);
}

void DrawFrame()
{
    BeginDrawing();
    ClearBackground(GetColor(0x242424));
    EndDrawing();
    if (IsKeyDown(KEY_H))
    {
        DrawText("Hello, world!", 12, 12, 20, WHITE);
    }
}

void DrawDebug()
{
}

void HandleInput()
{
}

void ExitGame()
{
}
