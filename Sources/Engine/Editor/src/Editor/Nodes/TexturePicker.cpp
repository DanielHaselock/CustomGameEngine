#include "Editor/Nodes/TexturePicker.h"
#include <QtCore/qdir.h>
#include <QtWidgets/qfiledialog.h>
#include <QtCore/qmimedata.h>
#include "Editor/Utils/Utils.h"
#include <EngineCore/Service/ServiceLocator.h>
#include <Editor/Widget/WidgetEditor.h>
#include <QtWidgets/qmessagebox.h>

using namespace Editor::Nodes;

TexturePicker::TexturePicker() : 
    mLabel(new Editor::Widget::WidgetDropTexture("Click or drag to load image"))
{
    mLabel->setAlignment(Qt::AlignVCenter | Qt::AlignHCenter);

    QFont f = mLabel->font();
    f.setBold(true);
    f.setItalic(true);

    mLabel->setFont(f);

    mLabel->setMinimumSize(200, 200);
    mLabel->setMaximumSize(500, 300);

    mLabel->installEventFilter(this);
}

unsigned int TexturePicker::nPorts(QtNodes::PortType const portType) const
{
    unsigned int result = 1;

    switch (portType) 
    {
        case QtNodes::PortType::In:
            result = 0;
            break;

        case QtNodes::PortType::Out:
            result = 1;

        default:
            break;
    }

    return result;
}

QtNodes::NodeDataType TexturePicker::dataType(QtNodes::PortType const portType, QtNodes::PortIndex const portIndex) const
{
    return TextureOut().type();
}

std::shared_ptr<QtNodes::NodeData> TexturePicker::outData(QtNodes::PortIndex const port)
{
    return mTexture;
}

bool TexturePicker::eventFilter(QObject* object, QEvent* event)
{
    if (object == mLabel) {
        int w = mLabel->width();
        int h = mLabel->height();

        if (event->type() == QEvent::MouseButtonPress) {

            QString Defaultpath = service(Data::ProjectLocation).mFolder;

            std::string filter = "Image Files (*.png *.jpg *.bmp)";

            QFileDialog dialog(nullptr, tr("Open Image"), Defaultpath, tr(filter.c_str()));
            connect(&dialog, &QFileDialog::directoryEntered, [&dialog, Defaultpath, this](QString path)
            {
                if (!path.contains(Defaultpath))
                    dialog.setDirectory(Defaultpath);
            });

            if (dialog.exec() != QDialog::Accepted)
                return true;

            QString fileName = dialog.selectedFiles().first();
            if (fileName.isEmpty())
                return true;

            mPixmap = QPixmap(fileName);
            mTexture = std::make_shared<TextureOut>(fileName);
            mLabel->setPixmap(mPixmap.scaled(w, h, Qt::KeepAspectRatio));
            mLabel->adjustSize();
            Q_EMIT dataUpdated(0);
            return true;
        }
        if (event->type() == QEvent::GraphicsSceneDrop || event->type() == QEvent::Drop)
        {
            QFileInfo info = QFileInfo(((QDropEvent*)event)->mimeData()->text());

            QString Defaultpath = service(Data::ProjectLocation).mFolder;
            if (!info.absoluteFilePath().contains(Defaultpath))
            {
                QMessageBox::warning(nullptr, tr("Application"), tr("Can\'t use file outside of project"));
                return true;
            }

            if (Utils::getFileType(info.suffix()) == Tools::Utils::PathParser::EFileType::TEXTURE)
            {
                mPixmap = QPixmap(info.absoluteFilePath());
                mTexture = std::make_shared<TextureOut>(info.absoluteFilePath());
                mLabel->setPixmap(mPixmap.scaled(w, h, Qt::KeepAspectRatio));
                mLabel->adjustSize();
                Q_EMIT dataUpdated(0);
                return true;
            }
        }
    }

    return QtNodes::NodeDelegateModel::eventFilter(object, event);
}

QWidget* TexturePicker::embeddedWidget()
{
    return mLabel;
}

QJsonObject TexturePicker::save() const
{
    QJsonObject modelJson = NodeDelegateModel::save();

    QString finalStr = mTexture.get() == nullptr ? 
                        "" :
                    mTexture.get()->mPath.split(service(Editor::Widget::WidgetEditor).mLocation.mFolder + "/")[1];

    modelJson["Texture"] = Utils::qStringToStdString(finalStr).c_str();

    return modelJson;
}

void TexturePicker::load(QJsonObject const& p)
{
    QJsonValue v = p["Texture"];

    if (!v.isUndefined()) 
    {
        if (v.toString().isEmpty())
            return;

        int w = mLabel->width();
        int h = mLabel->height();
        
        QString path = service(Editor::Widget::WidgetEditor).mLocation.mFolder + "/" + v.toString();

        mTexture = std::make_shared<TextureOut>(path);

        mPixmap = QPixmap(path);
        mLabel->setPixmap(mPixmap.scaled(w, h, Qt::KeepAspectRatio));
        mLabel->adjustSize();

        Q_EMIT dataUpdated(0);
    }
}