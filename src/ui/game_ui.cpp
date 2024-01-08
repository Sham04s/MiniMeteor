#include "ui/game_ui.hpp"

#include <stdio.h>

#include "game/game.hpp"
#include "utils/utils.hpp"
#include "utils/resource_manager.hpp"

#include "ui/components/menus/raylib_logo.hpp"
#include "ui/components/hud/player_hud.hpp"
#include "ui/components/menus/game_over.hpp"
#include "ui/components/common/label.hpp"
#include "ui/components/menus/controls.hpp"
#include "ui/components/menus/menu.hpp"
#include "ui/components/menus/credits.hpp"

char fpsButtonText[20];

UIObject *CreateMainMenu()
{
    // main menu title
    Label *title = new Label({0, (float)GetScreenHeight() / 8, (float)GetScreenWidth(), (float)GetScreenHeight() / 4}, "MiniMeteor", WHITE, ALIGN_CENTER, ALIGN_CENTER, 64, nullptr);

    // main menu buttons
#ifdef PLATFORM_WEB
    const int mainMenuButtonCount = 4;
#else
    const int mainMenuButtonCount = 5;
#endif // PLATFORM_WEB
    Button *playButton = new Button(Vector2{0, 0}, nullptr, "Play", BUTTON_PRIMARY, BUTTON_MEDIUM, []()
                                    { ChangeScreen(GAME); });

    Button *controlsButton = new Button(Vector2{0, 0}, nullptr, "Controls", BUTTON_PRIMARY, BUTTON_MEDIUM, []()
                                        { ChangeScreen(CONTROLS); });

    Button *optionsButton = new Button(Vector2{0, 0}, nullptr, "Options", BUTTON_PRIMARY, BUTTON_MEDIUM, []()
                                       { ChangeScreen(OPTIONS); });

    Button *creditsButton = new Button(Vector2{0, 0}, nullptr, "Credits", BUTTON_PRIMARY, BUTTON_MEDIUM, []()
                                       { ChangeScreen(CREDITS); });

#ifdef PLATFORM_WEB
    Button *mainMenuButtons[mainMenuButtonCount] = {playButton, controlsButton, optionsButton, creditsButton};
#else

    Button *quitButton = new Button(Vector2{0, 0}, nullptr, "Quit", BUTTON_PRIMARY, BUTTON_MEDIUM, []()
                                    { ChangeScreen(EXITING); });

    Button *mainMenuButtons[mainMenuButtonCount] = {playButton, controlsButton, optionsButton, creditsButton, quitButton};

#endif // PLATFORM_WEB

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

    UIObject *mainMenu = new Menu("");

    mainMenu->AddChild(title);
    mainMenu->AddChild(mainMenuContainer);

    return mainMenu;
}

UIObject *CreatePlayerHUD(Player *player)
{
    // in-game ui
    UIObject *game = new PlayerHUD(player);
    return game;
}

UIObject *CreatePauseMenu()
{
    // pause menu buttons
    const int pauseButtonCount = 6;
    Button *pauseButtons[pauseButtonCount] = {
        new Button(Vector2{0, 0}, nullptr, "Resume", BUTTON_PRIMARY, BUTTON_MEDIUM, []()
                   { ResumeGame(); }),
        new Button(Vector2{0, 0}, nullptr, "Restart", BUTTON_PRIMARY, BUTTON_MEDIUM, []()
                   { RestartGame(); }),
        new Button(Vector2{0, 0}, nullptr, "Main Menu", BUTTON_PRIMARY, BUTTON_MEDIUM, []()
                   { ChangeScreen(MAIN_MENU); }),
        new Button(Vector2{0, 0}, nullptr, "Controls", BUTTON_PRIMARY, BUTTON_MEDIUM, []()
                   { ChangeScreen(CONTROLS); }),
        new Button(Vector2{0, 0}, nullptr, "Options", BUTTON_PRIMARY, BUTTON_MEDIUM, []()
                   { ChangeScreen(OPTIONS); }),
        new Button(Vector2{0, 0}, nullptr, "Quit", BUTTON_PRIMARY, BUTTON_MEDIUM, []()
                   { ChangeScreen(EXITING); }),
    };

    // pause menu
    UIObject *pauseMenuButtons = new UIObject(CreateCenteredButtonRec(pauseButtons, pauseButtonCount), nullptr, ResourceManager::GetDefaultTexture());

    Button *b;
    for (int i = 0; i < pauseButtonCount; i++)
    {
        b = pauseButtons[i];
        b->SetRelPos({0, (float)i * (b->GetHeight() + b->GetPadding() * 2)});
        pauseMenuButtons->AddChild(b);
    }

    UIObject *pauseMenu = new Menu("Paused");
    pauseMenu->AddChild(pauseMenuButtons);

    return pauseMenu;
}

