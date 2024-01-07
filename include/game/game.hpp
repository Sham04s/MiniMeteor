#ifndef __GAME_H__
#define __GAME_H__

#include "ui/components/common/ui_object.hpp"
#include "game/objects/enemy.hpp"

#ifdef WINDOWS_HOT_RELOAD
#define CORE_API extern "C" __declspec(dllexport)
#else
#define CORE_API
#endif // WINDOWS_HOT_RELOAD

/**
 * @brief Initializes the game
 *
 * @return true if the game was initialized successfully, false otherwise
 *
 */
CORE_API bool InitGame();

/**
 * @brief Main game loop
 *
 * @return true if the game loop should continue, false otherwise
 *
 */
CORE_API bool GameLoop();

/**
 * @brief Unload resources and exit the game
 *
 */
CORE_API void ExitGame();

/**
 * @brief The game current screen
 *
 */
enum ScreenID
{
    GAME,
    GAME_OVER,
    MAIN_MENU,
    PAUSE_MENU,
    OPTIONS,
    CONTROLS,
    CREDITS,
    LOADING,
    EXITING,
    NUM_SCREENS
};

enum Difficulty
{
    EASY,
    MEDIUM,
    HARD,
    NUM_DIFFICULTIES
};

/**
 * @brief The game difficulty settings (number of max asteroids, enemies, spawn chances, enemies attributes, etc)
 *
 */
typedef struct DifficultySettings
{
    Difficulty difficulty;
    int maxAsteroids;
    int maxEnemies;
    float powerupSpawnChance;
    float asteroidsSpawnChance;
    float enemiesSpawnChance;
    float asteroidSpeedMultiplier;
    float scoreMultiplier;
    EnemyAttributes enemiesAttributes;
} DifficultySettings;

typedef struct GameState
{
    ScreenID currentScreen;
    ScreenID previousScreen;
    Player *player;
    std::vector<GameObject *> gameObjects;
    Texture2D *spaceBackground = nullptr;
    UIObject *screens[NUM_SCREENS];
    int fps;
    bool fullscreen;
    bool hasStartedGame;
    Vector2 originalWindowSize;
    Vector2 windowSize;
    bool powerupSpawned;
    int asteroidsCount;
    int enemiesCount;
    DifficultySettings diffSettings;
} GameState;

extern GameState gameState;

// creates a new game with the given number of asteroids and enemies
void CreateNewGame(size_t numAsteroids, size_t numEnemies);

void PauseGame();
void ResumeGame();
void RestartGame();

void TryToSpawnObject(GameObjectType type);
void UpdateGame();
void DrawFrame();
void DrawDebug();

void ChangeScreen(ScreenID screen);
void PreviousScreen();

void ToggleGameFullscreen();
void ResizeCallback(Vector2 prevWindowSize);
void ChangeFPS(); // rotates between 15, 30, 60, 120 and unlimited in PLATFORM_DESKTOP

#endif // __GAME_H__