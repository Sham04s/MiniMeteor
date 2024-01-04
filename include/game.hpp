#ifndef __GAME_H__
#define __GAME_H__

#include "ui_object.hpp"
#include "enemy.hpp"

#ifdef WINDOWS_HOT_RELOAD
#define CORE_API extern "C" __declspec(dllexport)
#else
#define CORE_API
#endif // WINDOWS_HOT_RELOAD

CORE_API void InitGame();
CORE_API bool GameLoop();
CORE_API void ExitGame();

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

enum Difficulty
{
    EASY,
    MEDIUM,
    HARD,
    NUM_DIFFICULTIES
};

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

void CreateNewGame(size_t numAsteroids, size_t numEnemies, Difficulty difficulty);
void PauseGame();
void ResumeGame();
void RestartGame();
void DrawFrame();
void DrawDebug();
void ChangeScreen(ScreenID screen);
void PreviousScreen();
void ToggleGameFullscreen();
void ResizeCallback(Vector2 prevWindowSize);
void ChangeFPS();
void TryToSpawnObject(GameObjectType type);
void UpdateGame();

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
#define BACKGROUND_COLOR GetColor(0x242424ff)

#endif // __GAME_H__