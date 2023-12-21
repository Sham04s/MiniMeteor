#include <stdio.h>
#include <math.h>
#include <algorithm>

#include "raylib.h"

#include "game.hpp"
#include "game_ui.hpp"

#include "resource_manager.hpp"
#include "player.hpp"
#include "asteroid.hpp"
#include "enemy.hpp"
#include "power_up.hpp"

#ifdef _DEBUG
bool SHOW_DEBUG = true;
#else
bool SHOW_DEBUG = false;
#endif // _DEBUG

bool HIDE_SPRITES = false;

GameState gameState;

Player *player;
std::vector<GameObject *> gameObjects;
bool powerupSpawned = false;
// std::vector<Asteroid *> asteroids;
// std::vector<BasicEnemy *> enemies;
// PowerUp *powerup; // only one powerup can be in the screen at a time

void InitGame()
{
    SetExitKey(KEY_NULL);
    SetTargetFPS(60);

#ifdef _DEBUG
    SetTraceLogLevel(LOG_ALL);
#endif // _DEBUG

    InitAudioDevice();

    if (!ResourceManager::LoadResources())
    {
        printf("Failed to load resources!\n");
    }

    gameState.currentScreen = MAIN_MENU;
    player = new Player({(float)GetScreenWidth() / 2, (float)GetScreenHeight() / 2});

    CreateUIElements(player);
}

void CreateNewGame()
{
    gameState.previousScreen = gameState.currentScreen;
    gameState.currentScreen = GAME;
    if (player != nullptr)
    {
        player->Reset();
    }
    else
    {
        player = new Player({(float)GetScreenWidth() / 2, (float)GetScreenHeight() / 2});
    }
    for (size_t i = 0; i < gameObjects.size(); i++)
    {
        delete gameObjects[i];
    }
    gameObjects.clear();

    const size_t numAsteroids = 5;
    const size_t numEnemies = 2;

    for (size_t i = 0; i < numAsteroids + numEnemies; i++)
    {
        // generate random position exluding the player's position and other asteroids positions
        Vector2 pos;
        bool insidePlayer;
        bool insideGameObject;
        do
        {
            pos = {(float)GetRandomValue(0, GetScreenWidth()), (float)GetRandomValue(0, GetScreenHeight())};
            insidePlayer = Vector2Distance(pos, player->GetOrigin()) < 100;
            insideGameObject = false;
            for (size_t j = 0; j < gameObjects.size(); j++)
            {
                if (Vector2Distance(pos, gameObjects[j]->GetOrigin()) < 100)
                {
                    insideGameObject = true;
                    break;
                }
            }
        } while (insidePlayer || insideGameObject);

        if (i < numAsteroids)
        {
            gameObjects.push_back(new Asteroid(pos));
        }
        else
        {
            gameObjects.push_back(new BasicEnemy(pos, player));
        }
    }
}

void PauseGame()
{
    gameState.previousScreen = gameState.currentScreen;
    gameState.currentScreen = PAUSE_MENU;
    player->PauseSounds();
    for (size_t i = 0; i < gameObjects.size(); i++)
    {
        gameObjects[i]->PauseSounds();
    }
}

void ResumeGame()
{
    gameState.previousScreen = gameState.currentScreen;
    gameState.currentScreen = GAME;
    player->ResumeSounds();
    for (size_t i = 0; i < gameObjects.size(); i++)
    {
        gameObjects[i]->ResumeSounds();
    }
}

void DrawFrame()
{
    BeginDrawing();
    ClearBackground(BACKGROUND_COLOR);

    if (gameState.currentScreen == GAME || gameState.currentScreen == GAME_OVER || gameState.currentScreen == PAUSE_MENU)
    {
        for (size_t i = 0; i < gameObjects.size(); i++)
        {
            gameObjects[i]->Draw();
        }
        player->Draw();
    }
    if (gameState.screens[gameState.currentScreen] != nullptr)
    {
        if (gameState.currentScreen == PAUSE_MENU)
        {
            DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), Fade(BLACK, 0.25f));
        }

        gameState.screens[gameState.currentScreen]->Draw();
    }
    if (SHOW_DEBUG)
    {
        DrawDebug();
    }

    EndDrawing();
}

PowerUpType powerupToSpawn = LIFE;

