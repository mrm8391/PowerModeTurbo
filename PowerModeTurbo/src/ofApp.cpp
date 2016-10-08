#include "ofApp.h"
#include <math.h>

typedef struct Point_ {
	float x, y;
} Point;

typedef struct ColorAlpha_ {
	int r, g, b, alpha;
} ColorAlpha;

//a vector of vectors of active particles from keystrokes.
//the first point is the origin of the explosion.
vector<vector<Point>> activeParticles;

//to go alongside the active particles
vector<vector<ColorAlpha>> particleColors;

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

int comboCount = 0;
float comboTimer = 10.f;
float timeSinceLastKey = 0.f;

ofTrueTypeFont player2, player2num;
float comboWidth;

//--------------------------------------------------------------
void ofApp::setup(){

	ofTrueTypeFont::setGlobalDpi(72);
	consolas.load("consola.ttf", 10, true, true);
	consolas.setLineHeight(12.0f);
	consolas.setLetterSpacing(1);

	player2.load("PressStart2P.ttf", 20, true, true);
	player2.setLineHeight(22.f);
	player2.setLetterSpacing(1);

	player2num.load("PressStart2P.ttf", 60, true, true);
	player2num.setLineHeight(22.f);
	player2num.setLetterSpacing(1);

	comboWidth = player2.getStringBoundingBox("Combo", 0, 0).width;

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
	consolas.drawString(lineStr, ofGetScreenWidth()/2, 100);

	//display combo meter
	float edgeOfScreen = ofGetScreenWidth();

	player2.drawString("Combo:",edgeOfScreen-2*comboWidth, 50);

	//display time left until combo resets
	float timerWidth = edgeOfScreen - 2 * comboWidth - 20;
	float percentageLeft = (ofGetElapsedTimef() - timeSinceLastKey) / comboTimer;

	ofDrawRectangle(ofRectangle(timerWidth, 200, (1-percentageLeft)*comboWidth*2, 20));

	ofRectangle bounds = player2num.getStringBoundingBox(std::to_string(comboCount), 0, 0);
	player2num.drawString(std::to_string(comboCount), timerWidth+comboWidth - bounds.width/2, 150);

	if (ofGetElapsedTimef() - timeSinceLastKey > comboTimer) {
		comboCount = 0;
		timeSinceLastKey = ofGetElapsedTimef();
	}

	
	//display strings
	ofPushMatrix();
		bounds = consolas.getStringBoundingBox(lines.at(activeLine), 0, 0);
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

	if (!activeParticles.empty())
		rendersExplosions();
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
		lines.insert(lines.begin()+activeLine+1,lines.at(activeLine).substr(activeColumn,lines.at(activeLine).size()));
		lines.at(activeLine).erase(activeColumn, lines.at(activeLine).size() - activeColumn);
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

	comboCount++;
	timeSinceLastKey = ofGetElapsedTimef();
	initExplosions(10, ofRandom(0,ofGetScreenWidth()),ofRandom(0, ofGetScreenHeight()));
}

void ofApp::initExplosions(int count,float xOrig,float yOrig) {
	vector<Point> particles;
	vector<ColorAlpha> colors;

	Point origin;
	origin.x = xOrig;
	origin.y = yOrig;
	particles.push_back(origin);

	ColorAlpha originCol;
	originCol.r = 0; originCol.g = 0; originCol.b = 0;
	originCol.alpha = 255;
	colors.push_back(originCol);

	for (int i = 1; i < count; i++) {
		ColorAlpha col;
		col.r = ofRandom(0, 255);
		col.g= ofRandom(0, 255);
		col.b= ofRandom(0, 255);
		col.alpha = 255;
		colors.push_back(col);

		//set x and y to be 1 unit away from the origin
		Point p;
		float angle = ofRandom(0, 360);
		p.x = 500;// sin(angle) / tan(angle);
		p.y = 500;// tan(angle)*cos(angle);
		particles.push_back(p);
	}

	activeParticles.push_back(particles);
	particleColors.push_back(colors);
}

void ofApp::rendersExplosions() {
	for(int h=0;h<activeParticles.size();h++){
		vector<Point> pointSet = activeParticles.at(h);
		Point origin = pointSet.at(0);

		//if alpha is zero, delete the particles
		if (particleColors.at(h).at(0).alpha <= 0) {
			activeParticles.erase(activeParticles.begin()+h);
			particleColors.erase(particleColors.begin() + h);
			h--;
		}

		particleColors.at(h).at(0).alpha--;

		for (int i = 1; i < pointSet.size(); i++) {

			/*float relX = pointSet.at(i).x - origin.x;
			float relY = pointSet.at(i).y - origin.y;
			float hyp = std::sqrt(std::pow(relX, 2) + std::pow(relY, 2));

			if (relX < 0) pointSet.at(i).x -= relY / hyp;
			else pointSet.at(i).x += relY / hyp;

			if (relY < 0) pointSet.at(i).y -= relX / hyp;
			else pointSet.at(i).y += relX / hyp;*/

			//display particle and decrease alpha to have it fade
			ofSetColor(particleColors.at(h).at(i).r, particleColors.at(h).at(i).g, particleColors.at(h).at(i).b, particleColors.at(h).at(i).alpha);
			ofDrawCircle(pointSet.at(i).x, pointSet.at(i).y, 5.f);
			//particleColors.at(h).at(i).alpha--;
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
