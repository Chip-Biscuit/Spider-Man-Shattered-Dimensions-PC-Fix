/**
* Copyright (C) 2020 Elisha Riedlinger
*
* This software is  provided 'as-is', without any express  or implied  warranty. In no event will the
* authors be held liable for any damages arising from the use of this software.
* Permission  is granted  to anyone  to use  this software  for  any  purpose,  including  commercial
* applications, and to alter it and redistribute it freely, subject to the following restrictions:
*
*   1. The origin of this software must not be misrepresented; you must not claim that you  wrote the
*      original  software. If you use this  software  in a product, an  acknowledgment in the product
*      documentation would be appreciated but is not required.
*   2. Altered source versions must  be plainly  marked as such, and  must not be  misrepresented  as
*      being the original software.
*   3. This notice may not be removed or altered from any source distribution.
*/

// Fix Enhancers/ Chips Spiderman Shattered Dimensions Patches Added

#include "d3d9.h"
#include "d3dx9.h"
#include "iathook.h"
#include "helpers.h"
#include <windows.h> // chip
#include <iostream> // chip  
#include <vector> // chip
#include <Psapi.h> // chip
#include <tchar.h> //chip
#include <sstream> //chip

#pragma comment(lib, "d3dx9.lib")
#pragma comment(lib, "winmm.lib") // needed for timeBeginPeriod()/timeEndPeriod()

// chip - adding macros
# define DX_PRINT(x) std::cout << x << std::endl;
# define DX_ERROR(x) std::cerr << x << std::endl;
#define DX_MBPRINT(x) MessageBox(NULL, x, "Message", MB_OK);
#define DX_MBERROR(x) MessageBox(NULL, x, "Error", MB_ICONERROR | MB_OK);

Direct3DShaderValidatorCreate9Proc m_pDirect3DShaderValidatorCreate9;
PSGPErrorProc m_pPSGPError;
PSGPSampleTextureProc m_pPSGPSampleTexture;
D3DPERF_BeginEventProc m_pD3DPERF_BeginEvent;
D3DPERF_EndEventProc m_pD3DPERF_EndEvent;
D3DPERF_GetStatusProc m_pD3DPERF_GetStatus;
D3DPERF_QueryRepeatFrameProc m_pD3DPERF_QueryRepeatFrame;
D3DPERF_SetMarkerProc m_pD3DPERF_SetMarker;
D3DPERF_SetOptionsProc m_pD3DPERF_SetOptions;
D3DPERF_SetRegionProc m_pD3DPERF_SetRegion;
DebugSetLevelProc m_pDebugSetLevel;
DebugSetMuteProc m_pDebugSetMute;
Direct3D9EnableMaximizedWindowedModeShimProc m_pDirect3D9EnableMaximizedWindowedModeShim;
Direct3DCreate9Proc m_pDirect3DCreate9;
Direct3DCreate9ExProc m_pDirect3DCreate9Ex;

HWND g_hFocusWindow = NULL;
HMODULE g_hWrapperModule = NULL;

HMODULE d3d9dll = NULL;

bool bForceWindowedMode;
bool bUsePrimaryMonitor;
bool bCenterWindow;
bool bAlwaysOnTop;
bool bDoNotNotifyOnTaskSwitch;
bool bDisplayFPSCounter;
float fFPSLimit;
int nFullScreenRefreshRateInHz;
int nForceWindowStyle;
bool toggle = false; //chip
int customHotkey = '9'; // chip
bool isMouseButton = false; // chip
int customHotkeyfps; //chip
bool isMouseButton2 = false; // chip


// Function prototype for the hotkey thread chip
DWORD WINAPI HotkeyThread(LPVOID lpParam);

// Function prototype for the hotkey thread chip
DWORD WINAPI HotkeyThread2(LPVOID lpParam);

char WinDir[MAX_PATH+1];

// List of registered window classes and procedures
// WORD classAtom, ULONG_PTR WndProcPtr
std::vector<std::pair<WORD,ULONG_PTR>> WndProcList;

//=======================================================================================================================================================================================
// chip - toggle where user chooses input with message box confirmation 

// Function to read hotkey from INI file
int ReadHotkeyFromINI(const std::string& filePath) {
    char keyHex[8] = { 0 };  // Buffer for hex key (e.g., "0x41")

    GetPrivateProfileString("Hotkey", "FOV_Toggle", "0x41", keyHex, sizeof(keyHex), filePath.c_str());

    return strtol(keyHex, nullptr, 0);  // Convert string to integer (handles hex)
}

// Convert integer to hex string manually (since std::to_string may not exist)
std::string IntToHexString(int value) {
    std::stringstream stream;
    stream << "0x" << std::hex << std::uppercase << value;  // Convert to uppercase hex
    return stream.str();
}

void ShowMessageBox(const std::string& message) {
    MessageBox(NULL, message.c_str(), "Hotkey Notification", MB_OK | MB_ICONINFORMATION);
}

void SetCustomHotkey() {
    // Get module path
    char path[MAX_PATH];
    HMODULE hm = NULL;

    if (GetModuleHandleExA(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
        (LPCSTR)&Direct3DCreate9, &hm)) {
        GetModuleFileNameA(hm, path, sizeof(path));

        // Replace filename with d3d9.ini
        std::string iniPath = path;
        iniPath.replace(iniPath.find_last_of('\\'), std::string::npos, "\\d3d9.ini");

        // Read custom hotkey from INI
        customHotkey = ReadHotkeyFromINI(iniPath);
        isMouseButton = (customHotkey >= VK_LBUTTON && customHotkey <= VK_XBUTTON2);
    }
    else {
        DX_ERROR("Failed to get module handle.")
    }
}


// chip - toggle where user chooses input with message box confirmation 
//=======================================================================================================================================================================================

//=======================================================================================================================================================================================
// chip - toggle where user chooses input with message box confirmation 

// Function to read hotkey from INI file
int ReadHotkeyFromINI2(const std::string& filePath) {
    char keyHex[8] = { 0 };  // Buffer for hex key (e.g., "0x41")

    GetPrivateProfileString("Hotkey", "FPS_Toggle", "0x55", keyHex, sizeof(keyHex), filePath.c_str());

    return strtol(keyHex, nullptr, 0);  // Convert string to integer (handles hex)
}

// Convert integer to hex string manually (since std::to_string may not exist)
std::string IntToHexString2(int value) {
    std::stringstream stream;
    stream << "0x" << std::hex << std::uppercase << value;  // Convert to uppercase hex
    return stream.str();
}

void ShowMessageBox2(const std::string& message) {
    MessageBox(NULL, message.c_str(), "Hotkey Notification", MB_OK | MB_ICONINFORMATION);
}

void SetCustomHotkey2() {
    // Get module path
    char path[MAX_PATH];
    HMODULE hm = NULL;

    if (GetModuleHandleExA(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
        (LPCSTR)&Direct3DCreate9, &hm)) {
        GetModuleFileNameA(hm, path, sizeof(path));

        // Replace filename with d3d9.ini
        std::string iniPath = path;
        iniPath.replace(iniPath.find_last_of('\\'), std::string::npos, "\\d3d9.ini");

        // Read custom hotkey from INI
        customHotkeyfps = ReadHotkeyFromINI2(iniPath);
        isMouseButton2 = (customHotkeyfps >= VK_LBUTTON && customHotkeyfps <= VK_XBUTTON2);
    }
    else {
        DX_ERROR("Failed to get module handle.")
    }
}


// chip - toggle where user chooses input with message box confirmation 
//=======================================================================================================================================================================================

//=======================================================================================================================================================================================

// chip - 1: fps origional  

// Function to perform the hex edit
void PerformHexEdit1(LPBYTE lpAddress, DWORD moduleSize) {
    // Define the patterns to search for and their corresponding new values
    struct HexEdit5 {
        std::vector<BYTE> pattern;
        std::vector<BYTE> newValue;
        size_t offset; // Offset of the byte to modify within the pattern
    };

    // Define the edits
    std::vector<HexEdit5> edits5 = {
        // FPS uncap 120 fps
        { { 0x61, 0x0B, 0x36, 0x3B }, { 0x89, 0x88, 0x88, 0x3C }, 0 },
    };

    // Iterate through the edits
    for (const auto& edit5 : edits5) {
        // Search for the pattern in memory
        for (DWORD i = 0; i < moduleSize - edit5.pattern.size(); ++i) {
            if (memcmp(lpAddress + i, edit5.pattern.data(), edit5.pattern.size()) == 0) {
                // Pattern found in memory
                DX_PRINT("Pattern found in memory.")

                    // Modify memory
                    LPVOID lpAddressToWrite = lpAddress + i + edit5.offset;
                SIZE_T numberOfBytesWritten;
                DWORD oldProtect;
                if (!VirtualProtectEx(GetCurrentProcess(), lpAddressToWrite, edit5.newValue.size(), PAGE_EXECUTE_READWRITE, &oldProtect)) {
                    DX_ERROR("Failed to change memory protection.")
                        return;
                }

                BOOL result = WriteProcessMemory(GetCurrentProcess(), lpAddressToWrite, edit5.newValue.data(), edit5.newValue.size(), &numberOfBytesWritten);
                if (!result || numberOfBytesWritten != edit5.newValue.size()) {
                    std::cerr << "Failed to write memory." << std::endl;
                    return;
                }

                // Restore original protection
                DWORD dummy;
                if (!VirtualProtectEx(GetCurrentProcess(), lpAddressToWrite, edit5.newValue.size(), oldProtect, &dummy)) {
                    DX_ERROR("Failed to restore memory protection.")
                        return;
                }

                DX_PRINT("Hex edited successfully.")
                    break;
            }
        }
    }
}

