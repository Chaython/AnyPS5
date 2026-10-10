#define UNICODE
#define _UNICODE
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <commdlg.h>
#include <shellapi.h>

#include <exception>
#include <filesystem>
#include <string>
#include <thread>
#include <vector>

namespace {

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
            result = RunRelinker(input, output, guiSubsystem, diagnostics, toIntel,
                                 registry, lazyBinding, skipSyscall, skipSce, autorun);
        } catch (const std::exception& error) {
            result.log = L"Unexpected relinker error: " + Utf8ToWide(error.what());
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
    AddControl(0, L"STATIC", L"PS5 executable:", WS_CHILD | WS_VISIBLE,
               18, 20, 112, 22, 0);
    g_input = AddControl(WS_EX_CLIENTEDGE, L"EDIT", L"",
                         WS_CHILD | WS_VISIBLE | WS_TABSTOP | ES_AUTOHSCROLL,
                         130, 17, 530, 25, ID_INPUT);
    AddControl(0, L"BUTTON", L"Browse...",
               WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON,
               670, 16, 88, 27, ID_INPUT_BROWSE);

    AddControl(0, L"STATIC", L"Output .exe:", WS_CHILD | WS_VISIBLE,
               18, 57, 112, 22, 0);
    g_output = AddControl(WS_EX_CLIENTEDGE, L"EDIT", L"",
                          WS_CHILD | WS_VISIBLE | WS_TABSTOP | ES_AUTOHSCROLL,
                          130, 54, 530, 25, ID_OUTPUT);
    AddControl(0, L"BUTTON", L"Browse...",
               WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON,
               670, 53, 88, 27, ID_OUTPUT_BROWSE);

    AddControl(0, L"BUTTON", L"Conversion options",
               WS_CHILD | WS_VISIBLE | BS_GROUPBOX,
               18, 94, 740, 177, 0);

    AddControl(0, L"BUTTON", L"GUI subsystem for translated game (no console window)",
               WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_AUTOCHECKBOX,
               34, 119, 350, 23, ID_GUI_SUBSYSTEM);
    AddControl(0, L"BUTTON", L"Windows dependency diagnostics",
               WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_AUTOCHECKBOX,
               34, 148, 260, 23, ID_DIAGNOSTICS);
    AddControl(0, L"BUTTON", L"Intel compatibility lowering",
               WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_AUTOCHECKBOX,
               34, 177, 260, 23, ID_TO_INTEL);
    AddControl(0, L"BUTTON", L"Write registry JSON",
               WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_AUTOCHECKBOX,
               34, 206, 260, 23, ID_REGISTRY);

    AddControl(0, L"BUTTON", L"Lazy binding",
               WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_AUTOCHECKBOX,
               405, 119, 180, 23, ID_LAZY_BINDING);
    AddControl(0, L"BUTTON", L"Skip syscall check",
               WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_AUTOCHECKBOX,
               405, 148, 180, 23, ID_SKIP_SYSCALL);
    AddControl(0, L"BUTTON", L"Skip sce_module (advanced)",
               WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_AUTOCHECKBOX,
               405, 177, 220, 23, ID_SKIP_SCE);
    AddControl(0, L"BUTTON", L"Run translated game after conversion",
               WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_AUTOCHECKBOX,
               405, 206, 270, 23, ID_AUTORUN);

    // Keep a console visible by default while translated runtime support is
    // experimental, so startup/import failures cannot disappear silently.
    SendDlgItemMessageW(g_window, ID_DIAGNOSTICS, BM_SETCHECK, BST_CHECKED, 0);

    AddControl(0, L"STATIC",
               L"Windows output is automatic. Leave 'Skip sce_module' off unless a title specifically requires it.",
               WS_CHILD | WS_VISIBLE,
               405, 237, 330, 26, 0);

    g_convert = AddControl(0, L"BUTTON", L"Convert",
                           WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_DEFPUSHBUTTON,
                           18, 286, 105, 34, ID_CONVERT);
    g_openFolder = AddControl(0, L"BUTTON", L"Open output folder",
                              WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON,
                              133, 286, 150, 34, ID_OPEN_FOLDER);
    EnableWindow(g_openFolder, FALSE);

    AddControl(0, L"STATIC", L"Status:", WS_CHILD | WS_VISIBLE,
               305, 294, 50, 22, 0);
    g_status = AddControl(0, L"STATIC", L"Ready",
                          WS_CHILD | WS_VISIBLE,
                          358, 294, 400, 22, ID_STATUS);

    AddControl(0, L"STATIC", L"Relinker output:", WS_CHILD | WS_VISIBLE,
               18, 336, 120, 22, 0);
    g_log = AddControl(WS_EX_CLIENTEDGE, L"EDIT", L"",
                       WS_CHILD | WS_VISIBLE | WS_VSCROLL |
                       ES_MULTILINE | ES_AUTOVSCROLL | ES_READONLY,
                       18, 358, 740, 244, ID_LOG);
}

LRESULT CALLBACK WindowProc(HWND window, UINT message, WPARAM wParam, LPARAM lParam) {
    switch (message) {
    case WM_CREATE:
        g_window = window;
        CreateControls();
        return 0;

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
                SetText(g_status, L"Conversion completed successfully.");
                EnableWindow(g_openFolder, TRUE);
                MessageBoxW(g_window, L"Conversion completed successfully.", L"AnyPS5", MB_OK | MB_ICONINFORMATION);
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
        g_window = nullptr;
        PostQuitMessage(0);
        return 0;
    }

    return DefWindowProcW(window, message, wParam, lParam);
}

} // namespace

int WINAPI WinMain(HINSTANCE instance, HINSTANCE, LPSTR, int showCommand) {
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
        WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
        CW_USEDEFAULT, CW_USEDEFAULT, 795, 655,
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
