#include "resource_manager.hpp"

std::vector<Texture2D> ResourceManager::spriteTextures;
std::vector<Texture2D> ResourceManager::uiTextures;
std::vector<Sound> ResourceManager::sounds;
std::vector<Music> ResourceManager::music;
Font ResourceManager::font;

bool ResourceManager::LoadResources()
{
    if(!IsWindowReady())
    {
        return false;
    }
    Texture2D texture = LoadTexture("resources/characters/player.png");
    spriteTextures.push_back(texture);
    if (texture.id == 0)
    {
        return false;
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
}

Texture2D ResourceManager::GetSpriteTexture(SpriteTextureID id)
{
    return spriteTextures[id];
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