// Function to perform the hex edits
void PerformHexEdits1() {
    // Get the handle to the current module
    HMODULE hModule = GetModuleHandle(NULL);
    if (hModule == NULL) {
        DX_ERROR("Failed to get module handle.")
            return;
    }

    // Get the module information
    LPBYTE lpAddress = reinterpret_cast<LPBYTE>(hModule);
    DWORD moduleSize = 0; // Placeholder for module size
    TCHAR szFileName[MAX_PATH];
    if (GetModuleFileNameEx(GetCurrentProcess(), hModule, szFileName, MAX_PATH)) {
        moduleSize = GetFileSize(szFileName, NULL);
    }
    if (moduleSize == 0) {
        DX_ERROR("Failed to get module information.")
            return;
    }

    // Perform the hex edit
    PerformHexEdit1(lpAddress, moduleSize);
}

// chip - 1: fps origional
// 
//=======================================================================================================================================================================================

//=======================================================================================================================================================================================

// chip - 4: fps uncap 360

// Function to perform the hex edit
void PerformHexEdit4(LPBYTE lpAddress, DWORD moduleSize) {
    // Define the patterns to search for and their corresponding new values
    struct HexEdit4 {
        std::vector<BYTE> pattern;
        std::vector<BYTE> newValue;
        size_t offset; // Offset of the byte to modify within the pattern
    };

    // Define the edits
    std::vector<HexEdit4> edits4 = {
        // FPS uncap 360 fps
        { { 0x89, 0x88, 0x88, 0x3C }, { 0x61, 0x0B, 0x36, 0x3B }, 0 },
    };

    // Iterate through the edits
    for (const auto& edit4 : edits4) {
        // Search for the pattern in memory
        for (DWORD i = 0; i < moduleSize - edit4.pattern.size(); ++i) {
            if (memcmp(lpAddress + i, edit4.pattern.data(), edit4.pattern.size()) == 0) {
                // Pattern found in memory
                DX_PRINT("Pattern found in memory.")

                    // Modify memory
                    LPVOID lpAddressToWrite = lpAddress + i + edit4.offset;
                SIZE_T numberOfBytesWritten;
                DWORD oldProtect;
                if (!VirtualProtectEx(GetCurrentProcess(), lpAddressToWrite, edit4.newValue.size(), PAGE_EXECUTE_READWRITE, &oldProtect)) {
                    DX_ERROR("Failed to change memory protection.")
                        return;
                }

                BOOL result = WriteProcessMemory(GetCurrentProcess(), lpAddressToWrite, edit4.newValue.data(), edit4.newValue.size(), &numberOfBytesWritten);
                if (!result || numberOfBytesWritten != edit4.newValue.size()) {
                    std::cerr << "Failed to write memory." << std::endl;
                    return;
                }

                // Restore original protection
                DWORD dummy;
                if (!VirtualProtectEx(GetCurrentProcess(), lpAddressToWrite, edit4.newValue.size(), oldProtect, &dummy)) {
                    DX_ERROR("Failed to restore memory protection.")
                        return;
                }

                DX_PRINT("Hex edited successfully.")
                    break;
            }
        }
    }
}

// Function to perform the hex edits
void PerformHexEdits4() {
    // Get the handle to the current module
    HMODULE hModule = GetModuleHandle(NULL);
    if (hModule == NULL) {
        DX_ERROR("Failed to get module handle.")
            return;
    }

    // Get the module information
    LPBYTE lpAddress = reinterpret_cast<LPBYTE>(hModule);
    DWORD moduleSize = 0; // Placeholder for module size
    TCHAR szFileName[MAX_PATH];
    if (GetModuleFileNameEx(GetCurrentProcess(), hModule, szFileName, MAX_PATH)) {
        moduleSize = GetFileSize(szFileName, NULL);
    }
    if (moduleSize == 0) {
        DX_ERROR("Failed to get module information.")
            return;
    }

    // Perform the hex edit
    PerformHexEdit4(lpAddress, moduleSize);
}

// chip - 4: fps uncap 360
// 
//=======================================================================================================================================================================================

//=======================================================================================================================================================================================
// chip - 2: fov

// Function to perform the hex edit
void PerformHexEdits2() {
    // INI file path
    char path[MAX_PATH];
    GetModuleFileNameA(NULL, path, MAX_PATH);
    char* lastBackslash = strrchr(path, '\\');
    if (lastBackslash != NULL)
        *(lastBackslash + 1) = '\0';
    strcat(path, "d3d9.ini");

    // Read the index value from the INI file
    int index = GetPrivateProfileIntA("fov", "fov", 0, path);
    if (index < 1 || index > 3) {
        DX_ERROR("Invalid index value in INI file.")
            return;
    }

    // Define the predefined float values
    float predefinedFloats[] = { 0.7f, 0.8f, 0.9f };

    // Select the float value based on the index
    float floatNewValue = predefinedFloats[index - 1];

    // Define the offset to write the new float value
    DWORD offset = 0x0074B7A8;

    // Open the process to write memory
    HANDLE hProcess = OpenProcess(PROCESS_VM_WRITE | PROCESS_VM_OPERATION, FALSE, GetCurrentProcessId());
    if (hProcess == NULL) {
        DX_ERROR("Failed to open process.")
            return;
    }

    // Change memory protection to allow writing
    DWORD oldProtect;
    if (!VirtualProtectEx(hProcess, reinterpret_cast<LPVOID>(offset), sizeof(float), PAGE_EXECUTE_READWRITE, &oldProtect)) {
        DX_ERROR("Failed to change memory protection.")
            return;
    }

    // Write the new value to memory
    if (!WriteProcessMemory(hProcess, reinterpret_cast<LPVOID>(offset), &floatNewValue, sizeof(float), NULL)) {
        DX_ERROR("Failed to write memory.")
            return;
    }

    // Restore original protection
    DWORD dummy;
    if (!VirtualProtectEx(hProcess, reinterpret_cast<LPVOID>(offset), sizeof(float), oldProtect, &dummy)) {
        DX_ERROR("Failed to restore memory protection.")
            return;
    }

    DX_PRINT("Value successfully updated.")
}

// chip - 2: fov
//=======================================================================================================================================================================================

//=======================================================================================================================================================================================
// chip - 3: fov 2



// Function to perform the hex edit
void PerformHexEdits3() {
    float floatNewValue = 0.5f;

    // Define the offset to write the new float value
    DWORD offset = 0x0074B7A8;

    // Open the process to write memory
    HANDLE hProcess = OpenProcess(PROCESS_VM_WRITE | PROCESS_VM_OPERATION, FALSE, GetCurrentProcessId());
    if (hProcess == NULL) {
        DX_ERROR("Failed to open process.")
            return;
    }

    // Change memory protection to allow writing
    DWORD oldProtect;
    if (!VirtualProtectEx(hProcess, reinterpret_cast<LPVOID>(offset), sizeof(float), PAGE_EXECUTE_READWRITE, &oldProtect)) {
        DX_ERROR("Failed to change memory protection.")
            return;
    }

    // Write the new value to memory
    if (!WriteProcessMemory(hProcess, reinterpret_cast<LPVOID>(offset), &floatNewValue, sizeof(float), NULL)) {
        DX_ERROR("Failed to write memory.")
            return;
    }

    // Restore original protection
    DWORD dummy;
    if (!VirtualProtectEx(hProcess, reinterpret_cast<LPVOID>(offset), sizeof(float), oldProtect, &dummy)) {
        DX_ERROR("Failed to restore memory protection.")
            return;
    }

    DX_PRINT("Value successfully updated.")
}

// chip - 3: fov 2 origional
//=======================================================================================================================================================================================

//=======================================================================================================================================================================================
//chip - window mode

// Function to read the window mode setting from the registry
bool GetWindowModeSetting()
{
    HKEY hKey;
    DWORD dwValue = 0;
    DWORD dwSize = sizeof(DWORD);

    if (RegOpenKeyEx(HKEY_CURRENT_USER, _T("SOFTWARE\\Activision\\Spider-Man (TM) SD\\Settings\\Display"), 0, KEY_READ, &hKey) == ERROR_SUCCESS)
    {
        if (RegQueryValueEx(hKey, _T("Fullscreen"), NULL, NULL, (LPBYTE)&dwValue, &dwSize) == ERROR_SUCCESS)
        {
            RegCloseKey(hKey);
            return dwValue != 0;
        }
        RegCloseKey(hKey);
    }
    return false;
}

// Function to set the window mode setting in the registry
void SetWindowModeSetting(bool enable)
{
    HKEY hKey;
    DWORD dwValue = enable ? 1 : 0;  // Correct logic: 1 for fullscreen, 0 for windowed

    if (RegOpenKeyEx(HKEY_CURRENT_USER, _T("SOFTWARE\\Activision\\Spider-Man (TM) SD\\Settings\\Display"), 0, KEY_WRITE, &hKey) == ERROR_SUCCESS)
    {
        RegSetValueEx(hKey, _T("Fullscreen"), 0, REG_DWORD, (const BYTE*)&dwValue, sizeof(DWORD));
        RegCloseKey(hKey);
    }
}

// Function to read the window mode setting from an INI file
bool ReadIniSetting(const TCHAR* iniFilePath)
{
    TCHAR result[10];
    GetPrivateProfileString(_T("Settings"), _T("WindowedMode"), _T("0"), result, 10, iniFilePath);
    return _ttoi(result) != 0;
}

// Function to get the path to the DLL
void GetDllPath(HMODULE hModule, TCHAR* dllPath, size_t size)
{
    GetModuleFileName(hModule, dllPath, size);
    for (size_t i = _tcslen(dllPath); i > 0; --i)
    {
        if (dllPath[i] == '\\' || dllPath[i] == '/')
        {
            dllPath[i + 1] = '\0';
            break;
        }
    }
}

