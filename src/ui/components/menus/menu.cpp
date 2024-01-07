#include "ui/components/menus/menu.hpp"
#include "utils/resource_manager.hpp"

Menu::Menu(const char *title)
    : UIObject({0, 0, (float)GetScreenWidth(), (float)GetScreenHeight()}, nullptr, ResourceManager::GetDefaultTexture())
{
    this->title = new Label({0, 50, (float)GetScreenWidth(), 100}, title, true, this);
    AddChild(this->title);
}

Menu::~Menu()
{
}

void Menu::Update()
{
    UIObject::Update();
}

void Menu::Draw()
{
    // dim background
    DrawRectangle(0, 0, (float)GetScreenWidth(), (float)GetScreenHeight(), Fade(BLACK, 0.35f));
    UIObject::Draw();
}

void Menu::DrawDebug()
{
    UIObject::DrawDebug();
}

void Menu::Resize(Vector2 prevScreenSize)
{
    UIObject::Resize(prevScreenSize);
}
