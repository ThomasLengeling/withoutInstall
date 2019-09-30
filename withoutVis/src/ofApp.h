#pragma once

#include "ofMain.h"
#include "ofxCsv.h"
#include "ofxNetwork.h"


#include "without.h"
#include "Timer.h"
#include "ParticleSystem.h"

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


		//Particle System
		int kParticles;
		ParticleSystem particleSystem;
		void drawParticles();
		

		//without
		void loadWithoutFile();
		ofxCsv mWithoutCSV;
		int mCSVRowCounter;
		std::vector<WithoutRef> mWithouts;

		//history
		std::string currentWithout;
		std::vector<WithoutRef> mWithoutHistory;



		//img
		ofImage mWithoutImg;

		//without
		ofTrueTypeFont  mWithoutFontSmall;
		ofTrueTypeFont  mWithoutFontBig;

		//draw without
		void drawWithout();

		//global timer
		TimerRef  mTimer;
		float  mWithoutTime;

		//communication
		void setupCommunication();
		void updateUdpMsg();
		ofxUDPManager mUdpConnection;

		std::string tokenizeStr(std::string const &in, char sep);

		std::vector<std::string> tokenizeVec(std::string const &in, char del);

		//visualization events
		int mDrawType;

		bool mFullScreen;

		//timer
		TimerRef mTimerForce;
		bool     activateForce;


};