UIObject *CreateGameOverMenu()
{
    // game over buttons
    const int gameOverButtonCount = 2;
    Button *gameOverButtons[gameOverButtonCount] = {
        new Button(Vector2{0, 0}, nullptr, "Restart", BUTTON_PRIMARY, BUTTON_MEDIUM, []()
                   { RestartGame(); }),
        new Button(Vector2{0, 0}, nullptr, "Main Menu", BUTTON_PRIMARY, BUTTON_MEDIUM, []()
                   { ChangeScreen(MAIN_MENU); }),
    };

    Rectangle gameOverButtonRec = CreateCenteredButtonRec(gameOverButtons, gameOverButtonCount);
    gameOverButtonRec.y += gameOverButtonRec.height * 2;
    UIObject *buttons = new UIObject(gameOverButtonRec, nullptr, ResourceManager::GetDefaultTexture());

    // game over score summary
    Rectangle scoreSummaryRec = {gameOverButtonRec.x + gameOverButtonRec.width / 2 - gameOverButtonRec.width * (1.5f / 2.0f),
                                 gameOverButtonRec.y - gameOverButtonRec.height * 4,
                                 gameOverButtonRec.width * 1.5f, gameOverButtonRec.height * 4};
    UIObject *scoreSummary = new GameOver(scoreSummaryRec, nullptr);
    scoreSummary->SetRelBounds(scoreSummaryRec);

    // game over
    UIObject *gameOver = new Menu("Game Over");
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

UIObject *CreateControlsMenu()
{
    Button *backButton = new Button(Vector2{0, 0}, nullptr, "Back", BUTTON_PRIMARY, BUTTON_MEDIUM, []()
                                    { PreviousScreen(); });

    Rectangle controlsRec = CreateCenteredButtonRec(&backButton, 1);
    controlsRec.y = GetScreenHeight() - backButton->GetHeight() - backButton->GetPadding() * 2;

    UIObject *controlsMenuButtons = new UIObject(controlsRec, nullptr, ResourceManager::GetDefaultTexture());
    controlsMenuButtons->AddChild(backButton);

    Controls *controlsMenu = new Controls();
    controlsMenu->AddChild(controlsMenuButtons);

    return controlsMenu;
}

UIObject *CreateCreditsMenu()
{
    Button *backButton = new Button(Vector2{0, 0}, nullptr, "Back", BUTTON_PRIMARY, BUTTON_MEDIUM, []()
                                    { PreviousScreen(); });

    Rectangle creditsRec = CreateCenteredButtonRec(&backButton, 1);
    creditsRec.y = GetScreenHeight() - backButton->GetHeight() - backButton->GetPadding() * 2;

    UIObject *creditsMenuButtons = new UIObject(creditsRec, nullptr, ResourceManager::GetDefaultTexture());
    creditsMenuButtons->AddChild(backButton);

    Credits *creditsMenu = new Credits();
    creditsMenu->AddChild(creditsMenuButtons);

    return creditsMenu;
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

    Button *changeFPSButton = new Button(Vector2{0, 0}, nullptr, fpsButtonText, BUTTON_PRIMARY, BUTTON_MEDIUM, nullptr);
    Button *backButton = new Button(Vector2{0, 0}, nullptr, "Back", BUTTON_PRIMARY, BUTTON_MEDIUM, nullptr);

    backButton->OnClick([]()
                        { PreviousScreen(); });
    changeFPSButton->OnClick([changeFPSButton]()
                             { ChangeFPSButtonFunc(changeFPSButton); });

#ifdef PLATFORM_DESKTOP
    const int optionButtonCount = 3;
    Button *fullscreenButton = new Button(Vector2{0, 0}, nullptr, "Fullscreen", BUTTON_PRIMARY, BUTTON_MEDIUM, nullptr);
    fullscreenButton->OnClick([]()
                              { ToggleGameFullscreen(); });
    Button *buttons[optionButtonCount] = {fullscreenButton, changeFPSButton, backButton};
#else
    const int optionButtonCount = 2;
    Button *buttons[optionButtonCount] = {changeFPSButton, backButton};
#endif // PLATFORM_DESKTOP

    Rectangle optionButtonRec = CreateCenteredButtonRec(buttons, optionButtonCount);

    UIObject *optionsMenuButtons = new UIObject(optionButtonRec, nullptr, ResourceManager::GetDefaultTexture());

    Button *b;

    for (int i = 0; i < optionButtonCount; i++)
    {
        b = buttons[i];
        b->SetRelPos({0, (float)i * (b->GetHeight() + b->GetPadding() * 2)});
        optionsMenuButtons->AddChild(b);
    }

    UIObject *optionsMenu = new Menu("Options");
    optionsMenu->AddChild(optionsMenuButtons);

    return optionsMenu;
}

UIObject *CreateExitUI()
{
    return nullptr;
}

void CreateUIElements(Player *player)
{
    gameState.screens[LOADING] = new RaylibLogo();
    gameState.screens[GAME] = CreatePlayerHUD(player);
    gameState.screens[GAME_OVER] = CreateGameOverMenu();
    gameState.screens[MAIN_MENU] = CreateMainMenu();
    gameState.screens[PAUSE_MENU] = CreatePauseMenu();
    gameState.screens[CONTROLS] = CreateControlsMenu();
    gameState.screens[CREDITS] = CreateCreditsMenu();
    gameState.screens[OPTIONS] = CreateOptionsMenu();
    gameState.screens[EXITING] = CreateExitUI();
}