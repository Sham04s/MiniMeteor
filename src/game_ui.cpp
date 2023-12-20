#include "game_ui.hpp"
#include "game.hpp"
#include "utils.hpp"
#include "resource_manager.hpp"

UIObject *CreateMainMenu()
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

    Button *b;
    for (int i = 0; i < mainMenuButtonCount; i++)
    {
        b = mainMenuButtons[i];
        b->SetRelPos({0, (float)i * (b->GetHeight() + b->GetPadding() * 2)});
        mainMenu->AddChild(b);
    }
    
    return mainMenu;
}

UIObject *CreateGameUI(Player *player)
{
    // game ui
    UIObject *game = new UIObject(Rectangle{0, 0, (float)GetScreenWidth(), (float)GetScreenHeight()}, nullptr, ResourceManager::GetDefaultTexture());

    game->AddChild(new LivesBar(Rectangle{(float)GetScreenWidth() / 2 + 100, 10, (float)GetScreenWidth() / 2 - 100, 50}, player));
    return game;
}

UIObject *CreatePauseMenu()
{
    // pause menu buttons
    const int pauseButtonCount = 5;
    Button *pauseButtons[pauseButtonCount] = {
        new Button(Vector2{0, 0}, nullptr, "Resume", BUTTON_PRIMARY, BUTTON_MEDIUM, []()
                   { ResumeGame(); }),
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

    Button *b;
    for (int i = 0; i < pauseButtonCount; i++)
    {
        b = pauseButtons[i];
        b->SetRelPos({0, (float)i * (b->GetHeight() + b->GetPadding() * 2)});
        pauseMenu->AddChild(b);
    }

    return pauseMenu;
}

UIObject *CreateGameOverMenu()
{
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
    for (int i = 0; i < gameOverButtonCount; i++)
    {
        b = gameOverButtons[i];
        b->SetRelPos({0, (float)i * (b->GetHeight() + b->GetPadding() * 2)});
        gameOver->AddChild(b);
    }
    
    return gameOver;
}

UIObject *CreateOptionsMenu()
{
    return nullptr;
}

UIObject *CreateExitUI()
{
    return nullptr;
}

void CreateUIElements(Player *player)
{

    gameState.screens[GAME] = CreateGameUI(player);
    gameState.screens[GAME_OVER] = CreateGameOverMenu();
    gameState.screens[MAIN_MENU] = CreateMainMenu();
    gameState.screens[PAUSE_MENU] = CreatePauseMenu();
    gameState.screens[OPTIONS] = CreateOptionsMenu();
    gameState.screens[EXITING] = CreateExitUI();
}