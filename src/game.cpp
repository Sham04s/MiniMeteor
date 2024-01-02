#include <stdio.h>
#include <math.h>
#include <algorithm>

#include "raylib.h"

#include "game.hpp"
#include "game_ui.hpp"

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

// TODO: make spawning animations for asteroids and enemies
#define ASTEROIDS_SPAWN_RATE 0.4f // 40% chance of spawning an asteroid every second when asteroidsCount < maxAsteroids
#define ENEMIES_SPAWN_RATE 0.2f   // 20% chance of spawning an enemy every second when enemiesCount < maxEnemies

GameState gameState;

Player *player;
std::vector<GameObject *> gameObjects;

Texture2D *spaceBackground = nullptr;

float oneSecondTimer = 0.0f;

void InitGame()
{
    SetExitKey(KEY_NULL);
    gameState.fps = 60;
    SetTargetFPS(gameState.fps);

    gameState.fullscreen = false;
    gameState.windowSize = {(float)GetScreenWidth(), (float)GetScreenHeight()};
    gameState.originalWindowSize = gameState.windowSize;

#ifdef _DEBUG
    SetTraceLogLevel(LOG_ALL);
#endif // _DEBUG

    InitAudioDevice();

    if (!ResourceManager::LoadResources())
    {
        printf("Failed to load resources!\n");
    }

    gameState.previousScreen = MAIN_MENU;
    gameState.currentScreen = MAIN_MENU;
    gameState.powerupSpawned = false;

    gameState.asteroidsCount = 0;
    gameState.enemiesCount = 0;

    player = new Player();

    CreateUIElements(player);
}

void UpdateDifficultySettings(Difficulty diff)
{
    DifficultySettings *diffSettings = &gameState.diffSettings;

    diffSettings->difficulty = diff;
    diffSettings->maxAsteroids = powf(3, diff + 1);                                                                // 3, 9, 27
    diffSettings->maxEnemies = powf(2, diff + 1);                                                                  // 2, 4, 8
    diffSettings->asteroidsSpawnChance = ASTEROIDS_SPAWN_RATE + (float)(gameState.diffSettings.difficulty) * 0.2f; // 0.4, 0.6, 0.8
    diffSettings->enemiesSpawnChance = ENEMIES_SPAWN_RATE + (float)(gameState.diffSettings.difficulty + 1) * 0.1f; // 0.2, 0.3, 0.4
    diffSettings->powerupSpawnChance = 0.2f + (float)(gameState.diffSettings.difficulty) * 0.1f;                   // 0.2, 0.3, 0.4
    diffSettings->asteroidSpeedMultiplier = 1.0f + (float)(gameState.diffSettings.difficulty) * 0.5f;              // 1.0, 1.5, 2.0
    diffSettings->scoreMultiplier = powf(1.2f, gameState.diffSettings.difficulty);

    EnemyAttributes *enemiesAttr = &gameState.diffSettings.enemiesAttributes;

    enemiesAttr->velocityMultiplier = 0.8f + (float)(gameState.diffSettings.difficulty) * 0.6f;       // 0.8, 1.4, 2.0
    enemiesAttr->precisionMultiplier = 0.5f + (float)(gameState.diffSettings.difficulty) * 0.25f;     // 0.5, 0.75, 1.0
    enemiesAttr->fireRateMultiplier = 0.5f + (float)(gameState.diffSettings.difficulty) * 0.25f;      // 0.5, 0.75, 1.0
    enemiesAttr->bulletSpeedMultiplier = 0.9f + (float)(gameState.diffSettings.difficulty) * 0.2f;    // 0.9, 1.1, 1.3
    enemiesAttr->probOfShootingToPlayer = 0.35f + (float)(gameState.diffSettings.difficulty) * 0.25f; // 0.35, 0.6, 0.85
}

