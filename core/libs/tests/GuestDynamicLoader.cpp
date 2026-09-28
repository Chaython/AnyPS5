#include "SceTypes.hpp"
#include "prx/libc/include/general/VabiMacros.hpp"
#include <cstdlib>
#include <filesystem>
#include <string>
#include <thread>
extern "C" {
void* APS5_VABI dlopen_nid_postfix(const char*, int);
void* APS5_VABI dlsym_nid_postfix(void*, const char*);
int APS5_VABI dlclose_nid_postfix(void*);
char* APS5_VABI dlerror_nid_postfix();
KernelModule APS5_VABI sceKernelLoadStartModule(const char*, size_t, const void*, uint32_t, const KernelLoadModuleOpt*, int*);
int APS5_VABI sceKernelStopUnloadModule(KernelModule, size_t, const void*, uint32_t, const KernelUnloadModuleOpt*, int*);
int APS5_VABI sceKernelDlsym(KernelModule, const char*, void**);
}
static void Require(bool value) { if (!value) std::abort(); }
int main(int argc, char** argv) {
    Require(argc == 2);
    Require(dlerror_nid_postfix() == nullptr);
    Require(dlopen_nid_postfix(argv[1], 0x2000) == nullptr);
    Require(dlerror_nid_postfix() != nullptr);
    Require(dlerror_nid_postfix() == nullptr);
    Require(dlopen_nid_postfix("anyps5-missing-module-for-test.prx", 2) == nullptr);
    Require(dlerror_nid_postfix() != nullptr);
    void* executable = dlopen_nid_postfix(nullptr, 2);
    Require(executable != nullptr && dlclose_nid_postfix(executable) == 0);
    void* module = dlopen_nid_postfix(argv[1], 2 | 0x100);
    Require(module != nullptr);
    using Add = int (APS5_VABI *)(int, int);
    auto add = reinterpret_cast<Add>(dlsym_nid_postfix(module, "GuestModuleAdd"));
    Require(add && add(17, 25) == 42);
    Require(dlsym_nid_postfix(reinterpret_cast<void*>(-2), "GuestModuleAdd") == reinterpret_cast<void*>(add));
    Require(dlsym_nid_postfix(module, "missing_symbol") == nullptr);
    std::thread other([] { Require(dlerror_nid_postfix() == nullptr); });
    other.join();
    Require(dlerror_nid_postfix() != nullptr);
    Require(dlerror_nid_postfix() == nullptr);
    void* second = dlopen_nid_postfix(argv[1], 1);
    Require(second && second != module);
    Require(dlclose_nid_postfix(module) == 0);
    Require(dlsym_nid_postfix(module, "GuestModuleAdd") == nullptr);
    add = reinterpret_cast<Add>(dlsym_nid_postfix(second, "GuestModuleAdd"));
    Require(add && add(2, 3) == 5);
    Require(dlclose_nid_postfix(second) == 0);
    Require(dlclose_nid_postfix(second) == -1);

    const std::string guestSource = "anyps5-kernel-module-fixture.prx";
    const std::string converted = guestSource + ".guest.prx";
    std::filesystem::copy_file(argv[1], converted, std::filesystem::copy_options::overwrite_existing);
    void* startupModule = dlopen_nid_postfix(converted.c_str(), 2);
    Require(startupModule != nullptr);
    int startResult = -1;
    const KernelModule kernelModule = sceKernelLoadStartModule(guestSource.c_str(), 0, nullptr, 0, nullptr, &startResult);
    Require(kernelModule > 0 && startResult == 0);
    void* kernelSymbol = nullptr;
    Require(sceKernelDlsym(kernelModule, "GuestModuleAdd", &kernelSymbol) == 0);
    auto kernelAdd = reinterpret_cast<Add>(kernelSymbol);
    Require(kernelAdd && kernelAdd(20, 22) == 42);
    int stopResult = -1;
    Require(sceKernelStopUnloadModule(kernelModule, 0, nullptr, 0, nullptr, &stopResult) == 0 && stopResult == 0);
    Require(dlclose_nid_postfix(startupModule) == 0);
    std::filesystem::remove(converted);
}
