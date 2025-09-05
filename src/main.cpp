#include "windows.h"
#include "imgui.h"
#include "imgui_memory_editor.h"
#include "Nexus.h"
#include "memtools.h"
#include "memory.h"
#include <cstring>

// =============================================================================
// ADDON DEFINITION
// =============================================================================
AddonDefinition_t AddonDef{};

extern "C" __declspec(dllexport) AddonDefinition_t *GetAddonDef()
{
    AddonDef.Signature = 0x4D454D56; // "MEMV"
    AddonDef.APIVersion = NEXUS_API_VERSION;
    AddonDef.Name = "MemoryViewer";
    AddonDef.Version.Major = 1;
    AddonDef.Version.Minor = 0;
    AddonDef.Version.Build = 0;
    AddonDef.Version.Revision = 0;
    AddonDef.Author = "oshico";
    AddonDef.Description = "Memory visualization tool";
    AddonDef.Load = AddonLoad;
    AddonDef.Unload = AddonUnload;
    AddonDef.Flags = AF_None;
    AddonDef.Provider = UP_None;
    AddonDef.UpdateLink = nullptr;

    return &AddonDef;
}

// =============================================================================
// ADDON LOAD - SETUP WHEN LOADED
// =============================================================================
void AddonLoad(AddonAPI_t *aApi)
{
    APIDefs = aApi;
    if (APIDefs->ImguiContext)
        ImGui::SetCurrentContext((ImGuiContext *)APIDefs->ImguiContext);
    APIDefs->InputBinds_RegisterWithString("MEMORY_SHOW_WINDOW", (INPUTBINDS_PROCESS)KeybindHandler, "CTRL+SHIFT+K");
    APIDefs->GUI_Register(RT_Render, AddonRender);
    APIDefs->GUI_Register(RT_OptionsRender, AddonOptions);
    APIDefs->Log(LOGL_INFO, "MemoryViewer", "Addon loaded - Press CTRL+SHIFT+K");
}

// =============================================================================
// ADDON UNLOAD - CLEANUP
// =============================================================================
void AddonUnload()
{
    if (APIDefs)
    {
        APIDefs->InputBinds_Deregister("MEMORY_SHOW_WINDOW");
        APIDefs->GUI_Deregister(AddonRender);
        APIDefs->GUI_Deregister(AddonOptions);
        APIDefs->Log(LOGL_INFO, "MemoryViewer", "Addon unloaded");
    }
}

// =============================================================================
// GUI RENDERING
// =============================================================================
void AddonRender()
{
    RenderMainWindow();
    if(!g_scannedAddress){
        ScanForPattern();
    }
}

void AddonOptions()
{
    ImGui::Text("Memory Viewer v1.0.0");
    ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.7f, 1.0f), "View game memory using pattern scanning");
    ImGui::Separator();

    if (ImGui::Button("Open Memory Viewer"))
    {
        g_showMainWindow = true;
    }

    ImGui::Separator();
    ImGui::Text("Current Address: %p", g_scannedAddress);
    ImGui::Text("Memory Size: %zu bytes", g_memorySize);
}

// =============================================================================
// RENDER MAIN WINDOW - SINGLE MEMORY VIEWER
// =============================================================================
void RenderMainWindow()
{
    if (!g_showMainWindow)
        return;

    ImGui::SetNextWindowSize(ImVec2(800, 600), ImGuiCond_FirstUseEver);

    if (ImGui::Begin("Memory Viewer", &g_showMainWindow))
    {
        // Control panel
        ImGui::BeginChild("Controls", ImVec2(0, 60), true);
        {

            if (ImGui::Button("Refresh Memory") && g_scannedAddress)
            {
                ReadGameMemory(g_scannedAddress, g_memorySize);
            }

            ImGui::SameLine();

            // Memory size control
            ImGui::SetNextItemWidth(100);
            int sizeKB = g_memorySize / 1024;
            if (ImGui::InputInt("Size (KB)", &sizeKB, 1, 10))
            {
                g_memorySize = std::max(1, sizeKB) * 1024;
                if (g_scannedAddress)
                {
                    ReadGameMemory(g_scannedAddress, g_memorySize);
                }
            }

            ImGui::SameLine();
            ImGui::Text("Address: %p", g_scannedAddress);

            ImGui::Separator();
        }
        ImGui::End();
    }
}