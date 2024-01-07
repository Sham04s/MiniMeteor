#include <stdio.h>
#include <math.h>
#include <algorithm>

#include "raylib.h"

#include "game/game.hpp"
#include "ui/game_ui.hpp"
#include "ui/components/menus/raylib_logo.hpp"

#include "utils/resource_manager.hpp"
#include "utils/score_registry.hpp"
#include "game/objects/player.hpp"
#include "game/objects/asteroid.hpp"
#include "game/objects/shooter.hpp"
#include "game/objects/power_up.hpp"
#include "utils/utils.hpp"

// -------------- Debug Flags --------------
#ifdef _DEBUG
bool SHOW_DEBUG = true;
#else
bool SHOW_DEBUG = false;
#endif // _DEBUG
// -----------------------------------------

#define BACKGROUND_COLOR GetColor(0x242424ff)

#define INITIAL_ASTEROIDS 5
#define INITIAL_ENEMIES 1

GameState gameState;

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
    gameState.hasEnteredGame = false;

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
    enemiesAttr->probOfShooting = 0.35f + (float)(gameState.diffSettings.difficulty) * 0.25f; // 0.35, 0.6, 0.85
}

void CreateNewGame(size_t numAsteroids, size_t numEnemies)
{

    gameState.asteroidsCount = 0;
    gameState.enemiesCount = 0;
    gameState.powerupSpawned = false;

    UpdateDifficultySettings(EASY); // difficulty starts at easy and is increased as the player scores more points

    ResetScoreRegistry();

    // unload previous background and create a new one
    if (gameState.spaceBackground != nullptr)
    {
        UnloadTexture(*gameState.spaceBackground);
    }
    gameState.spaceBackground = GenerateStarsBackground(4096, 4096, GetRandomValue(1000, 2000), 1, 2);

    // reset player
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
        if (i < numAsteroids)
        {
            gameState.gameObjects.push_back(new Asteroid(gameState.diffSettings.asteroidSpeedMultiplier));
            gameState.asteroidsCount++;
        }
        else
        {
            gameState.gameObjects.push_back(new Shooter(gameState.player, gameState.diffSettings.enemiesAttributes));
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

    // update the UIObject if not nullptr
    if (gameState.screens[gameState.currentScreen] != nullptr)
    {
        gameState.screens[gameState.currentScreen]->Update();
    }

    // create a new game if the player has started a new game and is returning to the main menu
    if (screen == MAIN_MENU && gameState.hasEnteredGame)
    {
        gameState.hasEnteredGame = false;
        gameState.previousScreen = MAIN_MENU;
        gameState.player->Hide();
        CreateNewGame(INITIAL_ASTEROIDS, INITIAL_ENEMIES);
    }

    // show the player if the player has started a new game and is returning to the game
    if (screen == GAME && !gameState.hasEnteredGame)
    {
        gameState.hasEnteredGame = true;
        gameState.player->Show();
    }

    // sets the master volume to 0 if the screen is not the game or game over
    // and also hides/lock and shows/unlock the cursor
    if (screen == GAME)
    {
        SetMasterVolume(1);

#ifndef _DEBUG
        DisableCursor();
        HideCursor();
#endif // !_DEBUG
    }
    else if (gameState.previousScreen == GAME)
    {
        if (screen != GAME_OVER)
        {
            SetMasterVolume(0);
        }

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

// for debug purposes
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
        gameState.gameObjects.push_back(new Shooter(gameState.player, gameState.diffSettings.enemiesAttributes));
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

    if (gameState.currentScreen == GAME || gameState.currentScreen == GAME_OVER)
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
                if (gameState.player->GetLives() > 0)
                {
                    AddScore(asteroid->GetVariant() == LARGE ? LARGE_ASTEROID_DESTROYED : SMALL_ASTEROID_DESTROYED, scoreMultiplier);
                }
                delete asteroid;
                gameState.gameObjects.erase(gameState.gameObjects.begin() + i);
                gameState.asteroidsCount--;
            }
        }
        else if (gameState.gameObjects[i]->GetType() == ENEMY)
        {
            Shooter *enemy = (Shooter *)gameState.gameObjects[i];
            if (enemy->IsDead() && enemy->GetBullets()->size() == 0)
            {
                if (gameState.player->GetLives() > 0)
                {
                    AddScore(ENEMY_SHOOTER_KILLED, scoreMultiplier);
                }
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
            Shooter *enemy = (Shooter *)gameState.gameObjects[i];
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
            Shooter *enemy = (Shooter *)gameState.gameObjects[i];
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
        gameState.gameObjects.push_back(new Asteroid(gameState.diffSettings.asteroidSpeedMultiplier));
        gameState.asteroidsCount++;
        break;
    case ENEMY:
        gameState.gameObjects.push_back(new Shooter(gameState.player, gameState.diffSettings.enemiesAttributes));
        gameState.enemiesCount++;
        break;
    case POWER_UP:
        gameState.gameObjects.push_back(new PowerUp());
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

            // update difficulty settings if the player has scored enough points
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
            ChangeScreen(GAME_OVER);
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
        // reset game
        ExitGame();
        InitGame();
#else
        // exit game
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
