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

	// Load font untuk custom line labels
	fontNormal.load("C:\\Windows\\Fonts\\calibri.ttf", 15);

	// Call modular setup methods
	setupCircles();
	setupCartesianAxes();
	setupCrossLines();
	setupParallelograms();
	setupRectangleLine();
	setupOctagramLine();

	// Initial dots cache build (cache di-build saat pertama kali getAllDots() dipanggil)
	dotsCacheDirty = true;
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

	vec2 intersecX = vec2(-radiusCircle * sqrt(2) / 2, radiusCircle * (1 - sqrt(2) / 2));
	vec2 intersecY = vec2(-radiusCircle * sqrt(2) / 2, radiusCircle * (sqrt(2) / 2 - 1));

	rectangleLineHtoF = std::make_unique<RectangleLine>(
		posH,        // Start: H
		posF,        // End:   F
		intersecX,   // Intersec1: H→F ∩ D→C (SCALABLE!)
		intersecY,   // Intersec2: H→F ∩ C→E (SCALABLE!)
		"X",         // Label1
		"Y"          // Label2
	);
}

//--------------------------------------------------------------
void ofApp::setupOctagramLine(){
	//OctagramLine: Dua garis dari Circle E → posEnd → posG
	// Start: Circle E (0, -240)
	// End: Posisi di -45° dari Circle E dengan distance r = 240 → (170, -410)
	// NextPoint: Dot G (170, -170)

	// Posisi Circle E
	vec2 posE = vec2(0, -radiusCircle);

	// Posisi di -45° dari Circle E dengan distance r = 240
	float angle0 = -PI / 4;  // -45°
	vec2 posEnd = vec2(
		posE.x + cos(angle0) * radiusCircle,
		posE.y + sin(angle0) * radiusCircle
	);  // (170, -410)

	// Posisi Dot G (di -45° dari center)
	vec2 posG = vec2(cos(angle0) * radiusCircle, sin(angle0) * radiusCircle);  // (170, -170)

	// Buat octagramLine0 dengan nextPoint (akan menggambar 2 garis)
	octagramLine0 = std::make_unique<OctagramLine>(
		posE,        // Start: Circle E (0, -240)
		posEnd,      // End: (170, -410)
		posG,        // NextPoint: Dot G (170, -170)
		"0"          // Label untuk end point
	);

	// === OCTAGRAM LINE 2 ===
	// Garis 1: Dot G → -45° dari Circle B
	// Garis 2: -45° Circle B → pusat Circle B

	// Posisi Circle B
	vec2 posB = vec2(radiusCircle, 0);  // (240, 0)

	// Posisi di -45° dari Circle B
	vec2 posEnd1 = vec2(
		posB.x + cos(angle0) * radiusCircle,
		posB.y + sin(angle0) * radiusCircle
	);  // (410, -170)

	// Buat octagramLine1 dengan nextPoint (akan menggambar 2 garis)
	octagramLine1 = std::make_unique<OctagramLine>(
		posG,         // Start: Dot G (170, -170)
		posEnd1,      // End: (410, -170)
		posB,         // NextPoint: Pusat Circle B (240, 0)
		"1"           // Label untuk end point
	);

	// === OCTAGRAM LINE 2 ===
	// Garis 1: Circle B → 45° dari Circle B
	// Garis 2: 45° Circle B → Dot I

	// Posisi di 45° dari Circle B
	float angle2 = PI / 4;  // 45°
	vec2 posEnd2 = vec2(
		posB.x + cos(angle2) * radiusCircle,
		posB.y + sin(angle2) * radiusCircle
	);  // (410, 170)

	// Posisi Dot I (45° dari center)
	vec2 posI = vec2(cos(angle2) * radiusCircle, sin(angle2) * radiusCircle);  // (170, 170)

	// Buat octagramLine2 dengan nextPoint (akan menggambar 2 garis)
	octagramLine2 = std::make_unique<OctagramLine>(
		posB,         // Start: Circle B (240, 0)
		posEnd2,      // End: (410, 170)
		posI,         // NextPoint: Dot I (170, 170)
		"2"           // Label untuk end point
	);


	// Posisi Circle D
	vec2 posD = vec2(0, radiusCircle);  // (0, 240)

	// Posisi di +45° dari Dot I
	float angle3 = angle2;  // 45°
	vec2 posEnd3 = vec2(
		posD.x + cos(angle3) * radiusCircle,
		posD.y + sin(angle3) * radiusCircle
	);

	// Buat octagramLine3 dengan nextPoint (akan menggambar 2 garis)
	octagramLine3 = std::make_unique<OctagramLine>(
		posI,         // Start Dot I
		posEnd3,      // End
		posD,         // NextPoint: Circle D (0, 240)
		"3"           // Label untuk end point
	);

	float angle4 = 3 * angle2;  // 135°
	vec2 posEnd4 = vec2(
		posD.x + cos(angle4) * radiusCircle,
		posD.y + sin(angle4) * radiusCircle
	); 
	vec2 posH = vec2(cos(angle4) * radiusCircle, sin(angle4) * radiusCircle);
	octagramLine4 = std::make_unique<OctagramLine>(
		posD,         // Start: Dot D
		posEnd4,
		posH,
		"4"           
	);

	vec2 posC = vec2(-radiusCircle, 0);
	float angle5 = angle4;  // 135°
	vec2 posEnd5 = vec2(
		posC.x + cos(angle5) * radiusCircle,
		posC.y + sin(angle5) * radiusCircle
	);
	octagramLine5 = std::make_unique<OctagramLine>(
		posH,         // Start: posH crossLine H
		posEnd5,
		posC,
		"5"
	);


	float angle6 = -angle5;  // -135°
	vec2 posEnd6 = vec2(
		posC.x + cos(angle6) * radiusCircle,
		posC.y + sin(angle6) * radiusCircle
	);
	vec2 posF = vec2(cos(angle6) * radiusCircle, sin(angle6) * radiusCircle);
	octagramLine6 = std::make_unique<OctagramLine>(
		posC,         // Start: circle C
		posEnd6,
		posF,
		"6"
	);

	float angle7 = angle6;  // -135°
	vec2 posEnd7 = vec2(
		posE.x + cos(angle7) * radiusCircle,
		posE.y + sin(angle7) * radiusCircle
	);
	octagramLine7 = std::make_unique<OctagramLine>(
		posF,         // Start: posF crossline F
		posEnd7,
		posE,
		"7"
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
	rectangleLineHtoF->update();
	//OctagramLine update
	octagramLine0->update();
	octagramLine1->update();
	octagramLine2->update();
	octagramLine3->update();
	octagramLine4->update();
	octagramLine5->update();
	octagramLine6->update();
	octagramLine7->update();


	// Update sequential drawing logic
	if (sequentialMode) {
		updateSequentialDrawing();
	}

	// Update sequential load logic dari FileManager
	fileManager.updateSequentialLoad(customLines);
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
	rectangleLineHtoF->draw();
	//Draw OctagramLine
	octagramLine0->draw();
	octagramLine1->draw();
	octagramLine2->draw();
	octagramLine3->draw();
	octagramLine4->draw();
	octagramLine5->draw();
	octagramLine6->draw();
	octagramLine7->draw();

	// Draw custom lines dan UI elements
	drawCustomLinesAndUI();
}

//--------------------------------------------------------------
void ofApp::drawCustomLinesAndUI() {
	// Update selection state dan draw semua custom lines
	for (int i = 0; i < customLines.size(); i++) {
		customLines[i].setSelected(i == selectedLineIndex);
		customLines[i].draw();
	}

	// Draw curve value label untuk garis yang selected
	if (selectedLineIndex >= 0 && selectedLineIndex < customLines.size()) {
		const CustomLine& line = customLines[selectedLineIndex];
		if (line.getPoints().size() >= 2) {
			vec2 midPoint = (line.getPoints()[0] + line.getPoints()[1]) / 2.0f;

			ofPushStyle();
			ofSetColor(0, 0, 0);  // Hitam untuk label
			fontNormal.drawString("Curve: " + ofToString(line.getCurve(), 1), midPoint.x + 10, midPoint.y - 10);
			ofPopStyle();
		}
	}

	// Draw preview polyline (sedang drag)
	if (drawState == DRAGGING && currentPolylinePoints.size() > 1) {
		ofPushStyle();
		ofSetColor(0, 0, 255, 150);  // Biru transparan
		ofSetLineWidth(mouseLineWidth);  // Pakai mouseLineWidth untuk preview

		// Gambar preview polyline
		ofPolyline previewPolyline;
		for (auto& point : currentPolylinePoints) {
			previewPolyline.addVertex(point.x, point.y);
		}
		previewPolyline.draw();

		ofPopStyle();
	}

	// Draw dot highlights (hover state) - HANYA ketika TIDAK sedang drag DAN dots visible
	if (drawState != DRAGGING && dotsVisible) {
		vector<DotInfo> dots = getAllDots();
		for (auto& dot : dots) {
			if (isMouseOverDot(mousePos, dot.position)) {
				// Highlight dot
				ofPushStyle();
				ofSetColor(255, 0, 0, 200);  // Red highlight
				ofDrawCircle(dot.position, threshold);  // Larger circle
				ofPopStyle();
			}
		}
	}
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
		if (isCtrlPressed) {
			// CTRL+DEL: Clear semua custom lines - HANYA jika TIDAK sedang load
			if (!fileManager.isLoadSequentialMode() && !fileManager.isLoadParallelMode()) {
				FileManager::clearCustomLines(customLines);
			}
			return;  // Jangan lanjut ke hideAllShapes()
		} else {
			// DEL saja: Hide semua shapes (hanya jika TIDAK sequential drawing)
			if (!sequentialMode) {
				hideAllShapes();
			}
		}
	}

	if (key == OF_KEY_BACKSPACE) {
		// Jika ada customLine yang terselect, hapus customLine tersebut
		if (selectedLineIndex != -1) {
			customLines.erase(customLines.begin() + selectedLineIndex);
			selectedLineIndex = -1;  // Reset selection
		} else {
			// Jika tidak ada customLine terselect, toggle CartesianAxes
			if (cartesianAxes->showing) {
				cartesianAxes->hide();
			} else {
				cartesianAxes->show();
			}
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

	// Handle CTRL key ditekan
	if (key == 3683 || key == OF_KEY_CONTROL) {  // 3683 = CTRL pada Windows
		isCtrlPressed = true;
		return;  // Jangan lanjut ke logic lain
	}

	// Handle tombol saat CTRL aktif - gunakan switch-case
	if (isCtrlPressed) {
		switch(key) {
			case 'z':
			case 'Z':
			case 26:  // CTRL+Z (ASCII 26)
				undoLastLine();
				break;

			case 's':
			case 'S':
			case 19:  // CTRL+S (ASCII 19)
				fileManager.saveCustomLines(customLines);
				break;

			case 'o':
			case 'O':
			case 15:  // CTRL+O (ASCII 15)
				// Cek apakah SHIFT juga ditekan
				if (ofGetKeyPressed(OF_KEY_SHIFT)) {
					fileManager.loadCustomLinesSequential(customLines);  // Sequential load dengan animasi
				} else {
					fileManager.loadCustomLines(customLines);  // Load semua sekaligus
				}
				break;
		}
	}
}

//--------------------------------------------------------------
// Interactive Line Creation Helpers
void ofApp::updateDotsCache() {
	cachedDots.clear();

	// Hanya tambahkan dots jika shape-nya visible (showing == true)

	// Circle centers - ambil langsung dari CircleShape objects
	if (circleA && circleA->showing) {
		cachedDots.push_back({vec2(circleA->posX, circleA->posY), "Circle"});
		cachedDots.push_back({vec2(circleB->posX, circleB->posY), "Circle"});
		cachedDots.push_back({vec2(circleC->posX, circleC->posY), "Circle"});
		cachedDots.push_back({vec2(circleD->posX, circleD->posY), "Circle"});
		cachedDots.push_back({vec2(circleE->posX, circleE->posY), "Circle"});
	}

	// CrossLine dots - ambil radiusDot yang sudah dihitung di draw() (SCALABLE!)
	if (crossLineF && crossLineF->showing) {
		// F, G, H, I pada radius
		cachedDots.push_back({crossLineF->radiusDot, "CrossLine"});
		cachedDots.push_back({crossLineG->radiusDot, "CrossLine"});
		cachedDots.push_back({crossLineH->radiusDot, "CrossLine"});
		cachedDots.push_back({crossLineI->radiusDot, "CrossLine"});

		// J, K, L, M ends
		cachedDots.push_back({crossLineF->end, "CrossLine"});
		cachedDots.push_back({crossLineG->end, "CrossLine"});
		cachedDots.push_back({crossLineH->end, "CrossLine"});
		cachedDots.push_back({crossLineI->end, "CrossLine"});
	}

	// Parallelogram intersections - ambil langsung dari ParallelogramLine objects (SCALABLE!)
	if (parallelogramCtoE && parallelogramCtoE->showing) {
		cachedDots.push_back({parallelogramCtoE->intersecCrossLine, "Parallelogram"});
		cachedDots.push_back({parallelogramEtoB->intersecCrossLine, "Parallelogram"});
		cachedDots.push_back({parallelogramBtoD->intersecCrossLine, "Parallelogram"});
		cachedDots.push_back({parallelogramDtoC->intersecCrossLine, "Parallelogram"});
	}

	// Rectangle intersections - ambil langsung dari RectangleLine objects (SCALABLE!)
	if (rectangleLineFtoG && rectangleLineFtoG->showing) {
		cachedDots.push_back({rectangleLineFtoG->intersec1, "Rectangle"});
		cachedDots.push_back({rectangleLineFtoG->intersec2, "Rectangle"});
		cachedDots.push_back({rectangleLineGtoI->intersec1, "Rectangle"});
		cachedDots.push_back({rectangleLineGtoI->intersec2, "Rectangle"});
		cachedDots.push_back({rectangleLineItoH->intersec1, "Rectangle"});
		cachedDots.push_back({rectangleLineItoH->intersec2, "Rectangle"});
		cachedDots.push_back({rectangleLineHtoF->intersec1, "Rectangle"});
		cachedDots.push_back({rectangleLineHtoF->intersec2, "Rectangle"});
	}

	// Octagram endpoints - ambil langsung dari OctagramLine objects (SCALABLE!)
	if (octagramLine0 && octagramLine0->showing) {
		cachedDots.push_back({octagramLine0->end, "Octagram"});
		cachedDots.push_back({octagramLine1->end, "Octagram"});
		cachedDots.push_back({octagramLine2->end, "Octagram"});
		cachedDots.push_back({octagramLine3->end, "Octagram"});
		cachedDots.push_back({octagramLine4->end, "Octagram"});
		cachedDots.push_back({octagramLine5->end, "Octagram"});
		cachedDots.push_back({octagramLine6->end, "Octagram"});
		cachedDots.push_back({octagramLine7->end, "Octagram"});
	}

	dotsCacheDirty = false;
}

//--------------------------------------------------------------
const vector<ofApp::DotInfo>& ofApp::getAllDots() {
	// Lazy update cache kalau dirty
	if (dotsCacheDirty) {
		updateDotsCache();
	}

	return cachedDots;  // Return reference, tidak copy
}

//--------------------------------------------------------------
bool ofApp::isMouseOverDot(vec2 mousePos, vec2 dotPos) {
	//Euclidean distance
	//ukuran jarak antara 2 titik
	float distance = glm::length(mousePos - dotPos);
	return distance <= this->threshold;
}

bool ofApp::lineExists(vec2 from, vec2 to) {
	// Cek apakah ada line dengan titik awal dan akhir yang sama
	for (auto& line : customLines) {
		const vector<vec2>& points = line.getPoints();
		if (points.size() >= 2) {
			vec2 lineStart = points.front();
			vec2 lineEnd = points.back();

			if ((glm::length(lineStart - from) < 1.0f && glm::length(lineEnd - to) < 1.0f) ||
				(glm::length(lineStart - to) < 1.0f && glm::length(lineEnd - from) < 1.0f)) {
				return true;
			}
		}
	}
	return false;
}

//--------------------------------------------------------------
bool ofApp::isMouseOverLine(vec2 mousePos, vec2 lineStart, vec2 lineEnd, float lineWidth) {
	// Hitung jarak dari titik ke garis (point-to-line distance)
	// Formula: jarak = |(end - start) × (start - point)| / |end - start|
	vec2 lineVec = lineEnd - lineStart;
	vec2 pointVec = mousePos - lineStart;
	float lineLength = glm::length(lineVec);

	if (lineLength == 0) return false;

	// Project pointVec ke lineVec
	float projection = glm::dot(pointVec, lineVec) / lineLength;
	vec2 closestPoint = lineStart + (lineVec / lineLength) * projection;

	// Clamp projection ke line segment (biar tidak extend)
	if (projection < 0) closestPoint = lineStart;
	if (projection > 1) closestPoint = lineEnd;

	// Hitung jarak dari mousePos ke closestPoint di garis
	float distance = glm::length(mousePos - closestPoint);

	// Threshold = setengah line width + sedikit buffer
	return distance <= (lineWidth / 2.0f) + 2.0f;
}

//--------------------------------------------------------------
float ofApp::distanceToLine(vec2 point, vec2 lineStart, vec2 lineEnd, float curve) {
	// Hitung jarak terpendek dari point ke garis/lengkungan

	// Jika curve != 0, hitung jarak ke bezier curve
	if (curve != 0.0f) {
		// Hitung control point
		vec2 midPoint = (lineStart + lineEnd) / 2.0f;
		vec2 dir = lineEnd - lineStart;
		vec2 perp = vec2(-dir.y, dir.x);
		float perpLen = glm::length(perp);
		if (perpLen > 0) {
			perp = perp / perpLen;
		}
		vec2 controlPoint = midPoint + perp * curve;

		// Sampling bezier curve untuk cari jarak minimum
		float minDistance = 999999.0f;
		int samples = 50;

		for (int i = 0; i <= samples; i++) {
			float t = (float)i / samples;
			// Quadratic bezier
			vec2 curvePoint =
				lineStart * (1 - t) * (1 - t) +
				controlPoint * 2 * (1 - t) * t +
				lineEnd * t * t;

			float distance = glm::length(point - curvePoint);
			if (distance < minDistance) {
				minDistance = distance;
			}
		}

		return minDistance;
	} else {
		// Hitung jarak ke garis lurus (original code)
		vec2 lineVec = lineEnd - lineStart;
		vec2 pointVec = point - lineStart;
		float lineLengthSq = glm::dot(lineVec, lineVec);

		if (lineLengthSq == 0) return 0.0f;

		// Project pointVec ke lineVec (hasilnya parameter t dalam [0,1])
		float projection = glm::dot(pointVec, lineVec) / lineLengthSq;

		// Clamp projection ke line segment (biar tidak extend)
		projection = ofClamp(projection, 0.0f, 1.0f);

		vec2 closestPoint = lineStart + lineVec * projection;

		// Hitung jarak dari point ke closestPoint
		return glm::length(point - closestPoint);
	}
}

//--------------------------------------------------------------
int ofApp::getLineIndexAtPosition(vec2 pos) {
	// Cari garis dengan JARAK TERDEKAT
	int closestLineIndex = -1;
	float closestDistance = 999999.0f;

	for (int i = 0; i < customLines.size(); i++) {
		const CustomLine& line = customLines[i];
		const vector<vec2>& points = line.getPoints();
		if (points.size() >= 2) {
			vec2 start = points[0];
			vec2 end = points[1];

			// Skip invalid lines (start == end)
			if (glm::length(start - end) < 1.0f) {
				continue;
			}

			// Hitung jarak ke garis ini (dengan curve support)
			float distance = distanceToLine(pos, start, end, line.getCurve());

			// Update jika lebih dekat
			if (distance < closestDistance) {
				closestDistance = distance;
				closestLineIndex = i;
			}
		}
	}

	// Return line terdekat (jika dalam threshold)
	if (closestDistance <= (mouseLineWidth / 2.0f) + 2.0f) {
		return closestLineIndex;
	}

	return -1;  // Tidak ada yang diklik
}

//--------------------------------------------------------------
void ofApp::undoLastLine() {
	// Hapus garis terakhir yang dibuat user
	if (!customLines.empty()) {
		customLines.pop_back();
	}
}

//--------------------------------------------------------------
// File operations sekarang ditangani oleh FileManager class

//--------------------------------------------------------------
// Mouse Event Handlers
void ofApp::mousePressed(int x, int y, int button) {
	// Logic lama: Cursor toggle dengan right click (button 2)
	if (button == 2) {
		cursorVisible = !cursorVisible;
		if (cursorVisible) ofShowCursor();
		if (!cursorVisible) ofHideCursor();
		return;  // Jangan lanjut ke interactive line creation untuk right click
	}

	// Logic untuk interactive line creation & line selection
	if (button == 0) {
		// Adjust mouse position untuk center translation
		vec2 adjustedMousePos(x - ofGetWidth() / 2, y - ofGetHeight() / 2);
		vector<DotInfo> dots = getAllDots();

		// Check jika klik di atas dot - HANYA bisa mulai dari dot
		bool clickedOnDot = false;
		for (auto& dot : dots) {
			if (isMouseOverDot(adjustedMousePos, dot.position)) {
				drawState = DRAGGING;
				currentPolylinePoints.clear();
				currentPolylinePoints.push_back(dot.position);  // Start dari dot
				startDotPos = dot.position;
				mousePos = adjustedMousePos;
				clickedOnDot = true;

				// Deselect line yang mungkin terpilih
				selectedLineIndex = -1;

				return;
			}
		}

		// Kalau tidak klik di dot, cek apakah klik di garis untuk SELECT
		if (!clickedOnDot) {
			int lineIndex = getLineIndexAtPosition(adjustedMousePos);
			if (lineIndex >= 0) {
				// Select garis ini
				selectedLineIndex = lineIndex;
			} else {
				// Klik di tempat kosong → deselect
				selectedLineIndex = -1;
			}
		}
	}
}

void ofApp::mouseDragged(int x, int y, int button) {
	if (drawState == DRAGGING) {
		// Adjust mouse position untuk center translation
		vec2 currentPos = vec2(x - ofGetWidth() / 2, y - ofGetHeight() / 2);
		mousePos = currentPos;

		// Untuk garis lurus, kita hanya perlu 2 titik: start dan end
		// currentPolylinePoints[0] = start (diset di mousePressed)
		// currentPolylinePoints[1] = end (diupdate setiap drag)
		if (currentPolylinePoints.size() < 2) {
			currentPolylinePoints.push_back(currentPos);  // Add end point
		} else {
			currentPolylinePoints[1] = currentPos;  // Update end point
		}
	}
}

void ofApp::mouseReleased(int x, int y, int button) {
	if (drawState != DRAGGING) {
		return;
	}

	// Adjust mouse position untuk center translation
	vec2 releasePos(x - ofGetWidth() / 2, y - ofGetHeight() / 2);
	vector<DotInfo> dots = getAllDots();

	// Check jika release di atas valid dot - HANYA bisa selesai di dot
	for (auto& dot : dots) {
		if (isMouseOverDot(releasePos, dot.position)) {
			// Cek apakah line sudah ada (duplicate check)
			if (!lineExists(startDotPos, dot.position)) {
				// Cek apakah start != end (bukan garis panjang 0)
				if (glm::length(startDotPos - dot.position) > 1.0f) {
					// Update end point ke posisi dot yang dilepas
					if (currentPolylinePoints.size() < 2) {
						currentPolylinePoints.push_back(dot.position);
					} else {
						currentPolylinePoints[1] = dot.position;
					}

					// Simpan polyline dengan 2 points (start dan end)
					CustomLine newLine(currentPolylinePoints, ofColor(0, 0, 255), mouseLineWidth);
					customLines.push_back(newLine);
				}
			}
			break;
		}
	}

	// Clear dan reset state
	currentPolylinePoints.clear();
	drawState = IDLE;
}

//--------------------------------------------------------------
void ofApp::mouseScrolled(int x, int y, float scrollX, float scrollY) {
	// Update curve untuk garis yang sedang dipilih
	if (selectedLineIndex >= 0 && selectedLineIndex < customLines.size()) {
		// scrollY positif = scroll up, scrollY negatif = scroll down
		float curveSpeed = 5.0f;  // Kecepatan perubahan curve
		float newCurve = customLines[selectedLineIndex].getCurve() + scrollY * curveSpeed;
		customLines[selectedLineIndex].setCurve(newCurve);
	}
}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y) {
	// Update mouse position untuk hover detection (adjust untuk center translation)
	mousePos = vec2(x - ofGetWidth()/2, y - ofGetHeight()/2);
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){
	if (key == OF_KEY_CONTROL) isCtrlPressed = false;
}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y) {}
void ofApp::mouseExited(int x, int y) {}
void ofApp::windowResized(int w, int h) {}
void ofApp::gotMessage(ofMessage msg) {}
void ofApp::dragEvent(ofDragInfo dragInfo) {}

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
		rectangleLineItoH->showing && rectangleLineHtoF->showing &&
		octagramLine0->showing && octagramLine1->showing && 
		octagramLine2->showing && octagramLine3->showing && 
		octagramLine4->showing && octagramLine5->showing && 
		octagramLine6->showing && octagramLine7->showing;

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
						!rectangleLineItoH->isComplete() || !rectangleLineHtoF->isComplete() ||
						!octagramLine0->isComplete() || !octagramLine1->isComplete() || 
						!octagramLine2->isComplete() || !octagramLine3->isComplete() || 
						!octagramLine4->isComplete() || !octagramLine5->isComplete() || 
						!octagramLine6->isComplete() || !octagramLine7->isComplete();

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
	rectangleLineHtoF->hide();
	octagramLine0->hide();
	octagramLine1->hide();
	octagramLine2->hide();
	octagramLine3->hide();
	octagramLine4->hide();
	octagramLine5->hide();
	octagramLine6->hide();
	octagramLine7->hide();

	// Set sequential mode untuk octagramLine
	octagramLine0->setSequentialMode(true);
	octagramLine1->setSequentialMode(true);
	octagramLine2->setSequentialMode(true);
	octagramLine3->setSequentialMode(true);
	octagramLine4->setSequentialMode(true);
	octagramLine5->setSequentialMode(true);
	octagramLine6->setSequentialMode(true);
	octagramLine7->setSequentialMode(true);

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
		case 17: currentShape = rectangleLineHtoF.get(); break;
		case 18: currentShape = octagramLine0.get(); break;
		case 19: currentShape = octagramLine1.get(); break;
		case 20: currentShape = octagramLine2.get(); break;
		case 21: currentShape = octagramLine3.get(); break;
		case 22: currentShape = octagramLine4.get(); break;
		case 23: currentShape = octagramLine5.get(); break;
		case 24: currentShape = octagramLine6.get(); break;
		case 25: currentShape = octagramLine7.get(); break;
	}

	// Cek jika current shape sudah complete
	if (currentShape && currentShape->isComplete()) {
		// Pindah ke shape berikutnya
		currentShapeIndex++;

		// Show shape berikutnya jika masih ada
		if (currentShapeIndex <= 25) {
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
				case 17: rectangleLineHtoF->show(); break;
				case 18: octagramLine0->show(); break;
				case 19: octagramLine1->show(); break;
				case 20: octagramLine2->show(); break;
				case 21: octagramLine3->show(); break;
				case 22: octagramLine4->show(); break;
				case 23: octagramLine5->show(); break;
				case 24: octagramLine6->show(); break;
				case 25: octagramLine7->show(); break;
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
		rectangleLineHtoF->showLabel();
		octagramLine0->showLabel();
		octagramLine1->showLabel();
		octagramLine2->showLabel();
		octagramLine3->showLabel();
		octagramLine4->showLabel();
		octagramLine5->showLabel();
		octagramLine6->showLabel();
		octagramLine7->showLabel();
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
		rectangleLineHtoF->hideLabel();
		octagramLine0->hideLabel();
		octagramLine1->hideLabel();
		octagramLine2->hideLabel();
		octagramLine3->hideLabel();
		octagramLine4->hideLabel();
		octagramLine5->hideLabel();
		octagramLine6->hideLabel();
		octagramLine7->hideLabel();
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
		rectangleLineHtoF->showDot();
		octagramLine0->showDot();
		octagramLine1->showDot();
		octagramLine2->showDot();
		octagramLine3->showDot();
		octagramLine4->showDot();
		octagramLine5->showDot();
		octagramLine6->showDot();
		octagramLine7->showDot();
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
		rectangleLineHtoF->hideDot();
		octagramLine0->hideDot();
		octagramLine1->hideDot();
		octagramLine2->hideDot();
		octagramLine3->hideDot();
		octagramLine4->hideDot();
		octagramLine5->hideDot();
		octagramLine6->hideDot();
		octagramLine7->hideDot();
	}
}

