#include "EngineCore/EventSystem/InputManager.h"
#include <windows.h>

using namespace EngineCore::EventSystem;

void InputManager::bindActionCallBack(KState pState, Key pKey, std::function<void()> pAction)
{
	mActions[std::make_pair(pKey, pState)].subscribe(pAction);
}

void InputManager::receiveInput(KState pState, Key pKey)
{
	if (mContinuousAction.find(pKey) != mContinuousAction.end())
		mContinuousAction[pKey].first = pState == KState::PRESS;

	std::pair pair = std::make_pair(pKey, pState);
	if (mActions.find(pair) == mActions.end())
		return;

	mActions[pair].dispatch();
}

void InputManager::receiveMouseInput(KState pState, MouseKey pKey)
{
	if (pKey == MouseKey::LEFT)
		mMouse.mLeft = { pState == KState::PRESS };
	else if (pKey == MouseKey::RIGHT)
		mMouse.mRight = { pState == KState::PRESS };

	mMouse.setCursorState(pState == KState::PRESS);
}

void InputManager::receiveMouseScroll(double pRoll)
{

	mMouse.mScroll = pRoll;
}

void InputManager::bindContinuousActionCallBack(Key pKey, std::function<void()> pAction)
{
	Event e;
	e.subscribe(pAction);
	mContinuousAction[pKey] = std::make_pair(false, e);
}

void InputManager::processInput()
{
	for (std::map<Key, std::pair<bool, Event<>>>::iterator it = mContinuousAction.begin(); it != mContinuousAction.end(); it++)
	{
		if (!it->second.first)
			continue;
		it->second.second.dispatch();
	}

	processMousePosition();
}

void InputManager::processMousePosition()
{
	POINT pos;
	GetCursorPos(&pos);
	mMouse.updatePos(pos.x, pos.y);
}