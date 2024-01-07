#include "utils/resource_manager.hpp"
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
    {CROSSHAIR_SPRITE, "resources/characters/player/crosshair.png"},
    {BULLET_SPRITE, "resources/characters/player/bullet.png"},
    {ENEMY_BULLET_SPRITE, "resources/characters/enemies/bullet.png"},
    {ENEMY_SHOOTER_SPRITES, "resources/characters/enemies/shooter.png"},
    {POWERUP_LIFE_ITEM_SPRITE, "resources/powerups/life_item.png"},
    {POWERUP_SHIELD_ITEM_SPRITE, "resources/powerups/shield_item.png"},
    {POWERUP_SHIELD_SPRITE, "resources/powerups/shield.png"},
    {POWERUP_TEMPORARY_SHIELD_ITEM_SPRITE, "resources/powerups/temporary_shield_item.png"},
    {POWERUP_SHOOT_COOLDOWN_UPGRADE_ITEM_SPRITE, "resources/powerups/shoot_cooldown_upgrade_item.png"},
    {POWERUP_BULLET_SPREAD_UPGRADE_ITEM_SPRITE, "resources/powerups/bullet_spread_upgrade_item.png"},
    {POWERUP_EXTRA_BULLET_UPGRADE_ITEM_SPRITE, "resources/powerups/extra_bullet_upgrade_item.png"},
    {POWERUP_BULLET_SPEED_UPGRADE_ITEM_SPRITE, "resources/powerups/bullet_speed_upgrade_item.png"},
    {POWERUP_TEMPORARY_INFINITE_BOOST_ITEM_SPRITE, "resources/powerups/temporary_infinite_boost_item.png"},
};

const std::map<UITextureID, const char *> uiTexturesPathsMap = {
    {BUTTON_PRIMARY_TEXTURE, "resources/ui/primary_button.png"},
    {BUTTON_SECONDARY_TEXTURE, "resources/ui/secondary_button.png"},
    {LIFE_TEXTURE, "resources/ui/life.png"},
    {DIRECTIONAL_SHIP_TEXTURE, "resources/ui/directional_ship.png"},
    {CONTROLS_TEXTURE, "resources/ui/controls.png"},
};

const std::map<SoundID, const char *> soundsPathsMap = {
    {BULLET_SOUND, "resources/sounds/bullet.wav"},
    {ENEMY_BULLET_SOUND, "resources/sounds/enemy_bullet.wav"},
    {THRUST_SOUND, "resources/sounds/thrust.wav"},
    {ENEMY_THRUST_SOUND, "resources/sounds/enemy_thrust.wav"},
    {EXPLOSION_SOUND, "resources/sounds/explosion.wav"},
    {SHIP_EXPLOSION_SOUND, "resources/sounds/ship_explosion.wav"},
    {CHANGE_TO_DIR_SHIP_SOUND, "resources/sounds/change_to_dir_ship.wav"},
    {CHANGE_TO_NORMAL_SHIP_SOUND, "resources/sounds/change_to_normal_ship.wav"},
    {ENEMY_EXPLOSION_SOUND, "resources/sounds/enemy_explosion.wav"},
    {POWERUP_SPAWN_SOUND, "resources/sounds/powerup_spawn.wav"},
    {POWERUP_PICKUP_SOUND, "resources/sounds/powerup_pickup.wav"},
    {POWERUP_CANT_PICKUP_SOUND, "resources/sounds/powerup_cant_pickup.wav"},
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
            return false;
        }
        sounds.push_back(LoadSound(soundsPathsMap.at((SoundID)i)));
    }

    const char *charactersToLoad =
        "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789!\"#$%&'()*+,-./:;<=>?@[\\]^_`{|}~ ∞©";
    int count = 0;
    int *codePoints = LoadCodepoints(charactersToLoad, &count);
    font = LoadFontEx("resources/common/SyneMono-Regular.ttf", 128, codePoints, count);
    UnloadCodepoints(codePoints);
    // font = LoadFontEx("resources/common/SyneMono-Regular.ttf", 128, 0, 95);

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
    int rows = 1;
    int columns = 1;

    switch (id)
    {

    case PLAYER_SPRITES:
        rows = 4;
        columns = 3;
        break;

    case ENEMY_SHOOTER_SPRITES:
        rows = 1;
        columns = 3;
        break;

    default:
        return {0, 0, (float)spriteTextures[id].width, (float)spriteTextures[id].height}; // default
    }

    int frameWidth = spriteTextures[id].width / columns;
    int frameHeight = spriteTextures[id].height / rows;

    int row = frame / columns;
    int column = frame % columns;

    return {(float)(frameWidth * column), (float)(frameHeight * row), (float)frameWidth, (float)frameHeight};
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
    return {0, 0, (float)uiTextures[id].width, (float)uiTextures[id].height};
}

Sound *ResourceManager::GetSound(SoundID id)
{
    return &sounds[id];
}

Sound ResourceManager::CreateSoundAlias(SoundID id)
{
    if (id >= sounds.size())
    {
        return {{0}};
    }
    return LoadSoundAlias(sounds[id]);
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