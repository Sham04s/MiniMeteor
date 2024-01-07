#include <stdio.h>
#include <math.h>
#include <algorithm>

#include "raylib.h"

#include "game.hpp"
#include "game_ui.hpp"
#include "raylib_logo.hpp"

#include "resource_manager.hpp"
#include "score_registry.hpp"
#include "player.hpp"
#include "asteroid.hpp"
#include "enemy.hpp"
#include "power_up.hpp"
#include "utils.hpp"

#ifdef _DEBUG
bool SHOW_DEBUG = true;
#else
bool SHOW_DEBUG = false;
#endif // _DEBUG

bool HIDE_SPRITES = false;

#define INITIAL_ASTEROIDS 5
#define INITIAL_ENEMIES 1

GameState gameState;

// TODO: add credits screen (kenney.nl, openfonts?)
// TODO: organize code files
// TODO: improve comments

float oneSecondTimer = 0.0f;

bool InitGame()
{
    SetExitKey(KEY_NULL);
    gameState.fps = 60;
    SetTargetFPS(gameState.fps);
    EnableCursor();
    ShowCursor();

    gameState.fullscreen = false;
    gameState.windowSize = {(float)GetScreenWidth(), (float)GetScreenHeight()};
    gameState.originalWindowSize = gameState.windowSize;
    gameState.hasStartedGame = false;

#ifdef _DEBUG
    SetTraceLogLevel(LOG_ALL);
#endif // _DEBUG

    InitAudioDevice();
    SetMasterVolume(0);

    if (!ResourceManager::LoadResources())
    {
        TraceLog(LOG_ERROR, "Failed to load resources!\n");
        ExitGame();
        return false;
    }

    gameState.previousScreen = LOADING;
    gameState.currentScreen = LOADING;
    gameState.powerupSpawned = false;

    gameState.asteroidsCount = 0;
    gameState.enemiesCount = 0;

    gameState.player = new Player();

    CreateUIElements(gameState.player);
    CreateNewGame(INITIAL_ASTEROIDS, INITIAL_ENEMIES);

    return true;
}

void UpdateDifficultySettings(Difficulty diff)
{
    DifficultySettings *diffSettings = &gameState.diffSettings;

    diffSettings->difficulty = diff;
    diffSettings->maxAsteroids = powf(3, diff + 1);                                                   // 3, 9, 27
    diffSettings->maxEnemies = powf(2, diff + 1);                                                     // 2, 4, 8
    diffSettings->asteroidsSpawnChance = 0.25f + (float)(gameState.diffSettings.difficulty) * 0.25f;  // 0.25, 0.5, 0.75
    diffSettings->enemiesSpawnChance = 0.2f + (float)(gameState.diffSettings.difficulty + 1) * 0.1f;  // 0.2, 0.3, 0.4
    diffSettings->powerupSpawnChance = 0.15f + (float)(gameState.diffSettings.difficulty) * 0.2f;     // 0.15, 0.35, 0.55
    diffSettings->asteroidSpeedMultiplier = 1.0f + (float)(gameState.diffSettings.difficulty) * 0.5f; // 1.0, 1.5, 2.0
    diffSettings->scoreMultiplier = powf(1.2f, gameState.diffSettings.difficulty);

    EnemyAttributes *enemiesAttr = &gameState.diffSettings.enemiesAttributes;

    enemiesAttr->velocityMultiplier = 0.8f + (float)(gameState.diffSettings.difficulty) * 0.6f;       // 0.8, 1.4, 2.0
    enemiesAttr->precisionMultiplier = 0.5f + (float)(gameState.diffSettings.difficulty) * 0.25f;     // 0.5, 0.75, 1.0
    enemiesAttr->fireRateMultiplier = 0.5f + (float)(gameState.diffSettings.difficulty) * 0.25f;      // 0.5, 0.75, 1.0
    enemiesAttr->bulletSpeedMultiplier = 0.9f + (float)(gameState.diffSettings.difficulty) * 0.2f;    // 0.9, 1.1, 1.3
    enemiesAttr->probOfShootingToPlayer = 0.35f + (float)(gameState.diffSettings.difficulty) * 0.25f; // 0.35, 0.6, 0.85
}

