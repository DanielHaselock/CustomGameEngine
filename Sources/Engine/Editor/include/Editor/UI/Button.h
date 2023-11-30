#pragma once

#include <QtCore/QObject>
#include <QtGui/qevent.h>
#include <memory>
#include <QtWidgets/qlabel.h>
#include <Editor/Widget/WidgetDropTexture.h>
#include "Editor/UI/UIResource.h"

namespace Editor::UI
{
    struct StyleCompact
    {
        UIStyle hover;
        UIStyle press;
        UIStyle normal;
    };

    class Button : public QtNodes::NodeDelegateModel
    {
    public:
        float mFontSize = 48;
        QString mText = "Text";

        QString mName;
        bool isHidden = false;
        int zOrder = 0;

        StyleCompact mImageStyle;
        StyleCompact mTextStyle;
        
        Editor::Widget::WidgetDropTexture* mLabel;
        QString mPath;
        QPixmap mPixmap;

        Button();
        virtual ~Button() {};

        bool resizable() const override { return true; }

        QString caption() const override { return QString("Button"); }

        QString name() const override { return QString("Button"); }

        unsigned int nPorts(QtNodes::PortType const portType) const override;

        QtNodes::NodeDataType dataType(QtNodes::PortType const portType, QtNodes::PortIndex const portIndex) const override;

        std::shared_ptr<QtNodes::NodeData> outData(QtNodes::PortIndex const port) override;

        void setInData(std::shared_ptr<QtNodes::NodeData>, QtNodes::PortIndex const portIndex) override {};

        QWidget* embeddedWidget() override;

        bool eventFilter(QObject* object, QEvent* event) override;



        QJsonObject save() const override;

        void load(QJsonObject const& pData) override;

        QJsonObject saveUIStyle(const UIStyle* pStyle) const;

        void loadStyle(QJsonObject& jsonStyle, UIStyle& uiStyle);
    };
}