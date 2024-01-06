#ifndef __MENU_H__
#define __MENU_H__

#include "ui_object.hpp"
#include "label.hpp"

class Menu : public UIObject
{
    Label *title;
    
    public:
        Menu(const char *title);
        ~Menu();

        void Update();
        void Draw();
        void DrawDebug();

        void Resize(Vector2 prevScreenSize);
};

#endif // __MENU_H__