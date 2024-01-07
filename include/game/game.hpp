// TODO:
/***********************************************************************************************
 * 
 *      MiniMeteor - A simple game developed by [Your Name]
 *      Copyright (c) [Year]
 *      All rights reserved.
 * 
 **********************************************************************************************/

#ifndef __GAME_H__
#define __GAME_H__

#include "ui/components/common/ui_object.hpp"
#include "game/objects/shooter.hpp"

#ifdef WINDOWS_HOT_RELOAD
#define CORE_API extern "C" __declspec(dllexport)
#else
#define CORE_API
#endif // WINDOWS_HOT_RELOAD

// ------------------------------------------------------------------------------------------ //
// -------------------------------- MAIN GAME CORE FUNCTIONS -------------------------------- //
// ------------------------------------------------------------------------------------------ //

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

// ------------------------------------------------------------------------------------------ //
// ------------------------------------------------------------------------------------------ //

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

/**
 * @brief The game state 
 * This stores global game information such as the current screen, the player, the game objects, etc
 */
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
    bool hasEnteredGame;
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

/**
 * @brief Pauses the game
 */
void PauseGame();

/**
 * @brief Resumes the game
 */
void ResumeGame();

/**
 * @brief Creates a new game
 */
void RestartGame();

/**
 * @brief Tries to spawn an object of the given type according to the spawn chances
 * and only if the maximum number of objects of that type hasn't been reached
 *
 * @param type The type of the object to spawn
 */
void TryToSpawnObject(GameObjectType type);

/**
 * @brief Updates the difficulty settings (number of max asteroids, enemies, spawn chances, enemies attributes, etc)
 * according to the given difficulty
 *
 * @param diff The difficulty to update the settings to
 */
void UpdateDifficultySettings(Difficulty diff);


// --------------------------------------------------------------------------------------------- //
// ------------------------------------ MAIN GAME FUNCTIONS ------------------------------------ //
// --------------------------------------------------------------------------------------------- //

/**
 * @brief Checks collisions between game objects and handles them
 */
void HandleCollisions();

/**
 * @brief Updates the game objects removing the ones that are marked for deletion
 */
void UpdateGameObjects();

/**
 * @brief The main game update function
 */
void UpdateGame();

/**
 * @brief The main game draw function
 */
void DrawFrame();

/**
 * @brief Draws debug information about the game and its objects
 */
void DrawDebug();

// --------------------------------------------------------------------------------------------- //
// --------------------------------------------------------------------------------------------- //

/**
 * @brief Changes the current screen to the given one
 *
 * @param screen The screen to change to
 */
void ChangeScreen(ScreenID screen);

/**
 * @brief Changes the current screen to the previous one
 */
void PreviousScreen();

/**
 * @brief Toggles the game fullscreen
 */
void ToggleGameFullscreen();

/**
 * @brief Updates the UI when the window is resized and updates the camera
 * 
 * @param prevWindowSize The previous window size
 */
void ResizeCallback(Vector2 prevWindowSize);

/**
 * @brief Changes the game FPS
 */
void ChangeFPS(); // rotates between 15, 30, 60, 120 and unlimited in PLATFORM_DESKTOP

#endif // __GAME_H__