void CreateNewGame()
{
    ChangeScreen(GAME);

    gameState.asteroidsCount = 0;
    gameState.enemiesCount = 0;
    gameState.powerupSpawned = false;

    UpdateDifficultySettings(EASY);

    ResetScoreRegistry();

    if (spaceBackground != nullptr)
    {
        UnloadTexture(*spaceBackground);
    }

    // create background with max resolution
    const int monitor = GetCurrentMonitor();
    const Vector2 monitorSize = {(float)GetMonitorWidth(monitor), (float)GetMonitorHeight(monitor)};
    Image spaceBackgroundImg = GenImageColor(monitorSize.x, monitorSize.y, BLANK);

    const int minStars = 30;
    const int maxStars = 70;
    const float minStarSize = 1.0f;
    const float maxStarSize = 2.0f;
    const float minStarAlpha = 0.3f;
    const float maxStarAlpha = 0.6f;

    for (int i = 0; i < GetRandomValue(minStars, maxStars); i++)
    {
        Color starColor = {255, 255, 255, (unsigned char)GetRandomValue(minStarAlpha * 255, maxStarAlpha * 255)};
        ImageDrawCircle(&spaceBackgroundImg, GetRandomValue(0, monitorSize.x), GetRandomValue(0, monitorSize.y), GetRandomValue(minStarSize, maxStarSize), starColor);
    }

    spaceBackground = new Texture2D(LoadTextureFromImage(spaceBackgroundImg));
    UnloadImage(spaceBackgroundImg);

    // reset player
    if (player != nullptr)
    {
        player->Reset();
    }
    else
    {
        player = new Player();
    }

    // delete all game objects
    for (size_t i = 0; i < gameObjects.size(); i++)
    {
        delete gameObjects[i];
    }
    gameObjects.clear();

    const size_t numAsteroids = 3;
    const size_t numEnemies = 1;

    // create new game objects
    for (size_t i = 0; i < numAsteroids + numEnemies; i++)
    {
        // generate random position exluding the player's position and other objects positions
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
            gameObjects.push_back(new Asteroid(pos, gameState.diffSettings.asteroidSpeedMultiplier));
            gameState.asteroidsCount++;
        }
        else
        {
            gameObjects.push_back(new BasicEnemy(pos, player, gameState.diffSettings.enemiesAttributes));
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

    if (screen == MAIN_MENU)
    {
        for (size_t i = 0; i < gameObjects.size(); i++)
        {
            delete gameObjects[i];
        }
        gameObjects.clear();
        player->Reset();
    }
}

void PreviousScreen()
{
    if (gameState.currentScreen == OPTIONS)
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

void ToggleGameFullscreen()
{
#ifdef PLATFORM_WEB
    TraceLog(LOG_WARNING, "Fullscreen is not supported on web");
    return;
#endif // PLATFORM_WEB

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

    player->UpdateCamera();
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
#ifndef WINDOWS_HOT_RELOAD
    ShowCursor(); // this doesn't work when linking raylib as a shared library to the core
#endif            // !WINDOWS_HOT_RELOAD
    player->PauseSounds();
    for (size_t i = 0; i < gameObjects.size(); i++)
    {
        gameObjects[i]->PauseSounds();
    }
}

void ResumeGame()
{
    ChangeScreen(GAME);
#ifndef WINDOWS_HOT_RELOAD
    HideCursor();
#endif // !WINDOWS_HOT_RELOAD
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
        // draw background centered
        DrawTexture(*spaceBackground, (int)(GetScreenWidth() - spaceBackground->width) / 2, (int)(GetScreenHeight() - spaceBackground->height) / 2, WHITE);

        BeginMode2D(player->GetCamera());

        for (size_t i = 0; i < gameObjects.size(); i++)
        {
            gameObjects[i]->Draw();
        }
        player->Draw();

        EndMode2D();
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
        BeginMode2D(player->GetCamera());

        player->DrawDebug();
        for (size_t i = 0; i < gameObjects.size(); i++)
        {
            gameObjects[i]->DrawDebug();
        }

        // Rectangle worldBox = {-(float)GetScreenWidth() / 2, -(float)GetScreenHeight() / 2, (float)GetScreenWidth(), (float)GetScreenHeight()};
        // DrawRectangleLinesEx(worldBox, 4, ORANGE);
        DrawCircleV(GetScreenToWorld2D(GetMousePosition(), player->GetCamera()), 5, PURPLE);

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
    DrawText(TextFormat("Enemy prob of shooting to player:  %.2f", gameState.diffSettings.enemiesAttributes.probOfShootingToPlayer), 10, GetScreenHeight() - 260, 20, WHITE);
    DrawText(TextFormat("Powerup spawn chance: %.2f", gameState.diffSettings.powerupSpawnChance), 10, GetScreenHeight() - 280, 20, WHITE);
    DrawText(TextFormat("Asteroids count: %d", gameState.asteroidsCount), 10, GetScreenHeight() - 300, 20, WHITE);
    DrawText(TextFormat("Enemies count: %d", gameState.enemiesCount), 10, GetScreenHeight() - 320, 20, WHITE);

    DrawText(TextFormat("Powerup to spawn: %s", PowerUp::GetPowerUpName(powerupToSpawn)), 400, GetScreenHeight() - 40, 20, WHITE);
}

void HandleInput()
{
    if (IsKeyPressed(KEY_R))
    {
        CreateNewGame();
    }
    if (IsKeyPressed(KEY_L))
    {
        UpdateDifficultySettings((Difficulty)((gameState.diffSettings.difficulty + 1) % NUM_DIFFICULTIES));
    }

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
        gameObjects.push_back(new Asteroid((AsteroidVariant)GetRandomValue(0, 1), gameState.diffSettings.asteroidSpeedMultiplier));
        gameState.asteroidsCount++;
    }
    // spawn an enemy
    if (IsKeyPressed(KEY_C))
    {
        gameObjects.push_back(new BasicEnemy(RandomVecOutsideScreen(100), player, gameState.diffSettings.enemiesAttributes));
        gameState.enemiesCount++;
    }
    // spawn the selected powerup in the mouse position
    if (IsKeyPressed(KEY_V))
    {
        if (gameState.powerupSpawned)
        {
            auto powerup = std::find_if(gameObjects.begin(), gameObjects.end(), [](GameObject *obj)
                                        { return obj->GetType() == POWER_UP; });
            if (powerup != gameObjects.end())
            {
                delete *powerup;
                gameObjects.erase(powerup);
            }
        }
        gameObjects.push_back(new PowerUp(GetScreenToWorld2D(GetMousePosition(), player->GetCamera()), powerupToSpawn));
        gameState.powerupSpawned = true;
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

    // moving objects with mouse
    static GameObject *movingObject = nullptr;

    Vector2 mouseWorldPos = GetScreenToWorld2D(GetMousePosition(), player->GetCamera());

    if (player != nullptr && CheckCollisionPointRec(mouseWorldPos, player->GetBounds()))
    {
        if (movingObject == nullptr && IsMouseButtonDown(MOUSE_LEFT_BUTTON))
        {
            movingObject = player;
        }
    }

    for (size_t i = 0; i < gameObjects.size(); i++)
    {
        if (CheckCollisionPointRec(mouseWorldPos, gameObjects[i]->GetBounds()))
        {
            if (movingObject == nullptr && (IsMouseButtonDown(MOUSE_LEFT_BUTTON) || IsMouseButtonDown(MOUSE_RIGHT_BUTTON)))
            {
                movingObject = gameObjects[i];
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

    player->Update();
    for (size_t i = 0; i < gameObjects.size(); i++)
    {
        gameObjects[i]->Update();

        if (gameObjects[i]->GetType() == ASTEROID)
        {
            Asteroid *asteroid = (Asteroid *)gameObjects[i];
            if (asteroid->IsDestroyed())
            {
                AddScore(asteroid->GetVariant() == LARGE ? LARGE_ASTEROID_DESTROYED : SMALL_ASTEROID_DESTROYED, scoreMultiplier);
                delete asteroid;
                gameObjects.erase(gameObjects.begin() + i);
                gameState.asteroidsCount--;
            }
        }
        else if (gameObjects[i]->GetType() == BASIC_ENEMY)
        {
            BasicEnemy *enemy = (BasicEnemy *)gameObjects[i];
            if (enemy->IsDead() && enemy->GetBullets()->size() == 0)
            {
                AddScore(BASIC_ENEMY_KILLED, scoreMultiplier);
                delete enemy;
                gameObjects.erase(gameObjects.begin() + i);
                gameState.enemiesCount--;
            }
        }
        else if (gameObjects[i]->GetType() == POWER_UP)
        {
            PowerUp *powerup = (PowerUp *)gameObjects[i];
            if (powerup->IsExpired())
            {
                delete powerup;
                gameObjects.erase(gameObjects.begin() + i);
                gameState.powerupSpawned = false;
            }
            else if (powerup->IsPickedUp())
            {
                AddScore((ScoreType)powerup->GetType(), scoreMultiplier);
                gameObjects.erase(gameObjects.begin() + i);
                gameState.powerupSpawned = false;
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

        float asteroidsSpawnChance = gameState.diffSettings.asteroidsSpawnChance;
        float enemiesSpawnChance = gameState.diffSettings.enemiesSpawnChance;
        float powerUpSpawnChance = gameState.diffSettings.powerupSpawnChance;

        if (player->IsAlive() && player->HasMoved())
        {
            if (gameState.asteroidsCount < gameState.diffSettings.maxAsteroids)
            {
                // spawn an asteroid
                if (oneSecondTimer > 1.0f && GetRandomValue(0, 100) < asteroidsSpawnChance * 100)
                {
                    gameObjects.push_back(new Asteroid((AsteroidVariant)GetRandomValue(0, 1), gameState.diffSettings.asteroidSpeedMultiplier));
                    gameState.asteroidsCount++;
                }
            }

            if (gameState.enemiesCount < gameState.diffSettings.maxEnemies)
            {
                // spawn an enemy
                if (oneSecondTimer > 1.0f && GetRandomValue(0, 100) < enemiesSpawnChance * 100)
                {
                    gameObjects.push_back(new BasicEnemy(RandomVecOutsideScreen(100), player, gameState.diffSettings.enemiesAttributes));
                    gameState.enemiesCount++;
                }
            }

            if (!gameState.powerupSpawned)
            {
                // spawn a power up
                if (oneSecondTimer > 1.0f && GetRandomValue(0, 100) < powerUpSpawnChance * 100)
                {
                    gameObjects.push_back(new PowerUp((Vector2){(float)GetRandomValue(0, GetScreenWidth()), (float)GetRandomValue(0, GetScreenHeight())},
                                                      (PowerUpType)GetRandomValue(0, NUM_POWER_UP_TYPES - 1)));
                    gameState.powerupSpawned = true;
                }
            }

            AddScore(TIME_ALIVE, 1.0f);
        }
        if (player->IsDead() && player->GetLives() <= 0)
        {
            gameState.previousScreen = gameState.currentScreen;
            gameState.currentScreen = GAME_OVER;
        }

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

    if (spaceBackground != nullptr)
    {
        UnloadTexture(*spaceBackground);
    }
    ResourceManager::UnloadResources();
    CloseAudioDevice();
}
