#pragma once

#include <QtDocking/DockWidget.h>
#include <QtCore/qsettings.h>
#include <QtWidgets/qboxlayout.h>
#include <memory>
#include <QtNodes/NodeDelegateModelRegistry>
#include <QtNodes/DataFlowGraphModel>
#include <QtNodes/DataFlowGraphicsScene>
#include <QtNodes/GraphicsView>

namespace Editor::Widget
{
	static int canvasWidth = 900;
	static int canvasHeight = 600;

	class Graphics : public QtNodes::GraphicsView
	{
		public:
			Graphics(QtNodes::BasicGraphicsScene* scene, QWidget* parent = Q_NULLPTR) : QtNodes::GraphicsView(scene, parent) { scale(0.83, 0.83); };
			void centerScene() override;
			void drawBackground(QPainter* painter, const QRectF& r) override;
			void showEvent(QShowEvent* event) override;
	};

	class WidgetUIApp : public ads::CDockWidget
	{
	public:
		QVBoxLayout* mLayout = new QVBoxLayout();
		QtNodes::DataFlowGraphModel mModel;
		QtNodes::DataFlowGraphicsScene* mScene = nullptr;
		Graphics* mView = nullptr;

		QString mCurrentUI;

		WidgetUIApp(QSettings& pSetting, QWidget* pParent = nullptr);
		void setStyle();
		std::shared_ptr<QtNodes::NodeDelegateModelRegistry> registerDataModels();

		bool eventFilter(QObject* object, QEvent* event) override;
		void dragEnterEvent(QDragEnterEvent* pEvent) override;
		void dragMoveEvent(QDragMoveEvent* event) override;
		void dropEvent(QDropEvent* pEvent) override;

		void saveUI();
		QJsonObject saveNode(QtNodes::NodeId const nodeId) const;
		void open(QString pPath);
		void close();

		void updateTextSize(QtNodes::NodeDelegateModel* data, QString text, float scale);
	};
}