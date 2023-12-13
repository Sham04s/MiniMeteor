#ifndef __GAME_H__
#define __GAME_H__

#ifdef WINDOWS_HOT_RELOAD
#define CORE_API extern "C" __declspec(dllexport)
#else
#define CORE_API
#endif // WINDOWS_HOT_RELOAD

CORE_API void InitGame();
CORE_API bool GameLoop();
CORE_API void ExitGame();

void CreateNewGame();
void DrawFrame();
void DrawDebug();
void UpdateGame();

#endif // __GAME_H__