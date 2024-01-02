#ifndef __PLAYER_UI_H__
#define __PLAYER_UI_H__

#include "ui_object.hpp"
#include "player.hpp"
#include "lives_bar.hpp"
#include "player_powerups.hpp"

#define SCORE_FONT_SIZE 30

class PlayerUI : public UIObject
{
private:
    Player *player;
    LivesBar *livesBar;
    PlayerPowerups *powerups;
    Rectangle scoreRect;

public:
    PlayerUI(Player *player);
    ~PlayerUI();

    void Draw();
    void DrawDebug();
};

#endif // __PLAYER_UI_H__