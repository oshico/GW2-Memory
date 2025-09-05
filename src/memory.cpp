#include "imgui.h"
#include "imgui_memory_editor.h"
#include "Nexus.h"
#include "memtools.h"
#include "memory.h"
#include <vector>
#include <string>

// =============================================================================
// GLOBAL VARIABLES - SINGLE MEMORY REGION
// =============================================================================
AddonAPI_t *APIDefs = nullptr;
bool g_showMainWindow = false;
MemoryEditor g_memEditor;
void *g_scannedAddress = nullptr;
size_t g_memorySize = 0x1000;
std::vector<uint8_t> g_memoryBuffer(g_memorySize);

// =============================================================================
// KEYBIND HANDLER - TOGGLES MEMORY WINDOW
// =============================================================================
void KeybindHandler(const char *identifier, bool isRelease)
{
    if (isRelease)
        return;

    if (strcmp(identifier, "MEMORY_SHOW_WINDOW") == 0)
    {
        g_showMainWindow = !g_showMainWindow;
    }
}

// =============================================================================
// HOOK VARIABLES
// =============================================================================
typedef long long(__fastcall *InitFunc)(void *param1);
InitFunc originalFunc = nullptr;
void *hookTargetAddress = nullptr;

// =============================================================================
// HOOKED FUNCTION
// =============================================================================
long long __fastcall HookedFunc(void *param1)
{
    if (param1 && APIDefs)
    {
        g_scannedAddress = param1;

        ReadGameMemory(param1, g_memorySize);
    }

    return originalFunc(param1);
}

// =============================================================================
// SETUP GRAPHICS HOOK
// =============================================================================
void SetupHook(void *address)
{
    if (!APIDefs || !address)
        return;

    hookTargetAddress = address;

    if (APIDefs->MinHook_Create)
    {
        HRESULT hr = APIDefs->MinHook_Create(
            address,
            reinterpret_cast<LPVOID>(&HookedFunc),
            reinterpret_cast<LPVOID *>(&originalFunc));

        if (hr == MH_OK)
        {
            hr = APIDefs->MinHook_Enable(address);
            if (hr == MH_OK)
            {
                APIDefs->Log(LOGL_INFO, "MemoryViewer", "Graphics function hooked successfully!");
                APIDefs->Log(LOGL_INFO, "MemoryViewer", "Hook will trigger when GW2 calls the graphics function");
            }
            else
            {
                APIDefs->Log(LOGL_INFO, "MemoryViewer", "Failed to enable hook");
            }
        }
        else
        {
            APIDefs->Log(LOGL_INFO, "MemoryViewer", "Failed to create hook");
        }
    }
    else
    {
        APIDefs->Log(LOGL_INFO, "MemoryViewer", "MinHook API not available");
    }
}
// =============================================================================
// PATTERN DEFINITIONS
// =============================================================================

const memtools::Pattern pattern = ("E8 ?? ?? ?? ?? 45 33 ?? BA 02 00 00 00");

// =============================================================================
// SCAN FOR PATTERN
// =============================================================================
void ScanForPattern()
{
    if (!APIDefs)
        return;

    g_scannedAddress = nullptr;

    memtools::DataScan scan(pattern, memtools::Offset(0));
    void *result = scan.Scan();

    if (result)
    {

        SetupHook(result);
        return;
    }
}

// =============================================================================
// READ GAME MEMORY
// =============================================================================
void ReadGameMemory(void *address, size_t size)
{
    if (!address)
    {
        return;
    }

    if (g_memoryBuffer.size() < size)
    {
        g_memoryBuffer.resize(size);
    }

    try
    {
        memcpy(g_memoryBuffer.data(), address, size);
    }
    catch (...)
    {
        APIDefs->Log(LOGL_INFO, "MemoryViewer", "Exception while reading memory");
    }
}