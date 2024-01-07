#include "ui/components/menus/credits.hpp"
#include "ui/components/common/button.hpp"
#include <string>

Credits::Credits()
    : Menu("Credits")
{
    const Vector2 creditsSize = {2 * bounds.width / 3, bounds.height / 2};
    this->creditsBounds = {bounds.x + bounds.width / 2 - creditsSize.x / 2, bounds.y + bounds.height / 2 - creditsSize.y / 2,
                           creditsSize.x, creditsSize.y};

    int creditsLineCount = 0;
    const char **creditsLines = TextSplit(CREDITS_TEXT, '\n', &creditsLineCount);

    const float creditsLineHeight = creditsBounds.height / creditsLineCount - CREDITS_LINE_PADDING;
    Rectangle labelRec = {creditsBounds.x, creditsBounds.y, creditsBounds.width, creditsLineHeight};

    for (int i = 0; i < creditsLineCount; i++)
    {
        Label *l = new Label(labelRec, creditsLines[i], CREDITS_COLORS[i], ALIGN_CENTER, ALIGN_CENTER, DEFAULT_FONT_SIZE, nullptr);
        if (i == 1)
        {
            labelRec.y += creditsLineHeight;
        }
        else
        {
            labelRec.y += creditsLineHeight + CREDITS_LINE_PADDING;
        }

        AddChild(l);
    }
}

Credits::~Credits()
{
}

void Credits::Update()
{
    Menu::Update();
}

void Credits::Draw()
{
    Menu::Draw();
    DrawLineEx({creditsBounds.x, creditsBounds.y - CREDITS_LINE_PADDING / 2}, {creditsBounds.x + creditsBounds.width, creditsBounds.y - CREDITS_LINE_PADDING / 2}, 1, WHITE);
    DrawLineEx({creditsBounds.x, creditsBounds.y + creditsBounds.height + CREDITS_LINE_PADDING / 2}, {creditsBounds.x + creditsBounds.width, creditsBounds.y + creditsBounds.height + CREDITS_LINE_PADDING / 2}, 1, WHITE);
}

void Credits::DrawDebug()
{
    Menu::DrawDebug();
}

void Credits::Resize(Vector2 prevScreenSize)
{
    Menu::Resize(prevScreenSize);
    Vector2 scale = {bounds.width / prevScreenSize.x, bounds.height / prevScreenSize.y};
    creditsBounds.x *= scale.x;
    creditsBounds.y *= scale.y;
    creditsBounds.width *= scale.x;
    creditsBounds.height *= scale.y;
}
