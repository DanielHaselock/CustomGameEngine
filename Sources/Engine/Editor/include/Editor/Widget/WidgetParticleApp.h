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
	class Graph : public QtNodes::GraphicsView
	{
		public:
			Graph(QtNodes::DataFlowGraphicsScene* pScene) : QtNodes::GraphicsView(pScene)
			{
				setAcceptDrops(true);
				viewport()->setAcceptDrops(true);
			}

			void dragEnterEvent(QDragEnterEvent* pEvent) override
			{
				pEvent->ignore();
			}

			void dragMoveEvent(QDragMoveEvent* pEvent) override
			{
				pEvent->ignore();
			}

			void dropEvent(QDropEvent* pEvent) override
			{
				pEvent->ignore();
				printf("Drop\n");
			}
	};

	class WidgetParticleApp : public ads::CDockWidget
	{
	public:
		QVBoxLayout* mLayout = new QVBoxLayout();
		QtNodes::DataFlowGraphModel mModel;
		QtNodes::DataFlowGraphicsScene* mScene = nullptr;
		QtNodes::GraphicsView* mView = nullptr;

		QString mCurrentPath;

		WidgetParticleApp(QSettings& pSetting, QWidget* pParent = nullptr);
		void setStyle();
		std::shared_ptr<QtNodes::NodeDelegateModelRegistry> registerDataModels();

		bool eventFilter(QObject* object, QEvent* event) override;
		void dragEnterEvent(QDragEnterEvent* pEvent) override;
		void dragMoveEvent(QDragMoveEvent* event) override;
		void dropEvent(QDropEvent* pEvent) override;

		void saveParticle();
		void open(QString pPath);
		void close();
	};
}