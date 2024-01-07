#ifndef __LABEL_H__
#define __LABEL_H__

#include "ui/components/common/ui_object.hpp"

#define LABEL_DEFAULT_FONT_SIZE 32

/**
 * @brief The alignment of the text inside the label.
 * Can be used to align the text horizontally and vertically.
 */
enum TextAlignment
{
    ALIGN_START,
    ALIGN_CENTER,
    ALIGN_END
};

class Label : public UIObject
{
private:
    const char *text;
    Color color;
    float fontSize;
    Rectangle textRelBounds;
    TextAlignment hAlignment, vAlignment;

public:
    /**
     * @brief Construct a new Label object with a relative position and size.
     * 
     * @param relBounds The relative bounds of the Label
     * @param text The text to display on the Label
     * @param title Whether the label is a title or not (bigger font size and centered text)
     * @param parent The parent UIObject
     */
    Label(Rectangle relBounds, const char *text, bool title, UIObject *parent)
        : Label(relBounds, text, WHITE, title ? ALIGN_CENTER : ALIGN_START, ALIGN_CENTER, LABEL_DEFAULT_FONT_SIZE * (title ? 1.5f : 1.0f), parent) {}
    
    /** 
     * @brief Construct a new Label object with a relative position and size.
     * 
     * @param relBounds The relative bounds of the Label
     * @param text The text to display on the Label
     * @param color The color of the text
     * @param hAlignment The horizontal alignment of the text
     * @param vAlignment The vertical alignment of the text
     * @param fontSize The font size of the text
     * @param parent The parent UIObject
     */
    Label(Rectangle relBounds, const char *text, Color color, TextAlignment hAlignment, TextAlignment vAlignment, int fontSize, UIObject *parent);
    ~Label();

    void Draw() override;
    void DrawDebug() override;
    void Resize(Vector2 prevScreenSize) override;

    void SetText(const char *text) { this->text = text; }
    void SetColor(Color color) { this->color = color; }

    /**
     * @brief Get the size of the text
     * 
     * @return The size of the text
     */
    Vector2 GetTextSize();
};

#endif // __LABEL_H__