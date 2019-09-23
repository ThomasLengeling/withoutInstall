#pragma once


#include "ofApp.h"


//-----------------------------------------------------------------
// FirstName Without SecondName
// Save JSON gesture position

class Without;
class GesturePos;

typedef std::shared_ptr<GesturePos> GesturePosRef;
typedef std::shared_ptr<Without> WithoutRef;


//-----------------------------------------------------------------
class GesturePos {
public:

	//init
	GesturePos() {
		mGesturePos.clear();
	}

	//create memory
	static GesturePosRef create() {
		return std::make_shared<GesturePos>();
	}

	//add point to vector
	void addPoint(glm::vec2  pos) {
		mGesturePos.push_back(pos);
	}

	//draw gestures
	void drawGesture() {
		ofBeginShape();
		for (auto & points : mGesturePos) {
			ofVertex(points.x, points.y);
		}
		ofEndShape();

		if (mGesturePos.size() > 0) {
			glm::vec2 last = mGesturePos.back();
			ofDrawCircle(last, 5);
		}
	}

	//save json data
	ofJson generateJSON() {
		ofJson stokesJson;
		for (auto & points : mGesturePos) {
			ofJson pt;
			pt["x"] = points.x;
			pt["y"] = points.y;
			stokesJson.push_back(pt);
		}
		return stokesJson;
	}

	//clear data
	void clear() {
		mGesturePos.clear();
	}




private:
	std::vector<glm::vec2> mGesturePos;

};


//-----------------------------------------------------------------

class Without {
public:

	Without() {
		firstActive  = false;
		secondActive = false;
		firstName    = "";
		secondName   = "";
	}

	static WithoutRef create() {
		return std::make_shared<Without>();
	}

	//set, gets, and change for the names
	void setFirstName(std::string & name) { firstName = name; }
	void setSecondName(std::string & name) { secondName = name; }

	std::string getFirstName() { return firstName; }
	std::string getSecondName() { return secondName; }

	void toggleFirst() { firstActive = !firstActive;}
	void toggleSecond() { secondActive = !secondActive; }

	//add point to gesture
	void addPoint(glm::vec2 pos) {
		if (mCurrentGesture != NULL) {
			mCurrentGesture->addPoint(pos);
		}
		else {
			std::cout << "not created" << std::endl;
		}
	}

	//-----------------------------------------------------------------
	// Init Gesture once mouse is pressed
	void initGesture() {
		//if mouse our touched add a gestures (word)

		mCurrentGesture = GesturePos::create();
		std::cout<<"Initialize Gestures "<<mGestures.size()<<std::endl;
	}

	// Finalize Gesture once mouse is release
	void endGesture() {
		if (mCurrentGesture != NULL) {
			mGestures.push_back(mCurrentGesture);
			
			//save json
			ofJson jsongestures = mCurrentGesture->generateJSON();
			mJSONGestures.push_back(jsongestures);
			std::cout << "Finalize Gestures " << mGestures.size() << std::endl;
		}
	}

	ofJson getWithoutJson() {
		return mJSONGestures;
	}

	// save all gestures
	void saveWithout() {
		//mJSONGestures.dump();
		int month = ofGetMonth();
		int day   = ofGetDay();
		int hour = ofGetHours();
		int minute = ofGetMinutes();
		int millis = ofGetSystemTimeMillis();
		ofSaveJson(firstName+" without_" + to_string(month) + "_" + to_string(day) + "_" + to_string(hour) + "_" + to_string(minute) + "_" + to_string(millis)+".json", mJSONGestures);
	}

	void drawCurrentGesture() {
		if (mCurrentGesture != NULL) {
			mCurrentGesture->drawGesture();
		}
	}

	void drawGestures() {
		for (auto & gesture : mGestures) {
			gesture->drawGesture();
		}
	}

	void clear() {
		mGestures.clear();
		mJSONGestures.clear();
		mCurrentGesture = GesturePos::create();
	}

private:

	// activation
	bool firstActive;
	bool secondActive;

	std::string firstName;
	std::string secondName;

	//json files
	std::vector<GesturePosRef> mGestures;
	GesturePosRef mCurrentGesture;
	ofJson mJSONGestures;
};


