#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
	ofSetVerticalSync(false); //Tidak ada limit dari refresh rate monitor
	ofSetFrameRate(60);
	ofSetEscapeQuitsApp(false);
	ofSetBackgroundAuto(false);
	ofEnableAntiAliasing();
	ofEnableSmoothing();
	ofHideCursor();
	//Circle
	circleA = std::make_unique<CircleShape>(radiusCircle,"A",0,0);
	circleB = std::make_unique<CircleShape>(radiusCircle, "B", radiusCircle, 0);
	circleC = std::make_unique<CircleShape>(radiusCircle, "C", -radiusCircle, 0);
	circleD = std::make_unique<CircleShape>(radiusCircle, "D", 0, radiusCircle);
	circleE = std::make_unique<CircleShape>(radiusCircle, "E", 0, -radiusCircle);

	//Cartesian
	cartesianAxes = std::make_unique<CartesianAxes>(radiusCircle);

	//CrossLine
	crossLineF = std::make_unique<CrossLine>(vec2(0, 0), vec2(-radiusCircle, -radiusCircle), "F");
	crossLineG = std::make_unique<CrossLine>(vec2(0, 0), vec2(radiusCircle, -radiusCircle), "G");
	crossLineH = std::make_unique<CrossLine>(vec2(0, 0), vec2(-radiusCircle, radiusCircle), "H");
}

//--------------------------------------------------------------
void ofApp::update(){
	//Circle Update
	circleA->update();
	circleB->update();
	circleC->update();
	circleD->update();
	circleE->update();
	//Cartesian update
	cartesianAxes->update();
	//CrossLine update
	crossLineF->update();
	crossLineG->update();
	crossLineH->update();
}

//--------------------------------------------------------------
void ofApp::draw(){
	ofSetColor(255, 25);
	ofFill();
	ofDrawRectangle(0, 0, ofGetWidth(), ofGetHeight());
	ofTranslate(ofGetWidth()/2, ofGetHeight()/2);
	//Draw circle
	circleA->draw();
	circleB->draw();
	circleC->draw();
	circleD->draw();
	circleE->draw();
	//Draw Cartesian
	cartesianAxes->draw();
	//Draw CrossLine
	crossLineF->draw();
	crossLineG->draw();
	crossLineH->draw();
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
	if (key == 'Q' || key == 'q') ofExit();

	if (key == 'x' || key == 'X') {
		//Hide circle
		circleA->hide();
		circleB->hide();
		circleC->hide();
		circleD->hide();
		circleE->hide();
		//Hide cartesian
		cartesianAxes->hide();
		//Hide crossline
		crossLineF->hide();
		crossLineG->hide();
		crossLineH->hide();
	}

	if (key == 's' || key  == 'S') {
		//Show Circle
		circleA->show();
		circleB->show();
		circleC->show();
		circleD->show();
		circleE->show();
		//Show cartesian
		cartesianAxes->show();
		//show crossline
		crossLineF->show();
		crossLineG->show();
		crossLineH->show();
	}

	if (key == '-' || key == '_') {
		float thinWidth = .5f;
		//bikin tipis line circle
		circleA->setLineWidth(thinWidth);
		circleB->setLineWidth(thinWidth);
		circleC->setLineWidth(thinWidth);
		circleD->setLineWidth(thinWidth);
		circleE->setLineWidth(thinWidth);
		//bikin tipis line cartesian
		cartesianAxes->setLineWidth(thinWidth);
		//bikin tipis line crossline
		crossLineF->setLineWidth(thinWidth);
		crossLineG->setLineWidth(thinWidth);
		crossLineH->setLineWidth(thinWidth);

		// Font jadi tipis juga
		circleA->setThin(true);
		circleB->setThin(true);
		circleC->setThin(true);
		circleD->setThin(true);
		circleE->setThin(true);

		//Font crossLine
		crossLineF->setThin(true);
		crossLineG->setThin(true);
		crossLineH->setThin(true);
	}

	if (key == '+' || key == '=') {
		float thickWidth = 4.f;
		//bikin nomral line circle
		circleA->setLineWidth(thickWidth);
		circleB->setLineWidth(thickWidth);
		circleC->setLineWidth(thickWidth);
		circleD->setLineWidth(thickWidth);
		circleE->setLineWidth(thickWidth);
		//bikin normal line cartesian
		cartesianAxes->setLineWidth(thickWidth);
		//bikin normal line cross line
		crossLineF->setLineWidth(thickWidth);
		crossLineG->setLineWidth(thickWidth);
		crossLineH->setLineWidth(thickWidth);

		// Font jadi tebal juga
		circleA->setThin(false);
		circleB->setThin(false);
		circleC->setThin(false);
		circleD->setThin(false);
		circleE->setThin(false);
		//Font normal crossLine
		crossLineF->setThin(false);
		crossLineG->setThin(false);
		crossLineH->setThin(false);
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
	if (button == 2) cursorVisible = !cursorVisible;
	if (cursorVisible) ofShowCursor();
	if (!cursorVisible) ofHideCursor();
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
