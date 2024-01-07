#ifndef __BUTTON_H__
#define __BUTTON_H__

#include "ui/components/common/ui_object.hpp"
#include "ui/components/common/label.hpp"
#include "functional"

#define BUTTON_DEFAULT_WIDTH 200
#define BUTTON_DEFAULT_HEIGHT 50

#define BUTTON_WIDTH (float)(size == BUTTON_SMALL ? 100 : (size == BUTTON_MEDIUM ? 200 : 300))
#define BUTTON_HEIGHT (float)(size == BUTTON_SMALL ? 30 : (size == BUTTON_MEDIUM ? 50 : 70))
#define BUTTON_FONT_SIZE (float)(size == BUTTON_SMALL ? 20 : (size == BUTTON_MEDIUM ? 30 : 40))
#define BUTTON_FONT_SPACING (float)(size == BUTTON_SMALL ? 10 : (size == BUTTON_MEDIUM ? 15 : 20))
#define BUTTON_FONT_COLOR (variant == BUTTON_SECONDARY ? BLACK : WHITE)
#define BUTTON_PADDING (float)(size == BUTTON_SMALL ? 5 : (size == BUTTON_MEDIUM ? 8 : 12))
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
    /**
     * @brief Whether the button is pressed down or not
     */
    bool pressed;

    /**
     * @brief Whether the mouse is hovering over the button or not
     */
    bool hovered;

    /**
     * @brief Whether the button was pressed and released while the mouse was hovering over it
     */
    bool clicked;
    
    std::function<void()> onClickFunc;
    
    ButtonVariant variant;
    ButtonSize size;

    /**
     * @brief The label that displays the text on the button
     */
    Label *label;

    void SetParent(UIObject *parent);

public:
    /**
     * @brief Construct a new Button object with a relative position and size
     * 
     * @param relBounds The relative bounds of the button
     * @param parent The parent UIObject
     * @param text The text to display on the button
     * @param variant The variant of the button
     * @param size The size of the button
     */
    Button(Rectangle relBounds, UIObject *parent, const char *text, ButtonVariant variant, ButtonSize size);

    /**
     * @brief Construct a new Button object with a relative position and size
     * 
     * @param relPos The relative position of the button
     * @param parent The parent UIObject
     * @param text The text to display on the button
     * @param variant The variant of the button
     * @param size The size of the button
     */
    Button(Vector2 relPos, UIObject *parent, const char *text, ButtonVariant variant, ButtonSize size, std::function<void()> onClickFunc);
    ~Button();

    void Update();
    void Draw();
    void DrawDebug();
    void Resize(Vector2 prevScreenSize);

    /**
     * @brief Set the OnClick function to be called when the button is clicked
     * 
     * @param func The function to call
     */
    void OnClick(std::function<void()> func) { onClickFunc = func; }

    bool IsPressed() { return pressed; }
    bool IsHovered() { return hovered; }
    bool IsClicked() { return clicked; }

    float GetWidth() { return bounds.width; }
    float GetHeight() { return bounds.height; }
    float GetTextSize() { return BUTTON_FONT_SIZE; }
    float GetTextSpacing() { return BUTTON_FONT_SPACING; }
    float GetPadding() { return BUTTON_PADDING; }

    void SetText(const char *text) { this->label->SetText(text); }
};
#endif // __BUTTON_H__