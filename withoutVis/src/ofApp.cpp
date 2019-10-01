#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
	ofSetVerticalSync(true);
	ofSetBackgroundAuto(false);
	ofBackground(0, 0, 0);
	ofEnableAlphaBlending();
	ofSetFullscreen(false);
	ofSetFrameRate(120);

	mFullScreen = false;

	//withoutfile
	loadWithoutFile();

	kParticles = mWithoutCSV.getNumRows() -1; //1024
	float padding = 20;
	float maxVelocity = .07; //.5
	for (int i = 0; i < kParticles*10; i++) {
		float x = ofRandom(padding, ofGetWidth() - padding);
		float y = ofRandom(padding, ofGetHeight() - padding);
		float xv = ofRandom(-maxVelocity, maxVelocity);
		float yv = ofRandom(-maxVelocity, maxVelocity);
		Particle particle(x, y, xv, yv);

		if (i < mWithoutCSV.getNumRows() - 1) {
			std::string firstName = mWithouts.at(i)->getFirstName();
			particle.setName(firstName);
			particle.setAllocate(1);
		}
		else {
			particle.setName(" ");
		}
		particleSystem.add(particle);
	}

	particleSystem.setTimeStep(1);

	ofBackground(0, 0, 0);

	//fonts
	mWithoutImg.load("cw_logo reduced-white_1920x1080.png");
	ofTrueTypeFont::setGlobalDpi(72);
	mWithoutFontSmall.load("AmsiPro-Ultra.ttf", 24, true, true);
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

	//timer input
	mTimerForce = Timer::create(400);
	activateForce = false;

}

//--------------------------------------------------------------
void ofApp::update(){
	particleSystem.setupForces();

	// apply per-particle forces
	for (int i = 0; i < particleSystem.size(); i++) {
		Particle& cur = particleSystem[i];
		// global force on other particles
		if (i < mWithoutCSV.getNumRows() - 1) {
			particleSystem.addRepulsionForce(cur, 95, 0.12);
		}
		else {
			particleSystem.addRepulsionForce(cur, 5, 0.12);
		}
		// forces on this particle
		cur.bounceOffWalls(-200, -200, ofGetWidth()+200, ofGetHeight()+200);
		cur.addDampingForce();
	}
	// single global forces
	particleSystem.addAttractionForce(ofGetWidth() / 2, ofGetHeight() / 2, 1400, 0.017);
	particleSystem.addRepulsionForce(mouseX, mouseY, 100, 4);

	//without center
	particleSystem.addRepulsionForce(ofGetWidth() / 2.0, ofGetHeight() / 2.0, 325, 3.2);

	if (activateForce && mTimerForce->isActivate() ) {
		particleSystem.addRepulsionForce(ofGetWidth() / 2.0, ofGetHeight() / 2.0, 380, 3.3);
	}

	if (mTimerForce->isFinished()) {
		//mTimerForce->reset();
		//mTimerForce->activate();
		mTimerForce->disable();
		activateForce = false;
	}

	//udp msg
	updateUdpMsg();

	//update
	particleSystem.update();

	//update timer
	for (auto & withoutname : mWithouts) {
		withoutname->mTimer->update();
	}

	mTimerForce->update();

}

