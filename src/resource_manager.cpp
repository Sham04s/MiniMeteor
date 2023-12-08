#include "resource_manager.hpp"

const char *spriteTexturesPaths[NUM_SPRITE_TEXTURES] = {
    "resources/characters/player.png",
    // "resources/characters/enemy.png",
    // "resources/characters/bullet.png",
    // "resources/characters/asteroid.png",
    // "resources/characters/explosion.png",
    "resources/environment/asteroid_detailed_large.png",
    "resources/environment/asteroid_detailed_small.png",
    "resources/environment/asteroid_large.png",
    "resources/environment/asteroid_small.png",
    "resources/environment/asteroid_squared_detailed_large.png",
    "resources/environment/asteroid_squared_detailed_small.png",
    "resources/environment/asteroid_squared_large.png",
    "resources/environment/asteroid_squared_small.png",
    "resources/other/bullet.png",
};

std::vector<Texture2D> ResourceManager::spriteTextures;
std::vector<Texture2D> ResourceManager::uiTextures;
std::vector<Sound> ResourceManager::sounds;
std::vector<Music> ResourceManager::music;
Font ResourceManager::font;

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
    return true;
}

bool ResourceManager::ReloadResouces()
{
    return true;
}

void ResourceManager::UnloadResources()
{
    for (size_t i = 0; i < spriteTextures.size(); i++)
    {
        UnloadTexture(spriteTextures[i]);
    }
    spriteTextures.clear();
}

Texture2D ResourceManager::GetSpriteTexture(SpriteTextureID id)
{
    return spriteTextures[id];
}

Rectangle ResourceManager::GetSpriteSrcRect(SpriteTextureID id, unsigned int frame)
{
    if (id == PLAYER_SPRITE)
    {
        return {static_cast<float>(128 * frame), 0, 128, 192};
    }
    return {0, 0, 0, 0}; // TODO: change this!
}

Texture2D ResourceManager::GetUITexture(UITextureID id)
{
    return uiTextures[id];
}

Sound ResourceManager::GetSoundEffect(SoundID id)
{
    return sounds[id];
}

Music ResourceManager::GetMusicTrack(MusicID id)
{
    return music[id];
}

Font ResourceManager::GetFont()
{
    return font;
}