#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
	ofSetVerticalSync(true);
	ofSetBackgroundAuto(false);
	ofBackground(0, 0, 0);
	ofEnableAlphaBlending();
	ofSetFullscreen(false);
	ofSetFrameRate(120);

	//withoutfile
	loadWithoutFile();

	kParticles = mWithoutCSV.getNumRows() -1; //1024
	float padding = 20;
	float maxVelocity = .07; //.5
	for (int i = 0; i < kParticles; i++) {
		float x = ofRandom(padding, ofGetWidth() - padding);
		float y = ofRandom(padding, ofGetHeight() - padding);
		float xv = ofRandom(-maxVelocity, maxVelocity);
		float yv = ofRandom(-maxVelocity, maxVelocity);
		Particle particle(x, y, xv, yv);

		if (i < mWithoutCSV.getNumRows() - 1) {
			std::string firstName = mWithouts.at(i)->getFirstName();
			particle.setName(firstName);
		}
		particleSystem.add(particle);
	}

	particleSystem.setTimeStep(1);

	ofBackground(0, 0, 0);

	//fonts
	mWithoutImg.load("cw_logo reduced-white_1920x1080.png");
	ofTrueTypeFont::setGlobalDpi(72);
	mWithoutFontSmall.load("AmsiPro-Ultra.ttf", 38, true, true);
	mWithoutFontBig.load("AmsiPro-Ultra.ttf", 192, true, true);

	//safe file
	ofSetLogLevel("ofxCsv", OF_LOG_VERBOSE); // See what's going on inside.


	//counter without
	mCSVRowCounter = 0;

	//timer
	mWithoutTime = 2000; //
	mTimer = Timer::create(mWithoutTime);
	mTimer->activate();

	//UDP
	setupCommunication();

	//change draw type depending on the visualization
	mDrawType = 0;

}

//--------------------------------------------------------------
void ofApp::update(){
	particleSystem.setupForces();

	// apply per-particle forces
	for (int i = 0; i < particleSystem.size(); i++) {
		Particle& cur = particleSystem[i];
		// global force on other particles
		particleSystem.addRepulsionForce(cur, 80, 0.1);
		// forces on this particle
		cur.bounceOffWalls(0, 0, ofGetWidth(), ofGetHeight());
		cur.addDampingForce();
	}
	// single global forces
	particleSystem.addAttractionForce(ofGetWidth() / 2, ofGetHeight() / 2, 1500, 0.01);
	particleSystem.addRepulsionForce(mouseX, mouseY, 100, 2);
	particleSystem.update();
}

//--------------------------------------------------------------
void ofApp::updateUdpMsg() {
	char udpMessage[100000];
	mUdpConnection.Receive(udpMessage, 100000);
	string message = udpMessage;
	
	if (message != "") {
		//update visualization when the msg is received
	}
}


//--------------------------------------------------------------
void ofApp::draw(){
	ofFill();
	ofSetColor(ofColor(0, 0, 0, 10));
	ofDrawRectangle(0, 0, ofGetWidth(), ofGetHeight());

	if (mDrawType == 0) {
		drawParticles();
	}
	if (mDrawType == 1) {
		drawWithout();
	}


	ofDrawBitmapString(ofToString(kParticles) + "k particles", 32, 32);
	ofDrawBitmapString(ofToString((int)ofGetFrameRate()) + " fps", 32, 52);
}

//--------------------------------------------------------------
void ofApp::drawParticles() {
	//ofFill();
	particleSystem.draw();
	for (int i = 0; i < particleSystem.size(); i++) {
		Particle& cur = particleSystem[i];
		if (i < mWithoutCSV.getNumRows() -1) {
			//std::string firstName = mWithouts.at(i)->getFirstName();
			//mWithoutFontSmall.drawString(firstName, cur.x, cur.y);
			//ofRectangle rect = mWithoutFontSmall.getStringBoundingBox(firstName, cur.x, cur.y);
			//ofDrawRectangle(rect);
		}
	}
}
//--------------------------------------------------------------
void ofApp::drawWithout() {
	ofSetColor(ofColor(255, 255, 255));
	mWithoutImg.draw(0, 0);

	//draw the without words
	ofNoFill();
	ofSetColor(255, 255, 255);
	mWithoutFontBig.drawString(mWithouts.at(mCSVRowCounter)->getFirstName(), 160, 380);

	//draw gestures
	ofNoFill();
	ofSetColor(255, 255, 255, 255);
	mWithouts.at(mCSVRowCounter)->drawGestures();
	mWithouts.at(mCSVRowCounter)->drawCurrentGesture();

}

//--------------------------------------------------------------
void ofApp::loadWithoutFile() {

	if (mWithoutCSV.load(WITHOUT_FILE)) {
		//csv.trim(); // Trim leading/trailing whitespace from non-quoted fields.

		// Like with C++ vectors, the index operator is a quick way to grab row
		// & col data, however this will cause a crash if the row or col doesn't
		// exist, ie. the file didn't load.
		ofLog() << "Print out a specific CSV value";
		ofLog() << mWithoutCSV[0][0] << " " << mWithoutCSV[1][0]; //1st CITIES
		// also you can write...
		ofLog() << mWithoutCSV[0].at(1);

		ofLog() << "Rows: " << mWithoutCSV.getNumRows() << " Cols: " << mWithoutCSV.getNumCols();

		// or you can get the row itself...
		ofxCsvRow row = mWithoutCSV[1];
		ofLog() << row.getString(0) << " " << row.getString(1); //CITIES WITHOUT
	}

	//creating the list for the without setences
	for (int i = 1; i < mWithoutCSV.getNumRows(); i++) {
		std::string firstName = mWithoutCSV[i][0];
		std::string secondName = mWithoutCSV[i][2];
		WithoutRef without = Without::create();
		without->setFirstName(firstName);
		without->setSecondName(secondName);
		mWithouts.push_back(without);
	}

	ofLog() << "DONE";
}

//--------------------------------------------------------------
void ofApp::setupCommunication() {
	//create the socket and bind to port 11999
	ofxUDPSettings settings;
	settings.receiveOn(PORT);
	settings.blocking = false;

	mUdpConnection.Setup(settings);
}
//--------------------------------------------------------------
void ofApp::keyPressed(int key){
	if (key == '0') {
		mDrawType = 0;
	}
	if (key == '1') {
		mDrawType = 1;
	}
}


//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}
