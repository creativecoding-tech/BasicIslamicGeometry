#include "MenuBar.h"
#include "../../ofApp.h"

MenuBar::MenuBar(ofApp* app) : app(app) {}

void MenuBar::draw() {
    if (ImGui::BeginMainMenuBar()) {
        if (ImGui::BeginMenu("File")) {
            if (ImGui::MenuItem("Open","CTRL+O ")) {
                app->openWorkspace();
            }
            ImGui::Separator();
            if (ImGui::MenuItem("Save Workspace", "CTRL+S")) {
                app->saveWorkspace();
            }
            if (ImGui::MenuItem("Save As...", "CTRL+SHIFT+S")) {
                app->saveWorkspaceAs();
            }
            ImGui::Separator();
            if (ImGui::MenuItem("Exit", "ALT+F4")) {
                ofExit();
            }
            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Edit")) {
            if (ImGui::MenuItem("Undo", "CTRL+Z")) {
                app->undo();
            }
            if (ImGui::MenuItem("Redo", "CTRL+SHIFT+Z")) {
                app->redo();
            }
            ImGui::Separator();
            if (ImGui::MenuItem("Delete All Custom Lines")) {
                app->deleteAllCustomLines();
            }
            if (ImGui::MenuItem("Delete All Polygons")) {
                app->deleteAllPolygons();
            }
            if (ImGui::MenuItem("Delete All Custom Dots")) {
                app->deleteAllUserDots();
            }
            if (ImGui::MenuItem("Delete Lines & Polygons", "CTRL+DEL")) {
                app->clearCustomLinesAndPolygons();
            }
            if (ImGui::MenuItem("Clean Canvas", "CTRL+SHIFT+DEL")) {
                app->cleanCanvas();
            }
            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("View")) {
            if (ImGui::MenuItem("Sacred Gemotery")) {
                app->toggleSacredGeometryWindow();
            }
            if (ImGui::MenuItem("Playground")) {
                app->togglePlaygroundWindow();
            }
            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Help")) {
            if (ImGui::MenuItem("About")) {

            }
            ImGui::EndMenu();
        }

        ImGui::EndMainMenuBar();
    }
}