void CreateNewGame(size_t numAsteroids, size_t numEnemies)
{

    gameState.asteroidsCount = 0;
    gameState.enemiesCount = 0;
    gameState.powerupSpawned = false;

    UpdateDifficultySettings(EASY); // difficulty starts at easy and is increased as the player scores more points

    ResetScoreRegistry();

    if (gameState.spaceBackground != nullptr)
    {
        UnloadTexture(*gameState.spaceBackground);
    }
    gameState.spaceBackground = GenerateStarsBackground(4096, 4096, GetRandomValue(1000, 2000), 1, 2);

    // reset gameState.player
    if (gameState.player != nullptr)
    {
        gameState.player->Reset();
    }
    else
    {
        gameState.player = new Player();
    }

    // delete all game objects
    for (size_t i = 0; i < gameState.gameObjects.size(); i++)
    {
        delete gameState.gameObjects[i];
    }
    gameState.gameObjects.clear();

    // create new game objects
    for (size_t i = 0; i < numAsteroids + numEnemies; i++)
    {
        // generate random position exluding the gameState.player's position and other objects positions
        Vector2 pos;
        bool insidePlayer;
        bool insideGameObject;
        do
        {
            pos = {(float)GetRandomValue(0, GetScreenWidth()), (float)GetRandomValue(0, GetScreenHeight())};
            insidePlayer = Vector2Distance(pos, gameState.player->GetOrigin()) < 100;
            insideGameObject = false;
            for (size_t j = 0; j < gameState.gameObjects.size(); j++)
            {
                if (Vector2Distance(pos, gameState.gameObjects[j]->GetOrigin()) < 100)
                {
                    insideGameObject = true;
                    break;
                }
            }
        } while (insidePlayer || insideGameObject);

        if (i < numAsteroids)
        {
            gameState.gameObjects.push_back(new Asteroid(pos, gameState.diffSettings.asteroidSpeedMultiplier));
            gameState.asteroidsCount++;
        }
        else
        {
            gameState.gameObjects.push_back(new BasicEnemy(pos, gameState.player, gameState.diffSettings.enemiesAttributes));
            gameState.enemiesCount++;
        }
    }
}

void ChangeScreen(ScreenID screen)
{
    if (gameState.currentScreen == screen)
    {
        return;
    }
    gameState.previousScreen = gameState.currentScreen;
    gameState.currentScreen = screen;

    if (gameState.screens[gameState.currentScreen] != nullptr)
    {
        gameState.screens[gameState.currentScreen]->Update();
    }

    if (screen == MAIN_MENU && gameState.hasStartedGame)
    {
        gameState.hasStartedGame = false;
        gameState.previousScreen = MAIN_MENU;
        gameState.player->Hide();
        CreateNewGame(INITIAL_ASTEROIDS, INITIAL_ENEMIES);
    }
    if (screen == GAME && !gameState.hasStartedGame)
    {
        gameState.hasStartedGame = true;
        gameState.player->Show();
    }

    if (screen == GAME)
    {
        SetMasterVolume(1);

#ifndef _DEBUG
        DisableCursor();
        HideCursor();
#endif // !_DEBUG
    }
    else
    {
        SetMasterVolume(0);

#ifndef _DEBUG
        EnableCursor();
        ShowCursor();
#endif // !_DEBUG
    }
}

void PreviousScreen()
{
    if (gameState.currentScreen >= 3 && gameState.currentScreen)
    {
        ChangeScreen(gameState.previousScreen);
    }
}

