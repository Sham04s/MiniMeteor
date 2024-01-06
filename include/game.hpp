#ifndef __GAME_H__
#define __GAME_H__

#include "ui_object.hpp"
#include "enemy.hpp"

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

// blue variants
#define PRIMARY_COLOR GetColor(0x3c9dd7ff)
#define PRIMARY_COLOR_BRIGHT GetColor(0x8dc3e3ff)
#define PRIMARY_COLOR_DARK GetColor(0x058ad9ff)
#define PRIMARY_COLOR_DARKER GetColor(0x004d7aff)

// light blue variants
#define SECONDARY_COLOR GetColor(0x4d50deff)
#define SECONDARY_COLOR_BRIGHT GetColor(0x989ae7ff)
#define SECONDARY_COLOR_DARK GetColor(0x0c12dfff)
#define SECONDARY_COLOR_DARKER GetColor(0x000386ff)

// green variants
#define SECONDARY_COLOR_2 GetColor(0x34df7fff)
#define SECONDARY_COLOR_2_BRIGTH GetColor(0x8ce9b4ff)
#define SECONDARY_COLOR_2_DAR GetColor(0x00e162ff)
#define SECONDARY_COLOR_2_DARKER GetColor(0x00893bff)

// background color
#define BACKGROUND_COLOR GetColor(0x242424ff)

#endif // __GAME_H__