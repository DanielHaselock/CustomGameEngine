#pragma once
#include "Editor/Data/ActionIcon.h"

using namespace Editor::Data;

ActionIcon::ActionIcon() {};


void ActionIcon::init(const QString& pIcon, const QString& pIconActif, QAction* pAction)
{
	mIcon = QIcon(pIcon);
	mIconActif = QIcon(pIconActif);
	mAction = pAction;

	mAction->setIcon(mIcon);
}

void ActionIcon::setTips(const QString& pTips)
{
	mAction->setToolTip(pTips);
}

void ActionIcon::setDisabled(bool pState)
{
	mAction->setDisabled(pState);
}

void ActionIcon::switchIcon(const int idx)
{
	if (idx == 1)
		switchIcon(mIcon);
	else
		switchIcon(mIconActif);
}

void ActionIcon::switchIcon(const QIcon& pIcon)
{
	return mAction->setIcon(pIcon);
}