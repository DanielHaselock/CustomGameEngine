#pragma once

namespace EngineCore::EventSystem
{
	enum class KState
	{
		PRESS,
		RELEASE
	};

	enum class MouseKey
	{
		LEFT,
		RIGHT
	};

	enum class Key
	{
		UNKNOWN,
		A,
		B,
		C,
		D,
		E,
		F,
		G,
		H,
		I,
		J,
		K,
		L,
		M,
		N,
		O,
		P,
		Q,
		R,
		S,
		T,
		U,
		V,
		W,
		X,
		Y,
		Z,
		ESCAPE,
		_1,
		_2,
		_3,
		_4,
		_5,
		_6,
		_7,
		_8,
		_9,
		TAB,
		ENTER,
		LEFT_SHIFT,
		LEFT_CTRL,
		SPACE
	};
}