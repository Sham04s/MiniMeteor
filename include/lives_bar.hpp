#include "ui_object.hpp"
#include "player.hpp"

#define MAX_LIVES 10

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