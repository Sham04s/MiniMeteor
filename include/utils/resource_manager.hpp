#ifndef __RESOURCE_MANAGER_H__
#define __RESOURCE_MANAGER_H__

#include "raylib.h"
#include <vector>

/**
 * @brief IDs for all the sprite textures of game objects
 */
enum SpriteTextureID
{
    PLAYER_SPRITES,
    ASTEROID_DETAILED_LARGE_SPRITE,
    ASTEROID_DETAILED_SMALL_SPRITE,
    ASTEROID_LARGE_SPRITE,
    ASTEROID_SMALL_SPRITE,
    ASTEROID_SQUARED_DETAILED_LARGE_SPRITE,
    ASTEROID_SQUARED_DETAILED_SMALL_SPRITE,
    ASTEROID_SQUARED_LARGE_SPRITE,
    ASTEROID_SQUARED_SMALL_SPRITE,
    BULLET_SPRITE,
    CROSSHAIR_SPRITE,
    ENEMY_BULLET_SPRITE,
    ENEMY_SHOOTER_SPRITES,
    ENEMY_STALKER_SPRITES,
    ENEMY_PULSER_SPRITES,
    POWERUP_LIFE_ITEM_SPRITE,
    POWERUP_SHIELD_SPRITE,
    POWERUP_SHIELD_ITEM_SPRITE,
    POWERUP_TEMPORARY_SHIELD_ITEM_SPRITE,
    POWERUP_SHOOT_COOLDOWN_UPGRADE_ITEM_SPRITE,
    POWERUP_BULLET_SPREAD_UPGRADE_ITEM_SPRITE,
    POWERUP_BULLET_SPEED_UPGRADE_ITEM_SPRITE,
    POWERUP_EXTRA_BULLET_UPGRADE_ITEM_SPRITE,
    POWERUP_TEMPORARY_INFINITE_BOOST_ITEM_SPRITE,
    // POWERUP_TEMPORARY_INFINITE_BOOST_SPRITE,
    // ENEMY_BOMBER_SPRITE,
    // EXPLOSION_SPRITE,
    NUM_SPRITE_TEXTURES
};

/**
 * @brief IDs for all the UI textures
 */
enum UITextureID
{
    BUTTON_PRIMARY_TEXTURE,
    BUTTON_SECONDARY_TEXTURE,
    LIFE_TEXTURE,
    DIRECTIONAL_SHIP_TEXTURE,
    CONTROLS_TEXTURE,
    NUM_UI_TEXTURES
};

/**
 * @brief IDs for all the sounds
 */
enum SoundID
{
    BULLET_SOUND,
    ENEMY_BULLET_SOUND,
    THRUST_SOUND,
    ENEMY_THRUST_SOUND,
    EXPLOSION_SOUND,
    SHIP_EXPLOSION_SOUND,
    CHANGE_TO_DIR_SHIP_SOUND,
    CHANGE_TO_NORMAL_SHIP_SOUND,
    ENEMY_EXPLOSION_SOUND,
    POWERUP_SPAWN_SOUND,
    POWERUP_PICKUP_SOUND,
    POWERUP_CANT_PICKUP_SOUND,
    NUM_SOUNDS
};

class ResourceManager
{
private:
    static Image icon;
    static std::vector<Texture2D> spriteTextures;
    static std::vector<Texture2D> uiTextures;
    static std::vector<Sound> sounds;
    static std::vector<Music> music;
    static Font font;

    /**
     * @brief Transparent texture
     */
    static Texture2D defaultTexture;

    /**
     * @brief Texture to use when a texture is not found
     */
    static Texture2D invalidTexture;

public:
    /**
     * @brief Loads all the required resources
     *
     * @return true if all resources were loaded successfully, false otherwise
     */
    static bool LoadResources();
    static void UnloadResources();

    static Image *GetIcon();

    static Texture2D *GetSpriteTexture(SpriteTextureID id);

    /**
     * @brief Returns the source rectangle for a sprite texture
     *
     * @param id the ID of the sprite texture
     * @param frame the frame of the sprite texture (from left to right and top to bottom)
     * @return Rectangle the source rectangle
     */
    static Rectangle GetSpriteSrcRect(SpriteTextureID id, unsigned int frame);
    static Texture2D *GetUITexture(UITextureID id);

    /**
     * @brief Returns the source rectangle for a UI texture
     *
     * @param id the ID of the UI texture
     * @param frame the frame of the UI texture (from left to right and top to bottom)
     * @return Rectangle the source rectangle
     */
    static Rectangle GetUISrcRect(UITextureID id, unsigned int frame);

    /**
     * @brief Returns a pointer to a sound, this way the sound doesn't need to be unloaded manually
     *
     * @param id the ID of the sound
     * @return Sound* a pointer to the sound
     */
    static Sound *GetSound(SoundID id);

    /**
     * @brief Creates a copy of a sound, this Sound should be unloaded manually with UnloadSoundAlias()
     *
     * @param id the ID of the sound
     * @return Sound a copy of the sound
     */
    static Sound CreateSoundAlias(SoundID id);

    static Font *GetFont();

    /**
     * @brief Returns the default texture (transparent)
     *
     * @return Texture* the default texture
     */
    static Texture *GetDefaultTexture();

    /**
     * @brief Returns the invalid texture (texture to use when a texture is not found)
     *
     * @return Texture* the invalid texture
     */
    static Texture *GetInvalidTexture();
};

#endif // __RESOURCE_MANAGER_H__