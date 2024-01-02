#ifndef __PLAYER_UI_H__
#define __PLAYER_UI_H__

#include "ui_object.hpp"
#include "player.hpp"
#include "lives_bar.hpp"
#include "player_powerups.hpp"
#include "label.hpp"

#define SCORE_FONT_SIZE 30

class PlayerUI : public UIObject
{
private:
    Player *player;
    LivesBar *livesBar;
    PlayerPowerups *powerups;
    Label *scoreLabel;
    char scoreText[32];

public:
    PlayerUI(Player *player);
    ~PlayerUI();

    void Update();
    void Draw();
    void DrawDebug();
};

#endif // __PLAYER_UI_H__