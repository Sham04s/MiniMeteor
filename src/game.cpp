#include <stdio.h>

#include "raylib.h"
#include "game.hpp"
#include <math.h>

#include "resource_manager.hpp"
#include "ui_manager.hpp"
#include "asteroid.hpp"
#include "player.hpp"

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

Player player;
std::vector<Asteroid> asteroids;

void InitGame()
{
    SetExitKey(KEY_NULL);
    SetTargetFPS(60);

    if (!ResourceManager::LoadResources())
    {
        printf("Failed to load resources!\n");
    }
    if (!UIManager::LoadUI())
    {
        printf("Failed to load UI!\n");
    }

    player = Player((Vector2){GetScreenWidth() / (float)2, GetScreenHeight() / (float)2});
    for (size_t i = 0; i < 2; i++)
    {
        asteroids.push_back(Asteroid((Vector2){(float)GetRandomValue(0, GetScreenWidth()), (float)GetRandomValue(0, GetScreenHeight())}));
    }
}

void DrawFrame()
{
    ClearBackground(BACKGROUND_COLOR);

    player.Draw();
    for (size_t i = 0; i < asteroids.size(); i++)
    {
        asteroids[i].Draw();
    }
    if (SHOW_DEBUG)
    {
        DrawDebug();
    }
}

void DrawDebug()
{
    player.DrawDebug();

    for (size_t i = 0; i < asteroids.size(); i++)
    {
        asteroids[i].DrawDebug();
    }

    DrawText(TextFormat("Player lives: %d", player.GetLives()), 10, 30, 20, WHITE);

    DrawFPS(10, 10);
}

void HandleInput()
{
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
            player.Respawn();
            player.SetLives(3);
            asteroids.clear();
            for (size_t i = 0; i < 1; i++)
            {
                asteroids.push_back(Asteroid((Vector2){(float)GetRandomValue(0, GetScreenWidth()), (float)GetRandomValue(0, GetScreenHeight())}));
            }
        }
    }

#ifdef _DEBUG
    if (IsKeyPressed(KEY_F3))
    {
        SHOW_DEBUG = !SHOW_DEBUG;
    }
    if (IsKeyDown(KEY_LEFT_SHIFT))
    {
        if (IsKeyPressed(KEY_ZERO))
        {
            SetTargetFPS(5);
        }
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
        if (IsKeyPressed(KEY_H))
        {
            HIDE_SPRITES = !HIDE_SPRITES;
        }
    }
    if (IsKeyPressed(KEY_X))
    {
        asteroids.push_back(Asteroid((Vector2){(float)GetRandomValue(0, GetScreenWidth()), (float)GetRandomValue(0, GetScreenHeight())}));
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
                Vector2 velocity = Vector2Subtract(GetMousePosition(), asteroids[i].GetOrigin());
                asteroids[i].SetVelocity(Vector2Scale(velocity, 3));
                asteroids[i].SetAngularVelocity(GetRandomValue(-100, 100));
            }
        }
    }
#endif // _DEBUG
}

void UpdateGame()
{
    player.Update();

    Vector2 collisionPoint = {0, 0};
    auto bullets = player.GetBullets();

    for (size_t i = 0; i < asteroids.size(); i++)
    {
        asteroids[i].Update();

        // check for collisions between player and asteroids
        if (player.CheckCollision(&asteroids[i], &collisionPoint))
        {
            player.Push(&asteroids[i], collisionPoint);
            player.Kill();
        }

        // check for collisions between asteroids
        for (size_t j = 0; j < asteroids.size(); j++)
        {
            if (i < j) // check each pair only once
            {
                if (asteroids[i].IsFloating() && asteroids[i].CheckCollision(&asteroids[j], &collisionPoint))
                {
                    // push asteroids away from each other with the SAT push vector
                    asteroids[i].Push(&asteroids[j], collisionPoint);
                }
            }
        }

        // check for collisions between bullets and asteroids
        for (size_t b = 0; b < bullets->size(); b++)
        {
            if ((*bullets)[b].CheckCollision(&asteroids[i], &collisionPoint))
            {
                // destroy bullet and asteroid
                bullets->erase(bullets->begin() + b);
                asteroids[i].Destroy();
            }
        }
        bullets->shrink_to_fit();

        // erase destroyed asteroids
        if (asteroids[i].IsDestroyed())
        {
            asteroids.erase(asteroids.begin() + i);
        }
    }
    asteroids.shrink_to_fit();
}

bool GameLoop()
{
    HandleInput();
    UIManager::HandleUIInput();

    BeginDrawing();

    if (UIManager::GetCurrentScreen() == GAME)
    {
        DrawFrame();
    }

    UIManager::DrawUI();

    EndDrawing();

    if (UIManager::GetCurrentScreen() == GAME)
    {
        UpdateGame();
    }
    UIManager::UpdateUI();

    if (UIManager::GetCurrentScreen() == EXITING)
    {
        return false;
    }

    return true;
}

void ExitGame()
{
    UIManager::UnloadUI();
    ResourceManager::UnloadResources();
}
