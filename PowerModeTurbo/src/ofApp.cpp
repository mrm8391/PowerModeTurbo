#include "ofApp.h"
#include <math.h>
#include <locale>
#include <iostream>
#include <fstream>


typedef struct Point_ {
	float x, y, angle;
} Point;

typedef struct ColorAlpha_ {
	int r, g, b, alpha;
} ColorAlpha;

vector<ColorAlpha> solidColors = {ColorAlpha{255,0,255,255}, ColorAlpha{255,0,0,255},ColorAlpha{255,128,0,255},
ColorAlpha{255,255,0,255},ColorAlpha{0,255,0,255},ColorAlpha{0,0,255,255},ColorAlpha{128,0,255,255}};

int currentColor = 0;

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
float comboWidth, lightWidth;

std::locale loc("C");

vector<string> happyText = {"Psychedelic!", "WHOA", "Far out!", "Keep it up!", "C-C-C-COMBO", "Great!", "Quite.", "I'm getting dizzy...", "M-M-MONSTER KILL", "Top notch!", "Mo' combos, mo' problem", "Simply amazing", "Splendid!", "That keyboard's gonna hurt tomorrow", "I can't even type that fast!", "Keyboard wizard","Mama would be proud"};
string currentHappyLine = "";
Point hLine;
float hLineAlpha;
int lastHappyLine = 0;

string fileN = "";

ofApp::ofApp() :ofBaseApp() {

}

ofApp::ofApp(char *file) : ofBaseApp() {
	fileN = string(file);
}