// Main function to be called when the DLL is injected
void ToggleWindowMode(HMODULE hModule)
{
    TCHAR dllPath[MAX_PATH];
    GetDllPath(hModule, dllPath, MAX_PATH);

    _tcscat_s(dllPath, MAX_PATH, _T("d3d9.ini"));

    bool iniSetting = ReadIniSetting(dllPath);
    SetWindowModeSetting(iniSetting);
}

//chip - window mode
//=======================================================================================================================================================================================

//=======================================================================================================================================================================================
// chip resolution edit in registry 

// Function to read the resolution settings from the registry
bool GetResolutionSettings(int& resolutionWidth, int& resolutionHeight)
{
    HKEY hKey;
    DWORD dwResolutionWidth = 0;
    DWORD dwResolutionHeight = 0;
    DWORD dwSize = sizeof(DWORD);

    if (RegOpenKeyEx(HKEY_CURRENT_USER, _T("SOFTWARE\\Activision\\Spider-Man (TM) SD\\Settings\\Display"), 0, KEY_READ, &hKey) == ERROR_SUCCESS)
    {
        if (RegQueryValueEx(hKey, _T("ResolutionWidth"), NULL, NULL, (LPBYTE)&dwResolutionWidth, &dwSize) == ERROR_SUCCESS &&
            RegQueryValueEx(hKey, _T("ResolutionHeight"), NULL, NULL, (LPBYTE)&dwResolutionHeight, &dwSize) == ERROR_SUCCESS)
        {
            resolutionWidth = static_cast<int>(dwResolutionWidth);
            resolutionHeight = static_cast<int>(dwResolutionHeight);
            RegCloseKey(hKey);
            return true;
        }
        RegCloseKey(hKey);
    }
    return false;
}

// Function to set the resolution settings in the registry
void SetResolutionSettings(int resolutionWidth, int resolutionHeight)
{
    HKEY hKey;
    DWORD dwResolutionX = static_cast<DWORD>(resolutionWidth);
    DWORD dwResolutionY = static_cast<DWORD>(resolutionHeight);

    if (RegOpenKeyEx(HKEY_CURRENT_USER, _T("SOFTWARE\\Activision\\Spider-Man (TM) SD\\Settings\\Display"), 0, KEY_WRITE, &hKey) == ERROR_SUCCESS)
    {
        RegSetValueEx(hKey, _T("Width"), 0, REG_DWORD, (const BYTE*)&dwResolutionX, sizeof(DWORD));
        RegSetValueEx(hKey, _T("Height"), 0, REG_DWORD, (const BYTE*)&dwResolutionY, sizeof(DWORD));
        RegCloseKey(hKey);
    }
}

// Function to read the resolution settings from an INI file
bool ReadIniSettings(const TCHAR* iniFilePath, int& resolutionWidth, int& resolutionHeight)
{
    TCHAR resultX[10], resultY[10];
    GetPrivateProfileString(_T("Resolution"), _T("ResolutionWidth"), _T("0"), resultX, 10, iniFilePath);
    GetPrivateProfileString(_T("Resolution"), _T("ResolutionHeight"), _T("0"), resultY, 10, iniFilePath);
    resolutionWidth = _ttoi(resultX);
    resolutionHeight = _ttoi(resultY);
    return (resolutionWidth > 0 && resolutionHeight > 0);
}



// Main function to be called when the DLL is injected
void ToggleResolutionSettings(HMODULE hModule)
{
    TCHAR dllPath[MAX_PATH];
    GetDllPath(hModule, dllPath, MAX_PATH);

    _tcscat_s(dllPath, MAX_PATH, _T("d3d9.ini"));

    int resolutionWidth, resolutionHeight;
    if (ReadIniSettings(dllPath, resolutionWidth, resolutionHeight))
    {
        SetResolutionSettings(resolutionWidth, resolutionHeight);
    }
}

//=======================================================================================================================================================================================

void HookModule(HMODULE hmod);

class FrameLimiter
{
private:
    static inline double TIME_Frequency = 0.0;
    static inline double TIME_Ticks = 0.0;
    static inline double TIME_Frametime = 0.0;

public:
    static inline ID3DXFont* pFPSFont = nullptr;
    static inline ID3DXFont* pTimeFont = nullptr;

public:
    enum FPSLimitMode { FPS_NONE, FPS_REALTIME, FPS_ACCURATE };
    static void Init(FPSLimitMode mode)
    {
        LARGE_INTEGER frequency;

        QueryPerformanceFrequency(&frequency);
        static constexpr auto TICKS_PER_FRAME = 1;
        auto TICKS_PER_SECOND = (TICKS_PER_FRAME * fFPSLimit);
        if (mode == FPS_ACCURATE)
        {
            TIME_Frametime = 1000.0 / (double)fFPSLimit;
            TIME_Frequency = (double)frequency.QuadPart / 1000.0; // ticks are milliseconds
        }
        else // FPS_REALTIME
        {
            TIME_Frequency = (double)frequency.QuadPart / (double)TICKS_PER_SECOND; // ticks are 1/n frames (n = fFPSLimit)
        }
        Ticks();
    }
    static DWORD Sync_RT()
    {
        DWORD lastTicks, currentTicks;
        LARGE_INTEGER counter;

        QueryPerformanceCounter(&counter);
        lastTicks = (DWORD)TIME_Ticks;
        TIME_Ticks = (double)counter.QuadPart / TIME_Frequency;
        currentTicks = (DWORD)TIME_Ticks;

        return (currentTicks > lastTicks) ? currentTicks - lastTicks : 0;
    }
    static DWORD Sync_SLP()
    {
        LARGE_INTEGER counter;
        QueryPerformanceCounter(&counter);
        double millis_current = (double)counter.QuadPart / TIME_Frequency;
        double millis_delta = millis_current - TIME_Ticks;
        if (TIME_Frametime <= millis_delta)
        {
            TIME_Ticks = millis_current;
            return 1;
        }
        else if (TIME_Frametime - millis_delta > 2.0) // > 2ms
            Sleep(1); // Sleep for ~1ms
        else
            Sleep(0); // yield thread's time-slice (does not actually sleep)

        return 0;
    }
    static void ShowFPS(LPDIRECT3DDEVICE9EX device)
    {
        static std::list<int> m_times;

        //https://github.com/microsoft/VCSamples/blob/master/VC2012Samples/Windows%208%20samples/C%2B%2B/Windows%208%20app%20samples/Direct2D%20geometry%20realization%20sample%20(Windows%208)/C%2B%2B/FPSCounter.cpp#L279
        LARGE_INTEGER frequency;
        LARGE_INTEGER time;
        QueryPerformanceFrequency(&frequency);
        QueryPerformanceCounter(&time);

        if (m_times.size() == 50)
            m_times.pop_front();
        m_times.push_back(static_cast<int>(time.QuadPart));

        uint32_t fps = 0;
        if (m_times.size() >= 2)
            fps = static_cast<uint32_t>(0.5f + (static_cast<float>(m_times.size() - 1) * static_cast<float>(frequency.QuadPart)) / static_cast<float>(m_times.back() - m_times.front()));

        static int space = 0;
        if (!pFPSFont || !pTimeFont)
        {
            D3DDEVICE_CREATION_PARAMETERS cparams;
            RECT rect;
            device->GetCreationParameters(&cparams);
            GetClientRect(cparams.hFocusWindow, &rect);
            
            D3DXFONT_DESC fps_font;
            ZeroMemory(&fps_font, sizeof(D3DXFONT_DESC));
            fps_font.Height = rect.bottom / 20;
            fps_font.Width = 0;
            fps_font.Weight = 400;
            fps_font.MipLevels = 0;
            fps_font.Italic = 0;
            fps_font.CharSet = DEFAULT_CHARSET;
            fps_font.OutputPrecision = OUT_DEFAULT_PRECIS;
            fps_font.Quality = ANTIALIASED_QUALITY;
            fps_font.PitchAndFamily = DEFAULT_PITCH | FF_DONTCARE;
            wchar_t FaceName[] = L"Arial";
            memcpy(&fps_font.FaceName, &FaceName, sizeof(FaceName));

            D3DXFONT_DESC time_font = fps_font;
            time_font.Height = rect.bottom / 35;
            space = fps_font.Height + 5;

            if (D3DXCreateFontIndirect(device, &fps_font, &pFPSFont) != D3D_OK)
                return;

            if (D3DXCreateFontIndirect(device, &time_font, &pTimeFont) != D3D_OK)
                return;
        }
        else
        {
            auto DrawTextOutline = [](ID3DXFont* pFont, FLOAT X, FLOAT Y, D3DXCOLOR dColor, CONST PCHAR cString, ...)
            {
                const D3DXCOLOR BLACK(D3DCOLOR_XRGB(0, 0, 0));
                CHAR cBuffer[101] = "";

                va_list oArgs;
                va_start(oArgs, cString);
                _vsnprintf((cBuffer + strlen(cBuffer)), (sizeof(cBuffer) - strlen(cBuffer)), cString, oArgs);
                va_end(oArgs);

                RECT Rect[5] =
                {
                    { X - 1, Y, X + 500.0f, Y + 50.0f },
                    { X, Y - 1, X + 500.0f, Y + 50.0f },
                    { X + 1, Y, X + 500.0f, Y + 50.0f },
                    { X, Y + 1, X + 500.0f, Y + 50.0f },
                    { X, Y, X + 500.0f, Y + 50.0f },
                };

                if (dColor != BLACK)
                {
                    for (auto i = 0; i < 4; i++)
                        pFont->DrawText(NULL, cBuffer, -1, &Rect[i], DT_NOCLIP, BLACK);
                }

                pFont->DrawText(NULL, cBuffer, -1, &Rect[4], DT_NOCLIP, dColor);
            };

            static char str_format_fps[] = "%02d";
            static char str_format_time[] = "%.01f ms";
            static const D3DXCOLOR YELLOW(D3DCOLOR_XRGB(0xF7, 0xF7, 0));
            DrawTextOutline(pFPSFont, 10, 10, YELLOW, str_format_fps, fps);
            DrawTextOutline(pTimeFont, 10, space, YELLOW, str_format_time, (1.0f / fps) * 1000.0f);
        }
    }

private:
    static void Ticks()
    {
        LARGE_INTEGER counter;
        QueryPerformanceCounter(&counter);
        TIME_Ticks = (double)counter.QuadPart / TIME_Frequency;
    }
};

