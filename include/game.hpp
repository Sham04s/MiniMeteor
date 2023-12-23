#ifndef __GAME_H__
#define __GAME_H__

#include "ui_object.hpp"

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

void CreateNewGame();
void PauseGame();
void ResumeGame();
void DrawFrame();
void DrawDebug();
void ChangeScreen(ScreenID screen);
void PreviousScreen();
void UpdateGame();

typedef struct GameState
{
    ScreenID currentScreen;
    ScreenID previousScreen;
    UIObject *screens[NUM_SCREENS];
} GameState;

extern GameState gameState;

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

#endif // __GAME_H__