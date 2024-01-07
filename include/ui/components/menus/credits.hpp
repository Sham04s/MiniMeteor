#ifndef __CREDITS_H__
#define __CREDITS_H__

#include <ui/components/menus/menu.hpp>
#include <ui/components/common/label.hpp>

#define CREDITS_TEXT "Made by Sham04\n"                        \
                     "Original Game Textures provided by\n"    \
                     "Kenney (kenney.nl) and modified by me\n" \
                     "Made with raylib (www.raylib.com)\n"     \
                     "Font used: Syne Mono (© 2017 The Syne Project Authors)"

#define CREDITS_COLORS \
    (Color[]) { DARKPURPLE, GetColor(0xffd400ff), GetColor(0xffd400ff), RAYWHITE, LIGHTGRAY }

#define CREDITS_LINE_PADDING 32

class Credits : public Menu
{
private:
    Rectangle creditsBounds;

public:
    Credits();
    ~Credits();

    void Update();
    void Draw();
    void DrawDebug();

    void Resize(Vector2 prevScreenSize);
};

#endif // __CREDITS_H__