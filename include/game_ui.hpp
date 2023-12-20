#ifndef __UI_H__
#define __UI_H__

#include "ui_object.hpp"
#include "button.hpp"
#include "lives_bar.hpp"

UIObject *CreateMainMenu();
UIObject *CreateGameUI(Player *player);
UIObject *CreatePauseMenu();
UIObject *CreateGameOverMenu();
UIObject *CreateOptionsMenu();
UIObject *CreateExitUI();
void CreateUIElements(Player *player);

#endif // __UI_H__