void DrawDebug()
{
    if (gameState.currentScreen == GAME || gameState.currentScreen == GAME_OVER)
    {
        player->DrawDebug();
        for (size_t i = 0; i < gameObjects.size(); i++)
        {
            gameObjects[i]->DrawDebug();
        }
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

    DrawText(TextFormat("Powerup to spawn: %i", powerupToSpawn), 10, GetScreenHeight() - 40, 20, WHITE);
}

void HandleInput()
{
    if (IsKeyPressed(KEY_R))
    {
        CreateNewGame();
    }

    if (IsKeyPressed(KEY_ESCAPE))
    {
        if (gameState.currentScreen == GAME)
        {
            PauseGame();
        }
        else if (gameState.currentScreen == PAUSE_MENU)
        {
            ResumeGame();
        }
        else if (gameState.currentScreen == OPTIONS)
        {
            if (gameState.previousScreen == PAUSE_MENU)
            {
                gameState.currentScreen = PAUSE_MENU;
            }
            else if (gameState.previousScreen == MAIN_MENU)
            {
                gameState.currentScreen = MAIN_MENU;
            }
            gameState.previousScreen = OPTIONS;
        }
    }

#ifdef _DEBUG
#ifdef PLATFORM_DESKTOP
    if (IsKeyPressed(KEY_F3))
    {
        SHOW_DEBUG = !SHOW_DEBUG;
    }
#else
    if (IsKeyDown(KEY_LEFT_CONTROL) && IsKeyPressed(KEY_B))
    {
        SHOW_DEBUG = !SHOW_DEBUG;
    }
#endif // PLATFORM_DESKTOP
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
        gameObjects.push_back(new Asteroid((Vector2){(float)GetRandomValue(0, GetScreenWidth()), (float)GetRandomValue(0, GetScreenHeight())}));
    }
    if (IsKeyPressed(KEY_C))
    {
        gameObjects.push_back(new BasicEnemy((Vector2){(float)GetRandomValue(0, GetScreenWidth()), (float)GetRandomValue(0, GetScreenHeight())}, player));
    }
    if (IsKeyPressed(KEY_V))
    {
        if (powerupSpawned)
        {
            auto powerup = std::find_if(gameObjects.begin(), gameObjects.end(), [](GameObject *obj)
                                        { return obj->GetType() == POWER_UP; });
            if (powerup != gameObjects.end())
            {
                delete *powerup;
                gameObjects.erase(powerup);
            }
        }
        gameObjects.push_back(new PowerUp(Vector2Clamp(GetMousePosition(), {0, 0}, {(float)GetScreenWidth(), (float)GetScreenHeight()}), powerupToSpawn));
        powerupSpawned = true;
    }

    if (IsKeyPressed(KEY_KP_ADD))
    {
        player->AddLife();
    }
    if (IsKeyPressed(KEY_KP_SUBTRACT))
    {
        player->Kill();
    }

    if (GetMouseWheelMove() > 0)
    {
        powerupToSpawn = (PowerUpType)((powerupToSpawn + 1) % NUM_POWER_UP_TYPES);
    }
    if (GetMouseWheelMove() < 0)
    {
        powerupToSpawn = (PowerUpType)((powerupToSpawn - 1 + NUM_POWER_UP_TYPES) % NUM_POWER_UP_TYPES);
    }

    if (player != nullptr && CheckCollisionPointRec(GetMousePosition(), player->GetBounds()))
    {
        if (IsMouseButtonDown(MOUSE_LEFT_BUTTON))
        {
            player->SetVelocity({0});
            player->Translate(GetMouseDelta());
        }
    }

    for (size_t i = 0; i < gameObjects.size(); i++)
    {
        if (CheckCollisionPointRec(GetMousePosition(), gameObjects[i]->GetBounds()))
        {
            if (IsMouseButtonDown(MOUSE_LEFT_BUTTON))
            {
                gameObjects[i]->SetVelocity({0});
                gameObjects[i]->Translate(GetMouseDelta());
            }
            if (IsMouseButtonDown(MOUSE_RIGHT_BUTTON))
            {
                Vector2 velocity = Vector2Subtract(GetMousePosition(), gameObjects[i]->GetOrigin());
                gameObjects[i]->SetVelocity(Vector2Scale(velocity, 3));
                gameObjects[i]->SetAngularVelocity(GetRandomValue(-100, 100));
            }
        }
    }

#endif // _DEBUG
}

