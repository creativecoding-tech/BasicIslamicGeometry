#include "BasicZelligeTemplate.h"
#include "../../shape/CircleShape.h"
#include "../../shape/CartesianAxes.h"
#include "../../shape/CrossLine.h"
#include "../../shape/ParallelogramLine.h"
#include "../../shape/RectangleLine.h"
#include "../../shape/OctagramLine.h"
#include "../../operation/FileManager.h"
#include "../../imgui/imgui.h"
#include "../../ofApp.h"

using glm::vec2;

//--------------------------------------------------------------
std::string BasicZelligeTemplate::getName() {
	return "Basic Zellige";
}

//--------------------------------------------------------------
std::string BasicZelligeTemplate::getDescription() {
	return "Moroccan Islamic Geometric Pattern - Basic Zellige mosaic with 8-fold symmetry";
}

//--------------------------------------------------------------
void BasicZelligeTemplate::setupShapes() {
	// Panggil semua setup methods dengan urutan yang BENAR
	// CartesianAxes DULU sebagai fondasi/sumbu koordinat
	// Note: 'shapes' adalah protected member dari SacredGeometryTemplate
	setupCartesianAxes();
	setupCircles();
	setupCrossLines();
	setupParallelograms();
	setupRectangleLines();
	setupOctagramLines();
}

//--------------------------------------------------------------
void BasicZelligeTemplate::setupCircles() {
	// Clear daftar circles dulu
	circleIndices.clear();

	// Circle A: Center (0,0) → distance=0
	shapes.push_back(std::make_unique<CircleShape>(radius, "A", 0, 0));
	circleIndices.push_back(shapes.size() - 1);  // Simpan index Circle A

	// Circle B: Kanan (0°) → distance=radius
	shapes.push_back(std::make_unique<CircleShape>(radius, "B", 0, radius));
	circleIndices.push_back(shapes.size() - 1);  // Simpan index Circle B

	// Circle C: Kiri (180°) → distance=radius
	shapes.push_back(std::make_unique<CircleShape>(radius, "C", PI, radius));
	circleIndices.push_back(shapes.size() - 1);  // Simpan index Circle C

	// Circle D: Atas (90°) → distance=radius
	shapes.push_back(std::make_unique<CircleShape>(radius, "D", PI/2, radius));
	circleIndices.push_back(shapes.size() - 1);  // Simpan index Circle D

	// Circle E: Bawah (270°) → distance=radius
	shapes.push_back(std::make_unique<CircleShape>(radius, "E", -PI/2, radius));
	circleIndices.push_back(shapes.size() - 1);  // Simpan index Circle E
}

//--------------------------------------------------------------
void BasicZelligeTemplate::setupCartesianAxes() {
	shapes.push_back(std::make_unique<CartesianAxes>(radius));
}

//--------------------------------------------------------------
void BasicZelligeTemplate::setupCrossLines() {
	// Clear daftar crosslines dulu
	crossLineIndices.clear();

	// CrossLine F
	shapes.push_back(std::make_unique<CrossLine>(vec2(0, 0), vec2(-radius, -radius), "F", "J", radius));
	crossLineIndices.push_back(shapes.size() - 1);  // Simpan index CrossLine F

	// CrossLine G
	shapes.push_back(std::make_unique<CrossLine>(vec2(0, 0), vec2(radius, -radius), "G", "K", radius));
	crossLineIndices.push_back(shapes.size() - 1);  // Simpan index CrossLine G

	// CrossLine H
	shapes.push_back(std::make_unique<CrossLine>(vec2(0, 0), vec2(-radius, radius), "H", "L", radius));
	crossLineIndices.push_back(shapes.size() - 1);  // Simpan index CrossLine H

	// CrossLine I
	shapes.push_back(std::make_unique<CrossLine>(vec2(0, 0), vec2(radius, radius), "I", "M", radius));
	crossLineIndices.push_back(shapes.size() - 1);  // Simpan index CrossLine I
}

