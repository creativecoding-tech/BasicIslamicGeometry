#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
	ofSetVerticalSync(false); //Tidak ada limit dari refresh rate monitor
	ofSetFrameRate(60);
	ofSetEscapeQuitsApp(false);
	ofSetBackgroundAuto(false);
	ofEnableAntiAliasing();
	ofEnableSmoothing();

	circleA = std::make_unique<CircleShape>(radiusCircle,"A",0,0);
	circleB = std::make_unique<CircleShape>(radiusCircle, "B", radiusCircle, 0);
	circleC = std::make_unique<CircleShape>(radiusCircle, "C", -radiusCircle, 0);
	circleD = std::make_unique<CircleShape>(radiusCircle, "D", 0, radiusCircle);
	circleE = std::make_unique<CircleShape>(radiusCircle, "E", 0, -radiusCircle);
	cartesianAxes = std::make_unique<CartesianAxes>(radiusCircle);
}

//--------------------------------------------------------------
void ofApp::update(){
	circleA->update();
	circleB->update();
	circleC->update();
	circleD->update();
	circleE->update();
	cartesianAxes->update();
}

//--------------------------------------------------------------
void ofApp::draw(){
	ofSetColor(255, 25);
	ofFill();
	ofDrawRectangle(0, 0, ofGetWidth(), ofGetHeight());
	ofTranslate(ofGetWidth()/2, ofGetHeight()/2);
	circleA->draw();
	circleB->draw();
	circleC->draw();
	circleD->draw();
	circleE->draw();
	cartesianAxes->draw();
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
	if (key == 'Q' || key == 'q') ofExit();

	if (key == 'x' || key == 'X') {
		circleA->hide();
		circleB->hide();
		circleC->hide();
		circleD->hide();
		circleE->hide();
		cartesianAxes->hide();
	}

	if (key == 's' || key  == 'S') {
		circleA->show();
		circleB->show();
		circleC->show();
		circleD->show();
		circleE->show();
		cartesianAxes->show();
	}

	if (key == '-' || key == '_') {
		float thinWidth = .5f;
		circleA->setLineWidth(thinWidth);
		circleB->setLineWidth(thinWidth);
		circleC->setLineWidth(thinWidth);
		circleD->setLineWidth(thinWidth);
		circleE->setLineWidth(thinWidth);
		cartesianAxes->setLineWidth(thinWidth);

		// Font jadi tipis juga
		circleA->setThin(true);
		circleB->setThin(true);
		circleC->setThin(true);
		circleD->setThin(true);
		circleE->setThin(true);
	}

	if (key == '+' || key == '=') {
		float thickWidth = 4.f;
		circleA->setLineWidth(thickWidth);
		circleB->setLineWidth(thickWidth);
		circleC->setLineWidth(thickWidth);
		circleD->setLineWidth(thickWidth);
		circleE->setLineWidth(thickWidth);
		cartesianAxes->setLineWidth(thickWidth);

		// Font jadi tebal juga
		circleA->setThin(false);
		circleB->setThin(false);
		circleC->setThin(false);
		circleD->setThin(false);
		circleE->setThin(false);
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
