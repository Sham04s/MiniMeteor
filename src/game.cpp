#include <stdio.h>
#include <math.h>

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
std::vector<Asteroid *> asteroids;
std::vector<BasicEnemy *> enemies;
PowerUp *powerup; // only one powerup can be in the screen at a time

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
    for (size_t i = 0; i < asteroids.size(); i++)
    {
        delete asteroids[i];
    }
    asteroids.clear();
    for (size_t i = 0; i < 5; i++)
    {
        // generate random position exluding the player's position and other asteroids positions
        Vector2 pos;
        bool insidePlayer;
        bool insideAsteroid;
        do
        {
            pos = {(float)GetRandomValue(0, GetScreenWidth()), (float)GetRandomValue(0, GetScreenHeight())};
            insidePlayer = Vector2Distance(pos, player->GetOrigin()) < 100;
            insideAsteroid = false;
            for (size_t j = 0; j < asteroids.size(); j++)
            {
                if (Vector2Distance(pos, asteroids[j]->GetOrigin()) < 100)
                {
                    insideAsteroid = true;
                    break;
                }
            }
        } while (insidePlayer || insideAsteroid);

        asteroids.push_back(new Asteroid(pos));
    }

    for (size_t i = 0; i < enemies.size(); i++)
    {
        delete enemies[i];
    }
    enemies.clear();
    for (size_t i = 0; i < 2; i++)
    {
        // generate random position exluding the player's position and other asteroids positions
        Vector2 pos;
        bool insidePlayer;
        bool insideAsteroid;
        bool insideEnemy;
        do
        {
            pos = {(float)GetRandomValue(0, GetScreenWidth()), (float)GetRandomValue(0, GetScreenHeight())};
            insidePlayer = Vector2Distance(pos, player->GetOrigin()) < 100;
            insideAsteroid = false;
            for (size_t j = 0; j < asteroids.size(); j++)
            {
                if (Vector2Distance(pos, asteroids[j]->GetOrigin()) < 100)
                {
                    insideAsteroid = true;
                    break;
                }
            }
            insideEnemy = false;
            for (size_t j = 0; j < enemies.size(); j++)
            {
                if (Vector2Distance(pos, enemies[j]->GetOrigin()) < 100)
                {
                    insideEnemy = true;
                    break;
                }
            }
        } while (insidePlayer || insideAsteroid || insideEnemy);

        enemies.push_back(new BasicEnemy(pos));
    }
}

void PauseGame()
{
    gameState.previousScreen = gameState.currentScreen;
    gameState.currentScreen = PAUSE_MENU;
    player->PauseSounds();
    for (size_t i = 0; i < asteroids.size(); i++)
    {
        asteroids[i]->PauseSounds();
    }
    for (size_t i = 0; i < enemies.size(); i++)
    {
        enemies[i]->PauseSounds();
    }
}

void ResumeGame()
{
    gameState.previousScreen = gameState.currentScreen;
    gameState.currentScreen = GAME;
    player->ResumeSounds();
    for (size_t i = 0; i < asteroids.size(); i++)
    {
        asteroids[i]->ResumeSounds();
    }
    for (size_t i = 0; i < enemies.size(); i++)
    {
        enemies[i]->ResumeSounds();
    }
}