void ToggleGameFullscreen()
{
#ifdef PLATFORM_WEB
    TraceLog(LOG_WARNING, "Fullscreen is not supported on web");
    return;
#endif // PLATFORM_WEB

#ifdef PLATFORM_DESKTOP
    gameState.fullscreen = !gameState.fullscreen;
    if (gameState.fullscreen)
    {
        int monitor = GetCurrentMonitor();
        SetWindowSize(GetMonitorWidth(monitor), GetMonitorHeight(monitor));
        SetWindowPosition(0, 0);
        ToggleBorderlessWindowed();
    }
    else
    {
        ToggleBorderlessWindowed();
        SetWindowSize((int)gameState.originalWindowSize.x, (int)gameState.originalWindowSize.y);
        SetWindowPosition((int)(GetMonitorWidth(0) - gameState.originalWindowSize.x) / 2, (int)(GetMonitorHeight(0) - gameState.originalWindowSize.y) / 2);
    }
#endif // PLATFORM_DESKTOP
}

void ResizeCallback(Vector2 prevWindowSize)
{
    gameState.windowSize = {(float)GetScreenWidth(), (float)GetScreenHeight()};

    for (size_t i = 0; i < NUM_SCREENS; i++)
    {
        if (gameState.screens[i] != nullptr)
        {
            gameState.screens[i]->Resize(prevWindowSize);
        }
    }

    gameState.player->UpdateCamera();
}

void ChangeFPS()
{
#ifdef PLATFORM_DESKTOP
    static const int fps[] = {15, 30, 60, 120, 0};
#else
    static const int fps[] = {60, 0};
#endif // PLATFORM_DESKTOP
    static int fpsIndex = 2;
    fpsIndex = (fpsIndex + 1) % (sizeof(fps) / sizeof(int));
    gameState.fps = fps[fpsIndex];
    SetTargetFPS(gameState.fps);
}

void PauseGame()
{
    ChangeScreen(PAUSE_MENU);
    gameState.player->PauseSounds();
    for (size_t i = 0; i < gameState.gameObjects.size(); i++)
    {
        gameState.gameObjects[i]->PauseSounds();
    }
}

void ResumeGame()
{
    ChangeScreen(GAME);
    gameState.player->ResumeSounds();
    for (size_t i = 0; i < gameState.gameObjects.size(); i++)
    {
        gameState.gameObjects[i]->ResumeSounds();
    }
}

void RestartGame()
{
    CreateNewGame(INITIAL_ASTEROIDS, INITIAL_ENEMIES);
    ChangeScreen(GAME);
}