//--------------------------------------------------------------
void ofApp::hideAllShapes() {
	// Set paralel mode untuk octagramLine (kedua line hide barengan)
	octagramLine0->setSequentialMode(false);
	octagramLine1->setSequentialMode(false);
	octagramLine2->setSequentialMode(false);
	octagramLine3->setSequentialMode(false);
	octagramLine4->setSequentialMode(false);
	octagramLine5->setSequentialMode(false);
	octagramLine6->setSequentialMode(false);
	octagramLine7->setSequentialMode(false);

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
	rectangleLineHtoF->hide();
	octagramLine0->hide();
	octagramLine1->hide();
	octagramLine2->hide();
	octagramLine3->hide();
	octagramLine4->hide();
	octagramLine5->hide();
	octagramLine6->hide();
	octagramLine7->hide();

	// Reset sequential completed flag agar bisa jalankan lagi
	sequentialCompleted = false;

	// Mark dots cache dirty karena visibility berubah
	dotsCacheDirty = true;
}

//--------------------------------------------------------------
void ofApp::showAllShapes() {
	// Set paralel mode untuk octagramLine (kedua line berjalan barengan)
	octagramLine0->setSequentialMode(false);
	octagramLine1->setSequentialMode(false);
	octagramLine2->setSequentialMode(false);
	octagramLine3->setSequentialMode(false);
	octagramLine4->setSequentialMode(false);
	octagramLine5->setSequentialMode(false);
	octagramLine6->setSequentialMode(false);
	octagramLine7->setSequentialMode(false);

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
	rectangleLineHtoF->show();
	octagramLine0->show();
	octagramLine1->show();
	octagramLine2->show();
	octagramLine3->show();
	octagramLine4->show();
	octagramLine5->show();
	octagramLine6->show();
	octagramLine7->show();

	// Reset sequential completed flag agar bisa jalankan lagi
	sequentialCompleted = false;

	// Mark dots cache dirty karena visibility berubah
	dotsCacheDirty = true;
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
	rectangleLineHtoF->setLineWidth(currentLineWidth);
	octagramLine0->setLineWidth(currentLineWidth);
	octagramLine1->setLineWidth(currentLineWidth);
	octagramLine2->setLineWidth(currentLineWidth);
	octagramLine3->setLineWidth(currentLineWidth);
	octagramLine4->setLineWidth(currentLineWidth);
	octagramLine5->setLineWidth(currentLineWidth);
	octagramLine6->setLineWidth(currentLineWidth);
	octagramLine7->setLineWidth(currentLineWidth);
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
	rectangleLineHtoF->setLineWidth(currentLineWidth);
	octagramLine0->setLineWidth(currentLineWidth);
	octagramLine1->setLineWidth(currentLineWidth);
	octagramLine2->setLineWidth(currentLineWidth);
	octagramLine3->setLineWidth(currentLineWidth);
	octagramLine4->setLineWidth(currentLineWidth);
	octagramLine5->setLineWidth(currentLineWidth);
	octagramLine6->setLineWidth(currentLineWidth);
	octagramLine7->setLineWidth(currentLineWidth);
}


