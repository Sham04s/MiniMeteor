#include "game_object.hpp"

class Asteroid : public GameObject
{
private:
    Vector2 velocity;
    float angularVelocity;
    
public:
    Asteroid(Vector2 center, int zIndex);
    ~Asteroid();

    void Update();
    void Draw();
    void DrawDebug();


    void SetVelocity(Vector2 velocity);
    void SetRotation(float rotation);
    void SetAngularVelocity(float angularVelocity);

    Vector2 GetVelocity();
    float GetRotation();
    float GetAngularVelocity();
};

