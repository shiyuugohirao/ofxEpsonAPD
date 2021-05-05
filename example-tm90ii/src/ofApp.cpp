#include "ofApp.h"

#define TIMESTAMP ofGetTimestampString("%Y%m%d-%H%M%S")

#define PRINTABLE_AREA_DOT 576 //ˆóŽš‰Â”\”ÍˆÍ(dot)

//--------------------------------------------------------------
void ofApp::setup(){
	//ofSetFrameRate(60);
	ofSetWindowShape(640, 480);
	cam.setup(640, 480);

	auto names = ofxPrinterUtils::getLocalPrinterNames();

	printer.setDefaultPrinterName("EPSON TM-T90II Receipt");
	printer.setUseCallback(true);

	ofAddListener(printer.callbackEvent, this, &ofApp::callback);
}

//--------------------------------------------------------------
void ofApp::update(){

	cam.update();

}

//--------------------------------------------------------------
void ofApp::draw(){

	if (cam.isInitialized()) cam.draw(0, 0);

	ofDrawBitmapStringHighlight(ofToString(ofGetFrameRate(), 2), 10, 10);

}

//--------------------------------------------------------------
void ofApp::keyPressed(int key) {

	if(printer.isWaitingCallback()) return;

	printer.setDefaultDocName(TIMESTAMP);

	switch (key) {
	case 't': {
		ofImage img("test.png");
		
		ofPixels pix = img.getPixels();
		pix.resize(576, pix.getHeight() * ((float)PRINTABLE_AREA_DOT / (float)pix.getWidth()));
		printer.begin();
		printer.drawImage(pix);
		printer.drawText(0, 0, TIMESTAMP);
		printer.end();
		break;
	}
	case ' ': {
		ofPixels pix = cam.getPixels();
		pix.resize(576, pix.getHeight() * ((float)PRINTABLE_AREA_DOT / (float)pix.getWidth()));

		printer.begin();
		printer.drawImage(pix);
		printer.drawText(0, 0, TIMESTAMP);
		printer.end();
		break;
	}

	default: {
		string text = "I was printed with openFrameworks!";

		printer.begin();
		printer.pushFontStyle(95, "FontA11");
		printer.drawText(20, 10, "Hello !");
		printer.drawText(50, 30, text);
		printer.popFontStyle();
		printer.end();
		break;
	}

	}
}

//--------------------------------------------------------------
void ofApp::callback(ofxEpsonAPD::PrinterCallbackEvent& e) {
	switch (e.printerStatus)
	{
	case ofxEpsonAPD::PRINT_SUCCESS: {
		cout << "Yeah! Nice Print!\nPrinter:" << e.printerName << "  Doc:" << e.docName << endl;
		break;
	}
	case ofxEpsonAPD::NO_RESPONSE: {
		break;
	}
	case ofxEpsonAPD::COVER_OPEN: {
		cout << "Omg! Cover is opening!\nPrinter:" << e.printerName << endl;
		break;
	}
	case ofxEpsonAPD::AUTOCUTTER_ERR: {
		break;
	}
	case ofxEpsonAPD::RECEIPT_NEAR_END: {
		break;
	}
	case ofxEpsonAPD::OTHER_ERROR: {
		break;
	}
	default:
		break;
	}
}