//--------------------------------------------------------------
void BasicZelligeTemplate::setupParallelograms() {
	// Clear daftar parallelograms dulu
	parallelogramIndices.clear();

	// Parallelogram dengan Polar Thinking
	// Intersection point dihitung menggunakan trigonometri: x = cos(angle) * distance, y = sin(angle) * distance

	// C→E memotong F: Northwest (-120, -120)
	float angleC_E_F = -3 * PI / 4;  // -135 derajat (Northwest)
	float distC_E_F = radius * sqrt(2) / 2;
	vec2 intersecC_E_F = vec2(cos(angleC_E_F) * distC_E_F, sin(angleC_E_F) * distC_E_F);
	shapes.push_back(std::make_unique<ParallelogramLine>(vec2(-radius, 0), vec2(0, -radius), intersecC_E_F, "N", radius));
	parallelogramIndices.push_back(shapes.size() - 1);  // Simpan index Parallelogram N

	// E→B memotong G: Northeast (120, -120)
	float angleE_B_G = -PI / 4;  // -45 derajat (Northeast)
	float distE_B_G = radius * sqrt(2) / 2;
	vec2 intersecE_B_G = vec2(cos(angleE_B_G) * distE_B_G, sin(angleE_B_G) * distE_B_G);
	shapes.push_back(std::make_unique<ParallelogramLine>(vec2(0, -radius), vec2(radius, 0), intersecE_B_G, "O", radius));
	parallelogramIndices.push_back(shapes.size() - 1);  // Simpan index Parallelogram O

	// B→D memotong I: Southeast (120, 120)
	float angleB_D_I = PI / 4;  // 45 derajat (Southeast)
	float distB_D_I = radius * sqrt(2) / 2;
	vec2 intersecB_D_I = vec2(cos(angleB_D_I) * distB_D_I, sin(angleB_D_I) * distB_D_I);
	shapes.push_back(std::make_unique<ParallelogramLine>(vec2(radius, 0), vec2(0, radius), intersecB_D_I, "P", radius));
	parallelogramIndices.push_back(shapes.size() - 1);  // Simpan index Parallelogram P

	// D→C memotong H: Southwest (-120, 120)
	float angleD_C_H = 3 * PI / 4;  // 135 derajat (Southwest)
	float distD_C_H = radius * sqrt(2) / 2;
	vec2 intersecD_C_H = vec2(cos(angleD_C_H) * distD_C_H, sin(angleD_C_H) * distD_C_H);
	shapes.push_back(std::make_unique<ParallelogramLine>(vec2(0, radius), vec2(-radius, 0), intersecD_C_H, "Q", radius));
	parallelogramIndices.push_back(shapes.size() - 1);  // Simpan index Parallelogram Q
}

