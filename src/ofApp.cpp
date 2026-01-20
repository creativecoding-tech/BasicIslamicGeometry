#include "ofApp.h"
#include "shape/AbstractShape.h"

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
	crossLineF = std::make_unique<CrossLine>(vec2(0, 0), vec2(-radiusCircle, -radiusCircle), "F","J", radiusCircle);
	crossLineG = std::make_unique<CrossLine>(vec2(0, 0), vec2(radiusCircle, -radiusCircle), "G","K", radiusCircle);
	crossLineH = std::make_unique<CrossLine>(vec2(0, 0), vec2(-radiusCircle, radiusCircle), "H","L", radiusCircle);
	crossLineI = std::make_unique<CrossLine>(vec2(0, 0), vec2(radiusCircle, radiusCircle), "I","M", radiusCircle);

	//Parallelogram
	parallelogramCtoE = std::make_unique<ParallelogramLine>(vec2(-radiusCircle, 0), vec2(0, -radiusCircle));
	parallelogramEtoB = std::make_unique<ParallelogramLine>(vec2(0, -radiusCircle), vec2(radiusCircle, 0));
	parallelogramBtoD = std::make_unique<ParallelogramLine>(vec2(radiusCircle,0),vec2(0,radiusCircle));
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
	crossLineI->update();
	//Parallelogram update
	parallelogramCtoE->update();
	parallelogramEtoB->update();
	parallelogramBtoD->update();

	// Update sequential drawing logic
	if (sequentialMode) {
		updateSequentialDrawing();
	}
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
	crossLineI->draw();
	//Draw paralellogram
	parallelogramCtoE->draw();
	parallelogramEtoB->draw();
	parallelogramBtoD->draw();
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
	if (key == 'Q' || key == 'q') ofExit();

	// Sequential drawing dengan SHIFT+1
	if (key == '!' && ofGetKeyPressed(OF_KEY_SHIFT)) {
		startSequentialDrawing();
	}

	// Toggle label visibility dengan ` atau ~
	if (key == '`' || key == '~') {
		toggleLabels();
	}

	if (key == 'x' || key == 'X') {
		// Hanya boleh hide jika TIDAK sedang sequential drawing
		if (!sequentialMode) {
			hideAllShapes();
		}
	}

	if (key == 's' || key  == 'S') {
		// Hanya boleh show semua jika TIDAK sedang sequential drawing
		if (!sequentialMode) {
			showAllShapes();
		}
	}

	if (key == '-' || key == '_') {
		decreaseLineWidth();
	}

	if (key == '+' || key == '=') {
		increaseLineWidth();
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

//--------------------------------------------------------------
void ofApp::startSequentialDrawing() {
	// Cek apakah semua shapes sudah visible/showing (berarti sudah show semua dengan S)
	bool allVisible = cartesianAxes->showing && circleA->showing &&
		circleB->showing && circleC->showing &&
		circleD->showing && circleE->showing &&
		crossLineF->showing && crossLineG->showing &&
		crossLineH->showing && crossLineI->showing &&
		parallelogramCtoE->showing && parallelogramEtoB->showing &&
		parallelogramBtoD->showing;

	if (allVisible) {
		// Semua shapes sudah visible, jangan jalankan sequential
		return;
	}

	// Cek apakah ada shape yang sedang drawing (belum complete)
	bool stillDrawing = !cartesianAxes->isComplete() || !circleA->isComplete() ||
	                   !circleB->isComplete() || !circleC->isComplete() ||
	                   !circleD->isComplete() || !circleE->isComplete() ||
	                   !crossLineF->isComplete() || !crossLineG->isComplete() ||
	                   !crossLineH->isComplete() || !crossLineI->isComplete() ||
	                   !parallelogramCtoE->isComplete() || !parallelogramEtoB->isComplete() ||
	                   !parallelogramBtoD->isComplete();

	if (stillDrawing) {
		// Ada shape yang masih drawing, jangan jalankan sequential
		return;
	}

	// Cek apakah sequential sudah pernah selesai
	if (sequentialCompleted) {
		// Sequential sudah selesai sebelumnya, jangan jalankan lagi
		return;
	}

	// Reset semua shapes ke hidden
	circleA->hide();
	circleB->hide();
	circleC->hide();
	circleD->hide();
	circleE->hide();
	cartesianAxes->hide();
	crossLineF->hide();
	crossLineG->hide();
	crossLineH->hide();
	crossLineI->hide();
	parallelogramCtoE->hide();
	parallelogramEtoB->hide();
	parallelogramBtoD->hide();

	// Mulai sequential mode
	sequentialMode = true;
	sequentialCompleted = false;  // Reset flag
	currentShapeIndex = 0;

	// Show shape pertama (CartesianAxes)
	cartesianAxes->show();
}

//--------------------------------------------------------------
void ofApp::updateSequentialDrawing() {
	AbstractShape* currentShape = nullptr;

	// Tentukan shape berdasarkan index
	switch (currentShapeIndex) {
		case 0: currentShape = cartesianAxes.get(); break;
		case 1: currentShape = circleA.get(); break;
		case 2: currentShape = circleB.get(); break;
		case 3: currentShape = circleC.get(); break;
		case 4: currentShape = circleD.get(); break;
		case 5: currentShape = circleE.get(); break;
		case 6: currentShape = crossLineF.get(); break;
		case 7: currentShape = crossLineG.get(); break;
		case 8: currentShape = crossLineH.get(); break;
		case 9: currentShape = crossLineI.get(); break;
		case 10: currentShape = parallelogramCtoE.get(); break;
		case 11: currentShape = parallelogramEtoB.get(); break;
		case 12: currentShape = parallelogramBtoD.get(); break;
	}

	// Cek jika current shape sudah complete
	if (currentShape && currentShape->isComplete()) {
		// Pindah ke shape berikutnya
		currentShapeIndex++;

		// Show shape berikutnya jika masih ada
		if (currentShapeIndex <= 12) {
			switch (currentShapeIndex) {
				case 1: circleA->show(); break;
				case 2: circleB->show(); break;
				case 3: circleC->show(); break;
				case 4: circleD->show(); break;
				case 5: circleE->show(); break;
				case 6: crossLineF->show(); break;
				case 7: crossLineG->show(); break;
				case 8: crossLineH->show(); break;
				case 9: crossLineI->show(); break;
				case 10: parallelogramCtoE->show(); break;
				case 11: parallelogramEtoB->show(); break;
				case 12: parallelogramBtoD->show(); break;
			}
		} else {
			// Semua shapes sudah complete, matikan sequential mode dan tandai selesai
			sequentialMode = false;
			sequentialCompleted = true;
		}
	}
}

//--------------------------------------------------------------
void ofApp::toggleLabels() {
	// Toggle label visibility
	labelsVisible = !labelsVisible;

	if (labelsVisible) {
		// Show semua labels
		circleA->showLabel();
		circleB->showLabel();
		circleC->showLabel();
		circleD->showLabel();
		circleE->showLabel();
		crossLineF->showLabel();
		crossLineG->showLabel();
		crossLineH->showLabel();
		crossLineI->showLabel();
	} else {
		// Hide semua labels
		circleA->hideLabel();
		circleB->hideLabel();
		circleC->hideLabel();
		circleD->hideLabel();
		circleE->hideLabel();
		crossLineF->hideLabel();
		crossLineG->hideLabel();
		crossLineH->hideLabel();
		crossLineI->hideLabel();
	}
}

//--------------------------------------------------------------
void ofApp::hideAllShapes() {
	// Hide semua shapes
	circleA->hide();
	circleB->hide();
	circleC->hide();
	circleD->hide();
	circleE->hide();
	cartesianAxes->hide();
	crossLineF->hide();
	crossLineG->hide();
	crossLineH->hide();
	crossLineI->hide();
	parallelogramCtoE->hide();
	parallelogramEtoB->hide();
	parallelogramBtoD->hide();

	// Reset sequential completed flag agar bisa jalankan lagi
	sequentialCompleted = false;
}

//--------------------------------------------------------------
void ofApp::showAllShapes() {
	// Show semua shapes
	circleA->show();
	circleB->show();
	circleC->show();
	circleD->show();
	circleE->show();
	cartesianAxes->show();
	crossLineF->show();
	crossLineG->show();
	crossLineH->show();
	crossLineI->show();
	parallelogramCtoE->show();
	parallelogramEtoB->show();
	parallelogramBtoD->show();

	// Reset sequential completed flag agar bisa jalankan lagi
	sequentialCompleted = false;
}

//--------------------------------------------------------------
void ofApp::decreaseLineWidth() {
	// Kurangi line width secara bertahap
	currentLineWidth -= 0.5f;

	// Batasi minimum line width
	if (currentLineWidth < 0.f) {
		currentLineWidth = 0.f;
	}

	// Set line width ke semua shapes
	circleA->setLineWidth(currentLineWidth);
	circleB->setLineWidth(currentLineWidth);
	circleC->setLineWidth(currentLineWidth);
	circleD->setLineWidth(currentLineWidth);
	circleE->setLineWidth(currentLineWidth);
	cartesianAxes->setLineWidth(currentLineWidth);
	crossLineF->setLineWidth(currentLineWidth);
	crossLineG->setLineWidth(currentLineWidth);
	crossLineH->setLineWidth(currentLineWidth);
	crossLineI->setLineWidth(currentLineWidth);
	parallelogramCtoE->setLineWidth(currentLineWidth);
	parallelogramEtoB->setLineWidth(currentLineWidth);
	parallelogramBtoD->setLineWidth(currentLineWidth);

	// Font jadi tipis juga
	circleA->setThin(true);
	circleB->setThin(true);
	circleC->setThin(true);
	circleD->setThin(true);
	circleE->setThin(true);
	crossLineF->setThin(true);
	crossLineG->setThin(true);
	crossLineH->setThin(true);
	crossLineI->setThin(true);
}

//--------------------------------------------------------------
void ofApp::increaseLineWidth() {
	// Tambah line width secara bertahap
	currentLineWidth += 0.5f;

	// Batasi maximum line width
	if (currentLineWidth > 4.0f) {
		currentLineWidth = 4.0f;
	}

	// Set line width ke semua shapes
	circleA->setLineWidth(currentLineWidth);
	circleB->setLineWidth(currentLineWidth);
	circleC->setLineWidth(currentLineWidth);
	circleD->setLineWidth(currentLineWidth);
	circleE->setLineWidth(currentLineWidth);
	cartesianAxes->setLineWidth(currentLineWidth);
	crossLineF->setLineWidth(currentLineWidth);
	crossLineG->setLineWidth(currentLineWidth);
	crossLineH->setLineWidth(currentLineWidth);
	crossLineI->setLineWidth(currentLineWidth);
	parallelogramCtoE->setLineWidth(currentLineWidth);
	parallelogramEtoB->setLineWidth(currentLineWidth);
	parallelogramBtoD->setLineWidth(currentLineWidth);

	// Font jadi tebal juga
	circleA->setThin(false);
	circleB->setThin(false);
	circleC->setThin(false);
	circleD->setThin(false);
	circleE->setThin(false);
	crossLineF->setThin(false);
	crossLineG->setThin(false);
	crossLineH->setThin(false);
	crossLineI->setThin(false);
}


