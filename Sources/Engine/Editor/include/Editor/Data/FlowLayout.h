#pragma once
#include <QtWidgets/qstyle.h>
#include <QtWidgets/qlayout.h>

namespace Editor::Data
{
	class FlowLayout : public QLayout
	{
		public:
            QList<QLayoutItem*> mItemList;
            int mHSpace;
            int mVSpace;

            FlowLayout(QWidget* pParent, int pMargin = -1, int pHSpacing = -1, int pVSpacing = -1);
            FlowLayout(int pMargin = -1, int pHSpacing = -1, int pVSpacing = -1);
            ~FlowLayout();

            void addItem(QLayoutItem* pItem) override;
            int horizontalSpacing() const;
            int verticalSpacing() const;
            Qt::Orientations expandingDirections() const override;
            bool hasHeightForWidth() const override;
            int heightForWidth(int) const override;
            int count() const override;
            QLayoutItem* itemAt(int pIndex) const override;
            QSize minimumSize() const override;
            void setGeometry(const QRect& pRect) override;
            QSize sizeHint() const override;
            QLayoutItem* takeAt(int pIndex) override;


            int doLayout(const QRect& pRect, bool pTestOnly) const;
            int smartSpacing(QStyle::PixelMetric pPm) const;
	};
}