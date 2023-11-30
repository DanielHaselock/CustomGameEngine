#pragma once
#include <Editor/Widget/WidgetEditor.h>
#include <EngineCore/Service/ServiceLocator.h>
#include <QtNodes/NodeDelegateModel>
#include <QtNodes/NodeData>
#include <QtCore/qjsonarray.h>

namespace Editor::UI
{
    class DummyOut : public QtNodes::NodeData
    {
    public:
        DummyOut() {};
        QtNodes::NodeDataType type() const override { return QtNodes::NodeDataType{ "Dummy", "Dummy out" }; }
    };

	class UIStyle
	{
		public:
			QColor color = QColor(255, 255, 255, 255);
			QString pTextPath = "";
	};

	class UIResource : public QtNodes::NodeDelegateModel
	{
		public:
			QString mName;
			bool isHidden = false;
			int zOrder = 0;
			UIStyle hover;
			UIStyle press;
			UIStyle normal;


            bool resizable() const override { return true; }

			QJsonObject save() const override;

			void load(QJsonObject const& pData) override;

			QJsonObject saveUIStyle(const UIStyle* pStyle) const;

			void loadStyle(QJsonObject& jsonStyle, UIStyle& uiStyle);
	};
}