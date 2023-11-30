#include "Editor/Nodes/ColorPicker.h"
#include <QtWidgets/qmdiarea.h>
#include <QtWidgets/qboxlayout.h>
#include <QtWidgets/qmdisubwindow.h>
#include <QtCore/qjsonarray.h>

using namespace Editor::Nodes;

ColorPicker::ColorPicker() : mColor(std::make_shared<ColorOut>(QColor(255, 255, 255, 255))) {}

unsigned int ColorPicker::nPorts(QtNodes::PortType const portType) const
{
    unsigned int result = 1;

    switch (portType) 
    {
        case QtNodes::PortType::In:
            result = 0;
            break;

        case QtNodes::PortType::Out:
            result = 1;
            break;
        case QtNodes::PortType::None:
            break;
    }

    return result;
}

QtNodes::NodeDataType ColorPicker::dataType(QtNodes::PortType const portType, QtNodes::PortIndex const portIndex) const
{
    return ColorOut().type();
}

std::shared_ptr<QtNodes::NodeData> ColorPicker::outData(QtNodes::PortIndex const port)
{
    return mColor;
}

QWidget* ColorPicker::embeddedWidget()
{
    if (widget == nullptr)
    {
        dialog = new QColorDialog(QColor(255, 255, 255, 255));
        dialog->setOption(QColorDialog::NoButtons, true);
        dialog->setOption(QColorDialog::ShowAlphaChannel, true);
        dialog->setWindowFlags(Qt::Widget);
        connect(dialog, &QColorDialog::currentColorChanged, this, &ColorPicker::onColorChange);

        widget = new QWidget();
        QVBoxLayout* layout = new QVBoxLayout(widget);

        layout->addWidget(dialog);
        widget->show();

        //Hide color panel child
        for (int i = 0; i < dialog->children().count(); i++)
        {
            std::string list = dialog->children().at(i)->metaObject()->className();
            
            if (list != "QColorPicker" && list != "QColorLuminancePicker" && list != "QColorShower")
            {
                
                QWidget* wid = dynamic_cast<QWidget*>(dialog->children().at(i));
                if (wid != nullptr)
                    wid->hide();
            }

            if (list == "QColorShower")
            {
                QWidget* obj = ((QWidget*)dialog->children().at(i));
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
        dialog->adjustSize();
    }

    return widget;
}

void ColorPicker::onColorChange(const QColor& pColor)
{
    mColor = std::make_shared<ColorOut>(pColor);
    Q_EMIT dataUpdated(0);
}

QJsonObject ColorPicker::save() const
{
    QJsonObject modelJson = NodeDelegateModel::save();

    QJsonArray colorArr = mColor.get() == nullptr ?
                            QJsonArray({ 0, 0, 0, 0 }) : 
                            QJsonArray({ mColor.get()->color.red(), mColor.get()->color.green(), 
                                        mColor.get()->color.blue(), mColor.get()->color.alpha() });

    modelJson["color"] = colorArr;

    return modelJson;
}

void ColorPicker::load(QJsonObject const& p)
{
    QJsonValue v = p["color"];

    if (!v.isUndefined()) {
        QJsonArray arr = v.toArray();

        if (dialog != nullptr)
            dialog->setCurrentColor(QColor(arr[0].toInt(), arr[1].toInt(), arr[2].toInt(), arr[3].toInt()));
    
    }
}