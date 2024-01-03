#ifndef __RESOURCE_MANAGER_H__
#define __RESOURCE_MANAGER_H__

#include "raylib.h"
#include <vector>

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
    ENEMY_BASIC_SPRITES,
    POWERUP_LIFE_ITEM_SPRITE,
    POWERUP_SHIELD_SPRITE,
    POWERUP_SHIELD_ITEM_SPRITE,
    POWERUP_TEMPORARY_SHIELD_ITEM_SPRITE,
    POWERUP_FIRE_RATE_UPGRADE_ITEM_SPRITE,
    POWERUP_BULLET_SPREAD_UPGRADE_ITEM_SPRITE,
    POWERUP_BULLET_SPEED_UPGRADE_ITEM_SPRITE,
    POWERUP_EXTRA_BULLET_UPGRADE_ITEM_SPRITE,
    POWERUP_TEMPORARY_INFINITE_BOOST_ITEM_SPRITE,
    // POWERUP_TEMPORARY_INFINITE_BOOST_SPRITE,
    // ENEMY_BOMBER_SPRITE,
    // EXPLOSION_SPRITE,
    NUM_SPRITE_TEXTURES
};

enum UITextureID
{
    BUTTON_PRIMARY_TEXTURE,
    BUTTON_SECONDARY_TEXTURE,
    LIFE_TEXTURE,
    DIRECTIONAL_SHIP_TEXTURE,
    NUM_UI_TEXTURES
};

enum SoundID
{
    BULLET_SOUND,
    ENEMY_BULLET_SOUND,
    THRUST_SOUND,
    ENEMY_THRUST_SOUND,
    EXPLOSION_SOUND,
    SHIP_EXPLOSION_SOUND,
    ENEMY_EXPLOSION_SOUND,
    POWERUP_PICKUP_SOUND,
    NUM_SOUNDS
};

enum MusicID
{
    MAIN_THEME,
    NUM_MUSIC
};

class ResourceManager
{
private:
    static std::vector<Texture2D> spriteTextures;
    static std::vector<Texture2D> uiTextures;
    static std::vector<Sound> sounds;
    static std::vector<Music> music;
    static Font font;

    static Texture2D defaultTexture;
    static Texture2D invalidTexture;

public:
    static bool LoadResources();
    static void UnloadResources();

    static Texture2D *GetSpriteTexture(SpriteTextureID id);
    static Rectangle GetSpriteSrcRect(SpriteTextureID id, unsigned int frame);
    static Texture2D *GetUITexture(UITextureID id);
    static Rectangle GetUISrcRect(UITextureID id, unsigned int frame);
    static Sound *GetSound(SoundID id);
    static Sound CreateSoundAlias(SoundID id);
    static Music *GetMusicTrack(MusicID id);
    static Font *GetFont();

    static Texture *GetDefaultTexture();
    static Texture *GetInvalidTexture();
};

#endif // __RESOURCE_MANAGER_H__