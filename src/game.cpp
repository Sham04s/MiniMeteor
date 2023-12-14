#include <stdio.h>

#include "raylib.h"
#include "game.hpp"
#include <math.h>

#include "resource_manager.hpp"
#include "player.hpp"
#include "asteroid.hpp"
#include "enemy.hpp"
#include "button.hpp"
#include "lives_bar.hpp"
#include "utils.hpp"

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
    GAME,
    GAME_OVER,
    MAIN_MENU,
    PAUSE_MENU,
    OPTIONS,
    EXITING,
    NUM_SCREENS
};

struct GameState
{
    ScreenID currentScreen;
    ScreenID previousScreen;
    UIObject *screens[NUM_SCREENS];
} gameState;

Player player;
std::vector<Asteroid> asteroids;
std::vector<BasicEnemy> enemies;

void InitGame()
{
    SetExitKey(KEY_NULL);
    SetTargetFPS(60);

    if (!ResourceManager::LoadResources())
    {
        printf("Failed to load resources!\n");
    }

    gameState.currentScreen = MAIN_MENU;

    CreateUIElements();
}

void CreateNewGame()
{
    gameState.previousScreen = gameState.currentScreen;
    gameState.currentScreen = GAME;
    player = Player(Vector2{(float)GetScreenWidth() / 2, (float)GetScreenHeight() / 2});
    player.SetLives(1);

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
            insidePlayer = Vector2Distance(pos, player.GetOrigin()) < 100;
            insideAsteroid = false;
            for (size_t j = 0; j < asteroids.size(); j++)
            {
                if (Vector2Distance(pos, asteroids[j].GetOrigin()) < 100)
                {
                    insideAsteroid = true;
                    break;
                }
            }
        } while (insidePlayer || insideAsteroid);

        asteroids.push_back(Asteroid(pos));
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
            insidePlayer = Vector2Distance(pos, player.GetOrigin()) < 100;
            insideAsteroid = false;
            for (size_t j = 0; j < asteroids.size(); j++)
            {
                if (Vector2Distance(pos, asteroids[j].GetOrigin()) < 100)
                {
                    insideAsteroid = true;
                    break;
                }
            }
            insideEnemy = false;
            for (size_t j = 0; j < enemies.size(); j++)
            {
                if (Vector2Distance(pos, enemies[j].GetOrigin()) < 100)
                {
                    insideEnemy = true;
                    break;
                }
            }
        } while (insidePlayer || insideAsteroid || insideEnemy);

        enemies.push_back(BasicEnemy(pos));
    }
}