FrameLimiter::FPSLimitMode mFPSLimitMode = FrameLimiter::FPSLimitMode::FPS_NONE;

HRESULT m_IDirect3DDevice9Ex::Present(CONST RECT* pSourceRect, CONST RECT* pDestRect, HWND hDestWindowOverride, CONST RGNDATA* pDirtyRegion)
{
    if (mFPSLimitMode == FrameLimiter::FPSLimitMode::FPS_REALTIME)
        while (!FrameLimiter::Sync_RT());
    else if (mFPSLimitMode == FrameLimiter::FPSLimitMode::FPS_ACCURATE)
        while (!FrameLimiter::Sync_SLP());

    return ProxyInterface->Present(pSourceRect, pDestRect, hDestWindowOverride, pDirtyRegion);
}

HRESULT m_IDirect3DDevice9Ex::PresentEx(THIS_ CONST RECT* pSourceRect, CONST RECT* pDestRect, HWND hDestWindowOverride, CONST RGNDATA* pDirtyRegion, DWORD dwFlags)
{
    if (mFPSLimitMode == FrameLimiter::FPSLimitMode::FPS_REALTIME)
        while (!FrameLimiter::Sync_RT());
    else if (mFPSLimitMode == FrameLimiter::FPSLimitMode::FPS_ACCURATE)
        while (!FrameLimiter::Sync_SLP());

    return ProxyInterface->PresentEx(pSourceRect, pDestRect, hDestWindowOverride, pDirtyRegion, dwFlags);
}

HRESULT m_IDirect3DDevice9Ex::EndScene()
{
    if (bDisplayFPSCounter)
        FrameLimiter::ShowFPS(ProxyInterface);
    
    return ProxyInterface->EndScene();
}

void ForceWindowed(D3DPRESENT_PARAMETERS* pPresentationParameters, D3DDISPLAYMODEEX* pFullscreenDisplayMode = NULL)
{
    HWND hwnd = pPresentationParameters->hDeviceWindow ? pPresentationParameters->hDeviceWindow : g_hFocusWindow;
    HMONITOR monitor = MonitorFromWindow((!bUsePrimaryMonitor && hwnd) ? hwnd : GetDesktopWindow(), MONITOR_DEFAULTTONEAREST);
    MONITORINFO info;
    info.cbSize = sizeof(MONITORINFO);
    GetMonitorInfo(monitor, &info);
    int DesktopResX = info.rcMonitor.right - info.rcMonitor.left;
    int DesktopResY = info.rcMonitor.bottom - info.rcMonitor.top;

    int left = (int)info.rcMonitor.left;
    int top = (int)info.rcMonitor.top;

    if (nForceWindowStyle != 1) // not borderless fullscreen
    {
        left += (int)(((float)DesktopResX / 2.0f) - ((float)pPresentationParameters->BackBufferWidth / 2.0f));
        top += (int)(((float)DesktopResY / 2.0f) - ((float)pPresentationParameters->BackBufferHeight / 2.0f));
    }

    pPresentationParameters->Windowed = 1;

    // This must be set to default (0) on windowed mode as per D3D9 spec
    pPresentationParameters->FullScreen_RefreshRateInHz = D3DPRESENT_RATE_DEFAULT;

    // If exists, this must match the rate in PresentationParameters
    if (pFullscreenDisplayMode != NULL)
        pFullscreenDisplayMode->RefreshRate = D3DPRESENT_RATE_DEFAULT;

    // This flag is not available on windowed mode as per D3D9 spec
    pPresentationParameters->PresentationInterval &= ~D3DPRESENT_DONOTFLIP;

    if (hwnd != NULL)
    {
        int cx, cy;
        UINT uFlags = SWP_SHOWWINDOW;
        LONG lOldStyle = GetWindowLong(hwnd, GWL_STYLE);
        LONG lOldExStyle = GetWindowLong(hwnd, GWL_EXSTYLE);
        LONG lNewStyle, lNewExStyle;
        if (nForceWindowStyle == 1) // borderless fullscreen
        {
            cx = DesktopResX;
            cy = DesktopResY;
            lNewStyle = lOldStyle & ~(WS_CAPTION | WS_THICKFRAME | WS_MINIMIZE | WS_MAXIMIZE | WS_SYSMENU | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_DLGFRAME);
            lNewStyle |= (lOldStyle & WS_CHILD) ? 0 : WS_POPUP;
            lNewExStyle = lOldExStyle & ~(WS_EX_CONTEXTHELP | WS_EX_DLGMODALFRAME | WS_EX_CLIENTEDGE | WS_EX_STATICEDGE | WS_EX_WINDOWEDGE | WS_EX_TOOLWINDOW);
            lNewExStyle |= WS_EX_APPWINDOW;
        }
        else
        {
            cx = pPresentationParameters->BackBufferWidth;
            cy = pPresentationParameters->BackBufferHeight;
            if (!bCenterWindow)
                uFlags |= SWP_NOMOVE;

            if (nForceWindowStyle) // force windowed style
            {
                lOldExStyle &= ~(WS_EX_TOPMOST);
                lNewStyle = (WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX);
                lNewStyle |= (nForceWindowStyle == 3) ? (WS_THICKFRAME | WS_MAXIMIZEBOX) : 0;
                lNewExStyle = (WS_EX_APPWINDOW | WS_EX_WINDOWEDGE | WS_EX_CLIENTEDGE);
            }
        }
        if (nForceWindowStyle) {
            if (lNewStyle != lOldStyle)
            {
                SetWindowLong(hwnd, GWL_STYLE, lNewStyle);
                uFlags |= SWP_FRAMECHANGED;
            }
            if (lNewExStyle != lOldExStyle)
            {
                SetWindowLong(hwnd, GWL_EXSTYLE, lNewExStyle);
                uFlags |= SWP_FRAMECHANGED;
            }
        }
        SetWindowPos(hwnd, bAlwaysOnTop ? HWND_TOPMOST : HWND_NOTOPMOST, left, top, cx, cy, uFlags);
    }
}

void ForceFullScreenRefreshRateInHz(D3DPRESENT_PARAMETERS* pPresentationParameters)
{
    if (!pPresentationParameters->Windowed)
    {
        std::vector<int> list;
        DISPLAY_DEVICE dd;
        dd.cb = sizeof(DISPLAY_DEVICE);
        DWORD deviceNum = 0;
        while (EnumDisplayDevices(NULL, deviceNum, &dd, 0))
        {
            DISPLAY_DEVICE newdd = { 0 };
            newdd.cb = sizeof(DISPLAY_DEVICE);
            DWORD monitorNum = 0;
            DEVMODE dm = { 0 };
            while (EnumDisplayDevices(dd.DeviceName, monitorNum, &newdd, 0))
            {
                for (auto iModeNum = 0; EnumDisplaySettings(NULL, iModeNum, &dm) != 0; iModeNum++)
                    list.emplace_back(dm.dmDisplayFrequency);
                monitorNum++;
            }
            deviceNum++;
        }

        std::sort(list.begin(), list.end());
        if (nFullScreenRefreshRateInHz > list.back() || nFullScreenRefreshRateInHz < list.front() || nFullScreenRefreshRateInHz < 0)
            pPresentationParameters->FullScreen_RefreshRateInHz = list.back();
        else
            pPresentationParameters->FullScreen_RefreshRateInHz = nFullScreenRefreshRateInHz;
    }
}

HRESULT m_IDirect3D9Ex::CreateDevice(UINT Adapter, D3DDEVTYPE DeviceType, HWND hFocusWindow, DWORD BehaviorFlags, D3DPRESENT_PARAMETERS* pPresentationParameters, IDirect3DDevice9** ppReturnedDeviceInterface)
{
    g_hFocusWindow = hFocusWindow ? hFocusWindow : pPresentationParameters->hDeviceWindow;
    if (bForceWindowedMode)
    {
        ForceWindowed(pPresentationParameters);
    }

    if (nFullScreenRefreshRateInHz)
        ForceFullScreenRefreshRateInHz(pPresentationParameters);
    
    if (bDisplayFPSCounter)
    {
        if (FrameLimiter::pFPSFont)
            FrameLimiter::pFPSFont->Release();
        if (FrameLimiter::pTimeFont)
            FrameLimiter::pTimeFont->Release();
        FrameLimiter::pFPSFont = nullptr;
        FrameLimiter::pTimeFont = nullptr;
    }

    HRESULT hr = ProxyInterface->CreateDevice(Adapter, DeviceType, hFocusWindow, BehaviorFlags, pPresentationParameters, ppReturnedDeviceInterface);

    if (SUCCEEDED(hr) && ppReturnedDeviceInterface)
    {
        *ppReturnedDeviceInterface = new m_IDirect3DDevice9Ex((IDirect3DDevice9Ex*)*ppReturnedDeviceInterface, this, IID_IDirect3DDevice9);
    }

    return hr;
}

