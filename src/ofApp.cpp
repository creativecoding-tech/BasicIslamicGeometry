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

	// Call modular setup methods
	setupCircles();
	setupCartesianAxes();
	setupCrossLines();
	setupParallelograms();
	setupRectangleLine();
}

//--------------------------------------------------------------
void ofApp::setupCircles(){
	//Circle
	circleA = std::make_unique<CircleShape>(radiusCircle,"A",0,0);
	circleB = std::make_unique<CircleShape>(radiusCircle, "B", radiusCircle, 0);
	circleC = std::make_unique<CircleShape>(radiusCircle, "C", -radiusCircle, 0);
	circleD = std::make_unique<CircleShape>(radiusCircle, "D", 0, radiusCircle);
	circleE = std::make_unique<CircleShape>(radiusCircle, "E", 0, -radiusCircle);
}

//--------------------------------------------------------------
void ofApp::setupCartesianAxes(){
	//Cartesian
	cartesianAxes = std::make_unique<CartesianAxes>(radiusCircle);
}

//--------------------------------------------------------------
void ofApp::setupCrossLines(){
	//CrossLine
	crossLineF = std::make_unique<CrossLine>(vec2(0, 0), vec2(-radiusCircle, -radiusCircle), "F","J", radiusCircle);
	crossLineG = std::make_unique<CrossLine>(vec2(0, 0), vec2(radiusCircle, -radiusCircle), "G","K", radiusCircle);
	crossLineH = std::make_unique<CrossLine>(vec2(0, 0), vec2(-radiusCircle, radiusCircle), "H","L", radiusCircle);
	crossLineI = std::make_unique<CrossLine>(vec2(0, 0), vec2(radiusCircle, radiusCircle), "I","M", radiusCircle);
}

//--------------------------------------------------------------
void ofApp::setupParallelograms(){
	//Parallelogram dengan Polar Thinking
	// Intersection point dihitung menggunakan trigonometri: x = cos(angle) * distance, y = sin(angle) * distance

	// C→E memotong F: Northwest (-120, -120)
	// Polar: angle = -3*PI/4 (-135°), distance = r*√2/2
	// Contoh: radiusCircle = 240, maka distance = 240 * √2 / 2 = 169.68
	// x = cos(-135°) × 169.68 = -0.707 × 169.68 = -120
	// y = sin(-135°) × 169.68 = -0.707 × 169.68 = -120
	float angleC_E_F = -3 * PI / 4;  // -135 derajat (Northwest)
	float distC_E_F = radiusCircle * sqrt(2) / 2;
	vec2 intersecC_E_F = vec2(cos(angleC_E_F) * distC_E_F, sin(angleC_E_F) * distC_E_F);
	parallelogramCtoE = std::make_unique<ParallelogramLine>(vec2(-radiusCircle, 0), vec2(0, -radiusCircle), intersecC_E_F, "N");

	// E→B memotong G: Northeast (120, -120)
	// Polar: angle = -PI/4 (-45°), distance = r*√2/2
	// Contoh: radiusCircle = 240, maka distance = 240 * √2 / 2 = 169.68
	// x = cos(-45°) × 169.68 = 0.707 × 169.68 = 120
	// y = sin(-45°) × 169.68 = -0.707 × 169.68 = -120
	float angleE_B_G = -PI / 4;  // -45 derajat (Northeast)
	float distE_B_G = radiusCircle * sqrt(2) / 2;
	vec2 intersecE_B_G = vec2(cos(angleE_B_G) * distE_B_G, sin(angleE_B_G) * distE_B_G);
	parallelogramEtoB = std::make_unique<ParallelogramLine>(vec2(0, -radiusCircle), vec2(radiusCircle, 0), intersecE_B_G, "O");

	// B→D memotong I: Southeast (120, 120)
	// Polar: angle = PI/4 (45°), distance = r*√2/2
	// Contoh: radiusCircle = 240, maka distance = 240 * √2 / 2 = 169.68
	// x = cos(45°) × 169.68 = 0.707 × 169.68 = 120
	// y = sin(45°) × 169.68 = 0.707 × 169.68 = 120
	float angleB_D_I = PI / 4;  // 45 derajat (Southeast)
	float distB_D_I = radiusCircle * sqrt(2) / 2;
	vec2 intersecB_D_I = vec2(cos(angleB_D_I) * distB_D_I, sin(angleB_D_I) * distB_D_I);
	parallelogramBtoD = std::make_unique<ParallelogramLine>(vec2(radiusCircle, 0), vec2(0, radiusCircle), intersecB_D_I, "P");

	// D→C memotong H: Southwest (-120, 120)
	// Polar: angle = 3*PI/4 (135°), distance = r*√2/2
	// Contoh: radiusCircle = 240, maka distance = 240 * √2 / 2 = 169.68
	// x = cos(135°) × 169.68 = -0.707 × 169.68 = -120
	// y = sin(135°) × 169.68 = 0.707 × 169.68 = 120
	float angleD_C_H = 3 * PI / 4;  // 135 derajat (Southwest)
	float distD_C_H = radiusCircle * sqrt(2) / 2;
	vec2 intersecD_C_H = vec2(cos(angleD_C_H) * distD_C_H, sin(angleD_C_H) * distD_C_H);
	parallelogramDtoC = std::make_unique<ParallelogramLine>(vec2(0, radiusCircle), vec2(-radiusCircle, 0), intersecD_C_H, "Q");
}

