#include "resource_manager.hpp"
#include <map>

using namespace std;

const std::map<SpriteTextureID, const char *> spriteTexturesPathsMap = {
    {PLAYER_SPRITES, "resources/characters/player/player.png"},
    {ASTEROID_DETAILED_LARGE_SPRITE, "resources/environment/asteroid_detailed_large.png"},
    {ASTEROID_DETAILED_SMALL_SPRITE, "resources/environment/asteroid_detailed_small.png"},
    {ASTEROID_LARGE_SPRITE, "resources/environment/asteroid_large.png"},
    {ASTEROID_SMALL_SPRITE, "resources/environment/asteroid_small.png"},
    {ASTEROID_SQUARED_DETAILED_LARGE_SPRITE, "resources/environment/asteroid_squared_detailed_large.png"},
    {ASTEROID_SQUARED_DETAILED_SMALL_SPRITE, "resources/environment/asteroid_squared_detailed_small.png"},
    {ASTEROID_SQUARED_LARGE_SPRITE, "resources/environment/asteroid_squared_large.png"},
    {ASTEROID_SQUARED_SMALL_SPRITE, "resources/environment/asteroid_squared_small.png"},
    {BULLET_SPRITE, "resources/characters/player/bullet.png"},
    {ENEMY_BULLET_SPRITE, "resources/characters/enemies/bullet.png"},
    {ENEMY_BASIC_SPRITES, "resources/characters/enemies/basic_enemy.png"},
    {POWERUP_LIFE_ITEM_SPRITE, "resources/powerups/life_item.png"},
    {POWERUP_TEMPORARY_SHIELD_SPRITE, "resources/powerups/shield.png"},
    {POWERUP_SHIELD_ITEM_SPRITE, "resources/powerups/shield_item.png"},
    {POWERUP_SHIELD_SPRITE, "resources/powerups/temporary_shield.png"},
    {POWERUP_TEMPORARY_SHIELD_ITEM_SPRITE, "resources/powerups/temporary_shield_item.png"},
    {POWERUP_FIRE_RATE_UPGRADE_ITEM_SPRITE, "resources/powerups/fire_rate_upgrade_item.png"},
    {POWERUP_BULLET_SPREAD_UPGRADE_ITEM_SPRITE, "resources/powerups/bullet_spread_upgrade_item.png"},
    {POWERUP_EXTRA_BULLET_UPGRADE_ITEM_SPRITE, "resources/powerups/extra_bullet_upgrade_item.png"},
    {POWERUP_BULLET_SPEED_UPGRADE_ITEM_SPRITE, "resources/powerups/bullet_speed_upgrade_item.png"},
    {POWERUP_TEMPORARY_INFINITE_BOOST_ITEM_SPRITE, "resources/powerups/temporary_infinite_boost_item.png"},
};

const std::map<UITextureID, const char *> uiTexturesPathsMap = {
    {BUTTON_PRIMARY_TEXTURE, "resources/ui/primary_button.png"},
    {BUTTON_SECONDARY_TEXTURE, "resources/ui/secondary_button.png"},
    {LIFE_TEXTURE, "resources/ui/life.png"},
};

const std::map<SoundID, const char *> soundsPathsMap = {
    {BULLET_SOUND, "resources/sounds/bullet.wav"},
    {ENEMY_BULLET_SOUND, "resources/sounds/enemy_bullet.wav"},
    {THRUST_SOUND, "resources/sounds/thrust.wav"},
    {ENEMY_THRUST_SOUND, "resources/sounds/enemy_thrust.wav"},
    {EXPLOSION_SOUND, "resources/sounds/explosion.wav"},
    {SHIP_EXPLOSION_SOUND, "resources/sounds/ship_explosion.wav"},
    {ENEMY_EXPLOSION_SOUND, "resources/sounds/enemy_explosion.wav"},
    {POWERUP_PICKUP_SOUND, "resources/sounds/powerup_pickup.wav"},
};

std::vector<Texture2D> ResourceManager::spriteTextures;
std::vector<Texture2D> ResourceManager::uiTextures;
std::vector<Sound> ResourceManager::sounds;
std::vector<Music> ResourceManager::music;
Font ResourceManager::font;
Texture2D ResourceManager::defaultTexture;
Texture2D ResourceManager::invalidTexture;

bool ResourceManager::LoadResources()
{
    if (!IsWindowReady())
    {
        return false;
    }

    // create invalid texture from image
    Image invalidTextureImage = GenImageColor(64, 64, BLACK);
    ImageDrawRectangle(&invalidTextureImage, 0, 0, 32, 32, PURPLE);
    ImageDrawRectangle(&invalidTextureImage, 32, 32, 32, 32, PURPLE);
    invalidTexture = LoadTextureFromImage(invalidTextureImage);
    UnloadImage(invalidTextureImage);

    for (size_t i = 0; i < NUM_SPRITE_TEXTURES; i++)
    {
        if (!FileExists(spriteTexturesPathsMap.at((SpriteTextureID)i)))
        {
            spriteTextures.push_back(invalidTexture);
        }
        else
        {
            spriteTextures.push_back(LoadTexture(spriteTexturesPathsMap.at((SpriteTextureID)i)));
        }
    }
    for (size_t i = 0; i < NUM_UI_TEXTURES; i++)
    {
        if (!FileExists(uiTexturesPathsMap.at((UITextureID)i)))
        {
            uiTextures.push_back(invalidTexture);
        }
        else
        {
            uiTextures.push_back(LoadTexture(uiTexturesPathsMap.at((UITextureID)i)));
        }
    }
    for (size_t i = 0; i < NUM_SOUNDS; i++)
    {
        if (!FileExists(soundsPathsMap.at((SoundID)i)))
        {
            sounds.push_back({{0}});
        }
        else
        {
            sounds.push_back(LoadSound(soundsPathsMap.at((SoundID)i)));
        }
    }

    // font = LoadFontEx("resources/common/SyneMono-Regular.ttf", 128, 0, 95);
    font = GetFontDefault();

    return true;
}

void ResourceManager::UnloadResources()
{
    for (size_t i = 0; i < spriteTextures.size(); i++)
    {
        UnloadTexture(spriteTextures[i]);
    }
    spriteTextures.clear();
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
    UnloadTexture(invalidTexture);
    UnloadFont(font);
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

// sound alias should be unloaded manually
Sound ResourceManager::CreateSoundAlias(SoundID id)
{
    if (id >= sounds.size())
    {
        return {{0}};
    }
    return LoadSoundAlias(sounds[id]);
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

Texture *ResourceManager::GetInvalidTexture()
{
    return &invalidTexture;
}