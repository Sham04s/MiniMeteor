#include "ui_manager.hpp"
#include "button.hpp"
#include "resource_manager.hpp"
#include <stdio.h>

ScreenID UIManager::currentScreen;
UIObject *UIManager::screens[NUM_SCREENS];

extern bool SHOW_DEBUG;

bool UIManager::LoadUI()
{
    currentScreen = MAIN_MENU;

    UIObject *mainMenu = new UIObject(Rectangle{(float)GetScreenWidth() / 2 - BUTTON_WIDTH / 2, (float)GetScreenHeight() / 2 - BUTTON_HEIGHT * 3 / 2 + BUTTON_PADDING,
                                                BUTTON_WIDTH, BUTTON_HEIGHT * 3 + BUTTON_PADDING * 2},
                                      nullptr, ResourceManager::GetDefaultTexture());

    mainMenu->AddChild(new Button(Vector2{0, 0}, mainMenu, "Play", []()
                                  { UIManager::SetCurrentScreen(GAME); }));
    mainMenu->AddChild(new Button(Vector2{0, 100}, mainMenu, "Options", []()
                                  { UIManager::SetCurrentScreen(OPTIONS); }));
    mainMenu->AddChild(new Button(Vector2{0, 200}, mainMenu, "Quit", []()
                                  { UIManager::SetCurrentScreen(EXITING); }));

    screens[MAIN_MENU] = mainMenu;

    return true;
}

void UIManager::UnloadUI()
{
    // for (size_t i = 0; i < NUM_SCREENS; i++)
    // {
    //     delete screens[i];
    // }
    delete screens[MAIN_MENU];
}

void UIManager::DrawUI()
{
    if (currentScreen != GAME)
    {
        ClearBackground(DARKGRAY);
    }
    if (screens[currentScreen] == nullptr)
    {
        DrawText("Screen not loaded!", 10, 10, 20, RED);
        return;
    }
    screens[currentScreen]->Draw();
    if (SHOW_DEBUG)
    {
        screens[currentScreen]->DrawDebug();
    }
}

void UIManager::UpdateUI()
{
    if (screens[currentScreen] == nullptr)
    {
        return;
    }
    screens[currentScreen]->Update();
}

void UIManager::HandleUIInput()
{
    if (IsKeyPressed(KEY_ESCAPE))
    {
        if (currentScreen == GAME)
        {
            currentScreen = MAIN_MENU; // TODO: pause menu
        }
        else if (currentScreen == MAIN_MENU)
        {
            currentScreen = GAME;
        }
    }
}
