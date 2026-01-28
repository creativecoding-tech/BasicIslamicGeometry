#include "MenuBar.h"
#include "../../ofApp.h"

MenuBar::MenuBar(ofApp* app) : app(app) {}

void MenuBar::draw() {
    if (ImGui::BeginMainMenuBar()) {
        if (ImGui::BeginMenu("File")) {
            if (ImGui::MenuItem("Save Workspace", "CTRL+S")) {
                app->saveWorkspace();
            }
            if (ImGui::MenuItem("Load Workspace", "CTRL+O")) {
                app->loadWorkspace();
            }
            if (ImGui::MenuItem("Load Workspace Seq", "CTRL+SHIFT+O")) {
                app->loadWorkspaceSeq();
            }
            ImGui::Separator();
            if (ImGui::MenuItem("Exit", "ALT+F4")) {
                ofExit();
            }
            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("View")) {
            if (ImGui::MenuItem("Sacred Gemotery", "G")) {

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
