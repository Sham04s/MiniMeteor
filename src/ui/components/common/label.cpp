#include "ui/components/common/label.hpp"
#include "utils/resource_manager.hpp"

Label::Label(Rectangle relBounds, const char *text, Color color, TextAlignment hAlignment, TextAlignment vAlignment, int fontSize, UIObject *parent)
    : UIObject(relBounds, parent, ResourceManager::GetDefaultTexture())
{
    this->text = text;
    this->color = color;
    this->fontSize = fontSize;
    this->hAlignment = hAlignment;
    this->vAlignment = vAlignment;
    this->Resize({(float)GetScreenWidth(), (float)GetScreenHeight()});
}

Label::~Label()
{
}

void Label::Draw()
{
    UIObject::Draw();
    DrawTextEx(*ResourceManager::GetFont(), text, {bounds.x + textRelBounds.x, bounds.y + textRelBounds.y}, fontSize, 1, color);
}

void Label::DrawDebug()
{
    UIObject::DrawDebug();
    DrawRectangleLinesEx({bounds.x + textRelBounds.x, bounds.y + textRelBounds.y, textRelBounds.width, textRelBounds.height}, 1, BLUE);
}

void Label::Resize(Vector2 prevScreenSize)
{
    UIObject::Resize(prevScreenSize);

    fontSize = (fontSize * (GetScreenHeight() / prevScreenSize.y));

    // calculate text bounds
    Vector2 textSize = MeasureTextEx(*ResourceManager::GetFont(), text, fontSize, 1);
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

    textRelBounds = {x, y, textSize.x, textSize.y};
}

Vector2 Label::GetTextSize()
{
    return MeasureTextEx(*ResourceManager::GetFont(), text, fontSize, 1);
}