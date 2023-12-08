#include <stdio.h>

#include "raylib.h"
#include "game.hpp"
#include <math.h>

#include "resource_manager.hpp"
#include "player.hpp"
#include "asteroid.hpp"

#ifdef _DEBUG
bool SHOW_DEBUG = true;
#else
bool SHOW_DEBUG = false;
#endif // _DEBUG

bool HIDE_SPRITES = false;

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
std::vector<Asteroid> asteroids;

void InitGame()
{
    
    SetTargetFPS(60);

    if (!resourceManager.LoadResources())
    {
        printf("Failed to load resources!\n");
    }

    player = Player((Vector2){GetScreenWidth() / (float)2, GetScreenHeight() / (float)2}, 0);
    for (size_t i = 0; i < 1; i++)
    {
        asteroids.push_back(Asteroid((Vector2){(float)GetRandomValue(0, GetScreenWidth()), (float)GetRandomValue(0, GetScreenHeight())}, 0));
    }
}

void DrawFrame()
{
    BeginDrawing();

    ClearBackground(BACKGROUND_COLOR);
    if (!HIDE_SPRITES)
    {
        player.Draw();
        for (size_t i = 0; i < asteroids.size(); i++)
        {
            asteroids[i].Draw();
        }
    }
    if(SHOW_DEBUG)
    {
        DrawDebug();
    }

    EndDrawing();
}

void DrawDebug()
{
    player.DrawDebug();
    for (size_t i = 0; i < asteroids.size(); i++)
    {
        asteroids[i].DrawDebug();
    }

    DrawFPS(10, 10);
    const char *mousePos = TextFormat("Mouse position: (%d, %d)", GetMouseX(), GetMouseY());
    DrawText(mousePos, GetRenderWidth() - MeasureText(mousePos, 20) - 10, 10, 20, WHITE);
}

std::vector<double> allCheckCollisionTimes(20);
double averageCheckCollisionTime = 0;

void Update()
{
    player.Update();
    for (size_t i = 0; i < asteroids.size(); i++)
    {
        asteroids[i].Update();
    }

    if (IsKeyDown(KEY_LEFT_CONTROL))
    {
        if (IsKeyPressed(KEY_R))
        {
            ResourceManager::UnloadResources();
            ResourceManager::LoadResources();
        }
    }
    else
    {
        if (IsKeyPressed(KEY_R))
        {
            player.Reset();
            asteroids.clear();
            for (size_t i = 0; i < 1; i++)
            {
                asteroids.push_back(Asteroid((Vector2){(float)GetRandomValue(0, GetScreenWidth()), (float)GetRandomValue(0, GetScreenHeight())}, 0));
            }
        }
    }

    double startTime = GetTime();
    Vector2 collisionPoint = {0}; 
    for (size_t i = 0; i < asteroids.size(); i++)
    {
        if (player.CheckCollision(&asteroids[i], &collisionPoint))
        {
            DrawText("Player Collision!", 10, 10, 20, WHITE);
        }
        for (size_t j = i; j < asteroids.size(); j++)
        {
            if (i < j)
            {
                if (asteroids[i].CheckCollision(&asteroids[j], &collisionPoint))
                {
                    DrawText("Collision!", 10, 10, 20, WHITE);
                }
            }
        }
    }
    double endTime = GetTime();
    allCheckCollisionTimes.push_back(endTime - startTime);
    if (allCheckCollisionTimes.size() > 20)
    {
        allCheckCollisionTimes.erase(allCheckCollisionTimes.begin());
    }
    if (fmod(GetTime(), 1) < 0.01)
    {
        averageCheckCollisionTime = 0;
        for (size_t i = 0; i < allCheckCollisionTimes.size(); i++)
        {
            averageCheckCollisionTime += allCheckCollisionTimes[i];
        }
        averageCheckCollisionTime /= allCheckCollisionTimes.size();
    }
    DrawText(TextFormat("Average collision time: %f ms", averageCheckCollisionTime * 1000), 10, 30, 20, WHITE);

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
        if (IsKeyPressed(KEY_N))
        {
            asteroids.push_back(Asteroid((Vector2){(float)GetRandomValue(0, GetScreenWidth()), (float)GetRandomValue(0, GetScreenHeight())}, 0));
        }
        if (IsKeyPressed(KEY_H))
        {
            HIDE_SPRITES = !HIDE_SPRITES;
        }
    }

    if (CheckCollisionPointRec(GetMousePosition(), player.GetBounds()))
    {
        if (IsMouseButtonDown(MOUSE_LEFT_BUTTON))
        {
            player.SetVelocity({0});
            player.Translate(GetMouseDelta());
        }
    }

    for (size_t i = 0; i < asteroids.size(); i++)
    {
        if (CheckCollisionPointRec(GetMousePosition(), asteroids[i].GetBounds()))
        {
            if (IsMouseButtonDown(MOUSE_LEFT_BUTTON))
            {
                asteroids[i].SetVelocity({0});
                asteroids[i].SetAngularVelocity(0);
                asteroids[i].Translate(GetMouseDelta());
            }
            if (IsMouseButtonDown(MOUSE_RIGHT_BUTTON))
            {
                asteroids[i].SetVelocity({(float)GetRandomValue(-100, 100), (float)GetRandomValue(-100, 100)});
                asteroids[i].SetAngularVelocity(GetRandomValue(-100, 100));
            }
        }
    }
#endif // _DEBUG
}

void ExitGame()
{
    resourceManager.UnloadResources();
}