void DrawFrame()
{
    BeginDrawing();
    ClearBackground(BACKGROUND_COLOR);

    if (gameState.currentScreen == GAME || gameState.currentScreen == GAME_OVER || gameState.currentScreen == PAUSE_MENU)
    {
        if (powerup != nullptr)
        {
            powerup->Draw();
        }
        player->Draw();
        for (size_t i = 0; i < asteroids.size(); i++)
        {
            asteroids[i]->Draw();
        }
        for (size_t i = 0; i < enemies.size(); i++)
        {
            enemies[i]->Draw();
        }
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
        if (powerup != nullptr)
        {
            powerup->DrawDebug();
        }
        player->DrawDebug();

        for (size_t i = 0; i < asteroids.size(); i++)
        {
            asteroids[i]->DrawDebug();
        }
        for (size_t i = 0; i < enemies.size(); i++)
        {
            enemies[i]->DrawDebug();
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
        asteroids.push_back(new Asteroid((Vector2){(float)GetRandomValue(0, GetScreenWidth()), (float)GetRandomValue(0, GetScreenHeight())}));
    }
    if (IsKeyPressed(KEY_C))
    {
        enemies.push_back(new BasicEnemy((Vector2){(float)GetRandomValue(0, GetScreenWidth()), (float)GetRandomValue(0, GetScreenHeight())}));
    }
    if (IsKeyPressed(KEY_V))
    {
        if (powerup != nullptr)
        {
            delete powerup;
        }
        powerup = new PowerUp(Vector2Clamp(GetMousePosition(), {0, 0}, {(float)GetScreenWidth(), (float)GetScreenHeight()}),
                              powerupToSpawn);
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

    for (size_t i = 0; i < asteroids.size(); i++)
    {
        if (CheckCollisionPointRec(GetMousePosition(), asteroids[i]->GetBounds()))
        {
            if (IsMouseButtonDown(MOUSE_LEFT_BUTTON))
            {
                asteroids[i]->SetVelocity({0});
                asteroids[i]->SetAngularVelocity(0);
                asteroids[i]->Translate(GetMouseDelta());
            }
            if (IsMouseButtonDown(MOUSE_RIGHT_BUTTON))
            {
                Vector2 velocity = Vector2Subtract(GetMousePosition(), asteroids[i]->GetOrigin());
                asteroids[i]->SetVelocity(Vector2Scale(velocity, 3));
                asteroids[i]->SetAngularVelocity(GetRandomValue(-100, 100));
            }
        }
    }
    for (size_t i = 0; i < enemies.size(); i++)
    {
        if (CheckCollisionPointRec(GetMousePosition(), enemies[i]->GetBounds()))
        {
            if (IsMouseButtonDown(MOUSE_LEFT_BUTTON))
            {
                enemies[i]->SetVelocity({0});
                enemies[i]->SetAngularVelocity(0);
                enemies[i]->Translate(GetMouseDelta());
            }
            if (IsMouseButtonDown(MOUSE_RIGHT_BUTTON))
            {
                Vector2 velocity = Vector2Subtract(GetMousePosition(), enemies[i]->GetOrigin());
                enemies[i]->SetVelocity(Vector2Scale(velocity, 3));
                enemies[i]->SetAngularVelocity(GetRandomValue(-100, 100));
            }
        }
    }

#endif // _DEBUG
}

void HandlePlayerUpdate()
{
    player->Update();
    // if player is dead and has 0 lives left, game over
    if (player->IsDead() && player->GetLives() <= 0)
    {
        gameState.previousScreen = gameState.currentScreen;
        gameState.currentScreen = GAME_OVER;
    }
}

void HandleAsteroidPlayerCollision(Asteroid *asteroid)
{
    Vector2 pushVector = {0, 0};
    // check for collisions between player and asteroids
    if (player->CheckCollision(asteroid, &pushVector))
    {
        if (player->CanBeKilled())
        {
            player->Kill(); // try to kill player if not invincible
        }
        player->Push(asteroid, pushVector); // push both player and asteroid away from each other
    }
}

void HandleAsteroidAsteroidCollision(Asteroid *asteroid, size_t asteroidIdx)
{
    Vector2 pushVector = {0, 0};

    // check for collisions between asteroids
    for (size_t j = 0; j < asteroids.size(); j++)
    {
        if (asteroidIdx < j) // check each pair only once
        {
            if (asteroid->IsFloating() && asteroid->CheckCollision(asteroids[j], &pushVector))
            {
                // push asteroids away from each other with the SAT push vector
                asteroid->Push(asteroids[j], pushVector);
            }
        }
    }
}

void HandleAsteroidEnemyCollision(Asteroid *asteroid)
{
    Vector2 pushVector = {0, 0};

    // check for collisions between enemies and asteroids
    for (size_t i = 0; i < enemies.size(); i++)
    {
        if (asteroid->CheckCollision(enemies[i], &pushVector))
        {
            // push asteroids away from each other with the SAT push vector
            asteroid->Push(enemies[i], pushVector);
        }
    }
}

void HandleAsteroidBulletCollision(Asteroid *asteroid)
{
    Vector2 pushVector = {0, 0};
    auto bullets = player->GetBullets();

    // check for collisions between bullets and asteroids
    for (size_t b = 0; b < bullets->size(); b++)
    {
        if ((*bullets)[b].CheckCollision(asteroid, &pushVector))
        {
            // destroy bullet and asteroid
            bullets->erase(bullets->begin() + b);
            asteroid->Destroy();
        }
    }
    bullets->shrink_to_fit();
}

void HandleEnemyPlayerCollision(BasicEnemy *enemy)
{
    Vector2 pushVector = {0, 0};

    // check for collisions between player and enemies
    if (player->CheckCollision(enemy, &pushVector))
    {
        if (player->CanBeKilled())
        {
            player->Kill();
            enemy->Kill();
        }
        if (player->CanBeHit())
        {
            player->Push(enemy, pushVector); // push both player and enemy away from each other
        }
    }
}

void HandleEnemyEnemyCollision(BasicEnemy *enemy, size_t enemyIdx)
{
    Vector2 pushVector = {0, 0};

    // check for collisions between enemies
    for (size_t j = 0; j < enemies.size(); j++)
    {
        if (enemyIdx < j) // check each pair only once
        {
            if (enemy->IsAlive() && enemy->CheckCollision(enemies[j], &pushVector))
            {
                // push enemies away from each other with the SAT push vector
                enemy->Push(enemies[j], pushVector);
            }
        }
    }
}

void HandleEnemyBulletCollision(BasicEnemy *enemy)
{
    Vector2 pushVector = {0, 0};
    auto bullets = player->GetBullets();

    // check for collisions between player bullets and enemies
    for (size_t b = 0; b < bullets->size(); b++)
    {
        if ((*bullets)[b].CheckCollision(enemy, &pushVector))
        {
            // destroy bullet and kill enemy
            bullets->erase(bullets->begin() + b);
            enemy->Kill();
        }
    }
    bullets->shrink_to_fit();
}

void HandleEnemyBulletPlayerCollision(BasicEnemy *enemy)
{
    Vector2 pushVector = {0, 0};
    auto enemyBullets = enemy->GetBullets();

    // check for collisions between enemy bullets and player
    for (size_t b = 0; b < enemyBullets->size(); b++)
    {
        if ((*enemyBullets)[b].CheckCollision(player, &pushVector))
        {
            if (player->CanBeKilled())
            {
                player->Kill();
            }
            if (player->CanBeHit())
            {
                (*enemyBullets).erase((*enemyBullets).begin() + b);
            }
        }
    }
    (*enemyBullets).shrink_to_fit();
}

void HandleAsteroidCollisions()
{
    for (size_t i = 0; i < asteroids.size(); i++)
    {
        asteroids[i]->Update();

        HandleAsteroidPlayerCollision(asteroids[i]);
        HandleAsteroidAsteroidCollision(asteroids[i], i);
        HandleAsteroidEnemyCollision(asteroids[i]);
        HandleAsteroidBulletCollision(asteroids[i]);

        // erase destroyed asteroids
        if (asteroids[i]->IsDestroyed())
        {
            asteroids.erase(asteroids.begin() + i);
        }
    }
    asteroids.shrink_to_fit();
}

void HandleEnemyActions()
{
    for (size_t i = 0; i < enemies.size(); i++)
    {
        enemies[i]->Update();
        enemies[i]->TryToShootAtPlayer(*player);

        HandleEnemyPlayerCollision(enemies[i]);
        HandleEnemyEnemyCollision(enemies[i], i);
        HandleEnemyBulletCollision(enemies[i]);
        HandleEnemyBulletPlayerCollision(enemies[i]);

        auto enemyBullets = enemies[i]->GetBullets();

        // remove dead enemies from the enemies vector
        if (enemies[i]->IsDead())
        {
            // erase dead enemies if they don't have any bullets left
            if (enemyBullets->size() == 0)
            {
                delete enemies[i];
                enemies.erase(enemies.begin() + i);
            }
            else // try to clean up bullets
            {
                enemies[i]->CleanBullets();
            }
        }
    }
}

void HandlePowerUpUpdate()
{
    if (powerup == nullptr)
    {
        // try to spawn a power up
        const float spawnChance = 0.10f; // 10% chance of spawning a power up every second
        if (fmodf(GetTime(), 1.0f) < GetFrameTime() && GetRandomValue(0, 100) < spawnChance * 100)
        {
            powerup = new PowerUp({(float)GetRandomValue(0, GetScreenWidth()), (float)GetRandomValue(0, GetScreenHeight())},
                                  (PowerUpType)GetRandomValue(0, NUM_POWER_UP_TYPES - 1));
        }
        return;
    }

    powerup->Update();

    Vector2 pushVector = {0, 0};

    if (powerup->CheckCollision(player, &pushVector))
    {
        if (player->AddPowerup(powerup))
        {
            // at this point the powerup is owned by the player
            powerup = nullptr;
            return;
        }
    }

    if (powerup->IsExpired())
    {
        delete powerup;
        powerup = nullptr;
    }
}

void UpdateGame()
{
    if (gameState.currentScreen == GAME || gameState.currentScreen == GAME_OVER)
    {
        HandlePlayerUpdate();

        HandleAsteroidCollisions();

        HandleEnemyActions();

        HandlePowerUpUpdate();
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

    if (powerup != nullptr)
    {
        delete powerup;
    }

    for (size_t i = 0; i < asteroids.size(); i++)
    {
        delete asteroids[i];
    }

    for (size_t i = 0; i < enemies.size(); i++)
    {
        delete enemies[i];
    }

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