//--------------------------------------------------------------
void ofApp::updateUdpMsg() {
	char udpMessage[100000];
	mUdpConnection.Receive(udpMessage, 100000);
	string message = udpMessage;

	if (message != "") {
		ofLog() <<"add: "<<message << std::endl;
		std::cout << message << std::endl;
		for (auto & withoutname : mWithouts) {
			std::string name = withoutname->getFirstName();
			if (message.compare(name) == 0) {
				particleSystem.addRepulsionForce(ofGetWidth() / 2, ofGetHeight() / 2, 300, 5);
				withoutname->incHit();

				mTimer->activate();

				withoutname->mTimer->reset();
				withoutname->mTimer->activate();

				//save value

				currentWithout = message;

			}
		}
		//update visualization when the msg is received
	}


	if (message != "") {
		//std::cout << message.substr(0, 1) << std::endl;
		if (message.substr(0, 1).compare("s") == 0) {
			std::string newMsg = message.substr(2, message.size());

			std::vector < std::string > gesturesStr = tokenizeVec(newMsg, (char)'g');

			int j = 0;
			cout << newMsg << std::endl;
			WithoutRef withoutGesture = Without::create();
			for (auto & ges : gesturesStr) {
				std::vector<std::string> gestures = tokenizeVec(ges, ' ');
				//std::cout << j << std::endl;
				withoutGesture->initGesture();
				for (int i = 0; i < gestures.size() / 2.0; i++) {

					int x = i * 2 + 0;
					int y = i * 2 + 1;

					float xpos = stof(gestures.at(x));
					float ypos = stof(gestures.at(y));

					//std::cout << xpos << " " << ypos << std::endl;
					withoutGesture->addPoint(glm::vec2(xpos, ypos));
					//add gestures point


				}
				//add gestures to
				withoutGesture->endGesture();
				//copy without
				j++;
			}

			withoutGesture->setFirstName(currentWithout);
			withoutGesture->generatFbo();
			mWithoutHistory.push_back(withoutGesture);

			for (int i = 0; i < particleSystem.size(); i++) {
				if (particleSystem[i].allocated == -1) {
					particleSystem[i].mFboBatch = withoutGesture->getFbo();
					particleSystem[i].setName(currentWithout);
					particleSystem[i].allocated = 2;
					particleSystem[i].cx = withoutGesture->getMid().x;//  particleSystem[i].mFboBatch.getWidth() / 2.0;
					particleSystem[i].cy = withoutGesture->getMid().y;// particleSystem[i].mFboBatch.getHeight() / 2.0;
					std::cout << "Fbo added" << currentWithout<< std::endl;
					break;
				}
			}

			//force
			activateForce = true;
			mTimerForce->reset();
			mTimerForce->activate();


		}
	}




}

std::string ofApp::tokenizeStr(std::string const &in, char sep) {
	std::string::size_type b = 0;
	std::string result;

	while ((b = in.find_first_not_of(sep, b)) != std::string::npos) {
		auto e = in.find_first_of(sep, b);
		result+=in.substr(b, e - b);
		b = e;
	}
	return result;
}


std::vector<std::string> ofApp::tokenizeVec(std::string const &in, char sep) {
	std::string::size_type b = 0;
	std::vector<std::string> result;

	while ((b = in.find_first_not_of(sep, b)) != std::string::npos) {
		auto e = in.find_first_of(sep, b);
		result.push_back(in.substr(b, e - b));
		b = e;
	}
	return result;
}

//--------------------------------------------------------------
void ofApp::draw(){
	ofFill();
	ofSetColor(ofColor(0, 0, 0, 15));
	ofDrawRectangle(0, 0, ofGetWidth(), ofGetHeight());

	if (mDrawType == 0) {
		drawParticles();
	}
	if (mDrawType == 1) {
		ofSetColor(255, 255, 255);
		drawWithout();
	}

	if (!mFullScreen) {
		ofDrawBitmapString(ofToString(kParticles) + "k particles", 32, 32);
		ofDrawBitmapString(ofToString((int)ofGetFrameRate()) + " fps", 32, 52);
	}

	//draw withouth
	mWithoutImg.draw(ofGetWidth() / 2.0 - 1920 * 0.15, ofGetHeight() / 2.0 - 1080 * 0.15, 1920 * 0.3, 1080 * 0.3);

	//draw history
	//for (auto & gestures : mWithoutHistory) {
		//gestures->drawGestures();
	//}

	//for (auto & gestures : mWithoutHistory) {
	//	gestures->drawGestureFbo();
	//}

	//draw connecting
}

