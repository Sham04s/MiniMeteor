#ifndef __PLAYER_UI_H__
#define __PLAYER_UI_H__

#include "ui_object.hpp"
#include "player.hpp"
#include "lives_bar.hpp"
#include "player_powerups.hpp"
#include "label.hpp"

#define SCORE_FONT_SIZE 30
#define DIRECTIONAL_SHIP_ICON_SIZE 24

class PlayerUI : public UIObject
{
private:
    Player *player;
    LivesBar *livesBar;
    PlayerPowerups *powerups;
    Label *scoreLabel;
    char scoreText[32];
    Rectangle directionalShipMeterBounds;
    Texture2D *directionalShipIcon;

public:
    PlayerUI(Player *player);
    ~PlayerUI();

    void Update();
    void Draw();
    void DrawDebug();
    void Resize(Vector2 prevScreenSize);
};

#endif // __PLAYER_UI_H__