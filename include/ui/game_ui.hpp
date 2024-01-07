#ifndef __UI_H__
#define __UI_H__

#include "ui/components/common/ui_object.hpp"
#include "ui/components/common/button.hpp"
#include "ui/components/hud/lives_bar.hpp"

UIObject *CreateMainMenu();
UIObject *CreateGameUI(Player *player);
UIObject *CreatePauseMenu();
UIObject *CreateGameOverMenu();
UIObject *CreateControlsMenu();
UIObject *CreateOptionsMenu();
UIObject *CreateExitUI();
void CreateUIElements(Player *player);

#endif // __UI_H__