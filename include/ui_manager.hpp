#ifndef __UI_MANAGER_H__
#define __UI_MANAGER_H__

#include "raylib.h"
#include "ui_object.hpp"

enum ScreenID
{
    GAME,
    LOADING,
    MAIN_MENU,
    PAUSE_MENU,
    OPTIONS,
    EXITING,
    NUM_SCREENS
};

class UIManager
{
private:
    static ScreenID currentScreen;
    static Texture2D buttonTexture;
    static UIObject *screens[NUM_SCREENS];
    static Font font; // TODO: maybe not?

public:
    static bool LoadUI();
    static void UnloadUI();

    static void DrawUI();
    static void UpdateUI();
    static void HandleUIInput();

    static ScreenID GetCurrentScreen() { return currentScreen; };
    static void SetCurrentScreen(ScreenID screen) { currentScreen = screen; };
};
#endif // __UI_MANAGER_H__