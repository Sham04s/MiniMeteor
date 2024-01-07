#ifndef __PLAYER_UI_H__
#define __PLAYER_UI_H__

#include "ui/components/common/ui_object.hpp"
#include "game/objects/player.hpp"
#include "ui/components/hud/lives_bar.hpp"
#include "ui/components/hud/player_powerups.hpp"
#include "ui/components/common/label.hpp"

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