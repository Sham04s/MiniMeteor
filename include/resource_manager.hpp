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
    ENEMY_BULLET_SPRITE,
    ENEMY_BASIC_SPRITES,
    ENEMY_BOMBER_SPRITE,
    EXPLOSION_SPRITE,
    NUM_SPRITE_TEXTURES
};

enum UITextureID
{
    BUTTON_PRIMARY_TEXTURE,
    BUTTON_SECONDARY_TEXTURE,
    LIFE_TEXTURE,
    NUM_UI_TEXTURES
};

enum SoundID
{
    SHOOT_SOUND,
    EXPLOSION_SOUND,
    NUM_SOUNDS
};

enum MusicID
{
    MAIN_THEME,
    NUM_MUSIC
};

enum FontID
{
    MAIN_FONT,
    NUM_FONTS
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
    static Texture2D transparentTexture;

public:
    static bool LoadResources();
    static bool ReloadResouces();
    static void UnloadResources();

    static Texture2D* GetSpriteTexture(SpriteTextureID id);
    static Rectangle GetSpriteSrcRect(SpriteTextureID id, unsigned int frame);
    static Texture2D* GetUITexture(UITextureID id);
    static Rectangle GetUISrcRect(UITextureID id, unsigned int frame);
    static Sound* GetSoundEffect(SoundID id);
    static Music* GetMusicTrack(MusicID id);
    static Font* GetFont();

    static Texture* GetDefaultTexture();
    static Texture* GetTransparentTexture();

};

#endif // __RESOURCE_MANAGER_H__