//--------------------------------------------------------------
void BasicZelligeTemplate::setupRectangleLines() {
	// Clear daftar rectangleLines dulu
	rectangleLineIndices.clear();

	// RectangleLine dari F ke G dengan intersection points
	float angleF = -3 * PI / 4;  // -135°
	vec2 posF = vec2(cos(angleF) * radius, sin(angleF) * radius);

	float angleG = -PI / 4;  // -45°
	vec2 posG = vec2(cos(angleG) * radius, sin(angleG) * radius);

	// Hitung intersection F→G ∩ C→E (Dot R)
	vec2 intersecR = vec2(-radius * (1 - sqrt(2) / 2), posF.y);

	// Hitung intersection F→G ∩ B→E (Dot S)
	vec2 intersecS = vec2(radius * (1 - sqrt(2) / 2), posF.y);

	shapes.push_back(std::make_unique<RectangleLine>(posF, posG, intersecR, intersecS, "R", "S", radius));
	rectangleLineIndices.push_back(shapes.size() - 1);  // Simpan index RectangleLine RS

	// RectangleLine G ke I
	float angleI = PI / 4; //45°
	vec2 posI = vec2(cos(angleI) * radius, sin(angleI) * radius);

	// Hitung intersection G→I ∩ B→E (Dot T)
	vec2 intersecT = vec2(radius * sqrt(2) / 2, radius * (sqrt(2) / 2 - 1));

	// Hitung intersection G→I ∩ B→D (Dot U)
	vec2 intersecU = vec2(radius * sqrt(2) / 2, radius * (1 - sqrt(2) / 2));

	shapes.push_back(std::make_unique<RectangleLine>(posG, posI, intersecT, intersecU, "T", "U", radius));
	rectangleLineIndices.push_back(shapes.size() - 1);  // Simpan index RectangleLine TU

	// RectangleLine I ke H
	float angleH = 3 * PI / 4; //135°
	vec2 posH = vec2(cos(angleH) * radius, sin(angleH) * radius);
	vec2 intersecV = vec2(radius * (1 - sqrt(2) / 2), posH.y);
	vec2 intersecW = vec2(-radius * (1 - sqrt(2) / 2), posH.y);
	shapes.push_back(std::make_unique<RectangleLine>(posI, posH, intersecV, intersecW, "V", "W", radius));
	rectangleLineIndices.push_back(shapes.size() - 1);  // Simpan index RectangleLine VW

	// RectangleLine H ke F
	vec2 intersecX = vec2(-radius * sqrt(2) / 2, radius * (1 - sqrt(2) / 2));
	vec2 intersecY = vec2(-radius * sqrt(2) / 2, radius * (sqrt(2) / 2 - 1));
	shapes.push_back(std::make_unique<RectangleLine>(posH, posF, intersecX, intersecY, "X", "Y", radius));
	rectangleLineIndices.push_back(shapes.size() - 1);  // Simpan index RectangleLine XY
}

//--------------------------------------------------------------
void BasicZelligeTemplate::setupOctagramLines() {
	// Clear daftar octagramLines dulu
	octagramLineIndices.clear();

	// Posisi Circle E
	vec2 posE = vec2(0, -radius);

	// Posisi di -45° dari Circle E dengan distance r = 240
	float angle0 = -PI / 4;  // -45°
	vec2 posEnd = vec2(posE.x + cos(angle0) * radius, posE.y + sin(angle0) * radius);

	// Posisi Dot G (di -45° dari center)
	vec2 posG = vec2(cos(angle0) * radius, sin(angle0) * radius);

	shapes.push_back(std::make_unique<OctagramLine>(posE, posEnd, posG, "0", radius));
	octagramLineIndices.push_back(shapes.size() - 1);  // Simpan index OctagramLine 0

	// OctagramLine 2: Dot G → -45° dari Circle B → pusat Circle B
	vec2 posB = vec2(radius, 0);  // (240, 0)
	vec2 posEnd1 = vec2(posB.x + cos(angle0) * radius, posB.y + sin(angle0) * radius);
	shapes.push_back(std::make_unique<OctagramLine>(posG, posEnd1, posB, "1", radius));
	octagramLineIndices.push_back(shapes.size() - 1);  // Simpan index OctagramLine 1

	// OctagramLine 3: Circle B → 45° dari Circle B → Dot I
	float angle2 = PI / 4;  // 45°
	vec2 posEnd2 = vec2(posB.x + cos(angle2) * radius, posB.y + sin(angle2) * radius);
	vec2 posI = vec2(cos(angle2) * radius, sin(angle2) * radius);
	shapes.push_back(std::make_unique<OctagramLine>(posB, posEnd2, posI, "2", radius));
	octagramLineIndices.push_back(shapes.size() - 1);  // Simpan index OctagramLine 2

	// OctagramLine 4: Circle D → +45° dari Dot I → Circle D
	vec2 posD = vec2(0, radius);
	float angle3 = angle2;  // 45°
	vec2 posEnd3 = vec2(posD.x + cos(angle3) * radius, posD.y + sin(angle3) * radius);
	shapes.push_back(std::make_unique<OctagramLine>(posI, posEnd3, posD, "3", radius));
	octagramLineIndices.push_back(shapes.size() - 1);  // Simpan index OctagramLine 3

	// OctagramLine 5: Dot D → posEnd
	float angle4 = 3 * angle2;  // 135°
	vec2 posEnd4 = vec2(posD.x + cos(angle4) * radius, posD.y + sin(angle4) * radius);
	vec2 posH = vec2(cos(angle4) * radius, sin(angle4) * radius);
	shapes.push_back(std::make_unique<OctagramLine>(posD, posEnd4, posH, "4", radius));
	octagramLineIndices.push_back(shapes.size() - 1);  // Simpan index OctagramLine 4

	// OctagramLine 6: posH → posEnd5 → posC
	vec2 posC = vec2(-radius, 0);
	float angle5 = angle4;  // 135°
	vec2 posEnd5 = vec2(posC.x + cos(angle5) * radius, posC.y + sin(angle5) * radius);
	shapes.push_back(std::make_unique<OctagramLine>(posH, posEnd5, posC, "5", radius));
	octagramLineIndices.push_back(shapes.size() - 1);  // Simpan index OctagramLine 5

	// OctagramLine 7: posC → posEnd6 → posF
	float angle6 = -angle5;  // -135°
	vec2 posEnd6 = vec2(posC.x + cos(angle6) * radius, posC.y + sin(angle6) * radius);
	vec2 posF = vec2(cos(angle6) * radius, sin(angle6) * radius);
	shapes.push_back(std::make_unique<OctagramLine>(posC, posEnd6, posF, "6", radius));
	octagramLineIndices.push_back(shapes.size() - 1);  // Simpan index OctagramLine 6

	// OctagramLine 8: posF → posEnd7 → posE
	float angle7 = angle6;  // -135°
	vec2 posEnd7 = vec2(posE.x + cos(angle7) * radius, posE.y + sin(angle7) * radius);
	shapes.push_back(std::make_unique<OctagramLine>(posF, posEnd7, posE, "7", radius));
	octagramLineIndices.push_back(shapes.size() - 1);  // Simpan index OctagramLine 7
}

