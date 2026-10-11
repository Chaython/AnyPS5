#define UNICODE
#define _UNICODE
#define WIN32_LEAN_AND_MEAN
#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0A00
#endif
#include <windows.h>
#include <commdlg.h>
#include <shellapi.h>
#include <dwmapi.h>
#include <uxtheme.h>

#include "SelfExtractor.hpp"

#include <algorithm>
#include <cwchar>
#include <exception>
#include <filesystem>
#include <string>
#include <thread>
#include <vector>

namespace {

constexpr int ID_KICKER = 1100;
constexpr int ID_TITLE = 1101;
constexpr int ID_SUBTITLE = 1102;
constexpr int ID_INPUT_LABEL = 1103;
constexpr int ID_OUTPUT_LABEL = 1104;
constexpr int ID_OPTIONS_LABEL = 1105;
constexpr int ID_OPTIONS_HINT = 1106;
constexpr int ID_LOG_LABEL = 1107;
constexpr int ID_STATUS_LABEL = 1108;
constexpr int ID_INPUT = 1001;
constexpr int ID_INPUT_BROWSE = 1002;
constexpr int ID_OUTPUT = 1003;
constexpr int ID_OUTPUT_BROWSE = 1004;
constexpr int ID_GUI_SUBSYSTEM = 1010;
constexpr int ID_DIAGNOSTICS = 1012;
constexpr int ID_TO_INTEL = 1013;
constexpr int ID_REGISTRY = 1014;
constexpr int ID_LAZY_BINDING = 1015;
constexpr int ID_SKIP_SYSCALL = 1016;
constexpr int ID_SKIP_SCE = 1017;
constexpr int ID_AUTORUN = 1018;
constexpr int ID_CONVERT = 1020;
constexpr int ID_OPEN_FOLDER = 1021;
constexpr int ID_STATUS = 1022;
constexpr int ID_LOG = 1023;
constexpr UINT WM_CONVERSION_FINISHED = WM_APP + 1;

HWND g_window = nullptr;
HWND g_input = nullptr;
HWND g_output = nullptr;
HWND g_convert = nullptr;
HWND g_openFolder = nullptr;
HWND g_status = nullptr;
HWND g_log = nullptr;
bool g_running = false;

struct WindowsPalette {
    bool dark = false;
    UINT dpi = 96;
    COLORREF background = RGB(245, 247, 250);
    COLORREF panel = RGB(255, 255, 255);
    COLORREF field = RGB(246, 248, 251);
    COLORREF foreground = RGB(25, 31, 43);
    COLORREF muted = RGB(91, 102, 118);
    COLORREF accent = RGB(0, 120, 215);
    COLORREF accentText = RGB(255, 255, 255);
    HBRUSH backgroundBrush = nullptr;
    HBRUSH panelBrush = nullptr;
    HBRUSH fieldBrush = nullptr;
    HFONT bodyFont = nullptr;
    HFONT titleFont = nullptr;
    HFONT headingFont = nullptr;
};

WindowsPalette g_palette;
int S(int n) { return MulDiv(n, static_cast<int>(g_palette.dpi), 96); }

void DeleteThemeResources() {
    for (HBRUSH brush : {g_palette.backgroundBrush, g_palette.panelBrush, g_palette.fieldBrush})
        if (brush) DeleteObject(brush);
    for (HFONT font : {g_palette.bodyFont, g_palette.titleFont, g_palette.headingFont})
        if (font) DeleteObject(font);
    g_palette.backgroundBrush = g_palette.panelBrush = g_palette.fieldBrush = nullptr;
    g_palette.bodyFont = g_palette.titleFont = g_palette.headingFont = nullptr;
}

HFONT MakeFont(int height, int weight) {
    return CreateFontW(-S(height), 0, 0, 0, weight, FALSE, FALSE, FALSE,
                       DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
                       CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_SWISS, L"Segoe UI");
}

bool WindowsAppsUseDarkMode() {
    DWORD value = 1;
    DWORD size = sizeof(value);
    const LSTATUS status = RegGetValueW(HKEY_CURRENT_USER,
        L"Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize",
        L"AppsUseLightTheme", RRF_RT_REG_DWORD, nullptr, &value, &size);
    return status == ERROR_SUCCESS && value == 0;
}

void ReadWindowsPalette() {
    g_palette.dark = WindowsAppsUseDarkMode();
    g_palette.dpi = g_window ? GetDpiForWindow(g_window) : 96;
    if (!g_palette.dpi) g_palette.dpi = 96;
    if (g_palette.dark) {
        g_palette.background = RGB(17, 20, 26);
        g_palette.panel = RGB(29, 33, 41);
        g_palette.field = RGB(38, 44, 54);
        g_palette.foreground = RGB(238, 241, 246);
        g_palette.muted = RGB(171, 184, 200);
    } else {
        g_palette.background = RGB(244, 247, 251);
        g_palette.panel = RGB(255, 255, 255);
        g_palette.field = RGB(247, 249, 252);
        g_palette.foreground = RGB(24, 35, 53);
        g_palette.muted = RGB(96, 107, 124);
    }
    DWORD colorization = 0;
    BOOL opaque = FALSE;
    if (SUCCEEDED(DwmGetColorizationColor(&colorization, &opaque))) {
        g_palette.accent = RGB((colorization >> 16) & 255, (colorization >> 8) & 255,
                               colorization & 255);
    } else {
        g_palette.accent = GetSysColor(COLOR_HIGHLIGHT);
    }
    const int brightness = 299 * GetRValue(g_palette.accent) +
                           587 * GetGValue(g_palette.accent) +
                           114 * GetBValue(g_palette.accent);
    g_palette.accentText = brightness > 155000 ? RGB(14, 21, 31) : RGB(255, 255, 255);
    g_palette.backgroundBrush = CreateSolidBrush(g_palette.background);
    g_palette.panelBrush = CreateSolidBrush(g_palette.panel);
    g_palette.fieldBrush = CreateSolidBrush(g_palette.field);
    g_palette.bodyFont = MakeFont(15, FW_NORMAL);
    g_palette.headingFont = MakeFont(16, FW_SEMIBOLD);
    g_palette.titleFont = MakeFont(28, FW_SEMIBOLD);
}

BOOL CALLBACK ThemeChild(HWND child, LPARAM) {
    const int id = GetDlgCtrlID(child);
    const HFONT font = id == ID_TITLE ? g_palette.titleFont :
        (id == ID_KICKER || id == ID_OPTIONS_LABEL || id == ID_LOG_LABEL)
            ? g_palette.headingFont : g_palette.bodyFont;
    SendMessageW(child, WM_SETFONT, reinterpret_cast<WPARAM>(font), TRUE);
    wchar_t className[32]{};
    GetClassNameW(child, className, 32);
    if (wcscmp(className, L"Button") == 0 || wcscmp(className, L"Edit") == 0) {
        SetWindowTheme(child, g_palette.dark ? L"DarkMode_Explorer" : L"Explorer", nullptr);
    }
    InvalidateRect(child, nullptr, TRUE);
    return TRUE;
}

void RefreshWindowsTheme() {
    DeleteThemeResources();
    ReadWindowsPalette();
    const BOOL dark = g_palette.dark ? TRUE : FALSE;
    if (FAILED(DwmSetWindowAttribute(g_window, 20, &dark, sizeof(dark))))
        DwmSetWindowAttribute(g_window, 19, &dark, sizeof(dark));
    EnumChildWindows(g_window, ThemeChild, 0);
    RedrawWindow(g_window, nullptr, nullptr, RDW_INVALIDATE | RDW_ALLCHILDREN | RDW_ERASE);
}

void MoveControl(int id, int x, int y, int width, int height) {
    const HWND control = GetDlgItem(g_window, id);
    if (control) MoveWindow(control, S(x), S(y), S(std::max(width, 1)),
                            S(std::max(height, 1)), TRUE);
}

void LayoutControls() {
    if (!g_window) return;
    RECT client{};
    GetClientRect(g_window, &client);
    const int w = MulDiv(client.right - client.left, 96, static_cast<int>(g_palette.dpi));
    const int h = MulDiv(client.bottom - client.top, 96, static_cast<int>(g_palette.dpi));
    if (w < 540 || h < 600) return;
    MoveControl(ID_KICKER, 43, 22, w - 80, 25);
    MoveControl(ID_TITLE, 43, 51, w - 80, 42);
    MoveControl(ID_SUBTITLE, 43, 99, w - 80, 24);
    MoveControl(ID_INPUT_LABEL, 45, 143, w - 90, 22);
    MoveControl(ID_INPUT, 45, 165, w - 213, 34);
    MoveControl(ID_INPUT_BROWSE, w - 151, 165, 104, 35);
    MoveControl(ID_OUTPUT_LABEL, 45, 210, w - 90, 22);
    MoveControl(ID_OUTPUT, 45, 230, w - 213, 34);
    MoveControl(ID_OUTPUT_BROWSE, w - 151, 230, 104, 35);
    MoveControl(ID_OPTIONS_LABEL, 45, 304, w - 90, 25);
    const int right = std::max(w / 2, 390);
    const int rightWidth = std::max(140, w - right - 53);
    for (const auto& control : {
        std::pair{ID_GUI_SUBSYSTEM, 0}, {ID_DIAGNOSTICS, 1},
        {ID_TO_INTEL, 2}, {ID_REGISTRY, 3} }) {
        MoveControl(control.first, 47, 341 + control.second * 31, right - 70, 25);
    }
    for (const auto& control : {
        std::pair{ID_LAZY_BINDING, 0}, {ID_SKIP_SYSCALL, 1},
        {ID_SKIP_SCE, 2}, {ID_AUTORUN, 3} }) {
        MoveControl(control.first, right, 341 + control.second * 31, rightWidth, 25);
    }
    MoveControl(ID_OPTIONS_HINT, 47, 468, w - 94, 25);
    MoveControl(ID_CONVERT, 25, 526, 145, 43);
    MoveControl(ID_OPEN_FOLDER, 185, 526, 158, 43);
    MoveControl(ID_STATUS_LABEL, 363, 538, 62, 24);
    MoveControl(ID_STATUS, 425, 538, w - 460, 26);
    MoveControl(ID_LOG_LABEL, 45, 598, w - 90, 25);
    MoveControl(ID_LOG, 45, 627, w - 90, std::max(65, h - 651));
}

HBRUSH PanelForControl(HWND control) {
    const int id = GetDlgCtrlID(control);
    if (id == ID_KICKER || id == ID_TITLE || id == ID_SUBTITLE ||
        id == ID_STATUS_LABEL || id == ID_STATUS)
        return g_palette.backgroundBrush;
    POINT location{};
    MapWindowPoints(control, g_window, &location, 1);
    return location.y >= S(126) ? g_palette.panelBrush : g_palette.backgroundBrush;
}

void DrawCard(HDC dc, int x, int y, int width, int height) {
    if (height <= 0) return;
    const COLORREF border = g_palette.dark ? RGB(50, 58, 70) : RGB(224, 231, 241);
    const HPEN pen = CreatePen(PS_SOLID, std::max(1, S(1)), border);
    const HGDIOBJ oldPen = SelectObject(dc, pen);
    const HGDIOBJ oldBrush = SelectObject(dc, g_palette.panelBrush);
    RoundRect(dc, S(x), S(y), S(x + width), S(y + height), S(16), S(16));
    SelectObject(dc, oldBrush);
    SelectObject(dc, oldPen);
    DeleteObject(pen);
}

void PaintShell(HDC dc) {
    RECT client{};
    GetClientRect(g_window, &client);
    FillRect(dc, &client, g_palette.backgroundBrush);
    const int width = MulDiv(client.right, 96, static_cast<int>(g_palette.dpi));
    const int height = MulDiv(client.bottom, 96, static_cast<int>(g_palette.dpi));
    RECT stripe{S(25), S(27), S(31), S(109)};
    HBRUSH accent = CreateSolidBrush(g_palette.accent);
    FillRect(dc, &stripe, accent);
    DeleteObject(accent);
    DrawCard(dc, 24, 131, width - 48, 148);
    DrawCard(dc, 24, 290, width - 48, 213);
    DrawCard(dc, 24, 582, width - 48, height - 604);
}

void PaintPrimaryButton(const DRAWITEMSTRUCT* item) {
    HDC dc = item->hDC;
    RECT rect = item->rcItem;
    const bool disabled = (item->itemState & ODS_DISABLED) != 0;
    const bool pressed = (item->itemState & ODS_SELECTED) != 0;
    const COLORREF fill = disabled ? g_palette.muted : g_palette.accent;
    const COLORREF text = disabled ? g_palette.panel : g_palette.accentText;
    HBRUSH brush = CreateSolidBrush(fill);
    HPEN pen = CreatePen(PS_SOLID, 1, fill);
    HGDIOBJ oldBrush = SelectObject(dc, brush);
    HGDIOBJ oldPen = SelectObject(dc, pen);
    RoundRect(dc, rect.left, rect.top, rect.right, rect.bottom, S(13), S(13));
    SelectObject(dc, oldPen);
    SelectObject(dc, oldBrush);
    DeleteObject(pen);
    DeleteObject(brush);
    SetBkMode(dc, TRANSPARENT);
    SetTextColor(dc, text);
    HGDIOBJ font = SelectObject(dc, g_palette.headingFont);
    if (pressed) OffsetRect(&rect, 0, S(1));
    DrawTextW(dc, L"Convert to Windows", -1, &rect,
              DT_CENTER | DT_VCENTER | DT_SINGLELINE);
    SelectObject(dc, font);
    if (item->itemState & ODS_FOCUS) {
        InflateRect(&rect, -S(4), -S(4));
        DrawFocusRect(dc, &rect);
    }
}


std::wstring GetText(HWND control) {
    const int length = GetWindowTextLengthW(control);
    std::wstring value(static_cast<std::size_t>(length) + 1, L'\0');
    GetWindowTextW(control, value.data(), length + 1);
    value.resize(static_cast<std::size_t>(length));
    return value;
}

void SetText(HWND control, const std::wstring& value) {
    SetWindowTextW(control, value.c_str());
}

bool Checked(int id) {
    return SendDlgItemMessageW(g_window, id, BM_GETCHECK, 0, 0) == BST_CHECKED;
}

std::wstring QuoteArgument(const std::wstring& value) {
    std::wstring result = L"\"";
    std::size_t backslashes = 0;
    for (wchar_t ch : value) {
        if (ch == L'\\') {
            ++backslashes;
            continue;
        }
        if (ch == L'"') {
            result.append(backslashes * 2 + 1, L'\\');
            result.push_back(L'"');
            backslashes = 0;
            continue;
        }
        result.append(backslashes, L'\\');
        backslashes = 0;
        result.push_back(ch);
    }
    result.append(backslashes * 2, L'\\');
    result.push_back(L'"');
    return result;
}

std::wstring Utf8ToWide(const std::string& text) {
    if (text.empty()) return {};
    UINT codePage = CP_UTF8;
    DWORD flags = MB_ERR_INVALID_CHARS;
    int size = MultiByteToWideChar(codePage, flags, text.data(), static_cast<int>(text.size()), nullptr, 0);
    if (size == 0) {
        codePage = CP_ACP;
        flags = 0;
        size = MultiByteToWideChar(codePage, flags, text.data(), static_cast<int>(text.size()), nullptr, 0);
    }
    if (size == 0) return L"(Unable to decode relinker output.)";
    std::wstring result(static_cast<std::size_t>(size), L'\0');
    MultiByteToWideChar(codePage, flags, text.data(), static_cast<int>(text.size()), result.data(), size);
    return result;
}

std::wstring Win32Error(DWORD error) {
    wchar_t* buffer = nullptr;
    const DWORD count = FormatMessageW(
        FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
        nullptr, error, 0, reinterpret_cast<wchar_t*>(&buffer), 0, nullptr);
    std::wstring result = count && buffer ? std::wstring(buffer, count) : L"Unknown Windows error";
    if (buffer) LocalFree(buffer);
    return result;
}

std::filesystem::path ExecutableDirectory() {
    std::vector<wchar_t> buffer(32768);
    const DWORD length = GetModuleFileNameW(nullptr, buffer.data(), static_cast<DWORD>(buffer.size()));
    if (length == 0 || length >= buffer.size()) return std::filesystem::current_path();
    return std::filesystem::path(std::wstring(buffer.data(), length)).parent_path();
}

void BrowseInput() {
    wchar_t file[32768] = {};
    OPENFILENAMEW dialog{};
    dialog.lStructSize = sizeof(dialog);
    dialog.hwndOwner = g_window;
    dialog.lpstrFile = file;
    dialog.nMaxFile = 32768;
    dialog.lpstrFilter =
        L"PS5 executables (*.elf;*.bin;*.self)\0*.elf;*.bin;*.self\0"
        L"All files (*.*)\0*.*\0\0";
    dialog.nFilterIndex = 1;
    dialog.Flags = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST | OFN_EXPLORER;
    dialog.lpstrTitle = L"Select PS5 executable";

    if (!GetOpenFileNameW(&dialog)) return;
    SetText(g_input, file);

    if (GetText(g_output).empty()) {
        std::filesystem::path output(file);
        const std::wstring stem = output.stem().wstring();
        output.replace_filename(stem + L"-windows.exe");
        SetText(g_output, output.wstring());
    }
}

void BrowseOutput() {
    wchar_t file[32768] = {};
    const std::wstring current = GetText(g_output);
    if (!current.empty()) {
        lstrcpynW(file, current.c_str(), 32768);
    }

    OPENFILENAMEW dialog{};
    dialog.lStructSize = sizeof(dialog);
    dialog.hwndOwner = g_window;
    dialog.lpstrFile = file;
    dialog.nMaxFile = static_cast<DWORD>(std::size(file));
    dialog.lpstrFilter = L"Windows executable (*.exe)\0*.exe\0All files (*.*)\0*.*\0\0";
    dialog.nFilterIndex = 1;
    dialog.Flags = OFN_OVERWRITEPROMPT | OFN_PATHMUSTEXIST | OFN_EXPLORER;
    dialog.lpstrDefExt = L"exe";
    dialog.lpstrTitle = L"Choose translated output";

    if (GetSaveFileNameW(&dialog)) SetText(g_output, file);
}

struct ConversionResult {
    DWORD exitCode = static_cast<DWORD>(-1);
    std::wstring log;
    bool runtimeWarning = false;
};

ConversionResult RunRelinker(
    const std::wstring& input,
    const std::wstring& output,
    bool guiSubsystem,
    bool diagnostics,
    bool toIntel,
    bool registry,
    bool lazyBinding,
    bool skipSyscall,
    bool skipSce,
    bool autorun) {

    ConversionResult result;
    const auto directory = ExecutableDirectory();
    const auto relinker = directory / L"relinker.exe";
    if (!std::filesystem::exists(relinker)) {
        result.log = L"relinker.exe was not found next to AnyPS5.exe.\r\n"
                     L"Keep both files from the Windows artifact in the same folder.";
        return result;
    }

    std::wstring command = QuoteArgument(relinker.wstring()) + L" --windows";
    if (guiSubsystem) command += L" --windows-gui";
    if (diagnostics) command += L" --windows-diagnostics";
    if (toIntel) command += L" --to-intel";
    if (registry) command += L" --registry";
    if (lazyBinding) command += L" --lazy-binding";
    if (skipSyscall) command += L" --skip-syscall-check";
    if (skipSce) command += L" --skip-sce-module";
    if (autorun) command += L" --autorun";
    command += L" " + QuoteArgument(input) + L" " + QuoteArgument(output);

    SECURITY_ATTRIBUTES security{};
    security.nLength = sizeof(security);
    security.bInheritHandle = TRUE;

    HANDLE readPipe = nullptr;
    HANDLE writePipe = nullptr;
    if (!CreatePipe(&readPipe, &writePipe, &security, 0)) {
        result.log = L"Could not create output pipe: " + Win32Error(GetLastError());
        return result;
    }
    SetHandleInformation(readPipe, HANDLE_FLAG_INHERIT, 0);

    STARTUPINFOW startup{};
    startup.cb = sizeof(startup);
    startup.dwFlags = STARTF_USESTDHANDLES;
    startup.hStdInput = GetStdHandle(STD_INPUT_HANDLE);
    startup.hStdOutput = writePipe;
    startup.hStdError = writePipe;

    PROCESS_INFORMATION process{};
    std::vector<wchar_t> mutableCommand(command.begin(), command.end());
    mutableCommand.push_back(L'\0');

    const BOOL created = CreateProcessW(
        relinker.c_str(),
        mutableCommand.data(),
        nullptr,
        nullptr,
        TRUE,
        CREATE_NO_WINDOW,
        nullptr,
        directory.c_str(),
        &startup,
        &process);

    CloseHandle(writePipe);

    if (!created) {
        result.log = L"Could not start relinker.exe: " + Win32Error(GetLastError());
        CloseHandle(readPipe);
        return result;
    }

    std::string outputText;
    char buffer[4096];
    DWORD bytesRead = 0;
    while (ReadFile(readPipe, buffer, sizeof(buffer), &bytesRead, nullptr) && bytesRead != 0) {
        outputText.append(buffer, buffer + bytesRead);
    }

    WaitForSingleObject(process.hProcess, INFINITE);
    GetExitCodeProcess(process.hProcess, &result.exitCode);
    CloseHandle(process.hThread);
    CloseHandle(process.hProcess);
    CloseHandle(readPipe);

    result.log = Utf8ToWide(outputText);
    return result;
}

std::wstring StageBundledRuntimeLibraries(const std::wstring& output, bool& warning) {
    namespace fs = std::filesystem;
    const fs::path source = ExecutableDirectory() / L"libs";
    if (!fs::is_directory(source)) {
        warning = true;
        return L"\r\nWARNING: Bundled runtime libraries were not found. "
               L"Copy the appropriate libs folder next to the translated executable.\r\n";
    }

    const fs::path destination = fs::absolute(fs::path(output)).parent_path() / L"libs";
    std::error_code sameError;
    if (fs::equivalent(source, destination, sameError) && !sameError)
        return L"\r\nRuntime libraries are already alongside the output.\r\n";

    fs::create_directories(destination);
    std::size_t copied = 0;
    std::size_t present = 0;
    for (const auto& entry : fs::directory_iterator(source)) {
        if (!entry.is_regular_file()) continue;
        const auto extension = entry.path().extension().wstring();
        if (extension != L".prx" && extension != L".dll") continue;
        ++present;
        std::error_code copyError;
        if (fs::copy_file(entry.path(), destination / entry.path().filename(),
                          fs::copy_options::skip_existing, copyError)) ++copied;
        if (copyError)
            throw fs::filesystem_error("Failed to stage runtime library",
                                       entry.path(), destination / entry.path().filename(), copyError);
    }
    if (present == 0) {
        warning = true;
        return L"\r\nWARNING: The bundled libs folder contains no runtime libraries.\r\n";
    }
    return L"\r\nRuntime libraries available in: " + destination.wstring() +
           L" (new files copied: " + std::to_wstring(copied) + L").\r\n"
           L"Existing runtime library files were not overwritten.\r\n";
}

struct TemporaryElfWorkspace {
    std::filesystem::path directory;
    ~TemporaryElfWorkspace() {
        if (directory.empty()) return;
        std::error_code ignored;
        std::filesystem::remove_all(directory, ignored);
    }
    void Create() {
        wchar_t temp[32768]{}, name[MAX_PATH]{};
        const auto length=GetTempPathW(static_cast<DWORD>(std::size(temp)),temp);
        if(!length || length>=std::size(temp) || !GetTempFileNameW(temp,L"APS",0,name))
            throw std::runtime_error("Cannot create a temporary SELF workspace");
        std::filesystem::remove(name);
        directory=name;
        std::filesystem::create_directory(directory);
    }
};

std::filesystem::path PrepareElfInputs(const std::filesystem::path& selected, bool skipModules,
                                       TemporaryElfWorkspace& work, std::wstring& log) {
    namespace fs=std::filesystem;
    const fs::path input=fs::absolute(selected), root=input.parent_path();
    const auto kind=GuiSelf::Detect(input);
    if(kind==GuiSelf::Kind::Other) throw std::runtime_error("Selected executable is neither ELF nor SELF");
    bool needsExtraction=kind==GuiSelf::Kind::Self;
    std::vector<fs::path> modules;
    if(!skipModules) {
        for(const auto* name:{L"sce_module",L"sce_modules",L"prx"}) {
            const auto dir=root/name;
            if(!fs::exists(dir)) continue;
            if(!fs::is_directory(dir)) throw std::runtime_error("Guest module path is not a directory: "+dir.string());
            for(const auto& entry:fs::recursive_directory_iterator(dir)) {
                if(!entry.is_regular_file() || entry.is_symlink()) continue;
                const auto entryKind=GuiSelf::Detect(entry.path());
                if(entryKind==GuiSelf::Kind::Other) continue;
                modules.push_back(entry.path());
                if(entryKind==GuiSelf::Kind::Self) needsExtraction=true;
            }
        }
    }
    if(!needsExtraction) return input;
    work.Create();
    std::size_t converted=0;
    const auto stageFile=[&](const fs::path& from,const fs::path& to) {
        fs::create_directories(to.parent_path());
        if(GuiSelf::Detect(from)==GuiSelf::Kind::Self) {
            try { GuiSelf::Extract(from,to); }
            catch(const std::exception& error) { throw std::runtime_error(from.string()+": "+error.what()); }
            ++converted;
        } else {
            fs::copy_file(from,to,fs::copy_options::none);
        }
    };
    const fs::path prepared=work.directory/input.filename();
    stageFile(input,prepared);
    for(const auto& module:modules) {
        const auto relative=fs::relative(module,root);
        if(relative.empty() || *relative.begin()=="..")
            throw std::runtime_error("Module path escapes the game directory");
        stageFile(module,work.directory/relative);
    }
    log=L"Automatically prepared "+std::to_wstring(converted)+
        L" unencrypted SELF file(s) as ELF. Original files were not modified.\r\n";
    return prepared;
}

void StartConversion() {
    if (g_running) return;

    const std::wstring input = GetText(g_input);
    const std::wstring output = GetText(g_output);
    if (input.empty()) {
        MessageBoxW(g_window, L"Choose the PS5 executable first.", L"AnyPS5", MB_OK | MB_ICONINFORMATION);
        return;
    }
    if (!std::filesystem::exists(input)) {
        MessageBoxW(g_window, L"The selected input file does not exist.", L"AnyPS5", MB_OK | MB_ICONERROR);
        return;
    }
    if (output.empty()) {
        MessageBoxW(g_window, L"Choose where to save the translated Windows executable.", L"AnyPS5", MB_OK | MB_ICONINFORMATION);
        return;
    }

    g_running = true;
    EnableWindow(g_convert, FALSE);
    EnableWindow(g_openFolder, FALSE);
    SetText(g_status, L"Converting...");
    SetText(g_log, L"Running relinker.exe with the selected options...\r\n");

    const bool guiSubsystem = Checked(ID_GUI_SUBSYSTEM);
    const bool diagnostics = Checked(ID_DIAGNOSTICS);
    const bool toIntel = Checked(ID_TO_INTEL);
    const bool registry = Checked(ID_REGISTRY);
    const bool lazyBinding = Checked(ID_LAZY_BINDING);
    const bool skipSyscall = Checked(ID_SKIP_SYSCALL);
    const bool skipSce = Checked(ID_SKIP_SCE);
    const bool autorun = Checked(ID_AUTORUN);

    const HWND targetWindow = g_window;
    std::thread([=] {
        ConversionResult result;
        try {
            TemporaryElfWorkspace workspace;
            std::wstring preparation;
            const auto prepared=PrepareElfInputs(input,skipSce,workspace,preparation);
            result=RunRelinker(prepared.wstring(),output,guiSubsystem,diagnostics,toIntel,
                               registry,lazyBinding,skipSyscall,skipSce,autorun);
            result.log=preparation+result.log;
            if (result.exitCode == 0) {
                try {
                    result.log += StageBundledRuntimeLibraries(output, result.runtimeWarning);
                } catch (const std::exception& error) {
                    result.runtimeWarning = true;
                    result.log += L"\r\nWARNING: Could not stage runtime libraries: " +
                                  Utf8ToWide(error.what()) + L"\r\n";
                }
            }
        } catch (const std::exception& error) {
            result.log = L"Input preparation or relinker error: " + Utf8ToWide(error.what());
        }
        auto* heapResult = new ConversionResult(std::move(result));
        if (!PostMessageW(targetWindow, WM_CONVERSION_FINISHED, 0, reinterpret_cast<LPARAM>(heapResult))) {
            delete heapResult;
        }
    }).detach();
}

void OpenOutputFolder() {
    const std::wstring output = GetText(g_output);
    if (output.empty()) return;
    std::filesystem::path path(output);
    const auto folder = path.parent_path();
    if (!folder.empty()) {
        ShellExecuteW(g_window, L"open", folder.c_str(), nullptr, nullptr, SW_SHOWNORMAL);
    }
}

HWND AddControl(
    DWORD exStyle,
    const wchar_t* className,
    const wchar_t* text,
    DWORD style,
    int x,
    int y,
    int width,
    int height,
    int id) {

    HWND control = CreateWindowExW(
        exStyle, className, text, style,
        x, y, width, height, g_window,
        reinterpret_cast<HMENU>(static_cast<INT_PTR>(id)),
        GetModuleHandleW(nullptr), nullptr);

    SendMessageW(control, WM_SETFONT, reinterpret_cast<WPARAM>(GetStockObject(DEFAULT_GUI_FONT)), TRUE);
    return control;
}

void CreateControls() {
    AddControl(0, L"STATIC", L"ANYPS5  /  CONVERTER",
               WS_CHILD | WS_VISIBLE | SS_LEFT, 0, 0, 0, 0, ID_KICKER);
    AddControl(0, L"STATIC", L"PS5 to Windows",
               WS_CHILD | WS_VISIBLE | SS_LEFT, 0, 0, 0, 0, ID_TITLE);
    AddControl(0, L"STATIC", L"Convert an executable and prepare supported SELF modules automatically.",
               WS_CHILD | WS_VISIBLE | SS_LEFT, 0, 0, 0, 0, ID_SUBTITLE);
    AddControl(0, L"STATIC", L"SOURCE  /  ELF OR SELF",
               WS_CHILD | WS_VISIBLE, 0, 0, 0, 0, ID_INPUT_LABEL);
    g_input = AddControl(WS_EX_CLIENTEDGE, L"EDIT", L"",
                         WS_CHILD | WS_VISIBLE | WS_TABSTOP | ES_AUTOHSCROLL,
                         0, 0, 0, 0, ID_INPUT);
    AddControl(0, L"BUTTON", L"Browse...",
               WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON,
               0, 0, 0, 0, ID_INPUT_BROWSE);
    AddControl(0, L"STATIC", L"DESTINATION  /  WINDOWS EXECUTABLE",
               WS_CHILD | WS_VISIBLE, 0, 0, 0, 0, ID_OUTPUT_LABEL);
    g_output = AddControl(WS_EX_CLIENTEDGE, L"EDIT", L"",
                          WS_CHILD | WS_VISIBLE | WS_TABSTOP | ES_AUTOHSCROLL,
                          0, 0, 0, 0, ID_OUTPUT);
    AddControl(0, L"BUTTON", L"Browse...",
               WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON,
               0, 0, 0, 0, ID_OUTPUT_BROWSE);
    AddControl(0, L"STATIC", L"CONVERSION OPTIONS",
               WS_CHILD | WS_VISIBLE, 0, 0, 0, 0, ID_OPTIONS_LABEL);
    AddControl(0, L"BUTTON", L"Hide translated game console (GUI mode)",
               WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_AUTOCHECKBOX,
               0, 0, 0, 0, ID_GUI_SUBSYSTEM);
    AddControl(0, L"BUTTON", L"Windows dependency diagnostics",
               WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_AUTOCHECKBOX,
               0, 0, 0, 0, ID_DIAGNOSTICS);
    AddControl(0, L"BUTTON", L"Intel instruction compatibility",
               WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_AUTOCHECKBOX,
               0, 0, 0, 0, ID_TO_INTEL);
    AddControl(0, L"BUTTON", L"Write registry JSON",
               WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_AUTOCHECKBOX,
               0, 0, 0, 0, ID_REGISTRY);
    AddControl(0, L"BUTTON", L"Lazy binding",
               WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_AUTOCHECKBOX,
               0, 0, 0, 0, ID_LAZY_BINDING);
    AddControl(0, L"BUTTON", L"Skip syscall validation",
               WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_AUTOCHECKBOX,
               0, 0, 0, 0, ID_SKIP_SYSCALL);
    AddControl(0, L"BUTTON", L"Skip guest modules (advanced)",
               WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_AUTOCHECKBOX,
               0, 0, 0, 0, ID_SKIP_SCE);
    AddControl(0, L"BUTTON", L"Run after conversion",
               WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_AUTOCHECKBOX,
               0, 0, 0, 0, ID_AUTORUN);
    AddControl(0, L"STATIC", L"Plain SELF files are prepared automatically. Advanced skips are normally unnecessary.",
               WS_CHILD | WS_VISIBLE, 0, 0, 0, 0, ID_OPTIONS_HINT);
    SendDlgItemMessageW(g_window, ID_DIAGNOSTICS, BM_SETCHECK, BST_CHECKED, 0);
    g_convert = AddControl(0, L"BUTTON", L"Convert to Windows",
                           WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_OWNERDRAW,
                           0, 0, 0, 0, ID_CONVERT);
    g_openFolder = AddControl(0, L"BUTTON", L"Open output folder",
                              WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON,
                              0, 0, 0, 0, ID_OPEN_FOLDER);
    EnableWindow(g_openFolder, FALSE);
    AddControl(0, L"STATIC", L"STATUS",
               WS_CHILD | WS_VISIBLE, 0, 0, 0, 0, ID_STATUS_LABEL);
    g_status = AddControl(0, L"STATIC", L"Ready",
                          WS_CHILD | WS_VISIBLE, 0, 0, 0, 0, ID_STATUS);
    AddControl(0, L"STATIC", L"CONVERSION LOG",
               WS_CHILD | WS_VISIBLE, 0, 0, 0, 0, ID_LOG_LABEL);
    g_log = AddControl(WS_EX_CLIENTEDGE, L"EDIT", L"",
                       WS_CHILD | WS_VISIBLE | WS_VSCROLL | WS_TABSTOP |
                       ES_MULTILINE | ES_AUTOVSCROLL | ES_READONLY,
                       0, 0, 0, 0, ID_LOG);
    RefreshWindowsTheme();
    LayoutControls();
}

LRESULT CALLBACK WindowProc(HWND window, UINT message, WPARAM wParam, LPARAM lParam) {
    switch (message) {
    case WM_CREATE:
        g_window = window;
        CreateControls();
        return 0;
    case WM_SIZE:
        LayoutControls();
        return 0;
    case WM_GETMINMAXINFO: {
        auto* limits = reinterpret_cast<MINMAXINFO*>(lParam);
        limits->ptMinTrackSize.x = S(815);
        limits->ptMinTrackSize.y = S(735);
        return 0;
    }
    case WM_DPICHANGED: {
        const auto* suggested = reinterpret_cast<const RECT*>(lParam);
        SetWindowPos(window, nullptr, suggested->left, suggested->top,
                     suggested->right - suggested->left,
                     suggested->bottom - suggested->top,
                     SWP_NOZORDER | SWP_NOACTIVATE);
        RefreshWindowsTheme();
        LayoutControls();
        return 0;
    }
    case WM_THEMECHANGED:
    case WM_SETTINGCHANGE:
    case WM_DWMCOLORIZATIONCOLORCHANGED:
        RefreshWindowsTheme();
        return 0;
    case WM_ERASEBKGND:
        return 1;
    case WM_PAINT: {
        PAINTSTRUCT ps{};
        const HDC dc = BeginPaint(window, &ps);
        PaintShell(dc);
        EndPaint(window, &ps);
        return 0;
    }
    case WM_CTLCOLORSTATIC:
    case WM_CTLCOLORBTN: {
        const HDC dc = reinterpret_cast<HDC>(wParam);
        const HWND control = reinterpret_cast<HWND>(lParam);
        SetTextColor(dc, GetDlgCtrlID(control) == ID_KICKER ||
                         GetDlgCtrlID(control) == ID_SUBTITLE ||
                         GetDlgCtrlID(control) == ID_OPTIONS_HINT ||
                         GetDlgCtrlID(control) == ID_STATUS_LABEL
                         ? g_palette.muted : g_palette.foreground);
        const HBRUSH brush = PanelForControl(control);
        SetBkColor(dc, brush == g_palette.panelBrush ? g_palette.panel : g_palette.background);
        return reinterpret_cast<LRESULT>(brush);
    }
    case WM_CTLCOLOREDIT: {
        const HDC dc = reinterpret_cast<HDC>(wParam);
        SetTextColor(dc, g_palette.foreground);
        SetBkColor(dc, g_palette.field);
        return reinterpret_cast<LRESULT>(g_palette.fieldBrush);
    }
    case WM_DRAWITEM: {
        const auto* item = reinterpret_cast<const DRAWITEMSTRUCT*>(lParam);
        if (item && item->CtlID == ID_CONVERT) {
            PaintPrimaryButton(item);
            return TRUE;
        }
        break;
    }

    case WM_COMMAND:
        switch (LOWORD(wParam)) {
        case ID_INPUT_BROWSE:
            BrowseInput();
            return 0;
        case ID_OUTPUT_BROWSE:
            BrowseOutput();
            return 0;
        case ID_CONVERT:
            StartConversion();
            return 0;
        case ID_OPEN_FOLDER:
            OpenOutputFolder();
            return 0;
        default:
            break;
        }
        break;

    case WM_CONVERSION_FINISHED: {
        auto* result = reinterpret_cast<ConversionResult*>(lParam);
        g_running = false;
        EnableWindow(g_convert, TRUE);

        if (result) {
            SetText(g_log, result->log.empty() ? L"(relinker produced no output)" : result->log);
            if (result->exitCode == 0) {
                EnableWindow(g_openFolder, TRUE);
                if (result->runtimeWarning) {
                    SetText(g_status, L"Conversion finished; runtime libraries need attention.");
                    MessageBoxW(g_window, L"Conversion completed, but runtime libraries could not be fully staged. See the log.",
                                L"AnyPS5", MB_OK | MB_ICONWARNING);
                } else {
                    SetText(g_status, L"Conversion completed successfully.");
                    MessageBoxW(g_window, L"Conversion completed successfully.", L"AnyPS5", MB_OK | MB_ICONINFORMATION);
                }
            } else {
                SetText(g_status, L"Conversion failed. See the log below.");
                MessageBoxW(g_window, L"Conversion failed. See the relinker output in the window.", L"AnyPS5", MB_OK | MB_ICONERROR);
            }
            delete result;
        }
        return 0;
    }

    case WM_CLOSE:
        if (g_running) {
            MessageBoxW(window,
                L"A conversion is still running. Close the window after it completes.",
                L"AnyPS5", MB_OK | MB_ICONINFORMATION);
            return 0;
        }
        DestroyWindow(window);
        return 0;

    case WM_DESTROY:
        DeleteThemeResources();
        g_window = nullptr;
        PostQuitMessage(0);
        return 0;
    }

    return DefWindowProcW(window, message, wParam, lParam);
}

} // namespace