void DrawFrame()
{
    BeginDrawing();
    ClearBackground(BACKGROUND_COLOR);

    if (gameState.currentScreen != EXITING && gameState.currentScreen != LOADING)
    {
        // draw background centered
        DrawTexture(*gameState.spaceBackground, (int)(GetScreenWidth() - gameState.spaceBackground->width) / 2, (int)(GetScreenHeight() - gameState.spaceBackground->height) / 2, WHITE);

        BeginMode2D(gameState.player->GetCamera());

        for (size_t i = 0; i < gameState.gameObjects.size(); i++)
        {
            gameState.gameObjects[i]->Draw();
        }
        gameState.player->Draw();

        EndMode2D();
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

PowerUpType powerupToSpawn = LIFE;

void DrawDebug()
{
    if (gameState.currentScreen == LOADING)
    {
        gameState.screens[LOADING]->DrawDebug();
        return;
    }
    if (gameState.currentScreen == GAME || gameState.currentScreen == GAME_OVER)
    {
        BeginMode2D(gameState.player->GetCamera());

        gameState.player->DrawDebug();
        for (size_t i = 0; i < gameState.gameObjects.size(); i++)
        {
            gameState.gameObjects[i]->DrawDebug();
        }

        EndMode2D();
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

    DrawText(TextFormat("Difficulty: %d", gameState.diffSettings.difficulty), 10, GetScreenHeight() - 60, 20, WHITE);
    DrawText(TextFormat("Max asteroids: %d", gameState.diffSettings.maxAsteroids), 10, GetScreenHeight() - 80, 20, WHITE);
    DrawText(TextFormat("Max enemies: %d", gameState.diffSettings.maxEnemies), 10, GetScreenHeight() - 100, 20, WHITE);
    DrawText(TextFormat("Asteroids spawn rate: %.2f", gameState.diffSettings.asteroidsSpawnChance), 10, GetScreenHeight() - 120, 20, WHITE);
    DrawText(TextFormat("Enemies spawn rate: %.2f", gameState.diffSettings.enemiesSpawnChance), 10, GetScreenHeight() - 140, 20, WHITE);
    DrawText(TextFormat("Asteroid speed multiplier: %.2f", gameState.diffSettings.asteroidSpeedMultiplier), 10, GetScreenHeight() - 160, 20, WHITE);
    DrawText(TextFormat("Enemy speed multiplier:  %.2f", gameState.diffSettings.enemiesAttributes.velocityMultiplier), 10, GetScreenHeight() - 180, 20, WHITE);
    DrawText(TextFormat("Enemy precision multiplier:  %.2f", gameState.diffSettings.enemiesAttributes.precisionMultiplier), 10, GetScreenHeight() - 200, 20, WHITE);
    DrawText(TextFormat("Enemy fire rate multiplier:  %.2f", gameState.diffSettings.enemiesAttributes.fireRateMultiplier), 10, GetScreenHeight() - 220, 20, WHITE);
    DrawText(TextFormat("Enemy bullet speed multiplier:  %.2f", gameState.diffSettings.enemiesAttributes.bulletSpeedMultiplier), 10, GetScreenHeight() - 240, 20, WHITE);
    DrawText(TextFormat("Enemy prob of shooting to gameState.player:  %.2f", gameState.diffSettings.enemiesAttributes.probOfShootingToPlayer), 10, GetScreenHeight() - 260, 20, WHITE);
    DrawText(TextFormat("Powerup spawn chance: %.2f", gameState.diffSettings.powerupSpawnChance), 10, GetScreenHeight() - 280, 20, WHITE);
    DrawText(TextFormat("Asteroids count: %d", gameState.asteroidsCount), 10, GetScreenHeight() - 300, 20, WHITE);
    DrawText(TextFormat("Enemies count: %d", gameState.enemiesCount), 10, GetScreenHeight() - 320, 20, WHITE);

    DrawText(TextFormat("Powerup to spawn: %s", PowerUp::GetPowerUpName(powerupToSpawn)), 400, GetScreenHeight() - 40, 20, WHITE);
}

void HandleInput()
{
    if (IsKeyPressed(KEY_F11))
    {
        ToggleGameFullscreen();
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
        else
        {
            PreviousScreen();
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

    if (IsKeyPressed(KEY_R))
    {
        CreateNewGame(INITIAL_ASTEROIDS, INITIAL_ENEMIES);
    }
    if (IsKeyPressed(KEY_L))
    {
        UpdateDifficultySettings((Difficulty)((gameState.diffSettings.difficulty + 1) % NUM_DIFFICULTIES));
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
    // spawn an asteroid
    if (IsKeyPressed(KEY_X))
    {
        gameState.gameObjects.push_back(new Asteroid((AsteroidVariant)GetRandomValue(0, 1), gameState.diffSettings.asteroidSpeedMultiplier));
        gameState.asteroidsCount++;
    }
    // spawn an enemy
    if (IsKeyPressed(KEY_C))
    {
        gameState.gameObjects.push_back(new BasicEnemy(RandomVecOutsideScreen(100), gameState.player, gameState.diffSettings.enemiesAttributes));
        gameState.enemiesCount++;
    }
    // spawn the selected powerup in the mouse position
    if (IsKeyPressed(KEY_V))
    {
        if (gameState.powerupSpawned)
        {
            auto powerup = std::find_if(gameState.gameObjects.begin(), gameState.gameObjects.end(), [](GameObject *obj)
                                        { return obj->GetType() == POWER_UP; });
            if (powerup != gameState.gameObjects.end())
            {
                delete *powerup;
                gameState.gameObjects.erase(powerup);
            }
        }
        gameState.gameObjects.push_back(new PowerUp(GetScreenToWorld2D(GetMousePosition(), gameState.player->GetCamera()), powerupToSpawn));
        gameState.powerupSpawned = true;
    }

    if (IsKeyPressed(KEY_KP_ADD))
    {
        gameState.player->AddLife();
    }
    if (IsKeyPressed(KEY_KP_SUBTRACT))
    {
        gameState.player->Kill();
    }

    if (IsKeyPressed(KEY_P))
    {
        gameState.player->ToggleDirectionalShip();
    }

    if (GetMouseWheelMove() > 0)
    {
        powerupToSpawn = (PowerUpType)((powerupToSpawn + 1) % NUM_POWER_UP_TYPES);
    }
    if (GetMouseWheelMove() < 0)
    {
        powerupToSpawn = (PowerUpType)((powerupToSpawn - 1 + NUM_POWER_UP_TYPES) % NUM_POWER_UP_TYPES);
    }

    // moving objects with mouse
    static GameObject *movingObject = nullptr;

    Vector2 mouseWorldPos = GetScreenToWorld2D(GetMousePosition(), gameState.player->GetCamera());

    if (gameState.player != nullptr && CheckCollisionPointRec(mouseWorldPos, gameState.player->GetBounds()))
    {
        if (movingObject == nullptr && IsKeyDown(KEY_LEFT_CONTROL) && IsMouseButtonDown(MOUSE_LEFT_BUTTON))
        {
            movingObject = gameState.player;
        }
    }

    for (size_t i = 0; i < gameState.gameObjects.size(); i++)
    {
        if (CheckCollisionPointRec(mouseWorldPos, gameState.gameObjects[i]->GetBounds()))
        {
            if (movingObject == nullptr && IsKeyDown(KEY_LEFT_CONTROL) && (IsMouseButtonDown(MOUSE_LEFT_BUTTON) || IsMouseButtonDown(MOUSE_RIGHT_BUTTON)))
            {
                movingObject = gameState.gameObjects[i];
            }
        }
    }

    if (movingObject != nullptr && (IsMouseButtonReleased(MOUSE_LEFT_BUTTON) || IsMouseButtonReleased(MOUSE_RIGHT_BUTTON)))
    {
        movingObject = nullptr;
    }

    if (movingObject != nullptr)
    {
        if (IsMouseButtonDown(MOUSE_LEFT_BUTTON))
        {
            movingObject->SetVelocity({0, 0});
            movingObject->Translate(GetMouseDelta());
        }
        if (IsMouseButtonDown(MOUSE_RIGHT_BUTTON))
        {
            movingObject->SetVelocity(Vector2Scale(GetMouseDelta(), 0.4f / GetFrameTime()));
            movingObject->SetAngularVelocity(GetRandomValue(-180, 180));
        }
    }

#endif // _DEBUG
}

void UpdateGameObjects()
{
    const float scoreMultiplier = gameState.diffSettings.scoreMultiplier;

    if (gameState.currentScreen == GAME)
    {
        gameState.player->Update();
    }
    for (size_t i = 0; i < gameState.gameObjects.size(); i++)
    {
        gameState.gameObjects[i]->Update();

        if (gameState.gameObjects[i]->GetType() == ASTEROID)
        {
            Asteroid *asteroid = (Asteroid *)gameState.gameObjects[i];
            if (asteroid->IsDestroyed())
            {
                AddScore(asteroid->GetVariant() == LARGE ? LARGE_ASTEROID_DESTROYED : SMALL_ASTEROID_DESTROYED, scoreMultiplier);
                delete asteroid;
                gameState.gameObjects.erase(gameState.gameObjects.begin() + i);
                gameState.asteroidsCount--;
            }
        }
        else if (gameState.gameObjects[i]->GetType() == ENEMY)
        {
            BasicEnemy *enemy = (BasicEnemy *)gameState.gameObjects[i];
            if (enemy->IsDead() && enemy->GetBullets()->size() == 0)
            {
                AddScore(BASIC_ENEMY_KILLED, scoreMultiplier);
                delete enemy;
                gameState.gameObjects.erase(gameState.gameObjects.begin() + i);
                gameState.enemiesCount--;
            }
        }
        else if (gameState.gameObjects[i]->GetType() == POWER_UP)
        {
            PowerUp *powerup = (PowerUp *)gameState.gameObjects[i];
            if (powerup->IsExpired())
            {
                delete powerup;
                gameState.gameObjects.erase(gameState.gameObjects.begin() + i);
                gameState.powerupSpawned = false;
            }
            else if (powerup->IsPickedUp())
            {
                AddScore((ScoreType)powerup->GetType(), scoreMultiplier);
                gameState.gameObjects.erase(gameState.gameObjects.begin() + i);
                gameState.powerupSpawned = false;
            }
        }
    }
    gameState.gameObjects.shrink_to_fit();

    gameState.player->CleanBullets();
    for (size_t i = 0; i < gameState.gameObjects.size(); i++)
    {
        if (gameState.gameObjects[i]->GetType() == ENEMY)
        {
            BasicEnemy *enemy = (BasicEnemy *)gameState.gameObjects[i];
            enemy->CleanBullets();
        }
    }
}

void HandleCollisions()
{
    Vector2 pushVector = {0, 0};
    auto bullets = gameState.player->GetBullets();

    // check collisions between gameState.player and all main game objects
    for (size_t i = 0; i < gameState.gameObjects.size(); i++)
    {
        if (gameState.player->CheckCollision(gameState.gameObjects[i], &pushVector))
        {
            gameState.player->HandleCollision(gameState.gameObjects[i], &pushVector);
            pushVector = Vector2Negate(pushVector);
            gameState.gameObjects[i]->HandleCollision(gameState.player, &pushVector);
        }
        for (size_t j = 0; j < gameState.gameObjects.size(); j++)
        {
            if (i < j) // check each pair only once
            {
                if (gameState.gameObjects[i]->CheckCollision(gameState.gameObjects[j], &pushVector))
                {
                    gameState.gameObjects[i]->HandleCollision(gameState.gameObjects[j], &pushVector);
                    pushVector = Vector2Negate(pushVector);
                    gameState.gameObjects[j]->HandleCollision(gameState.gameObjects[i], &pushVector);
                }
            }
        }
    }

    // check collision between bullets and all main game objects
    for (size_t i = 0; i < gameState.gameObjects.size(); i++)
    {
        // player bullets
        for (size_t b = 0; b < bullets->size(); b++)
        {
            if ((*bullets)[b].CheckCollision(gameState.gameObjects[i], &pushVector))
            {
                // (*bullets)[b].HandleCollision(gameState.gameObjects[i], &pushVector);
                gameState.player->HandleBulletCollision(&(*bullets)[b], gameState.gameObjects[i], &pushVector);
                pushVector = Vector2Negate(pushVector);
                gameState.gameObjects[i]->HandleCollision(&(*bullets)[b], &pushVector);
            }
        }

        // enemy bullets
        if (gameState.gameObjects[i]->GetType() == ENEMY)
        {
            BasicEnemy *enemy = (BasicEnemy *)gameState.gameObjects[i];
            auto enemyBullets = enemy->GetBullets();
            for (size_t b = 0; b < enemyBullets->size(); b++)
            {
                if ((*enemyBullets)[b].CheckCollision(gameState.player, &pushVector))
                {
                    (*enemyBullets)[b].HandleCollision(gameState.player, &pushVector);
                    pushVector = Vector2Negate(pushVector);
                    gameState.player->HandleCollision(&(*enemyBullets)[b], &pushVector);
                }
            }
        }
    }
}

void TryToSpawnObject(GameObjectType type)
{
    if (oneSecondTimer < 1.0f)
    {
        return;
    }

    float spawnChance = 0.0f;
    switch (type)
    {
    case ASTEROID:
        spawnChance = gameState.asteroidsCount == gameState.diffSettings.maxAsteroids
                          ? 0.0f
                          : gameState.diffSettings.asteroidsSpawnChance;
        break;
    case ENEMY:
        spawnChance = gameState.enemiesCount == gameState.diffSettings.maxEnemies
                          ? 0.0f
                          : gameState.diffSettings.enemiesSpawnChance;
        break;
    case POWER_UP:
        spawnChance = gameState.powerupSpawned
                          ? 0.0f
                          : gameState.diffSettings.powerupSpawnChance;
        break;
    default:
        return;
    }

    if (GetRandomValue(0, 100) >= spawnChance * 100)
    {
        return;
    }

    switch (type)
    {
    case ASTEROID:
        gameState.gameObjects.push_back(new Asteroid((AsteroidVariant)GetRandomValue(0, 1), gameState.diffSettings.asteroidSpeedMultiplier));
        gameState.asteroidsCount++;
        break;
    case ENEMY:
        gameState.gameObjects.push_back(new BasicEnemy(RandomVecOutsideScreen(CHARACTER_SIZE), gameState.player, gameState.diffSettings.enemiesAttributes));
        gameState.enemiesCount++;
        break;
    case POWER_UP:
        gameState.gameObjects.push_back(new PowerUp(RandomVecInsideScreen(POWER_UP_SIZE),
                                                    (PowerUpType)GetRandomValue(0, NUM_POWER_UP_TYPES - 1)));
        gameState.powerupSpawned = true;
        break;
    default:
        break;
    }
}

void UpdateGame()
{
    if (gameState.currentScreen != PAUSE_MENU && !(gameState.previousScreen == PAUSE_MENU && gameState.currentScreen != GAME))
    {
        UpdateGameObjects();

        HandleCollisions();

        if (gameState.currentScreen == GAME && gameState.player->IsAlive() && gameState.player->HasMoved())
        {
            TryToSpawnObject(ASTEROID);
            TryToSpawnObject(ENEMY);
            TryToSpawnObject(POWER_UP);

            // this score is scaled relative to frame time
            AddScore(TIME_ALIVE, 1.0f);
            if (GetTotalScore() >= 50000 && gameState.diffSettings.difficulty == EASY)
            {
                UpdateDifficultySettings(MEDIUM);
            }
            else if (GetTotalScore() >= 100000 && gameState.diffSettings.difficulty == MEDIUM)
            {
                UpdateDifficultySettings(HARD);
            }
        }

        // game over
        if (gameState.player->IsDead() && gameState.player->GetLives() <= 0)
        {
            gameState.previousScreen = gameState.currentScreen;
            gameState.currentScreen = GAME_OVER;
        }

        // reset oneSecondTimer
        if (oneSecondTimer > 1.0f)
        {
            oneSecondTimer = 0.0f;
        }
        oneSecondTimer += GetFrameTime();
    }

    if (gameState.screens[gameState.currentScreen] == nullptr)
    {
        return;
    }
    gameState.screens[gameState.currentScreen]->Update();
    if (gameState.currentScreen == LOADING)
    {
        if (((RaylibLogo *)gameState.screens[LOADING])->IsDone())
        {
            ChangeScreen(MAIN_MENU);
        }
    }

    static Vector2 prevWindowSize = gameState.windowSize;
    if (IsWindowResized())
    {
        ResizeCallback(prevWindowSize);
        prevWindowSize = gameState.windowSize;
    }
}

bool GameLoop()
{
    HandleInput();

    UpdateGame();

    DrawFrame();

    if (gameState.currentScreen == EXITING)
    {
#ifdef PLATFORM_WEB
        ExitGame();
        InitGame();
#else
        return false;
#endif // PLATFORM_WEB
    }

    return true;
}

void ExitGame()
{
    delete gameState.player;

    for (size_t i = 0; i < gameState.gameObjects.size(); i++)
    {
        delete gameState.gameObjects[i];
    }
    gameState.gameObjects.clear();

    for (size_t i = 0; i < NUM_SCREENS; i++)
    {
        if (gameState.screens[i] != nullptr)
        {
            delete gameState.screens[i];
        }
    }

    if (gameState.spaceBackground != nullptr)
    {
        UnloadTexture(*gameState.spaceBackground);
    }
    ResourceManager::UnloadResources();
    CloseAudioDevice();
}
