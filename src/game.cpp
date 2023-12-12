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

    player = Player((Vector2){GetScreenWidth() / (float)2, GetScreenHeight() / (float)2});
    for (size_t i = 0; i < 2; i++)
    {
        asteroids.push_back(Asteroid((Vector2){(float)GetRandomValue(0, GetScreenWidth()), (float)GetRandomValue(0, GetScreenHeight())}));
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

double asteroidCheckingsTime = 0;
std::vector<double> asteroidCheckingsTimes = {};

void DrawDebug()
{
    player.DrawDebug();
    Vector2 pushVector = {0, 0};
    double start = GetTime();
    for (size_t i = 0; i < asteroids.size(); i++)
    {
        asteroids[i].DrawDebug();
        float xDraw = asteroids[i].GetBounds().x;
        float yDraw = asteroids[i].GetBounds().y + asteroids[i].GetBounds().height + 10;
        DrawText(TextFormat("Asteroid %d", i), xDraw, yDraw, 20, WHITE);
        yDraw += 20;
        for (size_t j = 0; j < asteroids.size(); j++)
        {
            if (i != j)
            {
                if (asteroids[i].IsFloating() && asteroids[i].CheckCollision(&asteroids[j], &pushVector))
                {
                    DrawLineV(asteroids[i].GetOrigin(), Vector2Add(asteroids[i].GetOrigin(), Vector2Scale(pushVector, 10)), PURPLE);
                }
                yDraw += 20;
            }
        }
    }
    double end = GetTime();
    asteroidCheckingsTime = end - start;
    asteroidCheckingsTimes.push_back(asteroidCheckingsTime);
    if (asteroidCheckingsTimes.size() > 100)
    {
        asteroidCheckingsTimes.erase(asteroidCheckingsTimes.begin());
    }
    double average = 0;
    for (size_t i = 0; i < asteroidCheckingsTimes.size(); i++)
    {
        average += asteroidCheckingsTimes[i];
    }
    average /= asteroidCheckingsTimes.size();
    DrawText(TextFormat("Average asteroid checkings time: %.5f ms", average * 1000), 10, 90, 20, WHITE);

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
            player.Respawn();
            player.SetLives(3);
            asteroids.clear();
            for (size_t i = 0; i < 1; i++)
            {
                asteroids.push_back(Asteroid((Vector2){(float)GetRandomValue(0, GetScreenWidth()), (float)GetRandomValue(0, GetScreenHeight())}));
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

void UpdatePhysics()
{
    Vector2 collisionPoint = {0, 0};
    auto bullets = player.GetBullets();
    for (size_t i = 0; i < asteroids.size(); i++)
    {
        if (player.CheckCollision(&asteroids[i], &collisionPoint))
        {
            player.Push(&asteroids[i], collisionPoint);                   // apply to player
            asteroids[i].Push(&player, Vector2Scale(collisionPoint, -1)); // apply to asteroid
            player.Kill();
        }
        for (size_t j = 0; j < asteroids.size(); j++)
        {
            if (i < j) // check for every different asteroid
            {
                if (asteroids[i].IsFloating() && asteroids[i].CheckCollision(&asteroids[j], &collisionPoint))
                {
                    asteroids[i].Push(&asteroids[j], collisionPoint);
                }
            }
        }
        for (size_t b = 0; b < bullets->size(); b++)
        {
            if ((*bullets)[b].CheckCollision(&asteroids[i], &collisionPoint))
            {
                bullets->erase(bullets->begin() + b);
                asteroids[i].Destroy();
            }
        }
        bullets->shrink_to_fit();
    }
}

void HandleLogic()
{
    // if (player.GetState() == DEAD && player.GetLives() > 0)
    // {
    //     player.Respawn();
    // }
    for (size_t i = 0; i < asteroids.size(); i++)
    {
        if (asteroids[i].IsDestroyed())
        {
            asteroids.erase(asteroids.begin() + i);
        }
    }
    asteroids.shrink_to_fit();
}

void UpdateGame()
{
    player.Update();
    for (size_t i = 0; i < asteroids.size(); i++)
    {
        asteroids[i].Update();
    }
}

void GameLoop()
{
    HandleInput();
    if (gameState == GAME)
    {
        HandleLogic();
        UpdatePhysics();
        UpdateGame();
    }
    if (gameState == PAUSE && IsKeyPressed(KEY_N))
    {
        HandleLogic();
        UpdatePhysics();
        UpdateGame();
    }

    DrawFrame();
}

void ExitGame()
{
    resourceManager.UnloadResources();
}
