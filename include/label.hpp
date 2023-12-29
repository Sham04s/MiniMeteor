#ifndef __LABEL_H__
#define __LABEL_H__

#include "ui_object.hpp"

#define LABEL_DEFAULT_FONT_SIZE 32

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
    int fontSize;
    Rectangle textBounds;

public:
    Label(Rectangle relBounds, const char *text, bool title, UIObject *parent)
        : Label(relBounds, text, WHITE, title ? ALIGN_CENTER : ALIGN_START, ALIGN_CENTER, LABEL_DEFAULT_FONT_SIZE * (title ? 1.5f : 1.0f), parent) {}
    Label(Rectangle relBounds, const char *text, Color color, TextAlignment hAlignment, TextAlignment vAlignment, int fontSize, UIObject *parent);
    ~Label();

    void Draw() override;
    void DrawDebug() override;

    void SetText(const char *text) { this->text = text; }
    void SetColor(Color color) { this->color = color; }
};

#endif // __LABEL_H__