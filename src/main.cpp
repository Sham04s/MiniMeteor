#include "raylib.h"
#include <fstream>

#define DEFAULT_WIDTH 1280
#define DEFAULT_HEIGHT 720

#ifdef PLATFORM_WEB
#include <emscripten/emscripten.h>
#endif // PLATFORM_WEB

#ifdef WINDOWS_HOT_RELOAD

#include <libloaderapi.h>
#include <math.h>

#define UNLOAD_GAME_DLL       \
    if (GameDLL)              \
    {                         \
        FreeLibrary(GameDLL); \
        deleteTempDLL();      \
    }

typedef void (*CoreFunc)(void);

HINSTANCE GameDLL = nullptr;
CoreFunc InitGame = nullptr;
CoreFunc DrawFrame = nullptr;
CoreFunc DrawDebug = nullptr;
CoreFunc HandleInput = nullptr;
CoreFunc ExitGame = nullptr;

// for notifiying that the dll has been reloaded
float notifyShowTime = 0; // in seconds

bool lastLoadSuccess = false;

const char *dllName = "core.dll";
const char *tempDllName = "temp_core.dll";
long lastModTime = 0;

void createTempDLL()
{
    std::ifstream src(dllName, std::ios::binary);
    if (!src.is_open())
    {
        printf("Failed to open original DLL, are you in the same folder as the executable?\n");
        return;
    }
    std::ofstream dst(tempDllName, std::ios::binary);
    dst << src.rdbuf();
    src.close();
    dst.close();
}

void deleteTempDLL()
{
    remove(tempDllName);
}

#else

#include "game.hpp"

#endif // WINDOWS_HOT_RELOAD

void InitRaylib()
{
    InitWindow(DEFAULT_WIDTH, DEFAULT_HEIGHT, "MiniMeteor");
}
void LoadGame()
{
#ifdef WINDOWS_HOT_RELOAD
    lastLoadSuccess = false;
    createTempDLL();
    GameDLL = LoadLibraryA(tempDllName);
    if (!GameDLL)
    {
        printf("Failed to load DLL\n");
        return;
    }

    InitGame = (CoreFunc)GetProcAddress(GameDLL, "InitGame");
    if (!InitGame)
    {
        printf("Failed to load InitGame\n");
        return;
    }
    DrawFrame = (CoreFunc)GetProcAddress(GameDLL, "DrawFrame");
    if (!DrawFrame)
    {
        printf("Failed to load DrawFrame\n");
        return;
    }
    DrawDebug = (CoreFunc)GetProcAddress(GameDLL, "DrawDebug");
    if (!DrawDebug)
    {
        printf("Failed to load DrawDebug\n");
        return;
    }
    HandleInput = (CoreFunc)GetProcAddress(GameDLL, "HandleInput");
    if (!HandleInput)
    {
        printf("Failed to load HandleInput\n");
        return;
    }
    ExitGame = (CoreFunc)GetProcAddress(GameDLL, "ExitGame");
    if (!ExitGame)
    {
        printf("Failed to load ExitGame\n");
        return;
    }
    // game dll loaded successfully
    notifyShowTime = 2.0f;
    lastLoadSuccess = true;

#endif // WINDOWS_HOT_RELOAD

    InitGame();
}

void UnloadGame()
{
    ExitGame();
#ifdef WINDOWS_HOT_RELOAD
    UNLOAD_GAME_DLL
#endif // WINDOWS_HOT_RELOAD
}
void ExitRaylib()
{
#ifdef WINDOWS_HOT_RELOAD
    UNLOAD_GAME_DLL
#endif // WINDOWS_HOT_RELOAD
    CloseWindow();
}

void ExecuteGameLoop()
{
    HandleInput();
    DrawFrame();
    DrawDebug();
}

int main()
{
    InitRaylib();
#ifdef PLATFORM_WEB
    LoadGame();
    emscripten_set_main_loop(ExecuteGameLoop, 60, 1);
#else
    LoadGame();
#ifdef WINDOWS_HOT_RELOAD
    if (!lastLoadSuccess)
    {
        printf("Failed to load game\n");
        ExitRaylib();
        return 1;
    }
#endif // DEBUG
    while (!WindowShouldClose())
    {
        ExecuteGameLoop();
#ifdef WINDOWS_HOT_RELOAD
        if (notifyShowTime > 0)
        {
            int textWidth = MeasureText("Reloaded!", 20);
            DrawText("Reloaded!", (DEFAULT_WIDTH - textWidth) / 2, 20, 20, ColorAlpha(WHITE, fminf(1.0f, notifyShowTime)));
            notifyShowTime -= GetFrameTime();
        }
        if (IsKeyPressed(KEY_F5))
        {
            UnloadGame();
            LoadGame();
            if (!lastLoadSuccess)
            {
                printf("Failed to reload game\n");
                ExitRaylib();
                return 1;
            }
        }
#endif // WINDOWS_HOT_RELOAD
    }
#endif // PLATFORM_WEB
    UnloadGame();
    ExitRaylib();
    return 0;
}