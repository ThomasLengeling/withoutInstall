#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup() {

	//setup 
	ofSetVerticalSync(true);
	ofSetBackgroundAuto(false);
	ofBackground(0, 0, 0);
	ofEnableAlphaBlending();
	ofSetFullscreen(false);

	//fonts
	mWithoutImg.load("cw_logo reduced-white_1920x1080.png");

	mWithoutLines.load("dashed-01.png");
	ofTrueTypeFont::setGlobalDpi(72);
	mWithoutFont.load("AmsiPro-Black.ttf", 275, true, true);

	//safe file
	ofSetLogLevel("ofxCsv", OF_LOG_VERBOSE); // See what's going on inside.


	loadWithoutFile();

	//counter without
	mCSVRowCounter = 0;

	//timer
	mWithoutTime = 5000; //
	mTimer = Timer::create(mWithoutTime);
	mTimer->activate();

	//setup 
	setupGestureDetection();

	//UDP sender
	setupCommunication();
}

//--------------------------------------------------------------
void ofApp::update(){

	mTimer->update();

	//update en reset timer
	if (mTimer->isFinished()) {
		mTimer->reset();
		mTimer->activate();



		//reset 
		if (mAddGesture) {
			std::string currentName = mWithouts.at(mCSVRowCounter)->getFirstName();
			mUDPConnection.Send(currentName.c_str(), currentName.length());
			mAddGesture = false;
		}

		mCSVRowCounter++;

		//finish going through all the withouts that are in the CSV file
		if (mCSVRowCounter >= mWithoutCSV.getNumRows() - 1) {
			ofLog() << "Done";
			mCSVRowCounter = 0;
			saveWithouths();
			for (auto & withouts : mWithouts) {
				withouts->clear();
			}
		}
	}

	//detect if the mouse movement is inside of the rectangle
	if (ofGetMouseX() > mRectGesture.getMinX() && ofGetMouseY() > mRectGesture.getMinY() && ofGetMouseX() < mRectGesture.getMaxX() && ofGetMouseY() < mRectGesture.getMaxY()) {
		mInsideGesture = true;
		mTimer->reset();
		mTimer->activate();
		mLightTimer->activate();
	}
	else {
		mLightTimer->reset();
		mInsideGesture = false;
	}
}

void ofApp::saveWithouths() {
	ofJson withoutsAll;
	for (auto & withouts : mWithouts) {
		std::string name = withouts->getFirstName();
		ofJson jsonWithouts = withouts->getWithoutJson();
		jsonWithouts.push_back(name);
		withoutsAll.push_back(jsonWithouts);
	}

	std::string month = to_string(ofGetMonth());
	std::string day = to_string(ofGetDay());
	std::string hour = to_string(ofGetHours());
	std::string minute = to_string(ofGetMinutes());
	std::string millis = to_string(ofGetSystemTimeMillis());
	ofSaveJson(" without_" + month + "_" + day + "_" + hour + "_" + minute + "_" + millis+".json", withoutsAll);
	ofLog() << "save withouts JSON file";

}