void UpdateGameObjects()
{
    player->Update();
    for (size_t i = 0; i < gameObjects.size(); i++)
    {
        gameObjects[i]->Update();

        if (gameObjects[i]->GetType() == ASTEROID)
        {
            Asteroid *asteroid = (Asteroid *)gameObjects[i];
            if (asteroid->IsDestroyed())
            {
                delete asteroid;
                gameObjects.erase(gameObjects.begin() + i);
            }
        }
        else if (gameObjects[i]->GetType() == BASIC_ENEMY)
        {
            BasicEnemy *enemy = (BasicEnemy *)gameObjects[i];
            if (enemy->IsDead() && enemy->GetBullets()->size() == 0)
            {
                delete enemy;
                gameObjects.erase(gameObjects.begin() + i);
            }
        }
        else if (gameObjects[i]->GetType() == POWER_UP)
        {
            PowerUp *powerup = (PowerUp *)gameObjects[i];
            if (powerup->IsExpired())
            {
                delete powerup;
                gameObjects.erase(gameObjects.begin() + i);
            }
            else if (powerup->IsPickedUp())
            {
                gameObjects.erase(gameObjects.begin() + i);
            }
        }
    }
    gameObjects.shrink_to_fit();

    player->CleanBullets();
    for (size_t i = 0; i < gameObjects.size(); i++)
    {
        if (gameObjects[i]->GetType() == BASIC_ENEMY)
        {
            BasicEnemy *enemy = (BasicEnemy *)gameObjects[i];
            enemy->CleanBullets();
        }
    }
}

void HandleCollisions()
{
    Vector2 pushVector = {0, 0};
    auto bullets = player->GetBullets();

    // check collisions between player and all main game objects
    for (size_t i = 0; i < gameObjects.size(); i++)
    {
        if (player->CheckCollision(gameObjects[i], &pushVector))
        {
            player->HandleCollision(gameObjects[i], &pushVector);
            pushVector = Vector2Negate(pushVector);
            gameObjects[i]->HandleCollision(player, &pushVector);
        }
        for (size_t j = 0; j < gameObjects.size(); j++)
        {
            if (i < j) // check each pair only once
            {
                if (gameObjects[i]->CheckCollision(gameObjects[j], &pushVector))
                {
                    gameObjects[i]->HandleCollision(gameObjects[j], &pushVector);
                    pushVector = Vector2Negate(pushVector);
                    gameObjects[j]->HandleCollision(gameObjects[i], &pushVector);
                }
            }
        }
    }

    // check collision between bullets and all main game objects
    for (size_t i = 0; i < gameObjects.size(); i++)
    {
        for (size_t b = 0; b < bullets->size(); b++)
        {
            if ((*bullets)[b].CheckCollision(gameObjects[i], &pushVector))
            {
                (*bullets)[b].HandleCollision(gameObjects[i], &pushVector);
                pushVector = Vector2Negate(pushVector);
                gameObjects[i]->HandleCollision(&(*bullets)[b], &pushVector);
            }
        }

        if (gameObjects[i]->GetType() == BASIC_ENEMY)
        {
            BasicEnemy *enemy = (BasicEnemy *)gameObjects[i];
            auto enemyBullets = enemy->GetBullets();
            for (size_t b = 0; b < enemyBullets->size(); b++)
            {
                if ((*enemyBullets)[b].CheckCollision(player, &pushVector))
                {
                    (*enemyBullets)[b].HandleCollision(player, &pushVector);
                    pushVector = Vector2Negate(pushVector);
                    player->HandleCollision(&(*enemyBullets)[b], &pushVector);
                }
            }
        }
    }
}

void UpdateGame()
{
    if (gameState.currentScreen == GAME || gameState.currentScreen == GAME_OVER)
    {
        UpdateGameObjects();

        HandleCollisions();

        if (!powerupSpawned)
        {
            // try to spawn a power up
            const float spawnChance = 0.10f; // 10% chance of spawning a power up every second
            if (fmodf(GetTime(), 1.0f) < GetFrameTime() && GetRandomValue(0, 100) < spawnChance * 100)
            {
                gameObjects.push_back(new PowerUp((Vector2){(float)GetRandomValue(0, GetScreenWidth()), (float)GetRandomValue(0, GetScreenHeight())}, powerupToSpawn));
                powerupSpawned = true;
            }
        }

        if (player->IsDead() && player->GetLives() <= 0)
        {
            gameState.previousScreen = gameState.currentScreen;
            gameState.currentScreen = GAME_OVER;
        }
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

    UpdateGame();

    DrawFrame();

    if (gameState.currentScreen == EXITING)
    {
        return false;
    }

    return true;
}

void ExitGame()
{
    delete player;

    for (size_t i = 0; i < gameObjects.size(); i++)
    {
        delete gameObjects[i];
    }
    gameObjects.clear();

    for (size_t i = 0; i < NUM_SCREENS; i++)
    {
        if (gameState.screens[i] != nullptr)
        {
            delete gameState.screens[i];
        }
    }

    ResourceManager::UnloadResources();
    CloseAudioDevice();
}