//--------------------------------------------------------------
bool BasicZelligeTemplate::hasCustomSettings() {
	return false;
}

//--------------------------------------------------------------
void BasicZelligeTemplate::showSettingsUI() {
	ImGui::Text("Zellige Pattern Controls");

	static bool showCircles = true;
	static bool showCrossLines = true;
	static bool showParallelograms = true;
	static bool showRectangleLines = true;
	static bool showOctagramLines = true;

	if (ImGui::Checkbox("Show Circles (A-E)", &showCircles)) {
		// TODO: Toggle circle shapes visibility
	}
	if (ImGui::Checkbox("Show Cross Lines (F-I)", &showCrossLines)) {
		// TODO: Toggle cross lines visibility
	}
	if (ImGui::Checkbox("Show Parallelograms (N-Q)", &showParallelograms)) {
		// TODO: Toggle parallelogram lines visibility
	}
	if (ImGui::Checkbox("Show Rectangle Lines", &showRectangleLines)) {
		// TODO: Toggle rectangle lines visibility
	}
	if (ImGui::Checkbox("Show Octagram Lines (0-7)", &showOctagramLines)) {
		// TODO: Toggle octagram lines visibility
	}

	ImGui::Separator();

	static int octagramPoints = 8;
	ImGui::SetNextItemWidth(150.0f);
	if (ImGui::SliderInt("Octagram Points", &octagramPoints, 4, 12)) {
		// TODO: Rebuild octagram with new point count
	}
}

//--------------------------------------------------------------
bool BasicZelligeTemplate::hasPlaybackSettings() {
	return true;
}

