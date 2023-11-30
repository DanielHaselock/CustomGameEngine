#include <QtCore/qdir.h>
#include "Editor/Widget/WidgetDropUI.h"
#include <QtWidgets/qlabel.h>
#include <QtWidgets/qtoolbar.h>
#include "Editor/Widget/WidgetStyleUITextButton.h"
#include "Editor/Resources/Loader/StyleSheetLoader.h"
#include "Tools/Utils/Define.h"
#include "Editor/Utils/Utils.h"
#include <Editor/Widget/WidgetCollapse.h>
#include <QtWidgets/qcheckbox.h>
#include <Editor/Widget/WidgetCollapseUI.h>
#include "Editor/Widget/WidgetDropInput.h"
#include <QtWidgets/qcolordialog.h>

using namespace Editor::Widget;

WidgetStyleUITextButton::WidgetStyleUITextButton(Editor::UI::Button& pResource, QWidget* pParent) :
    QDialog(pParent),
    mResource(pResource),
    mSettings("Resources/Editor/Config/StyleUIEN.ini", QSettings::IniFormat)
{
    setStyleSheet(Resources::Loaders::StyleSheetLoader::loadStyleSheet(mSettings.value("StyleSheet").toString()));

    mWindow = new QBoxLayout(QBoxLayout::TopToBottom, this);
    mWindow->setSpacing(mSettings.value("MainWindowSpacing").toInt());
    mWindow->setContentsMargins(
        mSettings.value("MainWindowMarginLeft").toInt(),
        mSettings.value("MainWindowMarginTop").toInt(),
        mSettings.value("MainWindowMarginRight").toInt(),
        mSettings.value("MainWindowMarginBottom").toInt());
    initCentralWidget();

}

void WidgetStyleUITextButton::initCentralWidget()
{
    QWidget* central = new QWidget();
    mWindow->addWidget(central);
    mWindow->setObjectName("central");
    
    QVBoxLayout* vBox = new QVBoxLayout();
    central->setLayout(vBox);

    // Check box
    {
        QWidget* wid = new QWidget();
        QGridLayout* widLayout = new QGridLayout();
        wid->setLayout(widLayout);
        
        QCheckBox* box = new QCheckBox();
        box->setCheckState(mResource.isHidden ? Qt::CheckState::Checked : Qt::CheckState::Unchecked);
        widLayout->addWidget(new QLabel("Is Hidden"), 0, 0, Qt::AlignLeft);
        widLayout->addWidget(new QLabel(" "), 0, 1, Qt::AlignRight);
        widLayout->addWidget(box, 0, 2, 1, 4, Qt::AlignLeft);
        connect(box, &QCheckBox::stateChanged, this, [this, box]
        {
            box->clearFocus();
            mResource.isHidden = box->checkState() == Qt::CheckState::Checked;
        });

        vBox->insertWidget(vBox->count(), wid, 0, Qt::AlignTop);
    }

    // Spin box
    {
        QWidget* wid = new QWidget();
        QGridLayout* widLayout = new QGridLayout();
        wid->setLayout(widLayout);

        WidgetDoubleSpinBox* spin = setSpinBox("Z Order", mResource.zOrder, widLayout, 1, 0);
        connect(spin, &WidgetDoubleSpinBox::valueChanged, this, [this, spin]
        {
            mResource.zOrder = spin->value();
            spin->setValue(mResource.zOrder);
        });

        vBox->insertWidget(vBox->count(), wid, 0, Qt::AlignTop);
    }

    // text
    {
        QWidget* wid = new QWidget();
        QGridLayout* widLayout = new QGridLayout();
        wid->setLayout(widLayout);

        QLineEdit* mName = new QLineEdit();
        mName->setObjectName("GameObject");
        mName->setText(mResource.mText);
        connect(mName, &QLineEdit::textChanged, this, [this, mName]
        {
            mResource.mText = mName->text();
        });

        widLayout->addWidget(new QLabel("Text"), 0, 0, Qt::AlignLeft);
        widLayout->addWidget(new QLabel(" "), 0, 1, Qt::AlignRight);
        widLayout->addWidget(mName, 0, 2, 1, 4, Qt::AlignLeft);

        vBox->insertWidget(vBox->count(), wid, 0, Qt::AlignTop);
    }

    // Font
    {
        QWidget* wid = new QWidget();
        QGridLayout* widLayout = new QGridLayout();
        wid->setLayout(widLayout);

        WidgetDoubleSpinBox* spin = setSpinBoxF("Font size", mResource.mFontSize, widLayout, 1, 0);
        connect(spin, &WidgetDoubleSpinBox::valueChanged, this, [this, spin]
        {
            mResource.mFontSize = spin->value();
            spin->setValue(mResource.mFontSize);
        });

        vBox->insertWidget(vBox->count(), wid, 0, Qt::AlignTop);
    }

    //Normal style
    {
        Editor::Widget::WidgetCollapseUI* hover = new Editor::Widget::WidgetCollapseUI(mSettings, QIcon(mSettings.value("normalUIStyle").toString()), mSettings.value("normalUIStyleTitle").toString());
        hover->setObjectName("Component");

        QHBoxLayout* h = new QHBoxLayout();

        //color
        {
            QWidget* right = new QWidget();
            QBoxLayout* vertical = new QBoxLayout(QBoxLayout::TopToBottom, nullptr);
            right->setLayout(vertical);

            setColor(*vertical, mResource.mTextStyle.normal);
            h->addWidget(right);
        }

        hover->setContentLayout(*h);
        vBox->insertWidget(vBox->count(), hover, 0, Qt::AlignTop);
    }

    //Hover style
    {
        Editor::Widget::WidgetCollapseUI* hover = new Editor::Widget::WidgetCollapseUI(mSettings, QIcon(mSettings.value("hoveUIStyle").toString()), mSettings.value("hoveUIStyleTitle").toString());
        hover->setObjectName("Component");

        QHBoxLayout* h = new QHBoxLayout();

        //color
        {
            QWidget* right = new QWidget();
            QBoxLayout* vertical = new QBoxLayout(QBoxLayout::TopToBottom, nullptr);
            right->setLayout(vertical);

            setColor(*vertical, mResource.mTextStyle.hover);
            h->addWidget(right);
        }

        hover->setContentLayout(*h);
        vBox->insertWidget(vBox->count(), hover, 0, Qt::AlignTop);
    }

    //Press style
    {
        Editor::Widget::WidgetCollapseUI* hover = new Editor::Widget::WidgetCollapseUI(mSettings, QIcon(mSettings.value("clickUIStyle").toString()), mSettings.value("clickUIStyleTitle").toString());
        hover->setObjectName("Component");

        QHBoxLayout* h = new QHBoxLayout();

        //color
        {
            QWidget* right = new QWidget();
            QBoxLayout* vertical = new QBoxLayout(QBoxLayout::TopToBottom, nullptr);
            right->setLayout(vertical);

            setColor(*vertical, mResource.mTextStyle.press);
            h->addWidget(right);
        }


        hover->setContentLayout(*h);
        vBox->insertWidget(vBox->count(), hover, 0, Qt::AlignTop);
    }
}

