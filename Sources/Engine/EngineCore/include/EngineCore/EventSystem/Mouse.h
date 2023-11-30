#pragma once
#include "Maths/FVector2.h"
#include "Maths/FVector3.h"
#include "Maths/FMatrix4.h"
namespace EngineCore::EventSystem
{
	struct Mouse
	{
		struct Key
		{
			bool mPressed = false;
			bool mRelease = !mRelease;
		};
		
		bool mCursorHidden = false;
		bool mManuelSetPos = false;
		Maths::FVector2 mPosition;
		Maths::FVector2 mOldPosition;
		Maths::FVector2 mOldPosLua;
		Maths::FVector2 mStartingPos;
		Key mLeft;
		Key mRight;
		float mScroll = 0;

		Maths::FVector2 getPositionDifference();
		Maths::FVector2 getPositionDifferenceLua(Maths::FVector2& pScreenCenter);
		bool isLeftPressed();
		bool isRightPressed();
		bool isScrolling();
		void setCursorState(bool pHiding);

		void updatePos(int pX, int pY);
	};
}