//--------------------------------------------------------------
void ofApp::setup(){

	ofBackground(166, 166, 166);

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
	lightWidth = player2.getStringBoundingBox("Code in the LIGHT", 0, 0).width;

	activeLine = 0;
	lines = vector<string>();
	lines.push_back("");

	if(fileN.size()>0)
		readInFile(fileN);
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
	player2.drawString("Code in the LIGHT", edgeOfScreen-1.5*lightWidth, ofGetScreenHeight()-50);

	//display time left until combo resets
	float timerWidth = edgeOfScreen - 2 * comboWidth - 20;
	float percentageLeft = (ofGetElapsedTimef() - timeSinceLastKey) / comboTimer;

	ofDrawRectangle(ofRectangle(timerWidth, 200, (1-percentageLeft)*comboWidth*2, 20));

	ofRectangle bounds = player2num.getStringBoundingBox(std::to_string(comboCount), 0, 0);
	player2num.drawString(std::to_string(comboCount), timerWidth+comboWidth - bounds.width/2, 150);

	//display combo text
	if (comboCount % 10 == 0 && comboCount!=lastHappyLine) {
		drawHappyText();
		lastHappyLine = comboCount;
	}
	continueHappyText();

	if (ofGetElapsedTimef() - timeSinceLastKey > comboTimer) {
		comboCount = 0;
		timeSinceLastKey = ofGetElapsedTimef();
	}
	
	//display strings
	ofPushMatrix();
		ofSetColor(0);
		bounds = consolas.getStringBoundingBox(lines.at(activeLine), 0, 0);
		ofTranslate(ofGetScreenWidth() / 16, ofGetScreenHeight() / 2, 0);

		//variable that sets how much the lines grow
		int growConstant = 4;
	
		//set in a variable so that all lines scale the same way
		float currentTime = ofGetElapsedTimef();
		ofScale(2.0 + sin(currentTime) / growConstant, 2.0 + sin(currentTime) / growConstant, 1.0);

		for (int i = 0; i < lines.size(); i++) {
			string temp;
			if (i <= 9) temp = "000";
			else if (i <= 99) temp = "00";
			else if (i <= 999) temp = "0";
			else temp = "";

			temp.append(std::to_string(i));
			temp.append("| ");
			//temp.append(lines.at(i));

			float yFromCenter = (i - activeLine) * spaceBetweenLines;
			consolas.drawString(lines.at(i),0,yFromCenter-bounds.height/2);
			consolas.drawString(temp, -50, yFromCenter - bounds.height / 2);
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
		renderExplosions();

	if (comboCount > 99 && comboCount < 200) {

		int r = int(ofRandom(0, 255));
		int g = int(ofRandom(0, 255));
		int b = int(ofRandom(0, 255));
		ofSetColor(r, g, b);
		drawPowerMode();

		ofSetColor(r, g, b,128);
		renderShiftingTriangles();
		strobeEffectSlow();
	}
		

	if (comboCount > 199){

		float r = sin(ofGetElapsedTimef()) * 127 + 128;
		float g = sin(ofGetElapsedTimef() + 2) * 127 + 128;
		float b = sin(ofGetElapsedTimef() + 4) * 127 + 128;

		int color = 0;
		int red = int(r);// *(16 * 16 * 16 * 16);
		int green = int(g);// *(16 * 16);
		int blue = int(b);
		ofSetColor(red, green, blue, 64);
		ofFill();

		renderTrippySinThing();
		renderShiftingTriangles();

		red = 255 - red;
		green = 255 - green;
		blue = 255 - blue;
		ofSetColor(red, green, blue,255);
		ofFill();
		drawPowerModeSeizure();

	}

	if (comboCount > 499) {
		strobeEffect();
	}
}

void ofApp::drawHappyText() {
	currentHappyLine= happyText.at(std::rand() % happyText.size());
	ofRectangle bounds = player2.getStringBoundingBox(currentHappyLine,0,0);
	hLine.x = ofGetScreenWidth() - bounds.width - 10;
	hLine.y = 250;
	hLineAlpha = 255;
}

void ofApp::continueHappyText() {
	if (currentHappyLine.size() > 0) {
		ofSetColor(0, 0, 0, hLineAlpha--);
		hLine.y += 1;
		player2.drawString(currentHappyLine, hLine.x, hLine.y);

		if (hLineAlpha == 0) currentHappyLine = "";
	}
}

void ofApp::drawPowerMode() {
	float currentTime = ofGetElapsedTimef();
	ofRectangle bounds=player2.getStringBoundingBox("POWER MODE", 0, 0);
	ofPushMatrix();
		ofTranslate(ofGetScreenWidth() / 2, ofGetScreenHeight() - 50);
		ofScale(2.0 + sin(currentTime) / 2, 2.0 + sin(currentTime) / 2, 1.0);
		player2.drawString("POWER MODE", -bounds.width/2, bounds.height);
	ofPopMatrix();
}

void ofApp::strobeEffect() {
	ColorAlpha col = solidColors.at(currentColor++%solidColors.size());
	ofPushMatrix();
		ofSetColor(col.r, col.g, col.b, 32);
		ofDrawRectangle(0, 0, ofGetScreenWidth(), ofGetScreenHeight());
	ofPopMatrix();
}

void ofApp::strobeEffectSlow() {
	float slowConstant = 2.f;
	int timeSlow=int(ofGetElapsedTimef() / slowConstant);

	ColorAlpha col = solidColors.at(timeSlow%solidColors.size());
	ofPushMatrix();
	ofSetColor(col.r, col.g, col.b, 32);
	ofDrawRectangle(0, 0, ofGetScreenWidth(), ofGetScreenHeight());
	ofPopMatrix();
}

void ofApp::drawPowerModeSeizure() {
	float currentTime = ofGetElapsedTimef();
	ofRectangle bounds = player2num.getStringBoundingBox("POWER MODE TURBO", 0, 0);

	ofPushMatrix();
		float maxDelta = 50;
		ofTranslate(ofGetScreenWidth() / 2, ofGetScreenHeight() -200);
		ofScale(2.0 + sin(currentTime), 2.0 + sin(currentTime), 1.0);
		player2num.drawString("POWER MODE TURBO", -bounds.width / 2 + ofRandom(-maxDelta, maxDelta), bounds.height + ofRandom(-maxDelta, maxDelta));
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
		lines.insert(lines.begin()+activeLine+1,lines.at(activeLine).substr(activeColumn,lines.at(activeLine).size()));
		lines.at(activeLine).erase(activeColumn, lines.at(activeLine).size() - activeColumn);
		incrActiveLine(TRUE);
		comboCount++;
		timeSinceLastKey = ofGetElapsedTimef();
		activeColumn = 0;
	break;
	case OF_KEY_DEL:
		if (activeColumn == lines.at(activeLine).size()) {
			if (activeLine + 1 < lines.size()) {
				lines.at(activeLine).append(lines.at(activeLine + 1));
				lines.erase(lines.begin() + activeLine + 1);
			}
		}
		else {
			lines.at(activeLine).erase(lines.at(activeLine).begin() + activeColumn);
		}
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
			if (activeLine < lines.size()-1) {
				activeLine++;
				activeColumn = 0;
			}
		}
		else {
			activeColumn++;
		}
		break;
	case OF_KEY_TAB:
		for(int i=0;i<4;i++)
			lines.at(activeLine).insert(lines.at(activeLine).begin() + activeColumn, ' ');
		activeColumn += 4;
		comboCount++;
		timeSinceLastKey = ofGetElapsedTimef();
		break;
	default:
		if (std::isprint(key,loc)) {
			lines.at(activeLine).insert(lines.at(activeLine).begin() + activeColumn, char(key));
			activeColumn++;
		}
		comboCount++;
		timeSinceLastKey = ofGetElapsedTimef();
		break;
	}

	initExplosions(10, ofRandom(0, ofGetScreenWidth()), ofRandom(0, ofGetScreenHeight()));
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
		angle *= (180/PI);//convert to radians

		p.x = xOrig+sin(angle) / tan(angle);
		p.y = yOrig+tan(angle)*cos(angle);
		p.angle = angle;

		particles.push_back(p);
	}

	activeParticles.push_back(particles);
	particleColors.push_back(colors);
}

