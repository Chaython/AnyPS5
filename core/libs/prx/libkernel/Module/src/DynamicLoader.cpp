#include "prx/libc/include/General.hpp"
#include <nid/NidCompute.hpp>
#include <array>
#include <cstdio>
#include <map>
#include <memory>
#include <mutex>
#include <string>
#include <vector>
#ifdef _WIN32
#define NOMINMAX
#include <windows.h>
#else
#include <dlfcn.h>
#endif

namespace {
thread_local std::array<char, 512> loaderError{};
thread_local bool pendingError = false;
void Error(const char* message) {
    std::snprintf(loaderError.data(), loaderError.size(), "%s", message);
    pendingError = true;
}
struct Module {
    void* native = nullptr;
    bool owned = true;
    bool global = false;
    ~Module() {
        if (owned && native) {
#ifdef _WIN32
            FreeLibrary(static_cast<HMODULE>(native));
#else
            ::dlclose(native);
#endif
        }
    }
};
std::mutex modulesMutex;
std::map<std::uintptr_t, std::shared_ptr<Module>> modules;
std::uintptr_t nextHandle = 0x20000000;
void* RegisterModule(std::shared_ptr<Module> module) {
    std::lock_guard lock(modulesMutex);
    const auto handle = nextHandle++;
    modules.emplace(handle, std::move(module));
    return reinterpret_cast<void*>(handle);
}
std::string HostLibraryName(const char* path) {
    auto name = std::filesystem::path(path).filename().string();
    if (name.ends_with(".sprx")) {
        name.resize(name.size() - 5);
        name += ".prx";
    }
    return name;
}
void* Symbol(Module& module, const char* name) {
#ifdef _WIN32
    return reinterpret_cast<void*>(GetProcAddress(static_cast<HMODULE>(module.native), name));
#else
    return ::dlsym(module.native, name);
#endif
}
void* FindSymbol(Module& module, const char* name) {
    if (auto* symbol = Symbol(module, name)) return symbol;
    const auto nid = Nid::ComputeNid(name, "");
    return Symbol(module, nid.c_str());
}
}

extern "C" {
char* APS5_VABI dlerror_nid_postfix() {
    if (!pendingError) return nullptr;
    pendingError = false;
    return loaderError.data();
}
void* APS5_VABI dlopen_nid_postfix(const char* path, int flags) {
    if ((flags & ~0x103) || (flags & 3) == 0 || (flags & 3) == 3) {
        Error("dlopen: unsupported flags"); return nullptr;
    }
    try {
        auto module = std::make_shared<Module>();
        module->global = (flags & 0x100) != 0 || !path;
#ifdef _WIN32
        if (!path) {
            module->native = GetModuleHandleW(nullptr);
            module->owned = false;
        } else {
            if (!*path) { Error("dlopen: empty module path"); return nullptr; }
            const auto resolved = ResolvePath_nid_no_patch(path);
            module->native = LoadLibraryExW(resolved.c_str(), nullptr, LOAD_WITH_ALTERED_SEARCH_PATH);
        }
        if (!module->native) {
            char message[128];
            std::snprintf(message, sizeof(message), "dlopen: Windows loader error %lu (module must be host-compatible)", GetLastError());
            Error(message); return nullptr;
        }
#else
        const auto resolved = path ? ResolvePath_nid_no_patch(path).string() : std::string{};
        const int nativeFlags = ((flags & 3) == 1 ? RTLD_LAZY : RTLD_NOW) |
            ((flags & 0x100) ? RTLD_GLOBAL : RTLD_LOCAL);
        module->native = ::dlopen(path ? resolved.c_str() : nullptr, nativeFlags);
        if (!module->native) { Error(::dlerror()); return nullptr; }
#endif
        return RegisterModule(std::move(module));
    } catch (const std::exception& error) { Error(error.what()); return nullptr; }
}
void* APS5_VABI dlopen_sce_module_nid_no_patch(const char* path, int flags) {
    if (!path || !*path) { Error("sce module load: empty module path"); return nullptr; }
    if ((flags & ~0x103) || (flags & 3) == 0 || (flags & 3) == 3) {
        Error("sce module load: unsupported flags"); return nullptr;
    }
    try {
        auto module = std::make_shared<Module>();
        module->global = (flags & 0x100) != 0;
        auto converted = ResolvePath_nid_no_patch(path);
        converted += ".guest.prx";
#ifdef _WIN32
        HMODULE native = nullptr;
        if (GetModuleHandleExW(0, converted.c_str(), &native)) {
            module->native = native;
        } else {
            const auto hostName = std::filesystem::path(HostLibraryName(path)).wstring();
            module->native = LoadLibraryW(hostName.c_str());
        }
        if (!module->native) {
            char message[160];
            std::snprintf(message, sizeof(message), "sce module load: Windows loader error %lu (no converted guest or host library)", GetLastError());
            Error(message); return nullptr;
        }
#else
        const int nativeFlags = ((flags & 3) == 1 ? RTLD_LAZY : RTLD_NOW) |
            ((flags & 0x100) ? RTLD_GLOBAL : RTLD_LOCAL);
        module->native = ::dlopen(converted.string().c_str(), nativeFlags);
        if (!module->native) {
            const auto hostName = HostLibraryName(path);
            module->native = ::dlopen(hostName.c_str(), nativeFlags);
        }
        if (!module->native) {
            const char* detail = ::dlerror();
            Error(detail ? detail : "sce module load: module could not be loaded");
            return nullptr;
        }
#endif
        return RegisterModule(std::move(module));
    } catch (const std::exception& error) { Error(error.what()); return nullptr; }
}
void* APS5_VABI dlsym_nid_postfix(void* handle, const char* name) {
    if (!name || !*name) { Error("dlsym: empty symbol name"); return nullptr; }
    try {
        std::vector<std::shared_ptr<Module>> search;
        {
            std::lock_guard lock(modulesMutex);
            if (handle == reinterpret_cast<void*>(static_cast<std::intptr_t>(-2))) {
                for (const auto& [key, module] : modules) if (module->global) search.push_back(module);
            } else {
                auto found = modules.find(reinterpret_cast<std::uintptr_t>(handle));
                if (found == modules.end()) { Error("dlsym: invalid or unsupported module handle"); return nullptr; }
                search.push_back(found->second);
            }
        }
        for (const auto& module : search) if (auto* result = FindSymbol(*module, name)) return result;
        Error("dlsym: symbol not found in supported module scope");
        return nullptr;
    } catch (const std::exception& error) { Error(error.what()); return nullptr; }
}
int APS5_VABI dlclose_nid_postfix(void* handle) {
    std::shared_ptr<Module> module;
    {
        std::lock_guard lock(modulesMutex);
        auto found = modules.find(reinterpret_cast<std::uintptr_t>(handle));
        if (found == modules.end()) { Error("dlclose: invalid module handle"); return -1; }
        module = std::move(found->second);
        modules.erase(found);
    }
    // Unload outside the registry lock: module destructors may call loader APIs.
    module.reset();
    return 0;
}
}
