#pragma once

#include "ofMain.h"
#include "ofxNetwork.h"
#include "ofxDatGui.h"
#include "ofxCsv.h"


#include <iostream>
#include <map>
#include <vector>

#include "without.h"
#include "Timer.h"

#define WITHOUT_FILE "WITHOUTS.csv"
#define IP_PORT     32000

class ofApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();

		void keyPressed(int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void mouseEntered(int x, int y);
		void mouseExited(int x, int y);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);


		void loadWithoutFile();

		//CSV
		ofxCsv mWithoutCSV;
		int mCSVRowCounter;

		//img
		ofImage mWithoutImg;

		//img
		ofImage mWithoutLines;

		//without
		ofTrueTypeFont  mWithoutFont;


		//without manager
		std::vector<WithoutRef> mWithouts;
		void saveWithouths();
		bool mAddGesture;

		//global timer
		TimerRef  mTimer;
		float  mWithoutTime;

		//Gesture detection
		ofRectangle mRectGesture;
		bool mInsideGesture;

		TimerRef  mLightTimer;

		void drawGestureBox();

		void setupGestureDetection();


		// send commands
		ofxUDPManager mUDPConnection;
		std::string   mUDPIp;
		int           mUDPPort;
		void		  setupCommunication();
		
};