void ofApp::renderExplosions() {
	for(int h=0;h<activeParticles.size();h++){
		vector<Point> &pointSet = activeParticles.at(h);
		Point origin = pointSet.at(0);

		//if alpha is zero, delete the particles
		if (particleColors.at(h).at(0).alpha <= 0) {
			activeParticles.erase(activeParticles.begin()+h);
			particleColors.erase(particleColors.begin() + h);
			h--;
			continue;
		}

		particleColors.at(h).at(0).alpha--;

		for (int i = 1; i < pointSet.size(); i++) {
			float hyp = 8;

			pointSet.at(i).x += hyp*cos(pointSet.at(i).angle);
			pointSet.at(i).y += hyp*sin(pointSet.at(i).angle);


			//display particle and decrease alpha to have it fade
			ofSetColor(particleColors.at(h).at(i).r, particleColors.at(h).at(i).g, particleColors.at(h).at(i).b, particleColors.at(h).at(i).alpha);
			ofDrawCircle(pointSet.at(i).x, pointSet.at(i).y, 3.f);
			particleColors.at(h).at(i).alpha--;
		}
	}
}

void ofApp::renderShiftingTriangles() {

	float middle = (ofGetScreenHeight() / 2);
	float tri1x = 300;
	float tri2x = ofGetScreenWidth() - 300;
	float maxDrift=ofGetScreenHeight()/2-200;
	float triangleLength = 100;
	float xyPos = std::sqrt(std::pow(triangleLength,2) / 2);
	float rotationConstant = 1 / 64.f;
	float actualY = middle + sin(ofGetElapsedTimef()*4)*maxDrift;

	float inTriLen = 10;
	ofSetPolyMode(OF_POLY_WINDING_ODD);
	

	ofPushMatrix();
		ofTranslate(tri1x, actualY, 0);
		ofRotateZ(ofGetElapsedTimef()/rotationConstant);
		ofBeginShape();
		ofVertex(-xyPos, xyPos);
		ofVertex(xyPos, xyPos);
		ofVertex(0, -triangleLength);

		ofVertex(-xyPos+inTriLen, xyPos-inTriLen);
		ofVertex(xyPos- inTriLen, xyPos- inTriLen);
		ofVertex(0, -triangleLength+ inTriLen);
		ofEndShape();
	ofPopMatrix();

	ofPushMatrix();
		ofTranslate(tri2x, actualY, 0);
		ofRotateZ(ofGetElapsedTimef() / rotationConstant);
		ofBeginShape();
		ofVertex(-xyPos, xyPos);
		ofVertex(xyPos, xyPos);
		ofVertex(0, -triangleLength);

		ofVertex(-xyPos + inTriLen, xyPos - inTriLen);
		ofVertex(xyPos - inTriLen, xyPos - inTriLen);
		ofVertex(0, -triangleLength + inTriLen);
		ofEndShape();
	ofPopMatrix();
}

void ofApp::renderTrippySinThing() {
	ofPushMatrix();
	ofTranslate(ofGetWidth() / 2.0f, ofGetHeight() / 2.0f, 0);

	
	ofSetPolyMode(OF_POLY_WINDING_ODD);
	ofBeginShape();

	float angleStepConstant = -1;
	angleStepConstant = ofGetElapsedTimef() / (360 / 64);
	if (angleStepConstant > 360) angleStepConstant = 0;

	float angleStep = angleStepConstant;

	int maxPoints = 40;
	float radiusAdder = (ofGetWidth() / 2.0f) / maxPoints;
	float radius = 0;
	for (int i = 0; i < maxPoints; i++) {
		float anglef = (i)* angleStep;
		float x = radius * cos(anglef);
		float y = radius * sin(anglef);
		ofVertex(x, y);
		radius += radiusAdder;
	}
	ofEndShape(OF_CLOSE);
	ofPopMatrix();
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

void ofApp::readInFile(string fileName) {
	ifstream theFile;
	theFile.open(fileName,ios::binary);

	string str;
	while (std::getline(theFile,str)) {
		lines.push_back(str);
	}

	theFile.close();
}

void ofApp::outputToFile(string fileName) {
	ofstream theFile;
	theFile.open(fileName, ios::binary);

	for(string str : lines) {
		theFile.write(str.c_str(),str.size());
		theFile.write("\n", 1);
	}

	theFile.close();
}

void ofApp::exit() {
	if (fileN.size() > 0)
		outputToFile(fileN);
	else
		outputToFile("default.txt");
}