#ifndef MEMORY_H
#define MEMORY_H

#include "imgui.h"
#include "imgui_memory_editor.h"
#include "imconfig.h"
#include "imgui_internal.h"
#include "imstb_textedit.h"
#include "Nexus.h"
#include "memtools.h"
#include "memory.h"
#include <vector>
#include <string>

extern AddonAPI_t *APIDefs;

// =============================================================================
// GLOBAL VARIABLES - SINGLE MEMORY REGION
// =============================================================================
extern bool g_showMainWindow;
extern MemoryEditor g_memEditor;            // memory editor instance
extern void *g_scannedAddress;              // scanned memory address
extern size_t g_memorySize;                 // memory size to read
extern std::vector<uint8_t> g_memoryBuffer; // memory buffer

// =============================================================================
// ADDON LIFECYCLE FUNCTIONS
// =============================================================================
void AddonLoad(AddonAPI_t *aApi);
void AddonUnload();

// =============================================================================
// GUI RENDERING FUNCTIONS
// =============================================================================
void AddonRender();
void AddonOptions();
void RenderMainWindow();

// =============================================================================
// Keybind Handler
// =============================================================================
void KeybindHandler(const char *identifier, bool isRelease);

// =============================================================================
// MEMORY SCANNING FUNCTIONS
// =============================================================================
void ScanForPattern();
void ReadGameMemory(void *address, size_t size);
void SetupHook(void *address);

#endif