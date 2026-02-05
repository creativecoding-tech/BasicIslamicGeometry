#include "UserCustom.h"
#include "../../ofApp.h"

UserCustom::UserCustom(ofApp* app) : app(app) {
    // Initialize color picker dari app->customLineColor (default biru)
    customLineColor[0] = app->customLineColor.r / 255.0f; // R
    customLineColor[1] = app->customLineColor.g / 255.0f; // G
    customLineColor[2] = app->customLineColor.b / 255.0f; // B
    customLineColor[3] = app->customLineColor.a / 255.0f; // A

    // Initialize color picker dari app->polygonColor (default biru)
    polygonColor[0] = app->polygonColor.r / 255.0f; // R
    polygonColor[1] = app->polygonColor.g / 255.0f; // G
    polygonColor[2] = app->polygonColor.b / 255.0f; // B
    polygonColor[3] = app->polygonColor.a / 255.0f; // A
}

//--------------------------------------------------------------
void UserCustom::updateColorFromApp() {
    // Update color picker values dari app->customLineColor
    customLineColor[0] = app->customLineColor.r / 255.0f; // R
    customLineColor[1] = app->customLineColor.g / 255.0f; // G
    customLineColor[2] = app->customLineColor.b / 255.0f; // B
    customLineColor[3] = app->customLineColor.a / 255.0f; // A
}

//--------------------------------------------------------------
void UserCustom::updatePolygonColorFromApp() {
    // Update polygon color picker values dari app->polygonColor
    polygonColor[0] = app->polygonColor.r / 255.0f; // R
    polygonColor[1] = app->polygonColor.g / 255.0f; // G
    polygonColor[2] = app->polygonColor.b / 255.0f; // B
    polygonColor[3] = app->polygonColor.a / 255.0f; // A
}

//--------------------------------------------------------------
void UserCustom::focusWindow() {
    focusRequested = true;
}

//--------------------------------------------------------------
void UserCustom::showWindow() {
    windowOpen = true;
}

//--------------------------------------------------------------
void UserCustom::draw() {
    ImGui::SetNextWindowSize(ImVec2(350, 400), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowPos(ImVec2(50, 450), ImGuiCond_FirstUseEver);

    // Focus window jika di-request
    if (focusRequested) {
        ImGui::SetNextWindowFocus();
        focusRequested = false;
    }

    // Begin window dengan close button (windowOpen flag)
    if (ImGui::Begin("User Custom", &windowOpen, ImGuiWindowFlags_None)) {
        ImGui::Text("Custom Line");
        // Color picker untuk custom line (bentuk melingkar)
        if (ImGui::ColorPicker4("L Color", customLineColor,
            ImGuiColorEditFlags_PickerHueWheel | ImGuiColorEditFlags_AlphaBar)) {
            // Event handler: warna berubah, update semua customLines
            ofColor newColor(
                customLineColor[0] * 255,
                customLineColor[1] * 255,
                customLineColor[2] * 255,
                customLineColor[3] * 255
            );
            app->updateCustomLineColor(newColor);
        }
        ImGui::Separator();
        ImGui::Text("Polygon");
        // Color picker untuk polygon (bentuk melingkar)
        if (ImGui::ColorPicker4("P Color", polygonColor,
            ImGuiColorEditFlags_PickerHueWheel | ImGuiColorEditFlags_AlphaBar)) {
            // Event handler: warna berubah, update semua polygons
            ofColor newColor(
                polygonColor[0] * 255,
                polygonColor[1] * 255,
                polygonColor[2] * 255,
                polygonColor[3] * 255
            );
            app->updatePolygonColor(newColor);
        }
    }
    ImGui::End();

    // Sync window open state ke app
    app->showUserCustom = windowOpen;
}
