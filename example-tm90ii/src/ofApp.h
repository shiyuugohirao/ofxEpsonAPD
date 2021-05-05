#pragma once

#include "ofxEpsonAPD.h" // Need to include"ofxEpsonAPD.h" before include "ofManin.h"(window.h)
#include "ofMain.h"

class ofApp : public ofBaseApp{

private:
	ofVideoGrabber cam;
	ofxEpsonAPD printer;


public:
	void setup();
	void update();
	void draw();
	void keyPressed(int key);

	void callback(ofxEpsonAPD::PrinterCallbackEvent& e);
};
