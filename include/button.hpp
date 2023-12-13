#ifndef __BUTTON_H__
#define __BUTTON_H__

#include "ui_object.hpp"

#define BUTTON_WIDTH 300
#define BUTTON_HEIGHT 100
#define BUTTON_PADDING 10
#define BUTTON_TEXT_PADDING 10
#define BUTTON_TEXT_COLOR WHITE
#define BUTTON_FONT_SIZE 20
#define BUTTON_FONT_COLOR_DISABLED GRAY

class Button : public UIObject
{
private:
    bool pressed;
    bool hovered;
    bool clicked;
    bool disabled;
    const char *text;
    void (*onClickFunc)();

public:
    Button(Rectangle relBounds, UIObject *parent, const char *text);
    Button(Vector2 relPos, UIObject *parent, const char *text, void (*onClick)());
    ~Button();

    void Update();
    void Draw();
    void DrawDebug();

    void OnClick(void (*onClick)());

    bool IsPressed() { return pressed; }
    bool IsHovered() { return hovered; }
    bool IsClicked() { return clicked; }
    bool IsDisabled() { return disabled; }
    
    void SetDisabled(bool disabled) { this->disabled = disabled; }
};
#endif // __BUTTON_H__