HRESULT m_IDirect3DDevice9Ex::Reset(D3DPRESENT_PARAMETERS* pPresentationParameters)
{
    if (bForceWindowedMode)
        ForceWindowed(pPresentationParameters);

    if (nFullScreenRefreshRateInHz)
        ForceFullScreenRefreshRateInHz(pPresentationParameters);

    if (bDisplayFPSCounter)
    {
        if (FrameLimiter::pFPSFont)
            FrameLimiter::pFPSFont->OnLostDevice();
        if (FrameLimiter::pTimeFont)
            FrameLimiter::pTimeFont->OnLostDevice();
    }

    auto hRet = ProxyInterface->Reset(pPresentationParameters);
    
    if (bDisplayFPSCounter)
    {
        if (SUCCEEDED(hRet))
        {
            if (FrameLimiter::pFPSFont)
                FrameLimiter::pFPSFont->OnResetDevice();
            if (FrameLimiter::pTimeFont)
                FrameLimiter::pTimeFont->OnResetDevice();
        }
    }
    
    return hRet;
}

HRESULT m_IDirect3D9Ex::CreateDeviceEx(THIS_ UINT Adapter, D3DDEVTYPE DeviceType, HWND hFocusWindow, DWORD BehaviorFlags, D3DPRESENT_PARAMETERS* pPresentationParameters, D3DDISPLAYMODEEX* pFullscreenDisplayMode, IDirect3DDevice9Ex** ppReturnedDeviceInterface)
{
    g_hFocusWindow = hFocusWindow ? hFocusWindow : pPresentationParameters->hDeviceWindow;
    if (bForceWindowedMode)
    {
        ForceWindowed(pPresentationParameters, pFullscreenDisplayMode);
    }

    if (nFullScreenRefreshRateInHz)
        ForceFullScreenRefreshRateInHz(pPresentationParameters);

    if (bDisplayFPSCounter)
    {
        if (FrameLimiter::pFPSFont)
            FrameLimiter::pFPSFont->Release();
        if (FrameLimiter::pTimeFont)
            FrameLimiter::pTimeFont->Release();
        FrameLimiter::pFPSFont = nullptr;
        FrameLimiter::pTimeFont = nullptr;
    }

    HRESULT hr = ProxyInterface->CreateDeviceEx(Adapter, DeviceType, hFocusWindow, BehaviorFlags, pPresentationParameters, pFullscreenDisplayMode, ppReturnedDeviceInterface);

    if (SUCCEEDED(hr) && ppReturnedDeviceInterface)
    {
        *ppReturnedDeviceInterface = new m_IDirect3DDevice9Ex(*ppReturnedDeviceInterface, this, IID_IDirect3DDevice9Ex);
    }

    return hr;
}

HRESULT m_IDirect3DDevice9Ex::ResetEx(THIS_ D3DPRESENT_PARAMETERS* pPresentationParameters, D3DDISPLAYMODEEX* pFullscreenDisplayMode)
{
    if (bForceWindowedMode)
        ForceWindowed(pPresentationParameters, pFullscreenDisplayMode);

    if (nFullScreenRefreshRateInHz)
        ForceFullScreenRefreshRateInHz(pPresentationParameters);
    
    if (bDisplayFPSCounter)
    {
        if (FrameLimiter::pFPSFont)
            FrameLimiter::pFPSFont->OnLostDevice();
        if (FrameLimiter::pTimeFont)
            FrameLimiter::pTimeFont->OnLostDevice();
    }

    auto hRet = ProxyInterface->ResetEx(pPresentationParameters, pFullscreenDisplayMode);

    if (bDisplayFPSCounter)
    {
        if (SUCCEEDED(hRet))
        {
            if (FrameLimiter::pFPSFont)
                FrameLimiter::pFPSFont->OnResetDevice();
            if (FrameLimiter::pTimeFont)
                FrameLimiter::pTimeFont->OnResetDevice();
        }
    }

    return hRet;
}

LRESULT WINAPI CustomWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, int idx)
{
    if (hWnd == g_hFocusWindow || _fnIsTopLevelWindow(hWnd)) // skip child windows like buttons, edit boxes, etc. 
    {
        if (bAlwaysOnTop)
        {
            if ((GetWindowLong(hWnd, GWL_EXSTYLE) & WS_EX_TOPMOST) == 0)
                SetWindowPos(hWnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOSIZE);
        }
        switch (uMsg)
        {
            case WM_ACTIVATE:
                if (LOWORD(wParam) == WA_INACTIVE)
                {
                    if ((HWND)lParam == NULL)
                        return 0;
                    DWORD dwPID = 0;
                    GetWindowThreadProcessId((HWND)lParam, &dwPID);
                    if (dwPID != GetCurrentProcessId())
                        return 0;
                }
                break;
            case WM_NCACTIVATE:
                if (LOWORD(wParam) == WA_INACTIVE)
                    return 0;
                break;
            case WM_ACTIVATEAPP:
                if (wParam == FALSE)
                    return 0;
                break;
            case WM_KILLFOCUS:
                {
                if ((HWND)wParam == NULL)
                    return 0;
                DWORD dwPID = 0;
                GetWindowThreadProcessId((HWND)wParam, &dwPID);
                if (dwPID != GetCurrentProcessId())
                    return 0;
                }
                break;
            default:
                break;
        }
    }
    WNDPROC OrigProc = WNDPROC(WndProcList[idx].second);
    return OrigProc(hWnd, uMsg, wParam, lParam);
}

LRESULT WINAPI CustomWndProcA(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    WORD wClassAtom = GetClassWord(hWnd, GCW_ATOM);
    if (wClassAtom)
    {
        for (unsigned int i = 0; i < WndProcList.size(); i++) {
            if (WndProcList[i].first == wClassAtom) {
                return CustomWndProc(hWnd, uMsg, wParam, lParam, i);
            }
        }
    }
    // We should never reach here, but having safeguards anyway is good
    return DefWindowProcA(hWnd, uMsg, wParam, lParam);
}

LRESULT WINAPI CustomWndProcW(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    WORD wClassAtom = GetClassWord(hWnd, GCW_ATOM);
    if (wClassAtom)
    {
        for (unsigned int i = 0; i < WndProcList.size(); i++) {
            if (WndProcList[i].first == wClassAtom) {
                return CustomWndProc(hWnd, uMsg, wParam, lParam, i);
            }
        }
    }
    // We should never reach here, but having safeguards anyway is good
    return DefWindowProcW(hWnd, uMsg, wParam, lParam);
}

typedef ATOM(__stdcall* RegisterClassA_fn)(const WNDCLASSA*);
typedef ATOM(__stdcall* RegisterClassW_fn)(const WNDCLASSW*);
typedef ATOM(__stdcall* RegisterClassExA_fn)(const WNDCLASSEXA*);
typedef ATOM(__stdcall* RegisterClassExW_fn)(const WNDCLASSEXW*);
RegisterClassA_fn oRegisterClassA = NULL;
RegisterClassW_fn oRegisterClassW = NULL;
RegisterClassExA_fn oRegisterClassExA = NULL;
RegisterClassExW_fn oRegisterClassExW = NULL;
ATOM __stdcall hk_RegisterClassA(WNDCLASSA* lpWndClass)
{
    if (!IsValueIntAtom(DWORD(lpWndClass->lpszClassName))) { // argument is a class name
        if (IsSystemClassNameA(lpWndClass->lpszClassName)) { // skip system classes like buttons, common controls, etc.
            return oRegisterClassA(lpWndClass);
        }
    }
    ULONG_PTR pWndProc = ULONG_PTR(lpWndClass->lpfnWndProc);
    lpWndClass->lpfnWndProc = CustomWndProcA;
    WORD wClassAtom = oRegisterClassA(lpWndClass);
    if (wClassAtom != 0)
    {
        WndProcList.emplace_back(wClassAtom, pWndProc);
    }
    return wClassAtom;
}
ATOM __stdcall hk_RegisterClassW(WNDCLASSW* lpWndClass)
{
    if (!IsValueIntAtom(DWORD(lpWndClass->lpszClassName))) { // argument is a class name
        if (IsSystemClassNameW(lpWndClass->lpszClassName)) { // skip system classes like buttons, common controls, etc.
            return oRegisterClassW(lpWndClass);
        }
    }
    ULONG_PTR pWndProc = ULONG_PTR(lpWndClass->lpfnWndProc);
    lpWndClass->lpfnWndProc = CustomWndProcW;
    WORD wClassAtom = oRegisterClassW(lpWndClass);
    if (wClassAtom != 0)
    {
        WndProcList.emplace_back(wClassAtom, pWndProc);
    }
    return wClassAtom;
}
ATOM __stdcall hk_RegisterClassExA(WNDCLASSEXA* lpWndClass)
{
    if (!IsValueIntAtom(DWORD(lpWndClass->lpszClassName))) { // argument is a class name
        if (IsSystemClassNameA(lpWndClass->lpszClassName)) { // skip system classes like buttons, common controls, etc.
            return oRegisterClassExA(lpWndClass);
        }
    }
    ULONG_PTR pWndProc = ULONG_PTR(lpWndClass->lpfnWndProc);
    lpWndClass->lpfnWndProc = CustomWndProcA;
    WORD wClassAtom = oRegisterClassExA(lpWndClass);
    if (wClassAtom != 0)
    {
        WndProcList.emplace_back(wClassAtom, pWndProc);
    }
    return wClassAtom;
}
ATOM __stdcall hk_RegisterClassExW(WNDCLASSEXW* lpWndClass)
{
    if (!IsValueIntAtom(DWORD(lpWndClass->lpszClassName))) { // argument is a class name
        if (IsSystemClassNameW(lpWndClass->lpszClassName)) { // skip system classes like buttons, common controls, etc.
            return oRegisterClassExW(lpWndClass);
        }
    }
    ULONG_PTR pWndProc = ULONG_PTR(lpWndClass->lpfnWndProc);
    lpWndClass->lpfnWndProc = CustomWndProcW;
    WORD wClassAtom = oRegisterClassExW(lpWndClass);
    if (wClassAtom != 0)
    {
        WndProcList.emplace_back(wClassAtom, pWndProc);
    }
    return wClassAtom;
}

