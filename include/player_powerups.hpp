#ifndef __PLAYER_POWERUPS_H__
#define __PLAYER_POWERUPS_H__

#include "ui_object.hpp"
#include "player.hpp"

#define ICONS_SIZE 30.0f
#define ICONS_SPACING 5.0f
#define FADE_DISTANCE_THRESHOLD 200.0f
#define MULTIPLIER_FONT_SIZE 16

enum CenteredPosition
{
    TOP,
    BOTTOM,
    LEFT,
    RIGHT,
    NUM_POSITIONS
};

class PlayerPowerups : public UIObject
{
    private: 
        Player *player;
        CenteredPosition position;
        Texture2D *powerupTextures[NUM_POWER_UP_TYPES];        
    
    public:
        PlayerPowerups(Player *player, CenteredPosition position = LEFT);
        ~PlayerPowerups();

        void Update();
        void Draw();
        void DrawDebug();
};
        
#endif // __PLAYER_POWERUPS_H__