int WINAPI WinMain(HINSTANCE instance, HINSTANCE, LPSTR, int showCommand) {
    using SetDpiAwareness = BOOL (WINAPI*)(HANDLE);
    const auto user32 = GetModuleHandleW(L"user32.dll");
    const auto setDpi = user32 ? reinterpret_cast<SetDpiAwareness>(
        GetProcAddress(user32, "SetProcessDpiAwarenessContext")) : nullptr;
    if (!setDpi || !setDpi(reinterpret_cast<HANDLE>(static_cast<INT_PTR>(-4))))
        SetProcessDPIAware();
    constexpr wchar_t ClassName[] = L"AnyPS5ConverterWindow";

    WNDCLASSEXW windowClass{};
    windowClass.cbSize = sizeof(windowClass);
    windowClass.lpfnWndProc = WindowProc;
    windowClass.hInstance = instance;
    windowClass.hCursor = LoadCursorW(nullptr, IDC_ARROW);
    windowClass.hIcon = LoadIconW(nullptr, IDI_APPLICATION);
    windowClass.hbrBackground = reinterpret_cast<HBRUSH>(COLOR_WINDOW + 1);
    windowClass.lpszClassName = ClassName;

    if (!RegisterClassExW(&windowClass)) {
        MessageBoxW(nullptr, L"Could not register the AnyPS5 window class.", L"AnyPS5", MB_OK | MB_ICONERROR);
        return 1;
    }

    HWND window = CreateWindowExW(
        0,
        ClassName,
        L"AnyPS5 - PS5 to Windows Converter",
        WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN,
        CW_USEDEFAULT, CW_USEDEFAULT, 930, 800,
        nullptr, nullptr, instance, nullptr);

    if (!window) {
        MessageBoxW(nullptr, L"Could not create the AnyPS5 window.", L"AnyPS5", MB_OK | MB_ICONERROR);
        return 1;
    }

    ShowWindow(window, showCommand);
    UpdateWindow(window);

    MSG message{};
    while (GetMessageW(&message, nullptr, 0, 0) > 0) {
        if (!IsDialogMessageW(window, &message)) {
            TranslateMessage(&message);
            DispatchMessageW(&message);
        }
    }
    return static_cast<int>(message.wParam);
}