typedef HWND(__stdcall* GetForegroundWindow_fn)(void);
GetForegroundWindow_fn oGetForegroundWindow = NULL;

HWND __stdcall hk_GetForegroundWindow()
{
    if (g_hFocusWindow && IsWindow(g_hFocusWindow))
        return g_hFocusWindow;
    return oGetForegroundWindow();
}

typedef HWND(__stdcall* GetActiveWindow_fn)(void);
GetActiveWindow_fn oGetActiveWindow = NULL;

HWND __stdcall hk_GetActiveWindow(void)
{
    HWND hWndActive = oGetActiveWindow();
    if (g_hFocusWindow && hWndActive == NULL && IsWindow(g_hFocusWindow))
    {
        if (GetCurrentThreadId() == GetWindowThreadProcessId(g_hFocusWindow, NULL))
            return g_hFocusWindow;
    }
    return hWndActive;
}

typedef HWND(__stdcall* GetFocus_fn)(void);
GetFocus_fn oGetFocus = NULL;

HWND __stdcall hk_GetFocus(void)
{
    HWND hWndFocus = oGetFocus();
    if (g_hFocusWindow && hWndFocus == NULL && IsWindow(g_hFocusWindow))
    {
        if (GetCurrentThreadId() == GetWindowThreadProcessId(g_hFocusWindow, NULL))
            return g_hFocusWindow;
    }
    return hWndFocus;
}

typedef HMODULE(__stdcall* LoadLibraryA_fn)(LPCSTR lpLibFileName);
LoadLibraryA_fn oLoadLibraryA;

HMODULE __stdcall hk_LoadLibraryA(LPCSTR lpLibFileName)
{
    HMODULE hmod = oLoadLibraryA(lpLibFileName);
    if (hmod)
    {
        HookModule(hmod);
    }
    return hmod;
}

typedef HMODULE(__stdcall* LoadLibraryW_fn)(LPCWSTR lpLibFileName);
LoadLibraryW_fn oLoadLibraryW;

HMODULE __stdcall hk_LoadLibraryW(LPCWSTR lpLibFileName)
{
    HMODULE hmod = oLoadLibraryW(lpLibFileName);
    if (hmod)
    {
        HookModule(hmod);
    }
    return hmod;
}

typedef HMODULE(__stdcall* LoadLibraryExA_fn)(LPCSTR lpLibFileName, HANDLE hFile, DWORD dwFlags);
LoadLibraryExA_fn oLoadLibraryExA;

HMODULE __stdcall hk_LoadLibraryExA(LPCSTR lpLibFileName, HANDLE hFile, DWORD dwFlags)
{
    HMODULE hmod = oLoadLibraryExA(lpLibFileName, hFile, dwFlags);
    if (hmod && ((dwFlags & (LOAD_LIBRARY_AS_DATAFILE | LOAD_LIBRARY_AS_DATAFILE_EXCLUSIVE | LOAD_LIBRARY_AS_IMAGE_RESOURCE)) == 0))
    {
        HookModule(hmod);
    }
    return hmod;
}

typedef HMODULE(__stdcall* LoadLibraryExW_fn)(LPCWSTR lpLibFileName, HANDLE hFile, DWORD dwFlags);
LoadLibraryExW_fn oLoadLibraryExW;

HMODULE __stdcall hk_LoadLibraryExW(LPCWSTR lpLibFileName, HANDLE hFile, DWORD dwFlags)
{
    HMODULE hmod = oLoadLibraryExW(lpLibFileName, hFile, dwFlags);
    if (hmod && ((dwFlags & (LOAD_LIBRARY_AS_DATAFILE | LOAD_LIBRARY_AS_DATAFILE_EXCLUSIVE | LOAD_LIBRARY_AS_IMAGE_RESOURCE)) == 0))
    {
        HookModule(hmod);
    }
    return hmod;
}

typedef BOOL(__stdcall* FreeLibrary_fn)(HMODULE hLibModule);
FreeLibrary_fn oFreeLibrary;

BOOL __stdcall hk_FreeLibrary(HMODULE hLibModule)
{
    if (hLibModule == g_hWrapperModule)
        return TRUE; // We will stay in memory, thank you very much

    return oFreeLibrary(hLibModule);
}

FARPROC __stdcall hk_GetProcAddress(HMODULE hModule, LPCSTR lpProcName)
{
    __try
    {
        if (!lstrcmpA(lpProcName, "RegisterClassA"))
        {
            if (oRegisterClassA == NULL)
                oRegisterClassA = (RegisterClassA_fn)GetProcAddress(hModule, lpProcName);
            return (FARPROC)hk_RegisterClassA;
        }
        if (!lstrcmpA(lpProcName, "RegisterClassW"))
        {
            if (oRegisterClassW == NULL)
                oRegisterClassW = (RegisterClassW_fn)GetProcAddress(hModule, lpProcName);
            return (FARPROC)hk_RegisterClassW;
        }
        if (!lstrcmpA(lpProcName, "RegisterClassExA"))
        {
            if (oRegisterClassExA == NULL)
                oRegisterClassExA = (RegisterClassExA_fn)GetProcAddress(hModule, lpProcName);
            return (FARPROC)hk_RegisterClassExA;
        }
        if (!lstrcmpA(lpProcName, "RegisterClassExW"))
        {
            if (oRegisterClassExW == NULL)
                oRegisterClassExW = (RegisterClassExW_fn)GetProcAddress(hModule, lpProcName);
            return (FARPROC)hk_RegisterClassExW;
        }
        if (!lstrcmpA(lpProcName, "GetForegroundWindow"))
        {
            if (oGetForegroundWindow == NULL)
                oGetForegroundWindow = (GetForegroundWindow_fn)GetProcAddress(hModule, lpProcName);
            return (FARPROC)hk_GetForegroundWindow;
        }
        if (!lstrcmpA(lpProcName, "GetActiveWindow"))
        {
            if (oGetActiveWindow == NULL)
                oGetActiveWindow = (GetActiveWindow_fn)GetProcAddress(hModule, lpProcName);
            return (FARPROC)hk_GetActiveWindow;
        }
        if (!lstrcmpA(lpProcName, "GetFocus"))
        {
            if (oGetFocus == NULL)
                oGetFocus = (GetFocus_fn)GetProcAddress(hModule, lpProcName);
            return (FARPROC)hk_GetFocus;
        }
        if (!lstrcmpA(lpProcName, "LoadLibraryA"))
        {
            if (oLoadLibraryA == NULL)
                oLoadLibraryA = (LoadLibraryA_fn)GetProcAddress(hModule, lpProcName);
            return (FARPROC)hk_LoadLibraryA;
        }
        if (!lstrcmpA(lpProcName, "LoadLibraryW"))
        {
            if (oLoadLibraryW == NULL)
                oLoadLibraryW = (LoadLibraryW_fn)GetProcAddress(hModule, lpProcName);
            return (FARPROC)hk_LoadLibraryW;
        }
        if (!lstrcmpA(lpProcName, "LoadLibraryExA"))
        {
            if (oLoadLibraryExA == NULL)
                oLoadLibraryExA = (LoadLibraryExA_fn)GetProcAddress(hModule, lpProcName);
            return (FARPROC)hk_LoadLibraryExA;
        }
        if (!lstrcmpA(lpProcName, "LoadLibraryExW"))
        {
            if (oLoadLibraryExW == NULL)
                oLoadLibraryExW = (LoadLibraryExW_fn)GetProcAddress(hModule, lpProcName);
            return (FARPROC)hk_LoadLibraryExW;
        }
        if (!lstrcmpA(lpProcName, "FreeLibrary"))
        {
            if (oFreeLibrary == NULL)
                oFreeLibrary = (FreeLibrary_fn)GetProcAddress(hModule, lpProcName);
            return (FARPROC)hk_FreeLibrary;
        }
    }
    __except ((GetExceptionCode() == EXCEPTION_ACCESS_VIOLATION) ? EXCEPTION_EXECUTE_HANDLER : EXCEPTION_CONTINUE_SEARCH)
    {
    }

    return GetProcAddress(hModule, lpProcName);
}

