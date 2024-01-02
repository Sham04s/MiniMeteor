#include "game_ui.hpp"
#include "game.hpp"
#include "utils.hpp"
#include "resource_manager.hpp"
#include "player_ui.hpp"
#include "score_summary.hpp"
#include <stdio.h>
#include "label.hpp"

char fpsButtonText[20];

UIObject *CreateMainMenu()
{
    // main menu title
    Label *title = new Label({0, (float)GetScreenHeight() / 8, (float)GetScreenWidth(), (float)GetScreenHeight() / 4}, "MiniMeteor", WHITE, ALIGN_CENTER, ALIGN_CENTER, 64, nullptr);

    // main menu buttons
    const int mainMenuButtonCount = 3;
    Button *playButton = new Button(Vector2{0, 0}, nullptr, "Play", BUTTON_PRIMARY, BUTTON_MEDIUM, []()
                                    { CreateNewGame(); });

    Button *optionsButton = new Button(Vector2{0, 0}, nullptr, "Options", BUTTON_PRIMARY, BUTTON_MEDIUM, []()
                                       { ChangeScreen(OPTIONS); });

    Button *quitButton = new Button(Vector2{0, 0}, nullptr, "Quit", BUTTON_PRIMARY, BUTTON_MEDIUM, []()
                                    { ChangeScreen(EXITING); });

    Button *mainMenuButtons[mainMenuButtonCount] = {playButton, optionsButton, quitButton};

    // main menu
    Rectangle mainMenuButtonRec = CreateCenteredButtonRec(mainMenuButtons, mainMenuButtonCount);
    UIObject *mainMenuContainer = new UIObject(mainMenuButtonRec, nullptr, ResourceManager::GetDefaultTexture());

    Button *b;
    for (int i = 0; i < mainMenuButtonCount; i++)
    {
        b = mainMenuButtons[i];
        b->SetRelPos({0, (float)i * (b->GetHeight() + b->GetPadding() * 2)});
        mainMenuContainer->AddChild(b);
    }

    UIObject *mainMenu = new UIObject({0, 0, (float)GetScreenWidth(), (float)GetScreenHeight()},
                                      nullptr, ResourceManager::GetDefaultTexture());

    mainMenu->AddChild(title);
    mainMenu->AddChild(mainMenuContainer);

    return mainMenu;
}

UIObject *CreateGameUI(Player *player)
{
    // in-game ui
    UIObject *game = new PlayerUI(player);
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
                   { ChangeScreen(MAIN_MENU); }),
        new Button(Vector2{0, 0}, nullptr, "Options", BUTTON_PRIMARY, BUTTON_MEDIUM, []()
                   { ChangeScreen(OPTIONS); }),
        new Button(Vector2{0, 0}, nullptr, "Quit", BUTTON_PRIMARY, BUTTON_MEDIUM, []()
                   { ChangeScreen(EXITING); }),
    };

    // pause menu
    UIObject *pauseMenu = new UIObject(CreateCenteredButtonRec(pauseButtons, pauseButtonCount), nullptr, ResourceManager::GetDefaultTexture());

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
                   { ChangeScreen(MAIN_MENU); }),
    };

    Rectangle gameOverButtonRec = CreateCenteredButtonRec(gameOverButtons, gameOverButtonCount);
    gameOverButtonRec.y += gameOverButtonRec.height * 2;
    UIObject *buttons = new UIObject(gameOverButtonRec, nullptr, ResourceManager::GetDefaultTexture());

    // game over score summary
    Rectangle scoreSummaryRec = {gameOverButtonRec.x - gameOverButtonRec.height * 1.5f, gameOverButtonRec.y - gameOverButtonRec.height * 6, gameOverButtonRec.width * 2, gameOverButtonRec.height * 6};
    UIObject *scoreSummary = new ScoreSummary(scoreSummaryRec, nullptr);
    scoreSummary->SetRelBounds(scoreSummaryRec);

    // game over
    UIObject *gameOver = new UIObject(Rectangle{0, 0, 0, 0}, nullptr, ResourceManager::GetDefaultTexture());
    gameOver->AddChild(scoreSummary);

    Button *b;
    for (int i = 0; i < gameOverButtonCount; i++)
    {
        b = gameOverButtons[i];
        b->SetRelPos({0, (float)i * (b->GetHeight() + b->GetPadding() * 2)});
        buttons->AddChild(b);
    }
    gameOver->AddChild(buttons);

    return gameOver;
}

void ChangeFPSButtonFunc(Button *button)
{
    ChangeFPS();
    if (gameState.fps == 0)
    {
#ifdef PLATFORM_DESKTOP
        sprintf(fpsButtonText, "FPS: \u221E"); // Unicode value for infinity symbol
#else
        sprintf(fpsButtonText, "FPS: VSync");
#endif // PLATFORM_DESKTOP
    }
    else
    {
        sprintf(fpsButtonText, "FPS: %d", gameState.fps);
    }
    button->SetText(fpsButtonText);
}

UIObject *CreateOptionsMenu()
{
    if (gameState.fps == 0)
    {
        sprintf(fpsButtonText, "FPS: \u221E");
    }
    else
    {
        sprintf(fpsButtonText, "FPS: %d", gameState.fps);
    }

    const int optionButtonCount = 3;
    Button *backButton = new Button(Vector2{0, 0}, nullptr, "Back", BUTTON_PRIMARY, BUTTON_MEDIUM, nullptr);
#ifdef PLATFORM_DESKTOP
    Button *fullscreenButton = new Button(Vector2{0, 0}, nullptr, "Fullscreen", BUTTON_PRIMARY, BUTTON_MEDIUM, nullptr);
#endif // PLATFORM_DESKTOP
    Button *changeFPSButton = new Button(Vector2{0, 0}, nullptr, fpsButtonText, BUTTON_PRIMARY, BUTTON_MEDIUM, nullptr);

    backButton->OnClick([]()
                        { PreviousScreen(); });
    fullscreenButton->OnClick([]()
                              { ToggleGameFullscreen(); });
    changeFPSButton->OnClick([changeFPSButton]()
                             { ChangeFPSButtonFunc(changeFPSButton); });

#ifdef PLATFORM_DESKTOP
    Button *buttons[optionButtonCount] = {fullscreenButton, changeFPSButton, backButton};
#else
    Button *buttons[optionButtonCount] = {changeFPSButton, backButton};
#endif // PLATFORM_DESKTOP

    Rectangle optionButtonRec = CreateCenteredButtonRec(buttons, optionButtonCount);

    UIObject *options = new UIObject(optionButtonRec, nullptr, ResourceManager::GetDefaultTexture());

    Button *b;

    for (int i = 0; i < optionButtonCount; i++)
    {
        b = buttons[i];
        b->SetRelPos({0, (float)i * (b->GetHeight() + b->GetPadding() * 2)});
        options->AddChild(b);
    }

    return options;
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