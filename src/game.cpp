#include <stdio.h>

#include "raylib.h"
#include "game.hpp"
#include <math.h>

#include "resource_manager.hpp"
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

enum GameState
{
    MENU,
    GAME,
    PAUSE,
    GAME_OVER,
};

GameState gameState = GAME; // TODO: Change to MENU

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
    for (size_t i = 0; i < 2; i++)
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
    if (SHOW_DEBUG)
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

    DrawText(TextFormat("Player lives: %d", player.GetLives()), 10, 30, 20, WHITE);
    DrawText(TextFormat("Asteroids: %d", asteroids.size()), 10, 50, 20, WHITE);

    DrawFPS(10, 10);
    const char *mousePos = TextFormat("Mouse position: (%d, %d)", GetMouseX(), GetMouseY());
    DrawText(mousePos, GetRenderWidth() - MeasureText(mousePos, 20) - 10, 10, 20, WHITE);
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
            player.Reset();
            player.SetLives(3);
            asteroids.clear();
            for (size_t i = 0; i < 1; i++)
            {
                asteroids.push_back(Asteroid((Vector2){(float)GetRandomValue(0, GetScreenWidth()), (float)GetRandomValue(0, GetScreenHeight())}, 0));
            }
        }
    }
    if (IsKeyPressed(KEY_P))
    {
        if (gameState == GAME)
        {
            gameState = PAUSE;
        }
        else if (gameState == PAUSE)
        {
            gameState = GAME;
        }
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

void UpdatePhysics()
{
    if (player.IsFirstCollision() && player.GetLastCollisionObject()->GetType() == ASTEROID)
    {
        Asteroid *asteroid = (Asteroid *)player.GetLastCollisionObject();

        float u = Vector2Length(Vector2Subtract(player.GetVelocity(), asteroid->GetVelocity()));
        float e = 0.7f;
        float mA = 50;
        float mp = 80;
        float angle = Vector2Angle(Vector2Subtract(asteroid->GetOrigin(), player.GetLastCollisionPoint()), player.GetVelocity());
        float q = (mA / mp);
        float f = (1 + e) / (2 * q + 1 + pow(sin(angle), 2));

        float va = f * u;
        float vp = (1 - 2 * q * f) * u;

        float w = (f * sin(angle) * u) / Vector2Length(Vector2Subtract(asteroid->GetOrigin(), player.GetLastCollisionPoint()));

        asteroid->SetVelocity(Vector2Scale(Vector2Normalize(Vector2Subtract(asteroid->GetOrigin(), player.GetLastCollisionPoint())), va));
        asteroid->SetAngularVelocity(w * RAD2DEG);

        player.SetVelocity(Vector2Scale(Vector2Normalize(Vector2Subtract(player.GetOrigin(), player.GetLastCollisionPoint())), vp));
        player.ResetCollisionChecks();
    }
}

void HandleLogic()
{
    if (player.IsFirstCollision())
    {
        player.Kill();
    }
    for (size_t i = 0; i < player.GetBullets().size(); i++)
    {
        if (player.GetBullets()[i].IsFirstCollision())
        {
            DrawText(TextFormat("Bullet collision!"), 10, 50, 20, WHITE);
            player.GetBullets().erase(player.GetBullets().begin() + i);
        }
    }
    for (size_t i = 0; i < asteroids.size(); i++)
    {
        if (asteroids[i].isDestroyed())
        {
            asteroids.erase(asteroids.begin() + i);
            asteroids.shrink_to_fit();
        }
    }
}

void UpdateGame()
{
    HandleLogic();
    UpdatePhysics();

    player.Update();
    for (size_t i = 0; i < asteroids.size(); i++)
    {
        asteroids[i].Update();
        if (!player.IsFirstCollision())
        {
            player.CheckCollision(&asteroids[i]);
        }
        for (size_t j = 0; j < asteroids.size(); j++)
        {
            if (i < j)
            {
                asteroids[i].CheckCollision(&asteroids[j]);
            }
        }
    }
}

void GameLoop()
{
    if (gameState == GAME)
    {
        UpdateGame();
    }
    DrawFrame();
    HandleInput();
}

void ExitGame()
{
    resourceManager.UnloadResources();
}
