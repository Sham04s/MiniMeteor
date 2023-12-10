#include "game_object.hpp"

#define BULLET_SIZE 40
#define BULLET_SPEED 250

class Bullet : public GameObject
{
private:

public:
    Bullet(Vector2 origin, Vector2 forwardDir);
    ~Bullet();

    void Update();
    void Draw();
    void DrawDebug();

    bool isOutOfBounds();
};