#ifndef __GAME_OBJECT_H__
#define __GAME_OBJECT_H__

#include "raylib.h"

#include <vector>
#include "raymath.h"
#include <math.h>

#include "utils/resource_manager.hpp"

/**
 * @brief Enumeration of different types of game objects.
 */
enum GameObjectType
{
    PLAYER,                /**< Player object */
    ENEMY,                 /**< Enemy object */
    BULLET,                /**< Bullet object */
    ENEMY_BULLET,          /**< Enemy bullet object */
    ASTEROID,              /**< Asteroid object */
    POWER_UP,              /**< Power-up object */
    NUM_GAME_OBJECT_TYPES, /**< Number of game object types */
    NONE                   /**< No type */
};

/**
 * @brief Base class for all game objects.
 */
class GameObject
{
protected:
    Rectangle bounds;              /**< Bounding rectangle of the object */
    Vector2 origin;                /**< Origin point of the object */
    float rotation;                /**< Rotation angle of the object */
    Vector2 forwardDir;            /**< Forward direction of the object */
    std::vector<Vector2> hitbox;   /**< Hitbox points of the object */
    Vector2 previousVelocity;      /**< Previous velocity of the object */
    Vector2 velocity;              /**< Velocity of the object */
    float previousAngularVelocity; /**< Previous angular velocity of the object */
    float angularVelocity;         /**< Angular velocity of the object */
    GameObjectType type;           /**< Type of the object */
    Texture2D *texture;            /**< Texture of the object */

public:
    /**
     * @brief Default constructor for GameObject class.
     */
    GameObject() : GameObject({0, 0}, 0, {0, 0}, {}, NONE){};

    /**
     * @brief Constructor for GameObject class.
     * @param bounds The bounding rectangle of the object.
     * @param rotation The rotation angle of the object.
     * @param forwardDir The forward direction of the object.
     * @param hitbox The hitbox points of the object.
     * @param type The type of the object.
     */
    GameObject(Rectangle bounds, float rotation, Vector2 forwardDir, std::vector<Vector2> hitbox, GameObjectType type);

    /**
     * @brief Virtual destructor for GameObject class.
     */
    virtual ~GameObject();

    /**
     * @brief Update function for the game object.
     */
    virtual void Update();

    /**
     * @brief Draw function for the game object.
     */
    virtual void Draw();

    /**
     * @brief Draw debug information for the game object.
     */
    virtual void DrawDebug();

    /**
     * @brief Handle collision with another game object.
     * @param other The other game object involved in the collision.
     * @param pushVector The push vector to resolve the collision.
     */
    virtual void HandleCollision(GameObject *other, Vector2 *pushVector);

    /**
     * @brief Pause any sounds associated with the game object.
     */
    virtual void PauseSounds();

    /**
     * @brief Resume any paused sounds associated with the game object.
     */
    virtual void ResumeSounds();

    /**
     * @brief Check collision with another game object.
     * @param other The other game object to check collision with.
     * @param pushVector Output parameter for the push vector to resolve the collision.
     * @return True if collision occurs, false otherwise.
     */
    bool CheckCollision(GameObject *other, Vector2 *pushVector);

    /**
     * @brief Push the other game object in direction of the push vector.
     * @param other The other game object to push.
     * @param pushVector The push vector to apply.
     */
    void Push(GameObject *other, Vector2 pushVector);

    /**
     * @brief Translate the game object by a given translation vector.
     * @param translation The translation vector to apply.
     */
    void Translate(Vector2 translation);

    /**
     * @brief Rotate the game object by a given angle.
     * @param angle The angle to rotate by (in degrees).
     */
    void Rotate(float angle);

    /**
     * @brief Scale the game object by a given scale factor.
     * @param scale The scale factor to apply.
     */
    void Scale(float scale);

    /**
     * @brief Get the bounding rectangle of the game object.
     * @return The bounding rectangle.
     */
    Rectangle GetBounds() { return bounds; }

    /**
     * @brief Get the origin point of the game object.
     * @return The origin point.
     */
    Vector2 GetOrigin() { return origin; }

    /**
     * @brief Get the rotation angle of the game object.
     * @return The rotation angle.
     */
    float GetRotation() { return rotation; }

    /**
     * @brief Get the velocity of the game object.
     * @return The velocity.
     */
    Vector2 GetVelocity() { return velocity; }

    /**
     * @brief Get the angular velocity of the game object.
     * @return The angular velocity.
     */
    float GetAngularVelocity() { return angularVelocity; }

    /**
     * @brief Get the forward direction of the game object.
     * @return The forward direction.
     */
    Vector2 GetForwardDir() { return forwardDir; }

    /**
     * @brief Get the hitbox points of the game object.
     * @return The hitbox points.
     */
    std::vector<Vector2> GetHitbox() { return hitbox; }

    /**
     * @brief Get the type of the game object.
     * @return The type.
     */
    GameObjectType GetType() { return type; }

    /**
     * @brief Set the bounding rectangle of the game object.
     * @param bounds The bounding rectangle to set.
     */
    void SetBounds(Rectangle bounds) { this->bounds = bounds; }

    /**
     * @brief Set the origin point of the game object.
     * @param origin The origin point to set.
     */
    void SetOrigin(Vector2 origin) { this->origin = origin; }

    /**
     * @brief Set the rotation angle of the game object.
     * @param rotation The rotation angle to set.
     */
    void SetRotation(float rotation) { this->rotation = rotation; }

    /**
     * @brief Set the velocity of the game object.
     * @param velocity The velocity to set.
     */
    void SetVelocity(Vector2 velocity) { this->velocity = velocity; }

    /**
     * @brief Set the angular velocity of the game object.
     * @param angularVelocity The angular velocity to set.
     */
    void SetAngularVelocity(float angularVelocity) { this->angularVelocity = angularVelocity; }

    /**
     * @brief Set the forward direction of the game object.
     * @param forwardDir The forward direction to set.
     */
    void SetForwardDir(Vector2 forwardDir) { this->forwardDir = forwardDir; }

    /**
     * @brief Set the hitbox points of the game object.
     * @param hitbox The hitbox points to set.
     */
    void SetHitbox(std::vector<Vector2> hitbox) { this->hitbox = hitbox; }
};

#endif // __GAME_OBJECT_H__