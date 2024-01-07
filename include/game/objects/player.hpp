#ifndef __PLAYER_H__
#define __PLAYER_H__

#include "game/objects/character.hpp"
#include "game/objects/asteroid.hpp"
#include "game/objects/power_up.hpp"
#include "utils/score_registry.hpp"

#include <vector>

#define PLAYER_INITIAL_LIVES 3

#define DIRECTIONAL_SHIP_METER_MAX 100.0f
#define CHANGING_SHIP_TIME 0.2f // seconds (needs 2 times this to disappear current ship and appear the other one)

#define BOOST_TIME 0.8f                                      // seconds
#define BOOST_RECHARGE_COOLDOWN 1.5f                         // seconds
#define BOOST_BAR_HIDE_TIME (BOOST_RECHARGE_COOLDOWN + 1.0f) // seconds
#define BOOST_BAR_FADE_TIME 0.5f                             // seconds
#define MAX_UPGRADES_PER_TYPE 5

class Player : public Character
{
private:
    /**
     * @brief The initial origin of the player.
     * This is used to reset the player to its initial position.
     */
    Vector2 initialOrigin;

    /**
     * @brief Camera used to view the world.
     */
    Camera2D camera;

    bool invincible;
    bool hasMoved; // used to disable invincibility when the player moves or shoots

    bool usingBoost;
    float boostTime;
    float lastBoostUsedTime;

    float changingShipTime;
    bool changingShip; // changing from ship to directional ship or vice versa
    bool directionalShip;
    float directionalShipMeter;

    // used to not draw the player sprite
    bool hidden;

    /**
     * @brief Stores the powerups the player has collected.
     */
    std::vector<PowerUp *> powerups;

    /**
     * @brief Tracks the number of powerups the player has for each type.
     */
    size_t powerupsCount[NUM_POWER_UP_TYPES];

    // crosshair texture when the player is a directional ship
    Texture2D *crosshair;

    Sound *changeToDirShipSound;
    Sound *changeToShipSound;

protected:
    /**
     * @brief Sets the default hitbox for the player.
     * The hitbox is a triangle that fits the player sprite.
     */
    void SetDefaultHitBox();

    /**
     * @brief Sets the hitbox for the player when it is a directional ship.
     * The hitbox is polygon that fits the player sprite.
     */
    void SetDirectionalShipHitBox();

public:
    Player() : Player({0, 0}) {}
    Player(Vector2 origin);
    ~Player();

    void Update();
    void UpdatePowerups();
    void Draw();
    void DrawDebug();
    void HandleInput();
    bool CheckCollision(GameObject *other, Vector2 *pushVector);
    void HandleCollision(GameObject *other, Vector2 *pushVector);
    void HandleBulletCollision(Bullet *bullet, GameObject *other, Vector2 *pushVector);

    PowerUp *GetPowerup(PowerUpType type);
    bool AddPowerup(PowerUp *powerup);
    bool RemovePowerup(PowerUpType type);
    bool HasPowerup(PowerUpType type);
    bool CanBeKilled();
    bool CanBeHit();
    bool HasMoved();

    Camera2D GetCamera() { return camera; }

    /**
     * @brief Updates the 2D camera to point at origin.
     * Useful for window resizing.
     */
    void UpdateCamera();
    
    /**
     * @brief Kills the player removing a life and one powerup of each type.
     *
     * @return true if the player is killed, false otherwise.
     */
    bool Kill();
    /**
     * @brief Resets player state to initial state.
     */
    void Respawn();
    /**
     * @brief Completely resets the player with its lives, powerups, bullets, and state.
     */
    void Reset();

    void Show() { hidden = false; }
    void Hide() { hidden = true; }

    void ToggleDirectionalShip();
    void IncreaseDirectionalShipMeter(ScoreType scoreType);

    /**
     * @brief Gets the source rectangle of the player's texture based on it's current state.
     * 
     * @return The source rectangle of the player's texture.
     */
    Rectangle GetFrameRec();

    std::vector<PowerUp *> GetPowerups() { return powerups; }

    /**
     * @brief Gets the multiplier for the given powerup type.
     * This is calculated inside the method.
     * 
     * @param type The powerup type. (see PowerUpType)
     * @return The multiplier for the given powerup type.
     */
    float GetPowerupMultiplier(PowerUpType type);

    /**
     * @brief Gets the number of powerups the player has for the given type.
     * 
     * @param type The powerup type. (see PowerUpType)
     * @return The number of powerups the player has for the given type.
     */
    size_t GetPowerupCount(PowerUpType type) { return powerupsCount[type]; }
    float GetDirectionalShipMeter() { return directionalShipMeter; }  
};

#endif // __PLAYER_H__