#ifndef __GAME_H__
#define __GAME_H__

#ifdef WINDOWS_HOT_RELOAD
#define CORE_API extern "C" __declspec(dllexport)
#else
#define CORE_API
#endif // WINDOWS_HOT_RELOAD

CORE_API void InitGame();
CORE_API void DrawFrame();
CORE_API void DrawDebug();
CORE_API void HandleInput();
CORE_API void ExitGame();

#endif // __GAME_H__