//--------------------------------------------------------------
void ofApp::drawParticles() {
	//ofFill();
	particleSystem.draw();



	
	for (int i = 0; i < particleSystem.size(); i++) {
		Particle& cur = particleSystem[i];
		if (i < mWithoutCSV.getNumRows() - 1) {

			//color 
			ofColor color = mWithouts.at(i)->getColor();

			float lightV = ofClamp(mWithouts.at(i)->mTimer->getValue(), 0.0, 1.0);
			ofSetColor(lightV * 255);

			//color
			ofNoFill();
			ofPushMatrix();
			ofTranslate(cur.x, cur.y);
			ofScale(1.0 + mWithouts.at(i)->getHits() / 10.0);
			std::string firstName = mWithouts.at(i)->getFirstName();
			mWithoutFontSmall.drawString(firstName, 0, 0);
			ofRectangle rect = mWithoutFontSmall.getStringBoundingBox(firstName, 0, 0);
			ofDrawRectangle(rect);
			ofPopMatrix();

			cur.cx = rect.getCenter().x;
			cur.cy = rect.getCenter().y;

		}

		//draw fbo drawing
		if (cur.allocated == 2) {
			ofPushMatrix();
			ofTranslate(cur.x, cur.y);
			ofScale(0.15, 0.15, 0.15);
			cur.mFboBatch.draw(0, 0);
			ofPopMatrix();
		}
	}


		//draw connecting lines
	for (int i = 0; i < mWithoutCSV.getNumRows() - 1; i++) {
		Particle& cur = particleSystem[i];
		for (int j = mWithoutCSV.getNumRows(); j < particleSystem.size(); j++) {
			Particle& cur2 = particleSystem[j];
			if (cur2.name.compare(cur.name) == 0) {
				ofSetColor(255, 255, 255, 30);
				ofDrawLine(cur.x + cur.cx, cur.y + cur.cy, cur2.x + cur2.cx, cur2.y + cur2.cy);

				ofPushMatrix();
				ofScale(0.15, 0.15, 0.15);
				ofDrawLine(cur.x + cur.cx, cur.y + cur.cy, cur2.x + cur2.cx, cur2.y + cur2.cy);
				ofPopMatrix();
			}

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
	settings.receiveOn(IP_PORT);
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

	if (key == 'f') {
		ofToggleFullscreen();
		mFullScreen = !mFullScreen;
	}

	if (key == 'a') {
		for (int i = 0; i < mWithouts.size(); i++) {
			mWithouts.at(i)->incHit();
			mWithouts.at(i)->mTimer->reset();
			mWithouts.at(i)->mTimer->activate();
		}
	}

	if (key == 'r') {
		int randomWithout = (int)ofRandom(mWithouts.size() - 1);

		mWithouts.at(randomWithout)->incHit();
		mWithouts.at(randomWithout)->mTimer->reset();
		mWithouts.at(randomWithout)->mTimer->activate();

		//particleSystem.addRepulsionForce(ofGetWidth() / 2, ofGetHeight() / 2, 300, 5);

		std::cout << "random hit" << std::endl;
	}

	if (key == 'z') {
		activateForce = true;
		mTimerForce->reset();
		mTimerForce->activate();
		std::cout << "reset timer" << std::endl;
	}

	if (key == 'v') {
		std::string message = "s 10.1 10.2 10.3 10.4 10.5 10.6 g 11.2 11.3 11.4 11.5";
		std::cout << message.substr(0, 1) << std::endl;
		if (message.substr(0, 1).compare("s") == 0) {
			std::string newMsg = message.substr(2, message.size());

			std::vector < std::string > gesturesStr = tokenizeVec(newMsg, (char)'g');

			int j = 0;
			for (auto & ges : gesturesStr) {
				std::vector<std::string> gestures = tokenizeVec(ges, ' ');
				//std::cout << j << std::endl;
				for (int i = 0; i < gestures.size() / 2.0; i++) {

					int x = i * 2 + 0;
					int y = i * 2 + 1;


					float xpos = stof(gestures.at(x));
					float ypos = stof(gestures.at(y));

					//std::cout << xpos << " " << ypos << std::endl;

				}
				j++;
			}
			
		}
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
