#pragma once

#include <QtCore/QObject>
#include <QtGui/qevent.h>
#include "Editor/UI/UIResource.h"
#include <memory>
#include <QtWidgets/qlabel.h>
#include <Editor/Widget/WidgetDropTexture.h>

namespace Editor::UI
{
    class Text : public UIResource
    {
    public:
        float mFontSize = 48;
        QString mText = "Text";

        Editor::Widget::WidgetDropTexture* mLabel;
        QString mPath;
        QPixmap mPixmap;

        Text();
        virtual ~Text() {};

        QString caption() const override { return QString("Text"); }

        QString name() const override { return QString("Text"); }

        bool resizable() const override { return false; }

        unsigned int nPorts(QtNodes::PortType const portType) const override;

        QtNodes::NodeDataType dataType(QtNodes::PortType const portType, QtNodes::PortIndex const portIndex) const override;

        std::shared_ptr<QtNodes::NodeData> outData(QtNodes::PortIndex const port) override;

        void setInData(std::shared_ptr<QtNodes::NodeData>, QtNodes::PortIndex const portIndex) override {};

        QWidget* embeddedWidget() override;

        QJsonObject save() const override;

        void load(QJsonObject const& p) override;

        bool eventFilter(QObject* object, QEvent* event) override;
    };
}