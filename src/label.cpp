#include "label.hpp"
#include "resource_manager.hpp"

Label::Label(Rectangle relBounds, const char *text, Color color, TextAlignment hAlignment, TextAlignment vAlignment, int fontSize, UIObject *parent)
    : UIObject(relBounds, parent, ResourceManager::GetDefaultTexture())
{
    this->text = text;
    this->color = color;
    this->fontSize = fontSize;

    // calculate text bounds
    Vector2 textSize = MeasureTextEx(GetFontDefault(), text, fontSize, 1);
    float x = 0, y = 0;
    switch (hAlignment)
    {
    case ALIGN_CENTER:
        x = (relBounds.width - textSize.x) / 2;
        break;
    case ALIGN_END:
        x = relBounds.width - textSize.x;
        break;
    default:
        break;
    }
    switch (vAlignment)
    {
    case ALIGN_CENTER:
        y = (relBounds.height - textSize.y) / 2;
        break;
    case ALIGN_END:
        y = relBounds.height - textSize.y;
        break;
    default:
        break;
    }
    
    textBounds = {x, y, textSize.x, textSize.y};
}

Label::~Label()
{
}

void Label::Draw()
{
    UIObject::Draw();
    DrawTextEx(*ResourceManager::GetFont(), text, {bounds.x + textBounds.x, bounds.y + textBounds.y}, fontSize, 1, color);
}

void Label::DrawDebug()
{
    UIObject::DrawDebug();
    DrawRectangleLinesEx({bounds.x + textBounds.x, bounds.y + textBounds.y, textBounds.width, textBounds.height}, 1, BLUE);
}