#ifndef __MENU_H__
#define __MENU_H__

#include "ui/components/common/ui_object.hpp"
#include "ui/components/common/label.hpp"


/**
 * @brief Generic menu class
 * 
 */
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