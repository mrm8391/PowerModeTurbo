#pragma once

#include "ofMain.h"

class ofApp : public ofBaseApp{

	public:
		ofTrueTypeFont consolas;
		string lineStr = "You never know if you don't go";

		void setup();
		void update();
		void draw();
		void renderExplosions();
		void initExplosions(int count, float xOrig, float yOrig);
		void renderTrippySinThing();
		void drawPowerModeSeizure();
		void drawPowerMode();
		void renderSpaceEffect();

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
		
};
