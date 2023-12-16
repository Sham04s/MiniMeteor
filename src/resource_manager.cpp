#include "resource_manager.hpp"

using namespace std;

const char *spriteTexturesPaths[NUM_SPRITE_TEXTURES] = {
    "resources/characters/player/player.png",
    "resources/environment/asteroid_detailed_large.png",
    "resources/environment/asteroid_detailed_small.png",
    "resources/environment/asteroid_large.png",
    "resources/environment/asteroid_small.png",
    "resources/environment/asteroid_squared_detailed_large.png",
    "resources/environment/asteroid_squared_detailed_small.png",
    "resources/environment/asteroid_squared_large.png",
    "resources/environment/asteroid_squared_small.png",
    "resources/characters/player/bullet.png",
    "resources/characters/enemies/bullet.png",
    "resources/characters/enemies/basic_enemy.png",
};

const char *uiTexturesPaths[NUM_UI_TEXTURES] = {
    "resources/ui/primary_button.png",
    "resources/ui/blue_button03.png",
    "resources/ui/life.png",
};

const char *soundsPaths[NUM_SOUNDS] = {
    "resources/sounds/bullet.wav",
    "resources/sounds/enemy_bullet.wav",
    "resources/sounds/thrust.wav",
    "resources/sounds/enemy_thrust.wav",
};

std::vector<Texture2D> ResourceManager::spriteTextures;
std::vector<Texture2D> ResourceManager::uiTextures;
std::vector<Sound> ResourceManager::sounds;
std::vector<Music> ResourceManager::music;
Font ResourceManager::font;
Texture2D ResourceManager::defaultTexture;
Texture2D ResourceManager::transparentTexture;

bool ResourceManager::LoadResources()
{
    if (!IsWindowReady())
    {
        return false;
    }

    for (size_t i = 0; i < NUM_SPRITE_TEXTURES; i++)
    {
        spriteTextures.push_back(LoadTexture(spriteTexturesPaths[i]));
    }
    for (size_t i = 0; i < NUM_UI_TEXTURES; i++)
    {
        uiTextures.push_back(LoadTexture(uiTexturesPaths[i]));
    }
    for (size_t i = 0; i < NUM_SOUNDS; i++)
    {
        sounds.push_back(LoadSound(soundsPaths[i]));
    }

    font = GetFontDefault();

    return true;
}

bool ResourceManager::ReloadResouces()
{
    UnloadResources();
    return LoadResources();
}

void ResourceManager::UnloadResources()
{
    for (size_t i = 0; i < spriteTextures.size(); i++)
    {
        UnloadTexture(spriteTextures[i]);
    }
    spriteTextures.clear();
    UnloadFont(font);
    for (size_t i = 0; i < uiTextures.size(); i++)
    {
        UnloadTexture(uiTextures[i]);
    }
    uiTextures.clear();
    for (size_t i = 0; i < sounds.size(); i++)
    {
        UnloadSound(sounds[i]);
    }
    sounds.clear();
    for (size_t i = 0; i < music.size(); i++)
    {
        UnloadMusicStream(music[i]);
    }
    music.clear();
    UnloadTexture(defaultTexture);
    UnloadTexture(transparentTexture);
}

Texture2D *ResourceManager::GetSpriteTexture(SpriteTextureID id)
{
    return &spriteTextures[id];
}

Rectangle ResourceManager::GetSpriteSrcRect(SpriteTextureID id, unsigned int frame)
{
    int totalFrames;

    // horizontal spritesheet
    switch (id)
    {

    case PLAYER_SPRITES:
        totalFrames = 3;
        break;

    case BULLET_SPRITE:
        totalFrames = 1;
        break;

    case ENEMY_BASIC_SPRITES:
        totalFrames = 3;
        break;

    default:
        return {0, 0, (float)spriteTextures[id].width, (float)spriteTextures[id].height}; // default
    }

    int frameWidth = spriteTextures[id].width / totalFrames;
    int frameHeight = spriteTextures[id].height;
    return {(float)(frameWidth * frame), 0, (float)frameWidth, (float)frameHeight};
}

Texture2D *ResourceManager::GetUITexture(UITextureID id)
{
    return &uiTextures[id];
}

Rectangle ResourceManager::GetUISrcRect(UITextureID id, unsigned int frame)
{
    if (id == BUTTON_PRIMARY_TEXTURE || id == BUTTON_SECONDARY_TEXTURE)
    {
        int totalFrames = 4; // vertical spritesheet
        int frameWidth = uiTextures[id].width;
        int frameHeight = uiTextures[id].height / totalFrames;
        return {0, (float)(frameHeight * frame), (float)frameWidth, (float)frameHeight};
    }
    return {0, 0, 0, 0}; // TODO: change this!
}

Sound *ResourceManager::GetSound(SoundID id)
{
    return &sounds[id];
}

Music *ResourceManager::GetMusicTrack(MusicID id)
{
    return &music[id];
}

Font *ResourceManager::GetFont()
{
    return &font;
}

Texture *ResourceManager::GetDefaultTexture()
{
    return &defaultTexture;
}

Texture *ResourceManager::GetTransparentTexture()
{
    return &transparentTexture;
}