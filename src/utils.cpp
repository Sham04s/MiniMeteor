#include "utils.hpp"

Rectangle createCenteredButtonRec(Button **mainMenuButtons, int numButtons)
{
    float buttonWidth = mainMenuButtons[0]->GetWidth();
    float buttonHeight = mainMenuButtons[0]->GetHeight();
    float buttonSpacing = mainMenuButtons[0]->GetPadding();
    float totalWidth = buttonWidth;
    float totalHeight = (buttonHeight + buttonSpacing * 2) * (numButtons - 1);
    Rectangle centeredMenu = {(float)GetScreenWidth() / 2 - totalWidth / 2, (float)GetScreenHeight() / 2 - totalHeight / 2,
                              totalWidth, totalHeight};
    return centeredMenu;
}
