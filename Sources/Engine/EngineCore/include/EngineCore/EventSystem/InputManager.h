#pragma once
#include "EngineCore/EventSystem/Event.h"
#include "EngineCore/EventSystem/Key.h"
#include <map>
#include "EngineCore/EventSystem/Mouse.h"

namespace EngineCore::EventSystem
{
	
	class InputManager
	{
	public:
		Mouse mMouse;

		std::map<std::pair<Key, KState>, Event<>> mActions;
		std::map<Key, std::pair<bool, Event<>>> mContinuousAction;

		void bindActionCallBack(KState pState, Key pKey, std::function<void()> pAction);
		void receiveInput(KState pState, Key pKey);
		void receiveMouseInput(KState pState, MouseKey pKey);
		void receiveMouseScroll(double pRoll);
		
		void bindContinuousActionCallBack(Key pKey, std::function<void()> pAction);
		void processInput();
		void processMousePosition();
	};
}