void CreateUIElements()
{
    // main menu buttons
    const int mainMenuButtonCount = 3;
    Button *mainMenuButtons[mainMenuButtonCount] = {
        new Button(Vector2{0, 0}, nullptr, "Play", BUTTON_PRIMARY, BUTTON_MEDIUM, []()
                   { CreateNewGame(); }),
        new Button(Vector2{0, 0}, nullptr, "Options", BUTTON_PRIMARY, BUTTON_MEDIUM, []()
                   { gameState.previousScreen = gameState.currentScreen; gameState.currentScreen = OPTIONS; }),
        new Button(Vector2{0, 0}, nullptr, "Quit", BUTTON_PRIMARY, BUTTON_MEDIUM, []()
                   { gameState.previousScreen = gameState.currentScreen; gameState.currentScreen = EXITING; }),
    };

    // main menu
    UIObject *mainMenu = new UIObject(createCenteredButtonRec(mainMenuButtons, mainMenuButtonCount),
                                      nullptr, ResourceManager::GetDefaultTexture());

    // game ui
    UIObject *game = new UIObject(Rectangle{0, 0, (float)GetScreenWidth(), (float)GetScreenHeight()}, nullptr, ResourceManager::GetDefaultTexture());

    game->AddChild(new LivesBar(Rectangle{(float)GetScreenWidth() / 2 + 100, 10, (float)GetScreenWidth() / 2 - 100, 50}, &player));

    // pause menu buttons
    const int pauseButtonCount = 5;
    Button *pauseButtons[pauseButtonCount] = {
        new Button(Vector2{0, 0}, nullptr, "Resume", BUTTON_PRIMARY, BUTTON_MEDIUM, []()
                   { gameState.previousScreen = gameState.currentScreen; gameState.currentScreen = GAME; }),
        new Button(Vector2{0, 0}, nullptr, "Restart", BUTTON_PRIMARY, BUTTON_MEDIUM, []()
                   { CreateNewGame(); }),
        new Button(Vector2{0, 0}, nullptr, "Main Menu", BUTTON_PRIMARY, BUTTON_MEDIUM, []()
                   { gameState.previousScreen = gameState.currentScreen; gameState.currentScreen = MAIN_MENU; }),
        new Button(Vector2{0, 0}, nullptr, "Options", BUTTON_PRIMARY, BUTTON_MEDIUM, []()
                   { gameState.previousScreen = gameState.currentScreen; gameState.currentScreen = OPTIONS; }),
        new Button(Vector2{0, 0}, nullptr, "Quit", BUTTON_PRIMARY, BUTTON_MEDIUM, []()
                   { gameState.previousScreen = gameState.currentScreen; gameState.currentScreen = EXITING; }),
    };

    // pause menu
    UIObject *pauseMenu = new UIObject(createCenteredButtonRec(pauseButtons, pauseButtonCount), nullptr, ResourceManager::GetDefaultTexture());

    // game over buttons
    const int gameOverButtonCount = 2;
    Button *gameOverButtons[gameOverButtonCount] = {
        new Button(Vector2{0, 0}, nullptr, "Restart", BUTTON_PRIMARY, BUTTON_MEDIUM, []()
                   { CreateNewGame(); }),
        new Button(Vector2{0, 0}, nullptr, "Main Menu", BUTTON_PRIMARY, BUTTON_MEDIUM, []()
                   { gameState.previousScreen = gameState.currentScreen; gameState.currentScreen = MAIN_MENU; }),
    };

    // game over
    UIObject *gameOver = new UIObject(createCenteredButtonRec(gameOverButtons, gameOverButtonCount), nullptr, ResourceManager::GetDefaultTexture());

    Button *b;
    for (int i = 0; i < mainMenuButtonCount; i++)
    {
        b = mainMenuButtons[i];
        b->SetRelPos({0, (float)i * (b->GetHeight() + b->GetPadding() * 2)});
        mainMenu->AddChild(b);
    }

    for (int i = 0; i < pauseButtonCount; i++)
    {
        b = pauseButtons[i];
        b->SetRelPos({0, (float)i * (b->GetHeight() + b->GetPadding() * 2)});
        pauseMenu->AddChild(b);
    }

    for (int i = 0; i < gameOverButtonCount; i++)
    {
        b = gameOverButtons[i];
        b->SetRelPos({0, (float)i * (b->GetHeight() + b->GetPadding() * 2)});
        gameOver->AddChild(b);
    }

    gameState.screens[GAME] = game;
    gameState.screens[GAME_OVER] = gameOver;
    gameState.screens[MAIN_MENU] = mainMenu;
    gameState.screens[PAUSE_MENU] = pauseMenu;
    gameState.screens[OPTIONS] = nullptr;
    gameState.screens[EXITING] = nullptr;
}