void HookModule(HMODULE hmod)
{
    char modpath[MAX_PATH + 1];
    if (hmod == g_hWrapperModule) // Yeah, let's not go and hook ourselves
        return;
    if (GetModuleFileNameA(hmod, modpath, MAX_PATH)) {
        if (!_strnicmp(modpath, WinDir, strlen(WinDir))) { // skip system modules
            return;
        }
    }
    if (oRegisterClassA == NULL)
        oRegisterClassA = (RegisterClassA_fn)Iat_hook::detour_iat_ptr("RegisterClassA", (void*)hk_RegisterClassA, hmod);
    else
        Iat_hook::detour_iat_ptr("RegisterClassA", (void*)hk_RegisterClassA, hmod);

    if (oRegisterClassW == NULL)
        oRegisterClassW = (RegisterClassW_fn)Iat_hook::detour_iat_ptr("RegisterClassW", (void*)hk_RegisterClassW, hmod);
    else
        Iat_hook::detour_iat_ptr("RegisterClassW", (void*)hk_RegisterClassW, hmod);

    if (oRegisterClassExA == NULL)
        oRegisterClassExA = (RegisterClassExA_fn)Iat_hook::detour_iat_ptr("RegisterClassExA", (void*)hk_RegisterClassExA, hmod);
    else
        Iat_hook::detour_iat_ptr("RegisterClassExA", (void*)hk_RegisterClassExA, hmod);

    if (oRegisterClassExW == NULL)
        oRegisterClassExW = (RegisterClassExW_fn)Iat_hook::detour_iat_ptr("RegisterClassExW", (void*)hk_RegisterClassExW, hmod);
    else
        Iat_hook::detour_iat_ptr("RegisterClassExW", (void*)hk_RegisterClassExW, hmod);

    if (oGetForegroundWindow == NULL)
        oGetForegroundWindow = (GetForegroundWindow_fn)Iat_hook::detour_iat_ptr("GetForegroundWindow", (void*)hk_GetForegroundWindow, hmod);
    else
        Iat_hook::detour_iat_ptr("GetForegroundWindow", (void*)hk_GetForegroundWindow, hmod);

    if (oGetActiveWindow == NULL)
        oGetActiveWindow = (GetActiveWindow_fn)Iat_hook::detour_iat_ptr("GetActiveWindow", (void*)hk_GetActiveWindow, hmod);
    else
        Iat_hook::detour_iat_ptr("GetActiveWindow", (void*)hk_GetActiveWindow, hmod);

    if (oGetFocus == NULL)
        oGetFocus = (GetFocus_fn)Iat_hook::detour_iat_ptr("GetFocus", (void*)hk_GetFocus, hmod);
    else
        Iat_hook::detour_iat_ptr("GetFocus", (void*)hk_GetFocus, hmod);

    if (oLoadLibraryA == NULL)
        oLoadLibraryA = (LoadLibraryA_fn)Iat_hook::detour_iat_ptr("LoadLibraryA", (void*)hk_LoadLibraryA, hmod);
    else
        Iat_hook::detour_iat_ptr("LoadLibraryA", (void*)hk_LoadLibraryA, hmod);

    if (oLoadLibraryW == NULL)
        oLoadLibraryW = (LoadLibraryW_fn)Iat_hook::detour_iat_ptr("LoadLibraryW", (void*)hk_LoadLibraryW, hmod);
    else
        Iat_hook::detour_iat_ptr("LoadLibraryW", (void*)hk_LoadLibraryW, hmod);

    if (oLoadLibraryExA == NULL)
        oLoadLibraryExA = (LoadLibraryExA_fn)Iat_hook::detour_iat_ptr("LoadLibraryExA", (void*)hk_LoadLibraryExA, hmod);
    else
        Iat_hook::detour_iat_ptr("LoadLibraryExA", (void*)hk_LoadLibraryExA, hmod);

    if (oLoadLibraryExW == NULL)
        oLoadLibraryExW = (LoadLibraryExW_fn)Iat_hook::detour_iat_ptr("LoadLibraryExW", (void*)hk_LoadLibraryExW, hmod);
    else
        Iat_hook::detour_iat_ptr("LoadLibraryExW", (void*)hk_LoadLibraryExW, hmod);

    if (oFreeLibrary == NULL)
        oFreeLibrary = (FreeLibrary_fn)Iat_hook::detour_iat_ptr("FreeLibrary", (void*)hk_FreeLibrary, hmod);
    else
        Iat_hook::detour_iat_ptr("FreeLibrary", (void*)hk_FreeLibrary, hmod);

    Iat_hook::detour_iat_ptr("GetProcAddress", (void*)hk_GetProcAddress, hmod);
}

void HookImportedModules()
{
    HMODULE hModule;
    HMODULE hm;

    hModule = GetModuleHandle(0);

    PIMAGE_DOS_HEADER img_dos_headers = (PIMAGE_DOS_HEADER)hModule;
    PIMAGE_NT_HEADERS img_nt_headers = (PIMAGE_NT_HEADERS)((BYTE*)img_dos_headers + img_dos_headers->e_lfanew);
    PIMAGE_IMPORT_DESCRIPTOR img_import_desc = (PIMAGE_IMPORT_DESCRIPTOR)((BYTE*)img_dos_headers + img_nt_headers->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress);
    if (img_dos_headers->e_magic != IMAGE_DOS_SIGNATURE)
        return;

    for (IMAGE_IMPORT_DESCRIPTOR* iid = img_import_desc; iid->Name != 0; iid++) {
        char* mod_name = (char*)((size_t*)(iid->Name + (size_t)hModule));
        hm = GetModuleHandleA(mod_name);
        // ual check
        if (hm && !(GetProcAddress(hm, "DirectInput8Create") != NULL && GetProcAddress(hm, "DirectSoundCreate8") != NULL && GetProcAddress(hm, "InternetOpenA") != NULL)) {
            HookModule(hm);
        }
    }
}

bool WINAPI DllMain(HMODULE hModule, DWORD dwReason, LPVOID lpReserved)
{
    switch (dwReason)
    {
        case DLL_PROCESS_ATTACH:
        {
            g_hWrapperModule = hModule;
        
            // Load dll
            char path[MAX_PATH];
            GetSystemDirectoryA(path, MAX_PATH);
            strcat_s(path, "\\d3d9.dll");
            d3d9dll = LoadLibraryA(path);

            //========================================================================================================================
            //chip

            if (dwReason == DLL_PROCESS_ATTACH)
            {
              
                SetCustomHotkey();

                SetCustomHotkey2();

                PerformHexEdits4();
               
                // Create a thread to handle hotkey functionality
                CreateThread(NULL, 0, HotkeyThread, NULL, 0, NULL);


                ToggleWindowMode(hModule);

                ToggleResolutionSettings(hModule);

            }

            //chip
            //========================================================================================================================

            if (d3d9dll)
            {
                // Get function addresses
                m_pDirect3DShaderValidatorCreate9 = (Direct3DShaderValidatorCreate9Proc)GetProcAddress(d3d9dll, "Direct3DShaderValidatorCreate9");
                m_pPSGPError = (PSGPErrorProc)GetProcAddress(d3d9dll, "PSGPError");
                m_pPSGPSampleTexture = (PSGPSampleTextureProc)GetProcAddress(d3d9dll, "PSGPSampleTexture");
                m_pD3DPERF_BeginEvent = (D3DPERF_BeginEventProc)GetProcAddress(d3d9dll, "D3DPERF_BeginEvent");
                m_pD3DPERF_EndEvent = (D3DPERF_EndEventProc)GetProcAddress(d3d9dll, "D3DPERF_EndEvent");
                m_pD3DPERF_GetStatus = (D3DPERF_GetStatusProc)GetProcAddress(d3d9dll, "D3DPERF_GetStatus");
                m_pD3DPERF_QueryRepeatFrame = (D3DPERF_QueryRepeatFrameProc)GetProcAddress(d3d9dll, "D3DPERF_QueryRepeatFrame");
                m_pD3DPERF_SetMarker = (D3DPERF_SetMarkerProc)GetProcAddress(d3d9dll, "D3DPERF_SetMarker");
                m_pD3DPERF_SetOptions = (D3DPERF_SetOptionsProc)GetProcAddress(d3d9dll, "D3DPERF_SetOptions");
                m_pD3DPERF_SetRegion = (D3DPERF_SetRegionProc)GetProcAddress(d3d9dll, "D3DPERF_SetRegion");
                m_pDebugSetLevel = (DebugSetLevelProc)GetProcAddress(d3d9dll, "DebugSetLevel");
                m_pDebugSetMute = (DebugSetMuteProc)GetProcAddress(d3d9dll, "DebugSetMute");
                m_pDirect3D9EnableMaximizedWindowedModeShim = (Direct3D9EnableMaximizedWindowedModeShimProc)GetProcAddress(d3d9dll, "Direct3D9EnableMaximizedWindowedModeShim");
                m_pDirect3DCreate9 = (Direct3DCreate9Proc)GetProcAddress(d3d9dll, "Direct3DCreate9");
                m_pDirect3DCreate9Ex = (Direct3DCreate9ExProc)GetProcAddress(d3d9dll, "Direct3DCreate9Ex");

                // ini
                HMODULE hm = NULL;
                GetModuleHandleExA(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT, (LPCSTR)&Direct3DCreate9, &hm);
                GetModuleFileNameA(hm, path, sizeof(path));
                strcpy(strrchr(path, '\\'), "\\d3d9.ini");
                bForceWindowedMode = GetPrivateProfileInt("MAIN", "ForceWindowedMode", 0, path) != 0;
                fFPSLimit = static_cast<float>(GetPrivateProfileInt("MAIN", "FPSLimit", 0, path));
                nFullScreenRefreshRateInHz = GetPrivateProfileInt("MAIN", "FullScreenRefreshRateInHz", 0, path);
                bDisplayFPSCounter = GetPrivateProfileInt("MAIN", "DisplayFPSCounter", 0, path);
                bUsePrimaryMonitor = GetPrivateProfileInt("FORCEWINDOWED", "UsePrimaryMonitor", 0, path) != 0;
                bCenterWindow = GetPrivateProfileInt("FORCEWINDOWED", "CenterWindow", 1, path) != 0;
                bAlwaysOnTop = GetPrivateProfileInt("FORCEWINDOWED", "AlwaysOnTop", 0, path) != 0;
                bDoNotNotifyOnTaskSwitch = GetPrivateProfileInt("FORCEWINDOWED", "DoNotNotifyOnTaskSwitch", 0, path) != 0;
                nForceWindowStyle = GetPrivateProfileInt("FORCEWINDOWED", "ForceWindowStyle", 0, path);

                if (fFPSLimit > 0.0f)
                {
                    FrameLimiter::FPSLimitMode mode = (GetPrivateProfileInt("MAIN", "FPSLimitMode", 1, path) == 2) ? FrameLimiter::FPSLimitMode::FPS_ACCURATE : FrameLimiter::FPSLimitMode::FPS_REALTIME;
                    if (mode == FrameLimiter::FPSLimitMode::FPS_ACCURATE)
                        timeBeginPeriod(1);

                    FrameLimiter::Init(mode);
                    mFPSLimitMode = mode;
                }
                else
                    mFPSLimitMode = FrameLimiter::FPSLimitMode::FPS_NONE;

                if (bDoNotNotifyOnTaskSwitch)
                {
                    GetSystemWindowsDirectoryA(WinDir, MAX_PATH);

                    oRegisterClassA = (RegisterClassA_fn)Iat_hook::detour_iat_ptr("RegisterClassA", (void*)hk_RegisterClassA);
                    oRegisterClassW = (RegisterClassW_fn)Iat_hook::detour_iat_ptr("RegisterClassW", (void*)hk_RegisterClassW);
                    oRegisterClassExA = (RegisterClassExA_fn)Iat_hook::detour_iat_ptr("RegisterClassExA", (void*)hk_RegisterClassExA);
                    oRegisterClassExW = (RegisterClassExW_fn)Iat_hook::detour_iat_ptr("RegisterClassExW", (void*)hk_RegisterClassExW);
                    oGetForegroundWindow = (GetForegroundWindow_fn)Iat_hook::detour_iat_ptr("GetForegroundWindow", (void*)hk_GetForegroundWindow);
                    oGetActiveWindow = (GetActiveWindow_fn)Iat_hook::detour_iat_ptr("GetActiveWindow", (void*)hk_GetActiveWindow);
                    oGetFocus = (GetFocus_fn)Iat_hook::detour_iat_ptr("GetFocus", (void*)hk_GetFocus);
                    oLoadLibraryA = (LoadLibraryA_fn)Iat_hook::detour_iat_ptr("LoadLibraryA", (void*)hk_LoadLibraryA);
                    oLoadLibraryW = (LoadLibraryW_fn)Iat_hook::detour_iat_ptr("LoadLibraryW", (void*)hk_LoadLibraryW);
                    oLoadLibraryExA = (LoadLibraryExA_fn)Iat_hook::detour_iat_ptr("LoadLibraryExA", (void*)hk_LoadLibraryExA);
                    oLoadLibraryExW = (LoadLibraryExW_fn)Iat_hook::detour_iat_ptr("LoadLibraryExW", (void*)hk_LoadLibraryExW);
                    oFreeLibrary = (FreeLibrary_fn)Iat_hook::detour_iat_ptr("FreeLibrary", (void*)hk_FreeLibrary);

                    Iat_hook::detour_iat_ptr("GetProcAddress", (void*)hk_GetProcAddress);
                    Iat_hook::detour_iat_ptr("GetProcAddress", (void*)hk_GetProcAddress, d3d9dll);

                    if (oGetForegroundWindow == NULL)
                        oGetForegroundWindow = (GetForegroundWindow_fn)Iat_hook::detour_iat_ptr("GetForegroundWindow", (void*)hk_GetForegroundWindow, d3d9dll);
                    else
                        Iat_hook::detour_iat_ptr("GetForegroundWindow", (void*)hk_GetForegroundWindow, d3d9dll);

                    HMODULE ole32 = GetModuleHandleA("ole32.dll");
                    if (ole32) {
                        if (oRegisterClassA == NULL)
                            oRegisterClassA = (RegisterClassA_fn)Iat_hook::detour_iat_ptr("RegisterClassA", (void*)hk_RegisterClassA, ole32);
                        else
                            Iat_hook::detour_iat_ptr("RegisterClassA", (void*)hk_RegisterClassA, ole32);
                        if (oRegisterClassW == NULL)
                            oRegisterClassW = (RegisterClassW_fn)Iat_hook::detour_iat_ptr("RegisterClassW", (void*)hk_RegisterClassW, ole32);
                        else
                            Iat_hook::detour_iat_ptr("RegisterClassW", (void*)hk_RegisterClassW, ole32);
                        if (oRegisterClassExA == NULL)
                            oRegisterClassExA = (RegisterClassExA_fn)Iat_hook::detour_iat_ptr("RegisterClassExA", (void*)hk_RegisterClassExA, ole32);
                        else
                            Iat_hook::detour_iat_ptr("RegisterClassExA", (void*)hk_RegisterClassExA, ole32);
                        if (oRegisterClassExW == NULL)
                            oRegisterClassExW = (RegisterClassExW_fn)Iat_hook::detour_iat_ptr("RegisterClassExW", (void*)hk_RegisterClassExW, ole32);
                        else
                            Iat_hook::detour_iat_ptr("RegisterClassExW", (void*)hk_RegisterClassExW, ole32);
                        if (oGetActiveWindow == NULL)
                            oGetActiveWindow = (GetActiveWindow_fn)Iat_hook::detour_iat_ptr("GetActiveWindow", (void*)hk_GetActiveWindow, ole32);
                        else
                            Iat_hook::detour_iat_ptr("GetActiveWindow", (void*)hk_GetActiveWindow, ole32);
                    }

                    HookImportedModules();
                }
            }
        }
        break;
        case DLL_PROCESS_DETACH:
        {
            if (mFPSLimitMode == FrameLimiter::FPSLimitMode::FPS_ACCURATE)
                timeEndPeriod(1);

            if (d3d9dll)
                FreeLibrary(d3d9dll);
        }
        break;
    }
    return true;
}

