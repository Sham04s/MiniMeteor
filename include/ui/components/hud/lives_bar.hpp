#ifndef __LIVES_BAR_H__
#define __LIVES_BAR_H__

#include "ui/components/common/ui_object.hpp"
#include "game/objects/player.hpp"

#define MAX_LIVES 10

/**
 * @brief A UIObject that displays the lives of the player
 * 
 */
class LivesBar : public UIObject
{
private:
    Player *player;

    float lifeSize;

    Rectangle centeredBounds;
    Texture2D *lifeTexture;

public:
    /**
     * @brief Construct a new LivesBar object with a relative position and size
     * 
     * @param relBounds The relative bounds of the LivesBar
     * @param player The player
     */
    LivesBar(Rectangle bounds, Player *player);
    ~LivesBar();

    void Draw();
    void DrawDebug();

    void Resize(Vector2 prevScreenSize);
    Rectangle GetCenteredBounds() { return centeredBounds; };
};
#endif // __LIVES_BAR_H__