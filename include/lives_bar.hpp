#include "ui_object.hpp"
#include "player.hpp"

#define MAX_LIVES 10

// TODO: make this a player info ui object, or maybe create a player info ui object that contains this
class LivesBar : public UIObject
{
private:
    Texture2D *lifeTexture;
    Player *player;
    float lifeSize;
    Rectangle centeredBounds;

public:
    LivesBar(Rectangle bounds, Player *player);
    ~LivesBar();

    void Draw();
    void DrawDebug();
};