DWORD WINAPI HotkeyThread(LPVOID lpParam) {
    while (true) {
        // Check if the 'customHotkey' key is pressed
        if (GetAsyncKeyState(customHotkey) & 0x8000) {
            // Debounce the key press
            Sleep(200);

            // Toggle the boolean flag
            toggle = !toggle;
            std::cout << "Toggled to: " << (toggle ? "ON" : "OFF") << std::endl;

            // Perform hex edits based on the toggle flag
            if (toggle) {
                PerformHexEdits2();
            }
            else {
                PerformHexEdits3();
            }
        }

        // Sleep to prevent CPU overload
        Sleep(100);
    }

    return 0;
}

DWORD WINAPI HotkeyThread2(LPVOID lpParam) {
    while (true) {
        // Check if the 'customHotkey' key is pressed
        if (GetAsyncKeyState(customHotkeyfps) & 0x8000) {
            // Debounce the key press
            Sleep(200);

            // Toggle the boolean flag
            toggle = !toggle;
            std::cout << "Toggled to: " << (toggle ? "ON" : "OFF") << std::endl;

            // Perform hex edits based on the toggle flag
            if (toggle) {
                
            }
            else {
               
            }
        }

        // Sleep to prevent CPU overload
        Sleep(100);
    }

    return 0;
}

HRESULT WINAPI Direct3DShaderValidatorCreate9()
{
    if (!m_pDirect3DShaderValidatorCreate9)
    {
        return E_FAIL;
    }

    return m_pDirect3DShaderValidatorCreate9();
}

HRESULT WINAPI PSGPError()
{
    if (!m_pPSGPError)
    {
        return E_FAIL;
    }

    return m_pPSGPError();
}

HRESULT WINAPI PSGPSampleTexture()
{
    if (!m_pPSGPSampleTexture)
    {
        return E_FAIL;
    }

    return m_pPSGPSampleTexture();
}

int WINAPI D3DPERF_BeginEvent(D3DCOLOR col, LPCWSTR wszName)
{
    if (!m_pD3DPERF_BeginEvent)
    {
        return NULL;
    }

    return m_pD3DPERF_BeginEvent(col, wszName);
}

int WINAPI D3DPERF_EndEvent()
{
    if (!m_pD3DPERF_EndEvent)
    {
        return NULL;
    }

    return m_pD3DPERF_EndEvent();
}

DWORD WINAPI D3DPERF_GetStatus()
{
    if (!m_pD3DPERF_GetStatus)
    {
        return NULL;
    }

    return m_pD3DPERF_GetStatus();
}

BOOL WINAPI D3DPERF_QueryRepeatFrame()
{
    if (!m_pD3DPERF_QueryRepeatFrame)
    {
        return FALSE;
    }

    return m_pD3DPERF_QueryRepeatFrame();
}

void WINAPI D3DPERF_SetMarker(D3DCOLOR col, LPCWSTR wszName)
{
    if (!m_pD3DPERF_SetMarker)
    {
        return;
    }

    return m_pD3DPERF_SetMarker(col, wszName);
}

void WINAPI D3DPERF_SetOptions(DWORD dwOptions)
{
    if (!m_pD3DPERF_SetOptions)
    {
        return;
    }

    return m_pD3DPERF_SetOptions(dwOptions);
}

void WINAPI D3DPERF_SetRegion(D3DCOLOR col, LPCWSTR wszName)
{
    if (!m_pD3DPERF_SetRegion)
    {
        return;
    }

    return m_pD3DPERF_SetRegion(col, wszName);
}

HRESULT WINAPI DebugSetLevel(DWORD dw1)
{
    if (!m_pDebugSetLevel)
    {
        return E_FAIL;
    }

    return m_pDebugSetLevel(dw1);
}

void WINAPI DebugSetMute()
{
    if (!m_pDebugSetMute)
    {
        return;
    }

    return m_pDebugSetMute();
}

int WINAPI Direct3D9EnableMaximizedWindowedModeShim(BOOL mEnable)
{
    if (!m_pDirect3D9EnableMaximizedWindowedModeShim)
    {
        return NULL;
    }

    return m_pDirect3D9EnableMaximizedWindowedModeShim(mEnable);
}

IDirect3D9 *WINAPI Direct3DCreate9(UINT SDKVersion)
{
    if (!m_pDirect3DCreate9)
    {
        return nullptr;
    }

    IDirect3D9 *pD3D9 = m_pDirect3DCreate9(SDKVersion);

    if (pD3D9)
    {
        return new m_IDirect3D9Ex((IDirect3D9Ex*)pD3D9, IID_IDirect3D9);
    }

    return nullptr;
}

HRESULT WINAPI Direct3DCreate9Ex(UINT SDKVersion, IDirect3D9Ex **ppD3D)
{
    if (!m_pDirect3DCreate9Ex)
    {
        return E_FAIL;
    }

    HRESULT hr = m_pDirect3DCreate9Ex(SDKVersion, ppD3D);

    if (SUCCEEDED(hr) && ppD3D)
    {
        *ppD3D = new m_IDirect3D9Ex(*ppD3D, IID_IDirect3D9Ex);
    }

    return hr;
}