void DrawFrame()
{
    BeginDrawing();
    ClearBackground(BACKGROUND_COLOR);

    if (gameState.currentScreen == GAME || gameState.currentScreen == GAME_OVER)
    {

        player.Draw();
        for (size_t i = 0; i < asteroids.size(); i++)
        {
            asteroids[i].Draw();
        }
        for (size_t i = 0; i < enemies.size(); i++)
        {
            enemies[i].Draw();
        }
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
    if (gameState.currentScreen == GAME || gameState.currentScreen == GAME_OVER)
    {
        player.DrawDebug();

        for (size_t i = 0; i < asteroids.size(); i++)
        {
            asteroids[i].DrawDebug();
        }
        for (size_t i = 0; i < enemies.size(); i++)
        {
            enemies[i].DrawDebug();
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
            CreateNewGame();
        }
    }

    if (IsKeyPressed(KEY_ESCAPE))
    {
        if (gameState.currentScreen == GAME)
        {
            gameState.previousScreen = GAME;
            gameState.currentScreen = PAUSE_MENU;
        }
        else if (gameState.currentScreen == PAUSE_MENU)
        {
            gameState.previousScreen = PAUSE_MENU;
            gameState.currentScreen = GAME;
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
        asteroids.push_back(Asteroid((Vector2){(float)GetRandomValue(0, GetScreenWidth()), (float)GetRandomValue(0, GetScreenHeight())}));
    }
    if (IsKeyPressed(KEY_KP_ADD))
    {
        player.AddLife();
    }
    if (IsKeyPressed(KEY_KP_SUBTRACT))
    {
        player.Kill();
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
    for (size_t i = 0; i < enemies.size(); i++)
    {
        if (CheckCollisionPointRec(GetMousePosition(), enemies[i].GetBounds()))
        {
            if (IsMouseButtonDown(MOUSE_LEFT_BUTTON))
            {
                enemies[i].SetVelocity({0});
                enemies[i].SetAngularVelocity(0);
                enemies[i].Translate(GetMouseDelta());
            }
            if (IsMouseButtonDown(MOUSE_RIGHT_BUTTON))
            {
                Vector2 velocity = Vector2Subtract(GetMousePosition(), enemies[i].GetOrigin());
                enemies[i].SetVelocity(Vector2Scale(velocity, 3));
                enemies[i].SetAngularVelocity(GetRandomValue(-100, 100));
            }
        }
    }

#endif // _DEBUG
}

void UpdateGame()
{
    if (gameState.currentScreen == GAME || gameState.currentScreen == GAME_OVER)
    {
        // if player is dead and has 0 lives left, game over
        if (player.IsDead() && player.GetLives() <= 0)
        {
            gameState.previousScreen = gameState.currentScreen;
            gameState.currentScreen = GAME_OVER;
        }
        else
        {
            player.Update();
        }

        Vector2 pushVector = {0, 0};
        auto bullets = player.GetBullets();

        for (size_t i = 0; i < asteroids.size(); i++)
        {
            asteroids[i].Update();

            // check for collisions between player and asteroids
            if (player.CheckCollision(&asteroids[i], &pushVector))
            {
                player.Push(&asteroids[i], pushVector); // push both player and asteroid away from each other
                player.Kill();
            }

            // check for collisions between asteroids
            for (size_t j = 0; j < asteroids.size(); j++)
            {
                if (i < j) // check each pair only once
                {
                    if (asteroids[i].IsFloating() && asteroids[i].CheckCollision(&asteroids[j], &pushVector))
                    {
                        // push asteroids away from each other with the SAT push vector
                        asteroids[i].Push(&asteroids[j], pushVector);
                    }
                }
            }

            // check for collisions between bullets and asteroids
            for (size_t b = 0; b < bullets->size(); b++)
            {
                if ((*bullets)[b].CheckCollision(&asteroids[i], &pushVector))
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

        for (size_t i = 0; i < enemies.size(); i++)
        {
            enemies[i].Update();

            enemies[i].TryToShootAtPlayer(player);

            // check for collisions between player and enemies
            if (player.CheckCollision(&enemies[i], &pushVector))
            {
                player.Push(&enemies[i], pushVector); // push both player and enemy away from each other
                player.Kill();
                enemies[i].Kill();
            }

            // check for collisions between enemies
            for (size_t j = 0; j < enemies.size(); j++)
            {
                if (i < j) // check each pair only once
                {
                    if (enemies[i].IsMoving() && enemies[i].CheckCollision(&enemies[j], &pushVector))
                    {
                        // push enemies away from each other with the SAT push vector
                        enemies[i].Push(&enemies[j], pushVector);
                    }
                }
            }

            // check for collisions between player bullets and enemies
            for (size_t b = 0; b < bullets->size(); b++)
            {
                if ((*bullets)[b].CheckCollision(&enemies[i], &pushVector))
                {
                    // destroy bullet and enemy
                    bullets->erase(bullets->begin() + b);
                    enemies[i].Kill();
                }
            }
            bullets->shrink_to_fit();

            // check for collisions between enemy bullets and player
            auto enemyBullets = enemies[i].GetBullets();
            for (size_t b = 0; b < enemyBullets->size(); b++)
            {
                if ((*enemyBullets)[b].CheckCollision(&player, &pushVector))
                {
                    // destroy bullet and player
                    (*enemyBullets).erase((*enemyBullets).begin() + b);
                    player.Kill();
                }
            }
            (*enemyBullets).shrink_to_fit();

            // erase dead enemies
            if (enemies[i].IsDead())
            {
                enemies.erase(enemies.begin() + i);
            }
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
    for (size_t i = 0; i < NUM_SCREENS; i++)
    {
        if (gameState.screens[i] != nullptr)
        {
            delete gameState.screens[i];
        }
    }

    ResourceManager::UnloadResources();
}
