#ifndef __PLAYER_POWERUPS_H__
#define __PLAYER_POWERUPS_H__

#include "ui/components/common/ui_object.hpp"
#include "game/objects/player.hpp"

#define ICONS_SIZE 30.0f
#define ICONS_SPACING 5.0f
#define FADE_DISTANCE_THRESHOLD 200.0f
#define MULTIPLIER_FONT_SIZE 16

enum SidePosition
{
    TOP,
    BOTTOM,
    LEFT,
    RIGHT,
    NUM_POSITIONS
};

/**
 * @brief A UIObject that displays the powerups the player currently has
 */
class PlayerPowerups : public UIObject
{
    private: 
        Player *player;

        /**
         * @brief The side of the screen the powerups are displayed on
         */
        SidePosition position;

        /**
         * @brief The textures for each powerup type
         */
        Texture2D *powerupTextures[NUM_POWER_UP_TYPES];
    
    public:
        /**
         * @brief Construct a new PlayerPowerups object with a relative position and size
         * 
         * @param relBounds The relative bounds of the PlayerPowerups
         * @param player The player
         * @param position The side of the screen the powerups are displayed on (default: LEFT)
         */
        PlayerPowerups(Player *player, SidePosition position = LEFT);
        ~PlayerPowerups();

        void Update();
        void Draw();
        void DrawDebug();
};
        
#endif // __PLAYER_POWERUPS_H__