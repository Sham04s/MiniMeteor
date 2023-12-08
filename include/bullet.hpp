#include "game_object.hpp"

#define BULLET_SIZE 40
#define BULLET_SPEED 250

class Bullet : public GameObject
{
private:
    Vector2 velocity;

public:
    Bullet(Vector2 origin, Vector2 forwardDir);
    ~Bullet();

    void Update();
    void Draw();
    void DrawDebug();

    bool isOutOfBounds();
    bool isColliding(GameObject *other);
};