//--------------------------------------------------------------
void ofApp::draw(){
	ofFill();
	ofSetColor(ofColor(0, 0, 0));
	ofDrawRectangle(0, 0, ofGetWidth(), ofGetHeight());

	//draw without
	ofSetColor(ofColor(255, 255, 255));
	mWithoutImg.draw(0, 0);

	//draw line
	ofSetColor(ofColor(255, 255, 255));

	mWithoutFont.drawString("_ _ _ _ _ _ _ _ _ _ _", 120, 940);
	mWithoutLines.draw(0, 1080 - 200);;//, 1920, 1080);

	//draw the without words
	ofPushMatrix();
	ofSetColor(255, 255, 255);
	ofRotate(-1);
	mWithoutFont.drawString(mWithouts.at(mCSVRowCounter)->getFirstName(), 120, 380);
	mWithoutFont.drawString("", 120, 890);//mWithouts.at(mCSVRowCounter)->getFirstName(), 160, 890);
	ofPopMatrix();

	drawGestureBox();

}
//--------------------------------------------------------------
void ofApp::drawGestureBox() {

	mLightTimer->update();

	//draw bouding box
	if (mInsideGesture) {
		ofNoFill();
		ofSetLineWidth(1);
		ofSetColor(255, 255, 255, 200);
		//ofDrawRectangle(mRectGesture);

	}

	//when there is no gestures detection
	if (!mInsideGesture) {
		ofNoFill();
		ofSetLineWidth(2.50);
		ofSetColor(120, 120, 120);
		//ofDrawRectangle(mRectGesture);
		ofSetLineWidth(1);
	}

	//light effect around the box
	float lightV = mLightTimer->getValue();
	//cout << lightV << std::endl;
	ofFill();
	ofSetColor(255, 255, 255, ofClamp(1.0 - lightV, 0, 1.0)*255);
	ofDrawRectangle(mRectGesture);

	ofNoFill();
	ofSetColor(255, 255, 255, 255);
	mWithouts.at(mCSVRowCounter)->drawGestures();
	mWithouts.at(mCSVRowCounter)->drawCurrentGesture();
}
//--------------------------------------------------------------
void ofApp::loadWithoutFile(){

	mWithouts.clear();
	ofLog() << mWithouts.size() << std::endl;
	if (mWithoutCSV.load(WITHOUT_FILE)) {
		//csv.trim(); // Trim leading/trailing whitespace from non-quoted fields.

		// Like with C++ vectors, the index operator is a quick way to grab row
		// & col data, however this will cause a crash if the row or col doesn't
		// exist, ie. the file didn't load.
		ofLog() << "Print out a specific CSV value";
		ofLog() << mWithoutCSV[0][0] <<" "<<mWithoutCSV[1][0]; //1st CITIES
		// also you can write...
		ofLog() << mWithoutCSV[0].at(1);

		ofLog() << "Rows: " << mWithoutCSV.getNumRows() << " Cols: " << mWithoutCSV.getNumCols();

		// or you can get the row itself...
		ofxCsvRow row = mWithoutCSV[1];
		ofLog() << row.getString(0) << " "<< row.getString(1); //CITIES WITHOUT
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
	ofLog() << mWithouts.size() << std::endl;
}
//--------------------------------------------------------------
void ofApp::setupGestureDetection() {
	mRectGesture = ofRectangle(120, 650, 1740, 340);

	mLightTimer = Timer::create(150);
	//mLightTimer->activate();
}

//--------------------------------------------------------------
void ofApp::setupCommunication() {
	//create the socket and set to send to 127.0.0.1:11999
	ofxUDPSettings settings;
	settings.sendTo("169.254.196.41", IP_PORT);
	settings.blocking = false;

	mUDPConnection.Setup(settings);
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
	if (key == 's') {

	}

	if (key == 'f') {
		ofToggleFullscreen();
	}
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){
	

	//detect if the mouse movement is inside of the rectangle
	if (x > mRectGesture.getMinX() && y > mRectGesture.getMinY() && x < mRectGesture.getMaxX() && y < mRectGesture.getMaxY()) {
		mInsideGesture = true;

		mTimer->reset();
		mTimer->activate();

		mLightTimer->activate();

	}
	else {
		mLightTimer->reset();
		mInsideGesture = false;
	}
}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

	if (x > mRectGesture.getMinX() && y > mRectGesture.getMinY() && x < mRectGesture.getMaxX() && y < mRectGesture.getMaxY()) {
		cout << x << " " << y << std::endl;
		mTimer->reset();
		mTimer->activate();

		mLightTimer->activate();
		mInsideGesture = true;
		mWithouts.at(mCSVRowCounter)->addPoint(glm::vec2(x, y));
	}
	else {
		mLightTimer->reset();
		mInsideGesture = false;
	}
}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){

	//only draw in bouding box of the second letter.
	if (x > mRectGesture.getMinX() && y > mRectGesture.getMinY() && x < mRectGesture.getMaxX() && y < mRectGesture.getMaxY()) {
		cout << x << " " << y << std::endl;
		mTimer->reset();
		mTimer->activate();

		mLightTimer->activate();
		mInsideGesture = true;
		mWithouts.at(mCSVRowCounter)->initGesture();
		mWithouts.at(mCSVRowCounter)->addPoint(glm::vec2(x, y));

		mAddGesture = true;

	}
	else {
		mLightTimer->reset();
		mInsideGesture = false;

	}


}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){
	if (x > mRectGesture.getMinX() && y > mRectGesture.getMinY() && x < mRectGesture.getMaxX() && y < mRectGesture.getMaxY()) {
		mWithouts.at(mCSVRowCounter)->endGesture();
	}
	else {
	}
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
	cout << "Got the message: " + msg.message << endl;
}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}
