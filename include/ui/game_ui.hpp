#ifndef __UI_H__
#define __UI_H__

#include "ui/components/common/ui_object.hpp"
#include "ui/components/common/button.hpp"
#include "ui/components/hud/lives_bar.hpp"

// ------------------ Menus/Screens ------------------
UIObject *CreateMainMenu();
UIObject *CreatePlayerHUD(Player *player);
UIObject *CreatePauseMenu();
UIObject *CreateGameOverMenu();
UIObject *CreateControlsMenu();
UIObject *CreateCreditsMenu();
UIObject *CreateOptionsMenu();
UIObject *CreateExitUI();
// ----------------------------------------------------

/**
 * @brief Creates the UI elements for the game (main menu, pause menu, etc.)
 * 
 * @param player The player object required for the player HUD
 */
void CreateUIElements(Player *player);

#endif // __UI_H__