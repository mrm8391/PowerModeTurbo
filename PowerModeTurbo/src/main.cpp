#include "ofMain.h"
#include "ofApp.h"

//========================================================================
int main(int argc,char *argv[]){

	//ofSetupOpenGL(1024, 728, OF_WINDOW);
	ofSetupOpenGL(1920,1080,OF_FULLSCREEN);			// <-------- setup the GL context

	// this kicks off the running of my app
	// can be OF_WINDOW or OF_FULLSCREEN
	// pass in width and height too:

	for (int i = 0; i < argc;i++) {
		ofLog() << argv[i];
	}

	if (argc < 2)
		ofRunApp(new ofApp());
	else
		ofRunApp(new ofApp(argv[1]));

}
