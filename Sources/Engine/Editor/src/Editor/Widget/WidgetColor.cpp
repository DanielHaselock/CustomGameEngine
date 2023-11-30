#include "Editor/Widget/WidgetColor.h"

using namespace Editor::Widget;

WidgetColor::WidgetColor(const QColor& pColor) : QColorDialog(pColor)
{
    setOption(QColorDialog::ShowAlphaChannel, false);
    //setWindowFlags(Qt::Widget);
    
    //Hide color panel child
    for (int i = 0; i < children().count(); i++)
    {
        std::string list = children().at(i)->metaObject()->className();

        if (list != "QColorPicker" && list != "QColorLuminancePicker" && list != "QColorShower")
        {
            QWidget* wid = dynamic_cast<QWidget*>(children().at(i));
            if (wid != nullptr)
                wid->hide();
        }

        if (list == "QColorShower")
        {
            QWidget* obj = ((QWidget*)children().at(i));
            for (int j = 0; j < obj->children().count(); j++)
            {
                std::string childList = obj->children().at(j)->metaObject()->className();

                if (childList != "QGridLayout" && childList != "QColorShowLabel")
                {
                    if (j > 7 && j < 16)
                        continue;

                    QWidget* wid = dynamic_cast<QWidget*>(obj->children().at(j));
                    if (wid != nullptr)
                        wid->hide();
                }
            }
            obj->adjustSize();
        }
    }
    
    adjustSize();
}