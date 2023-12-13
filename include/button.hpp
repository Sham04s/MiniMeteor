#ifndef __BUTTON_H__
#define __BUTTON_H__

#include "ui_object.hpp"
#include "functional"

#define BUTTON_WIDTH (float)(size == BUTTON_SMALL ? 100 : (size == BUTTON_MEDIUM ? 200 : 300))
#define BUTTON_HEIGHT (float)(size == BUTTON_SMALL ? 30 : (size == BUTTON_MEDIUM ? 50 : 70))
#define BUTTON_TEXT_SIZE (float)(size == BUTTON_SMALL ? 20 : (size == BUTTON_MEDIUM ? 30 : 40))
#define BUTTON_TEXT_SPACING (float)(size == BUTTON_SMALL ? 10 : (size == BUTTON_MEDIUM ? 15 : 20))
#define BUTTON_PADDING (float)(size == BUTTON_SMALL ? 5 : (size == BUTTON_MEDIUM ? 8 : 12))
#define BUTTON_TEXT_COLOR (variant == BUTTON_SECONDARY ? BLACK : WHITE)
#define BUTTON_PRESSED_OFFSET (float)(size == BUTTON_SMALL ? 2 : (size == BUTTON_MEDIUM ? 3 : 4))

enum ButtonVariant
{
    BUTTON_DEFAULT,
    BUTTON_PRIMARY,
    BUTTON_SECONDARY,
};

enum ButtonSize
{
    BUTTON_SMALL,
    BUTTON_MEDIUM,
    BUTTON_LARGE,
};

class Button : public UIObject
{
private:
    bool pressed;
    bool hovered;
    bool clicked;
    const char *text;
    std::function<void()> onClickFunc;
    ButtonVariant variant;
    ButtonSize size;
    Rectangle textBounds;

    void SetParent(UIObject *parent);

public:
    Button(Rectangle relBounds, UIObject *parent, const char *text, ButtonVariant variant, ButtonSize size);
    Button(Vector2 relPos, UIObject *parent, const char *text, ButtonVariant variant, ButtonSize size, std::function<void()> onClickFunc);
    ~Button();

    void Update();
    void Draw();
    void DrawDebug();

    void OnClick(std::function<void()> func) { onClickFunc = func; }

    bool IsPressed() { return pressed; }
    bool IsHovered() { return hovered; }
    bool IsClicked() { return clicked; }

    float GetWidth() { return bounds.width; }
    float GetHeight() { return bounds.height; }
    float GetTextSize() { return BUTTON_TEXT_SIZE; }
    float GetTextSpacing() { return BUTTON_TEXT_SPACING; }
    float GetPadding() { return BUTTON_PADDING; }
};
#endif // __BUTTON_H__