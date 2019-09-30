#pragma once


#include "ofApp.h"

//-----------------------------------------------------------------
// Class manages simple timer
class Timer;

typedef std::shared_ptr<Timer> TimerRef;

class Timer {
public:

	//Timer
	Timer(float endTime) {
		mTimerReached = false;
		mStartTime    = ofGetElapsedTimeMillis();
		mEndTime      = endTime;
		mMapValue	    = 0.0;
		mEnable		    = false;
	}

	//create timer
	static TimerRef create(float endTime) {
		return std::make_shared<Timer>(endTime);
	}

	//reset
	void reset() {
		mTimerReached = false;                    // reset the timer
		mStartTime = ofGetElapsedTimeMillis();  // get the start time
		mEnable = false;
	}

	//update time
	void update() {
		if (mEnable) {
			float timer = ofGetElapsedTimeMillis() - mStartTime;
			mMapValue = ofMap(timer, 0.0, mEndTime, 0.0, 1.0);

			if (timer >= mEndTime && !mTimerReached) {
				mTimerReached = true;
				//ofMessage msg("Timer Reached");
				//ofSendMessage(msg);
			}
		}
	}

	//get the map values from start time and end time
	float getValue() { return mMapValue; }

	//disable times
	void activate() { mEnable = true; }
	void disable() { mEnable = false; }

	bool isFinished() { return mTimerReached; }
	bool isActivate() { return mEnable; }

private:

	// activation
	float mStartTime; // store when we start time timer
	float mEndTime; // when do want to stop the timer

	bool  mTimerReached;

	bool  mEnable;

	//value from 0 - 1 that coresponds the start to end time
	float mMapValue;

};
