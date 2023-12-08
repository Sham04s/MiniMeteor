#ifndef __RESOURCE_MANAGER_H__
#define __RESOURCE_MANAGER_H__

#include "raylib.h"
#include <vector>

enum SpriteTextureID
{
    PLAYER_SPRITE,
    ASTEROID_DETAILED_LARGE_SPRITE,
    ASTEROID_DETAILED_SMALL_SPRITE,
    ASTEROID_LARGE_SPRITE,
    ASTEROID_SMALL_SPRITE,
    ASTEROID_SQUARED_DETAILED_LARGE_SPRITE,
    ASTEROID_SQUARED_DETAILED_SMALL_SPRITE,
    ASTEROID_SQUARED_LARGE_SPRITE,
    ASTEROID_SQUARED_SMALL_SPRITE,
    BULLET_SPRITE,
    ENEMY_SPRITE,
    EXPLOSION_SPRITE,
    NUM_SPRITE_TEXTURES
};

enum UITextureID
{
    BUTTON_TEXTURE,
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

public:
    static bool LoadResources();
    static bool ReloadResouces();
    static void UnloadResources();

    static Texture2D GetSpriteTexture(SpriteTextureID id);
    static Rectangle GetSpriteSrcRect(SpriteTextureID id, unsigned int frame);
    static Texture2D GetUITexture(UITextureID id);
    static Sound GetSoundEffect(SoundID id);
    static Music GetMusicTrack(MusicID id);
    static Font GetFont();
};

#endif // __RESOURCE_MANAGER_H__