//--------------------------------------------------------------
void BasicZelligeTemplate::showPlaybackUI(ofApp* app) {
	ImGui::Text("Mode Draw");
	static int playMode = -1;  // -1 = belum pilih, 0 = Parallel, 1 = Sequential

	if (ImGui::RadioButton("Parallel Per Group", &playMode, 0)) {
		// Radio button changed
	}
	ImGui::SetNextItemWidth(150.0f);
	if (ImGui::RadioButton("Sequential Per Group", &playMode, 1)) {
		// Radio button changed
	}
	ImGui::Separator();
	ImGui::Text("Hide/Show");
	if (ImGui::Checkbox("Cartesian", &showCartesianOnPlay)) {
		// JANGAN langsung apply Cartesian visibility di sini!
	}
	ImGui::SameLine();
	if (ImGui::Checkbox("Circles", &showCirclesOnPlay)) {
		// Checkbox simpan preferensi untuk saat Play diklik
	}
	ImGui::SameLine();
	if (ImGui::Checkbox("CrossLines", &showCrossLinesOnPlay)) {
		// Checkbox simpan preferensi untuk saat Play diklik
	}
	if (ImGui::Checkbox("Parallelograms", &showParallelogramsOnPlay)) {
		// Checkbox simpan preferensi untuk saat Play diklik
	}
	ImGui::SameLine();
	if (ImGui::Checkbox("RectangleLines", &showRectangleLinesOnPlay)) {
		// Checkbox simpan preferensi untuk saat Play diklik
	}
	if (ImGui::Checkbox("OctagramLines", &showOctagramLinesOnPlay)) {
		// Checkbox simpan preferensi untuk saat Play diklik
	}

	ImGui::Separator();
	ImGui::Text("Polygon Animate");
	if (ImGui::RadioButton("No Animation", &polygonAnimationMode, 0)) {
		// Radio button changed
	}
	ImGui::SameLine();
	if (ImGui::RadioButton("FadeIn", &polygonAnimationMode, 1)) {
		// Radio button changed
	}
	ImGui::SameLine();
	if (ImGui::RadioButton("Wobble", &polygonAnimationMode, 2)) {
		// Radio button changed
	}

	// Play arrow button
	if (ImGui::ArrowButton("Play", ImGuiDir_Left)) {
		// Cek apakah sudah ada file yang di-open
		if (app->lastSavedPath.empty()) {
			// Belum ada file, munculkan error popup
			app->errorPopup->show("No File Selected",
								 "Please open a .nay file first before clicking Play!",
								 "OK");
		} else if (playMode != 0 && playMode != 1) {
			// Belum pilih mode, munculkan error popup
			app->errorPopup->show("No Mode Selected",
								 "Please select a Draw Mode first!",
								 "OK");
		} else {
			// Semua validasi OK! Lanjut load
			// NOTE: loadWorkspace() akan otomatis menimpa semua data dengan yang baru
			// Jadi tidak perlu cek canvas kosong atau clean canvas dulu

			// Sinkronisasi SacredGeometry state dengan Playground preference
			app->currentTemplate->showCartesianInSacredGeometry = showCartesianOnPlay;

			// Simpan polygon animation mode - convert int ke PolygonAnimationMode
			// polygonAnimationMode: 0 = No Animation, 1 = FadeIn, 2 = Wobble
			PolygonAnimationMode polyMode;
			switch (polygonAnimationMode) {
				case 1:
					polyMode = PolygonAnimationMode::FADE_IN;
					break;
				case 2:
					polyMode = PolygonAnimationMode::WOBBLE;
					break;
				case 0:
				default:
					polyMode = PolygonAnimationMode::NO_ANIMATION;
					break;
			}
			app->fileManager.setPolygonAnimationMode(polyMode);

			// Set flag untuk delay load dan update state
			app->isWaitingForLoad = true;
			app->loadDelayTimer = ofGetElapsedTimef();
			app->pendingLoadMode = playMode;
			app->currentState = ofApp::UpdateState::DELAYED_LOAD;  // STRATEGY PATTERN: Set state ke DELAYED_LOAD

			app->imguiVisible = false;  // Hide ImGui
		}
	}
	ImGui::SameLine();
	ImGui::Text("Play");
}