//--------------------------------------------------------------
void ofApp::setupRectangleLine(){
	//RectangleLine dari F ke G dengan intersection points
	// F = dot pertama CrossLine F: cos(-135°) * radius
	// G = dot pertama CrossLine G: cos(-45°) * radius

	// Hitung posisi F dan G dengan polar (SCALABLE!)
	float angleF = -3 * PI / 4;  // -135°
	vec2 posF = vec2(cos(angleF) * radiusCircle, sin(angleF) * radiusCircle);

	float angleG = -PI / 4;  // -45°
	vec2 posG = vec2(cos(angleG) * radiusCircle, sin(angleG) * radiusCircle);

	// Hitung intersection F→G ∩ C→E (Dot R) dengan polar thinking
	// Garis C→E memotong F→G di y = posF.y
	// Dari geometri: x = -radiusCircle * (1 - √2/2)
	vec2 intersecR = vec2(-radiusCircle * (1 - sqrt(2) / 2), posF.y);

	// Hitung intersection F→G ∩ B→E (Dot S) dengan polar thinking
	// Garis B→E memotong F→G di y = posF.y
	// Dari geometri: x = radiusCircle * (√2/2)
	vec2 intersecS = vec2(radiusCircle * (1-sqrt(2) / 2), posF.y);

	rectangleLineFtoG = std::make_unique<RectangleLine>(
		posF,        // Start: F
		posG,        // End:   G
		intersecR,   // Intersec1: F→G ∩ C→E (SCALABLE!)
		intersecS,   // Intersec2: F→G ∩ B→E (SCALABLE!)
		"R",         // Label1
		"S"          // Label2
	);

	float angleI= PI / 4; //45°
	vec2 posI = vec2(cos(angleI) * radiusCircle, sin(angleI) * radiusCircle);

	// Hitung intersection G→I ∩ B→E (Dot T) dengan polar thinking
	// Garis B→E: y = x - r, G→I vertical di x = r*√2/2
	// Jadi: y = r*√2/2 - r = r(√2/2 - 1)
	vec2 intersecT = vec2(radiusCircle * sqrt(2) / 2, radiusCircle * (sqrt(2) / 2 - 1));

	// Hitung intersection G→I ∩ B→D (Dot U) dengan polar thinking
	// Garis B→D: y = -x + r, G→I vertical di x = r*√2/2
	// Jadi: y = -r*√2/2 + r = r(1 - √2/2)
	vec2 intersecU = vec2(radiusCircle * sqrt(2) / 2, radiusCircle * (1 - sqrt(2) / 2));

	rectangleLineGtoI = std::make_unique<RectangleLine>(
		posG,        // Start: G
		posI,        // End:   I
		intersecT,   // Intersec1: G→I ∩ B→E (SCALABLE!)
		intersecU,   // Intersec2: G→I ∩ B→D (SCALABLE!)
		"T",         // Label1
		"U"          // Label2
	);

	float angleH = 3 * PI / 4; //135°
	vec2 posH = vec2(cos(angleH) * radiusCircle, sin(angleH) * radiusCircle);
	vec2 intersecV = vec2(radiusCircle * (1 - sqrt(2) / 2), posH.y);
	vec2 intersecW = vec2(-radiusCircle * (1 - sqrt(2) / 2), posH.y);
	rectangleLineItoH = std::make_unique<RectangleLine>(
		posI,        // Start: I
		posH,        // End:   H
		intersecV,   // Intersec1: I→H ∩ B→D (SCALABLE!)
		intersecW,   // Intersec2: I→H ∩ D→C (SCALABLE!)
		"V",         // Label1
		"W"          // Label2
	);
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
	parallelogramDtoC->update();
	//RectangleLine update
	rectangleLineFtoG->update();
	rectangleLineGtoI->update();
	rectangleLineItoH->update();


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
	parallelogramDtoC->draw();
	//Draw RectangleLine
	rectangleLineFtoG->draw();
	rectangleLineGtoI->draw();
	rectangleLineItoH->draw();
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
	if (key == OF_KEY_END) ofExit();

	// Sequential drawing dengan SHIFT+1
	if (key == '!' && ofGetKeyPressed(OF_KEY_SHIFT)) {
		startSequentialDrawing();
	}

	// Toggle label visibility dengan ` atau ~
	if (key == '`' || key == '~') {
		toggleLabels();
	}

	// Toggle dot visibility dengan . atau >
	if (key == '.' || key == '>') {
		toggleDots();
	}

	if (key == OF_KEY_DEL) {
		// Hanya boleh hide jika TIDAK sedang sequential drawing
		if (!sequentialMode) {
			hideAllShapes();
		}
	}

	if (key == OF_KEY_BACKSPACE) {
		// Toggle CartesianAxes visibility
		if (cartesianAxes->showing) {
			cartesianAxes->hide();
		} else {
			cartesianAxes->show();
		}
	}

	if (key == ')' && ofGetKeyPressed(OF_KEY_SHIFT)) {
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
	bool allVisible = circleA->showing &&
		circleB->showing && circleC->showing &&
		circleD->showing && circleE->showing &&
		crossLineF->showing && crossLineG->showing &&
		crossLineH->showing && crossLineI->showing &&
		parallelogramCtoE->showing && parallelogramEtoB->showing &&
		parallelogramBtoD->showing && parallelogramDtoC->showing &&
		rectangleLineFtoG->showing && rectangleLineGtoI->showing &&
		rectangleLineItoH->showing;

	if (allVisible) {
		// Semua shapes sudah visible, jangan jalankan sequential
		return;
	}

	// Cek apakah ada shape yang sedang drawing (belum complete)
	bool stillDrawing = !circleA->isComplete() ||
	                   !circleB->isComplete() || !circleC->isComplete() ||
	                   !circleD->isComplete() || !circleE->isComplete() ||
	                   !crossLineF->isComplete() || !crossLineG->isComplete() ||
	                   !crossLineH->isComplete() || !crossLineI->isComplete() ||
	                   !parallelogramCtoE->isComplete() || !parallelogramEtoB->isComplete() ||
	                   !parallelogramBtoD->isComplete() || !parallelogramDtoC->isComplete() ||
	                   !rectangleLineFtoG->isComplete() || !rectangleLineGtoI->isComplete() ||
						!rectangleLineItoH->isComplete();

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
	parallelogramDtoC->hide();
	rectangleLineFtoG->hide();
	rectangleLineGtoI->hide();
	rectangleLineItoH->hide();

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
		case 13: currentShape = parallelogramDtoC.get(); break;
		case 14: currentShape = rectangleLineFtoG.get(); break;
		case 15: currentShape = rectangleLineGtoI.get(); break;
		case 16: currentShape = rectangleLineItoH.get(); break;
	}

	// Cek jika current shape sudah complete
	if (currentShape && currentShape->isComplete()) {
		// Pindah ke shape berikutnya
		currentShapeIndex++;

		// Show shape berikutnya jika masih ada
		if (currentShapeIndex <= 16) {
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
				case 13: parallelogramDtoC->show(); break;
				case 14: rectangleLineFtoG->show(); break;
				case 15: rectangleLineGtoI->show(); break;
				case 16: rectangleLineItoH->show(); break;
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
		cartesianAxes->showLabel();
		crossLineF->showLabel();
		crossLineG->showLabel();
		crossLineH->showLabel();
		crossLineI->showLabel();
		parallelogramCtoE->showLabel();
		parallelogramEtoB->showLabel();
		parallelogramBtoD->showLabel();
		parallelogramDtoC->showLabel();
		rectangleLineFtoG->showLabel();
		rectangleLineGtoI->showLabel();
		rectangleLineItoH->showLabel();
	} else {
		// Hide semua labels
		circleA->hideLabel();
		circleB->hideLabel();
		circleC->hideLabel();
		circleD->hideLabel();
		circleE->hideLabel();
		cartesianAxes->hideLabel();
		crossLineF->hideLabel();
		crossLineG->hideLabel();
		crossLineH->hideLabel();
		crossLineI->hideLabel();
		parallelogramCtoE->hideLabel();
		parallelogramEtoB->hideLabel();
		parallelogramBtoD->hideLabel();
		parallelogramDtoC->hideLabel();
		rectangleLineFtoG->hideLabel();
		rectangleLineGtoI->hideLabel();
		rectangleLineItoH->hideLabel();
	}
}

//--------------------------------------------------------------
void ofApp::toggleDots() {
	// Toggle dot visibility
	dotsVisible = !dotsVisible;

	if (dotsVisible) {
		// Show semua dots
		circleA->showDot();
		circleB->showDot();
		circleC->showDot();
		circleD->showDot();
		circleE->showDot();
		crossLineF->showDot();
		crossLineG->showDot();
		crossLineH->showDot();
		crossLineI->showDot();
		parallelogramCtoE->showDot();
		parallelogramEtoB->showDot();
		parallelogramBtoD->showDot();
		parallelogramDtoC->showDot();
		rectangleLineFtoG->showDot();
		rectangleLineGtoI->showDot();
		rectangleLineItoH->showDot();
	} else {
		// Hide semua dots
		circleA->hideDot();
		circleB->hideDot();
		circleC->hideDot();
		circleD->hideDot();
		circleE->hideDot();
		crossLineF->hideDot();
		crossLineG->hideDot();
		crossLineH->hideDot();
		crossLineI->hideDot();
		parallelogramCtoE->hideDot();
		parallelogramEtoB->hideDot();
		parallelogramBtoD->hideDot();
		parallelogramDtoC->hideDot();
		rectangleLineFtoG->hideDot();
		rectangleLineGtoI->hideDot();
		rectangleLineItoH->hideDot();
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
	parallelogramDtoC->hide();
	rectangleLineFtoG->hide();
	rectangleLineGtoI->hide();
	rectangleLineItoH->hide();

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
	parallelogramDtoC->show();
	rectangleLineFtoG->show();
	rectangleLineGtoI->show();
	rectangleLineItoH->show();

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
	parallelogramDtoC->setLineWidth(currentLineWidth);
	rectangleLineFtoG->setLineWidth(currentLineWidth);
	rectangleLineGtoI->setLineWidth(currentLineWidth);
	rectangleLineItoH->setLineWidth(currentLineWidth);
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
	parallelogramDtoC->setLineWidth(currentLineWidth);
	rectangleLineFtoG->setLineWidth(currentLineWidth);
	rectangleLineGtoI->setLineWidth(currentLineWidth);
	rectangleLineItoH->setLineWidth(currentLineWidth);
}