WidgetDoubleSpinBox* WidgetStyleUITextButton::setSpinBox(std::string pText, int& pAxisVal, QGridLayout* pLayout, int row, int column)
{
    pLayout->addWidget(new QLabel(pText.c_str()), row, column, Qt::AlignLeft);
    pLayout->addWidget(new QLabel(" "), row, column + 1, Qt::AlignRight);


    WidgetDoubleSpinBox* spinBox = new WidgetDoubleSpinBox(pAxisVal, -100, 100, 1);
    pLayout->addWidget(spinBox, row, column + 2, 1, 4, Qt::AlignLeft);

    return spinBox;
}

WidgetDoubleSpinBox* WidgetStyleUITextButton::setSpinBoxF(std::string pText, float& pAxisVal, QGridLayout* pLayout, int row, int column)
{
    pLayout->addWidget(new QLabel(pText.c_str()), row, column, Qt::AlignLeft);
    pLayout->addWidget(new QLabel(" "), row, column + 1, Qt::AlignRight);


    WidgetDoubleSpinBox* spinBox = new WidgetDoubleSpinBox(pAxisVal, 0, INT64_MAX, 1);
    pLayout->addWidget(spinBox, row, column + 2, 1, 4, Qt::AlignLeft);

    return spinBox;
}

void WidgetStyleUITextButton::setTexture(QBoxLayout& widLayout, Editor::UI::UIStyle& style)
{
    // Model name
    QLabel* mModelName = new QLabel(style.pTextPath);
    widLayout.addWidget(mModelName);

    // Model path
    WidgetDropUI* mModelPath = new WidgetDropUI(*mModelName);
    mModelPath->setText(style.pTextPath);
    mModelPath->setReadOnly(true);
    widLayout.addWidget(mModelPath);
    connect(mModelPath, &WidgetDropUI::textChanged, this, [&style, mModelPath]
    {
        style.pTextPath = mModelPath->text();
    });

    const QIcon folderIcon(mSettings.value("FolderIcon").toString());
    QAction* browseAction = new QAction(folderIcon, mSettings.value("FolderTip").toString());
    mModelPath->addAction(browseAction, QLineEdit::TrailingPosition);
    connect(browseAction, &QAction::triggered, this, [this, mModelName, mModelPath, &style]
        {
            QString Defaultpath = service(Data::ProjectLocation).mFolder;

            std::string filter = "Texture Files (*.png *.jpg *.jpeg)";
            QFileDialog dialog(this, tr("Open Texture"), Defaultpath, tr(filter.c_str()));
            connect(&dialog, &QFileDialog::directoryEntered, [&dialog, Defaultpath, this](QString path)
                {
                    if (!path.contains(Defaultpath))
                        dialog.setDirectory(Defaultpath);
                });

            if (dialog.exec() != QDialog::Accepted)
                return;

            QString fileName = dialog.selectedFiles().first();
            if (!fileName.isEmpty())
            {
                QFileInfo info(fileName);

                mModelName->setText(info.baseName());
                mModelPath->setText(fileName);

                style.pTextPath = fileName;
            }
        });
}

void WidgetStyleUITextButton::setColor(QBoxLayout& widLayout, Editor::UI::UIStyle& style)
{
    QColorDialog* dialog = new QColorDialog(style.color);
    dialog->setOption(QColorDialog::NoButtons, true);
    dialog->setOption(QColorDialog::ShowAlphaChannel, true);
    dialog->setWindowFlags(Qt::Widget);
    connect(dialog, &QColorDialog::currentColorChanged, this, [this, dialog, &style]
    {
        style.color = dialog->currentColor();
    });

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

    widLayout.addWidget(dialog);
}