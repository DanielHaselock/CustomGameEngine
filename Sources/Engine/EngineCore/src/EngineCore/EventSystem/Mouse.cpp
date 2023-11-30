#include "EngineCore/EventSystem/Mouse.h"
#include <windows.h>

using namespace EngineCore::EventSystem;

Maths::FVector2 Mouse::getPositionDifference()
{
	Maths::FVector2 res = mPosition - mOldPosition;
	mPosition = mStartingPos;
	mOldPosition = mPosition;

	if (mCursorHidden)
		SetCursorPos(mStartingPos.x, mStartingPos.y);

	return res;
}

Maths::FVector2 EngineCore::EventSystem::Mouse::getPositionDifferenceLua(Maths::FVector2& pScreenCenter)
{
	mOldPosLua = pScreenCenter;
	
	POINT pos;
	GetCursorPos(&pos);
	Maths::FVector2 CurrentPosition = Maths::FVector2(pos.x, pos.y);

	Maths::FVector2 res = mPosition - mOldPosLua;

	SetCursorPos(pScreenCenter.x, pScreenCenter.y);

	return res;
}

bool Mouse::isLeftPressed()
{
	return mLeft.mPressed;
}

bool Mouse::isRightPressed()
{
	return mRight.mPressed;
}

bool Mouse::isScrolling()
{
	return mScroll != 0;
}

void Mouse::setCursorState(bool pState)
{
	if (mCursorHidden == pState)
		return;

	if (pState)
	{
		POINT pos;
		GetCursorPos(&pos);
		mPosition = mStartingPos = Maths::FVector2(pos.x, pos.y);
	}

	mCursorHidden = pState;
	ShowCursor(!pState);
}

void Mouse::updatePos(int pX, int pY)
{
	mOldPosition = mPosition;
	mPosition = Maths::FVector2(pX, pY);
}