#include "ofApp.h"

//vector of all the lines of characters
vector<string> lines;
//int indicating the current active line
int activeLine;
//indicators of the active column in the active string.
int activeColumn;
//have the cursor indicator blink every X seconds
float activeColumnTick = 0.5f;
//size of a character (should all be the same since it's a monospace font)
float widthOfChar = 0.f, heightOfChar = 0.f;

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
		}

		//draw the cursor
		if (int(currentTime / activeColumnTick) % 2 == 0) {

			//find height of a line
			bounds = consolas.getStringBoundingBox("G", 0, 0);
			heightOfChar = bounds.height;

			//find x position of the beginning of the current character by finding the
			//rect bound of the substring up to that point + 1. It is +1 since that is
			//the default spacing between characters.
			bounds = consolas.getStringBoundingBox(lines.at(activeLine).substr(0, activeColumn), 0, 0);
			widthOfChar = bounds.width+1;
			ofDrawLine(widthOfChar, 0, widthOfChar, -2 * heightOfChar);
		}
		
	ofPopMatrix();

	
}


void incrActiveLine(BOOL inc) {
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
void ofApp::keyPressed(int key){
	switch (key) {
	case OF_KEY_RETURN:
		lines.push_back("");
		incrActiveLine(TRUE);
		activeColumn = 0;
	break;
	case OF_KEY_DEL:
		break;
	case OF_KEY_BACKSPACE:
		if (activeColumn==0) {
			if (activeLine > 0) {
				activeColumn = lines.at(activeLine-1).size();
				lines.at(activeLine - 1).append(lines.at(activeLine));
				lines.erase(lines.begin() + activeLine);
				incrActiveLine(FALSE);
			}
		}
		else {
			activeColumn--;
			lines.at(activeLine).erase(lines.at(activeLine).begin() + activeColumn);
		}
		break;
	case OF_KEY_UP:
		incrActiveLine(FALSE);
		if (activeColumn > lines.at(activeLine).size())
			activeColumn = lines.at(activeLine).size();
		break;
	case OF_KEY_DOWN:
		incrActiveLine(TRUE);
		if (activeColumn > lines.at(activeLine).size())
			activeColumn = lines.at(activeLine).size();
		break;
	case OF_KEY_LEFT:
		if (activeColumn == 0) {
			if (activeLine > 0) {
				activeLine--;
				activeColumn = lines.at(activeLine).size();
			}
		}
		else {
			activeColumn--;
		}
		break;
	case OF_KEY_RIGHT:
		if (activeColumn == lines.at(activeLine).size()) {
			if (activeLine != lines.size()) {
				activeLine++;
				activeColumn = 0;
			}
		}
		else {
			activeColumn++;
		}
		break;
	default:
		lines.at(activeLine).insert(lines.at(activeLine).begin()+activeColumn,char(key));
		activeColumn++;
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
