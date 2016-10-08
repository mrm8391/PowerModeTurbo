#include "ofApp.h"

//vector of all the lines of characters
vector<string> lines;
//int indicating the current active line
int activeLine;
//indicators of the active column in the active string.
//effective is when a line is moved down but the string is smaller.
int activeColumn, effectiveActiveColumn;
float spaceBetweenLines = 20.f;
float xStart = 300.f;

//--------------------------------------------------------------
void ofApp::setup(){

	ofTrueTypeFont::setGlobalDpi(72);
	consolas.load("consola.ttf", 14, true, true);
	consolas.setLineHeight(18.0f);
	consolas.setLetterSpacing(1.037);

	activeLine = 0;
	lines = vector<string>();
	lines.push_back("I");
	lines.push_back("love");
	lines.push_back("dogs");
}

//--------------------------------------------------------------
void ofApp::update(){

}

void changeActiveLine(BOOL inc) {
	if (inc) {
		if (activeLine < lines.size() - 1)
			activeLine++;
	}
	else {
		if (activeLine > 0)
			activeLine--;
	}
}

//--------------------------------------------------------------
void ofApp::draw(){

	ofSetColor(0);
	consolas.drawString(lineStr, 422, 92);

	
	ofPushMatrix();
	ofRectangle bounds = consolas.getStringBoundingBox(lines.at(activeLine), 0, 0);
	ofTranslate(ofGetScreenWidth() / 16, ofGetScreenHeight() / 2, 0);
	
	//variable that sets how much the lines grow
	int growConstant = 4;

	//set in a variable so that all lines scale the same way
	float currentTime = ofGetElapsedTimef();
	ofScale(2.0 + sin(currentTime) / growConstant, 2.0 + sin(currentTime) / growConstant, 1.0);

		for (int i = 0; i < lines.size(); i++) {
			float yFromCenter = (i - activeLine) * spaceBetweenLines;
			consolas.drawString(lines.at(i),0,yFromCenter-bounds.height/2);
			ofLog() << yFromCenter - bounds.height / 2;
		}
		
	ofPopMatrix();
	
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
	switch (key) {
	case OF_KEY_RETURN:
		lines.push_back("");
		changeActiveLine(TRUE);
	break;
	case OF_KEY_DEL:
		break;
	case OF_KEY_BACKSPACE:
		if (lines.at(activeLine).size() == 0) {
			if (lines.size() > 0) {
				lines.erase(lines.begin() + activeLine);
				changeActiveLine(FALSE);
			}
		}
		else {
			lines.at(activeLine).pop_back();
		}
		break;
	case OF_KEY_UP:
		changeActiveLine(FALSE);
		break;
	case OF_KEY_DOWN:
		changeActiveLine(TRUE);
		break;
	default:
		lines.at(activeLine).push_back(char(key));
		break;
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
