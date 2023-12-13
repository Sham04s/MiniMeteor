#include <stdio.h>

#include "raylib.h"
#include "game.hpp"
#include <math.h>

#include "resource_manager.hpp"
#include "asteroid.hpp"
#include "player.hpp"
#include "button.hpp"

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

enum ScreenID
{
    MAIN_MENU,
    GAME,
    OPTIONS,
    EXITING,
    NUM_SCREENS
};

struct GameState
{
    ScreenID currentScreen;
    UIObject *screens[NUM_SCREENS];
} gameState;

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

    gameState.currentScreen = GAME;
    CreateNewGame();

    UIObject *mainMenu = new UIObject(Rectangle{(float)GetScreenWidth() / 2 - BUTTON_WIDTH / 2, (float)GetScreenHeight() / 2 - BUTTON_HEIGHT * 3 / 2 + BUTTON_PADDING,
                                                BUTTON_WIDTH, BUTTON_HEIGHT * 3 + BUTTON_PADDING * 2},
                                      nullptr, ResourceManager::GetDefaultTexture());

    mainMenu->AddChild(new Button(Vector2{0, 0}, mainMenu, "Play", []()
                                  { gameState.currentScreen = GAME; }));
    mainMenu->AddChild(new Button(Vector2{0, 100}, mainMenu, "Options", []()
                                  { gameState.currentScreen = OPTIONS; }));
    mainMenu->AddChild(new Button(Vector2{0, 200}, mainMenu, "Quit", []()
                                  { gameState.currentScreen = EXITING; }));

    UIObject *game = new UIObject(Rectangle{0, 0, 0, 0}, nullptr, ResourceManager::GetDefaultTexture());

    game->AddChild(new UIObject(Rectangle{0, 0, 0, 0}, game, ResourceManager::GetUITexture(LIFE_TEXTURE)));

    gameState.screens[MAIN_MENU] = mainMenu;
}

void CreateNewGame()
{
    player = Player(Vector2{(float)GetScreenWidth() / 2, (float)GetScreenHeight() / 2});
    asteroids.clear();
    for (size_t i = 0; i < 1; i++)
    {
        asteroids.push_back(Asteroid((Vector2){(float)GetRandomValue(0, GetScreenWidth()), (float)GetRandomValue(0, GetScreenHeight())}));
    }
}

void DrawFrame()
{
    BeginDrawing();

    if (gameState.currentScreen == GAME)
    {
        ClearBackground(BACKGROUND_COLOR);

        player.Draw();
        for (size_t i = 0; i < asteroids.size(); i++)
        {
            asteroids[i].Draw();
        }
    }
    if (gameState.currentScreen != GAME)
    {
        ClearBackground(DARKGRAY);
    }
    if (gameState.screens[gameState.currentScreen] != nullptr)
    {
        gameState.screens[gameState.currentScreen]->Draw();
    }
    if (SHOW_DEBUG)
    {
        DrawDebug();
    }

    EndDrawing();
}

void DrawDebug()
{
    if (gameState.currentScreen == GAME)
    {
        player.DrawDebug();

        for (size_t i = 0; i < asteroids.size(); i++)
        {
            asteroids[i].DrawDebug();
        }

        DrawText(TextFormat("Player lives: %d", player.GetLives()), 10, 30, 20, WHITE);
    }

    DrawFPS(10, 10);

    if (gameState.screens[gameState.currentScreen] != nullptr)
    {
        gameState.screens[gameState.currentScreen]->DrawDebug();
    }
    else
    {
        DrawText("No screen", (GetScreenWidth() - MeasureText("No screen", 40)) / 2, GetScreenHeight() - 40, 40, RED);
    }
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

    if (IsKeyPressed(KEY_ESCAPE))
    {
        if (gameState.currentScreen == GAME)
        {
            gameState.currentScreen = MAIN_MENU; // TODO: pause menu
        }
        else if (gameState.currentScreen == MAIN_MENU)
        {
            gameState.currentScreen = GAME;
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
    if (gameState.currentScreen == GAME)
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

    if (gameState.screens[gameState.currentScreen] == nullptr)
    {
        return;
    }
    gameState.screens[gameState.currentScreen]->Update();
}

bool GameLoop()
{
    HandleInput();
    
    DrawFrame();

    UpdateGame();

    if (gameState.currentScreen == EXITING)
    {
        return false;
    }

    return true;
}

void ExitGame()
{
    for (size_t i = 0; i < NUM_SCREENS; i++)
    {
        if (gameState.screens[i] != nullptr)
        {
            delete gameState.screens[i];
        }
    }

    ResourceManager::UnloadResources();
}
