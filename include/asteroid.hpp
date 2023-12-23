#ifndef __ASTEROID_H__
#define __ASTEROID_H__

#include "game_object.hpp"

#define ASTEROID_SIZE_SMALL 64
#define ASTEROID_SIZE_LARGE 96
#define ASTEROID_EXPLOSION_TIME 0.2f

enum AsteroidVariant
{
    SMALL,
    LARGE
};

enum AsteroidState
{
    FLOATING,
    EXPLODING,
    DESTROYED
};

class Asteroid : public GameObject
{
private:
    AsteroidVariant variant; // The variant of the asteroid
    AsteroidState state;     // The current state of the asteroid
    Sound explosionSound;    // The sound played when the asteroid explodes
    float size;              // The size of the asteroid bounds
    float lastExplosionTime; // The time when the asteroid last exploded

public:
    /**
     * @brief Constructs an Asteroid object with the given origin. This generates a random variant, size,
     * rotation, velocity, and angular velocity for the asteroid.
     * @param origin The origin position of the asteroid.
     */
    Asteroid(Vector2 origin);

    /**
     * @brief Destroys the Asteroid object.
     */
    ~Asteroid();

    /**
     * @brief Updates the state of the asteroid.
     */
    void Update();

    /**
     * @brief Draws the asteroid.
     */
    void Draw();

    /**
     * @brief Draws debug information about the asteroid.
     */
    void DrawDebug();

    /**
     * @brief Destroys the asteroid.
     */
    void Destroy();

    /**
     * @brief Handles collision with other game objects.
     *
     * @param other The other game object involved in the collision.
     * @param pushVector The push vector to resolve the collision.
     */
    void HandleCollision(GameObject *other, Vector2 *pushVector);

    /**
     * @brief Sets the velocity of the asteroid.
     *
     * @param velocity The velocity to set.
     */
    void SetVelocity(Vector2 velocity) { this->velocity = velocity; }

    /**
     * @brief Sets the rotation of the asteroid.
     *
     * @param rotation The rotation to set.
     */
    void SetRotation(float rotation) { this->rotation = rotation; }

    /**
     * @brief Sets the angular velocity of the asteroid.
     *
     * @param angularVelocity The angular velocity to set.
     */
    void SetAngularVelocity(float angularVelocity) { this->angularVelocity = angularVelocity; }

    /**
     * @brief Gets the velocity of the asteroid.
     *
     * @return The velocity of the asteroid.
     */
    Vector2 GetVelocity() { return velocity; }

    /**
     * @brief Gets the rotation of the asteroid.
     *
     * @return The rotation of the asteroid.
     */
    float GetRotation() { return rotation; }

    /**
     * @brief Gets the angular velocity of the asteroid.
     *
     * @return The angular velocity of the asteroid.
     */
    float GetAngularVelocity() { return angularVelocity; }

    /**
     * @brief Checks if the asteroid is in the floating state.
     *
     * @return True if the asteroid is floating, false otherwise.
     */
    bool IsFloating() { return state == FLOATING; }

    /**
     * @brief Checks if the asteroid is in the exploding state.
     *
     * @return True if the asteroid is exploding, false otherwise.
     */
    bool IsExploding() { return state == EXPLODING; }

    /**
     * @brief Checks if the asteroid is in the destroyed state.
     *
     * @return True if the asteroid is destroyed, false otherwise.
     */
    bool IsDestroyed() { return state == DESTROYED; }
};

#endif // __ASTEROID_H__