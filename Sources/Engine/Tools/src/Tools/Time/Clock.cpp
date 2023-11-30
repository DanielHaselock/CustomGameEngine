
#include "Tools/Time/Clock.h"

using namespace Tools::Time;

float Clock::getFramerate()
{
	return 1.0f / (mDeltaTime);
}

float Clock::getDeltaTime()
{
	return mDeltaTime * mTimeScale;
}

float Clock::getDeltaTimeUnscaled()
{
	return mDeltaTime;
}

float Clock::getTimeSinceStart()
{
	return mTimeSinceStart;
}

float Clock::getTimeScale()
{
	return mTimeScale;
}

void Clock::scale(float p_coeff)
{
	mTimeScale *= p_coeff;
}

void Clock::setTimeScale(float p_timeScale)
{
	mTimeScale = p_timeScale;
}

void Clock::initialize()
{
	mDeltaTime = 0.0f;

	startTime = std::chrono::steady_clock::now();
	mCurrentTime = startTime;
	mLastTime = startTime;

	mInitialized = true;
}

void Clock::update()
{
	mLastTime = mCurrentTime;
	mCurrentTime = std::chrono::steady_clock::now();
	mElapsed = mCurrentTime - mLastTime;

	if (mInitialized)
	{
		mDeltaTime = mElapsed.count() > 0.1 ? 0.1f : static_cast<float>(mElapsed.count());
		mTimeSinceStart += mDeltaTime * mTimeScale;
	}
	else
		initialize();
}