#pragma GCC diagnostic ignored "-Wcast-function-type"

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
        DeleteTempDLL();      \
    }

typedef void (*CoreFunc)(void);
typedef bool (*CoreFuncBool)(void);

HINSTANCE GameDLL = nullptr;
CoreFunc InitGame = nullptr;
CoreFuncBool GameLoop = nullptr;
CoreFunc ExitGame = nullptr;

// for notifiying that the dll has been reloaded
float notifyShowTime = 0; // in seconds

bool lastLoadSuccess = false;
long lastCoreCompileTime = 0;
float elapsedTimeSinceLastCheck = 0;

const char *dllName = "core.dll";
const char *tempDllName = "temp_core.dll";

void CreateTempDLL()
{
    std::ifstream src(dllName, std::ios::binary);
    if (!src.is_open())
    {
        TraceLog(LOG_ERROR, "Failed to open original DLL, are you in the same folder as the executable?\n");
        return;
    }
    std::ofstream dst(tempDllName, std::ios::binary);
    dst << src.rdbuf();
    src.close();
    dst.close();
}

void DeleteTempDLL()
{
    remove(tempDllName);
}

#else

#include "game.hpp"

#endif // WINDOWS_HOT_RELOAD

void InitRaylib()
{
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(DEFAULT_WIDTH, DEFAULT_HEIGHT, "MiniMeteor");
}
void LoadGame()
{
#ifdef WINDOWS_HOT_RELOAD
    lastLoadSuccess = false;
    CreateTempDLL();
    GameDLL = LoadLibraryA(tempDllName);
    if (!GameDLL)
    {
        TraceLog(LOG_ERROR, "Failed to load DLL\n");
        return;
    }

    InitGame = (CoreFunc)GetProcAddress(GameDLL, "InitGame");
    if (!InitGame)
    {
        TraceLog(LOG_ERROR, "Failed to load InitGame\n");
        return;
    }
    GameLoop = reinterpret_cast<CoreFuncBool>(GetProcAddress(GameDLL, "GameLoop"));
    if (!GameLoop)
    {
        TraceLog(LOG_ERROR, "Failed to load GameLoop\n");
        return;
    }
    ExitGame = (CoreFunc)GetProcAddress(GameDLL, "ExitGame");
    if (!ExitGame)
    {
        TraceLog(LOG_ERROR, "Failed to load ExitGame\n");
        return;
    }
    // game dll loaded successfully
    notifyShowTime = 2.0f;
    lastLoadSuccess = true;
    SetWindowSize(DEFAULT_WIDTH, DEFAULT_HEIGHT);
    SetWindowPosition(GetMonitorWidth(0) / 2 - DEFAULT_WIDTH / 2, GetMonitorHeight(0) / 2 - DEFAULT_HEIGHT / 2);

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
    GameLoop();
}

int main()
{
    InitRaylib();
    LoadGame();
#ifdef PLATFORM_WEB
    emscripten_set_main_loop(ExecuteGameLoop, 0, 1);
#else
#ifdef WINDOWS_HOT_RELOAD
    if (!lastLoadSuccess)
    {
        TraceLog(LOG_ERROR, "Failed to load game\n");
        ExitRaylib();
        return 1;
    }
    lastCoreCompileTime = GetFileModTime(dllName);
#endif // DEBUG
    while (!WindowShouldClose())
    {
        if (!GameLoop())
        {
            break;
        }
#ifdef WINDOWS_HOT_RELOAD
        if (notifyShowTime > 0)
        {
            int textWidth = MeasureText("Reloaded!", 20);
            DrawText("Reloaded!", (DEFAULT_WIDTH - textWidth) / 2, 20, 20, ColorAlpha(WHITE, fminf(1.0f, notifyShowTime)));
            notifyShowTime -= GetFrameTime();
        }

        elapsedTimeSinceLastCheck += GetFrameTime();
        if (elapsedTimeSinceLastCheck > 1.0f)
        {
            elapsedTimeSinceLastCheck = 0;
            const long newLastCompileTime = GetFileModTime(dllName);
            if (newLastCompileTime > lastCoreCompileTime)
            {
                WaitTime(0.2f); // wait for the dll to be written to disk
                UnloadGame();
                LoadGame();
                lastCoreCompileTime = newLastCompileTime;
            }
        }
        if (IsKeyPressed(KEY_F5))
        {
            UnloadGame();
            LoadGame();
        }
        if (!lastLoadSuccess)
        {
            TraceLog(LOG_ERROR, "Failed to reload game\n");
            ExitRaylib();
            return 1;
        }

#endif // WINDOWS_HOT_RELOAD
    }
#endif // PLATFORM_WEB
    UnloadGame();
    ExitRaylib();
    return 0;
}