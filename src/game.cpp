#include <stdio.h>

#include "raylib.h"
#include "game.hpp"

#include "resource_manager.hpp"
#include "player.hpp"

#ifdef _DEBUG
bool SHOW_DEBUG = true;
#else
bool SHOW_DEBUG = false;
#endif // _DEBUG

#define PRIMARY_COLOR GetColor(0x3c9dd7ff)
#define PRIMARY_COLOR_BRIGHT GetColor(0x8dc3e3ff)
#define PRIMARY_COLOR_DARK GetColor(0x058ad9ff)
#define PRIMARY_COLOR_DARKER GetColor(0x004d7aff)
#define SECONDARY_COLOR GetColor(0x4d50deff)
#define SECONDARY_COLOR_BRIGHT GetColor(0x989ae7ff)
#define SECONDARY_COLOR_DARK GetColor(0x0c12dfff)
#define SECONDARY_COLOR_DARKER GetColor(0x000386ff)
#define SECONDARY_COLOR_2 GetColor(0x34df7fff)
#define SECONDARY_COLOR_2_BRIGTH GetColor(0x8ce9b4ff)
#define SECONDARY_COLOR_2_DAR GetColor(0x00e162ff)
#define SECONDARY_COLOR_2_DARKER GetColor(0x00893bff)
#define BACKGROUND_COLOR GetColor(0x242424ff)

enum GameState
{
    MENU,
    GAME,
    PAUSE,
    GAME_OVER,
};

ResourceManager resourceManager;
Player player;

void InitGame()
{
    SetTargetFPS(60);

    if (!resourceManager.LoadResources())
    {
        printf("Failed to load resources!\n");
    }

    player = Player((Vector2){GetScreenWidth() / (float)2, GetScreenHeight() / (float)2}, 0);
}

void DrawFrame()
{
    BeginDrawing();

    ClearBackground(BACKGROUND_COLOR);
    player.Draw();

    EndDrawing();
}

void DrawDebug()
{
    if (!SHOW_DEBUG)
    {
        return;
    }
    player.DrawDebug();

    DrawFPS(10, 10);
}

void Update()
{
    player.Update();

    if (IsKeyPressed(KEY_R))
    {
        player.Reset();
    }

#ifdef _DEBUG
    if (IsKeyPressed(KEY_F3))
    {
        SHOW_DEBUG = !SHOW_DEBUG;
    }
    if (IsKeyDown(KEY_LEFT_SHIFT))
    {
        if (IsKeyPressed(KEY_ONE))
        {
            SetTargetFPS(15);
        }
        if (IsKeyPressed(KEY_TWO))
        {
            SetTargetFPS(30);
        }
        if (IsKeyPressed(KEY_THREE))
        {
            SetTargetFPS(60);
        }
        if (IsKeyPressed(KEY_FOUR))
        {
            SetTargetFPS(120);
        }
        if (IsKeyPressed(KEY_FIVE))
        {
            SetTargetFPS(0);
        }
    }
#endif // _DEBUG
}

void ExitGame()
{
    resourceManager.UnloadResources();
}
