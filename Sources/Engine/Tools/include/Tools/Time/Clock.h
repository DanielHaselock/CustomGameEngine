
#pragma once

#include <string>
#include <chrono>


namespace Tools::Time
{

	class Clock
	{
	public:
		
		void update();

		float getFramerate();

		float getDeltaTime();

		float getDeltaTimeUnscaled();

		float getTimeSinceStart();

		float getTimeScale();

		void scale(float p_coeff);

		void setTimeScale(float p_timeScale);

		void initialize();

		std::chrono::steady_clock::time_point startTime;
		std::chrono::steady_clock::time_point mLastTime;
		std::chrono::steady_clock::time_point mCurrentTime;
		std::chrono::duration<double> mElapsed;

		bool mInitialized = false;
		float mTimeScale = 1.0f;
		float mDeltaTime = 0.0f;
		float mTimeSinceStart = 0.0f;
	};
}