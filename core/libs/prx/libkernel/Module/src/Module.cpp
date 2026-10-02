#include <cstdint>
#include <cstddef>
#include <cstring>
#include <stdexcept>
#include <string>
#include "SceTypes.hpp"
#include "prx/libc/include/General.hpp"

namespace {
constexpr int KernelEsrch = static_cast<int>(0x80020003u);
constexpr int KernelEfault = static_cast<int>(0x8002000Eu);
constexpr int KernelEinval = static_cast<int>(0x80020016u);
}

extern "C" {
void* APS5_VABI dlopen_sce_module_nid_no_patch(const char* path, int flags);
void* APS5_VABI dlsym_nid_postfix(void* handle, const char* name);
int APS5_VABI dlclose_nid_postfix(void* handle);

int APS5_VABI sceKernelDlsym(KernelModule handle, const char* symbol, void** addr) {
 if (!symbol || !addr) return KernelEfault;
 void* found = dlsym_nid_postfix(reinterpret_cast<void*>(static_cast<std::intptr_t>(handle)), symbol);
 if (!found) return KernelEsrch;
 *addr = found;
 return 0;
}

int APS5_VABI sceKernelGetModuleInfoForUnwind(uint64_t addr, int flags, ModuleInfoForUnwind* info) {
  (void)flags;
  if (!info) return SCE_KERNEL_ERROR_EFAULT;
#ifdef _WIN32
  MEMORY_BASIC_INFORMATION mbi{};
  if (!VirtualQuery(reinterpret_cast<LPCVOID>(addr), &mbi, sizeof(mbi))) return SCE_KERNEL_ERROR_ESRCH;
  info->st_size = sizeof(ModuleInfoForUnwind);
  info->eh_frame_hdr_addr = 0;
  info->eh_frame_addr = 0;
  info->eh_frame_size = 0;
  info->seg0_addr = reinterpret_cast<std::uint64_t>(mbi.BaseAddress);
  info->seg0_size = mbi.RegionSize;
  char path[4096] = {};
  DWORD len = GetMappedFileNameA(GetCurrentProcess(), mbi.BaseAddress, path, sizeof(path) - 1);
  path[len] = '\0';
  std::strncpy(info->name, path, sizeof(info->name) - 1);
  info->name[sizeof(info->name) - 1] = '\0';
  return 0;
#else
  std::ifstream maps("/proc/self/maps");
  if (!maps) throw std::runtime_error("sceKernelGetModuleInfoForUnwind: failed to open /proc/self/maps");
  std::string line;
  while (std::getline(maps, line)) {
    std::uint64_t start = 0;
    std::uint64_t end = 0;
    char perms[8] = {};
    std::uint64_t offset = 0;
    unsigned int devMajor = 0;
    unsigned int devMinor = 0;
    std::uint64_t inode = 0;
    char path[4096] = {};
    int parsed = std::sscanf(line.c_str(), "%llx-%llx %7s %llx %x:%x %llu %4095s",
      (unsigned long long*)&start, (unsigned long long*)&end, perms,
      (unsigned long long*)&offset, &devMajor, &devMinor, (unsigned long long*)&inode, path);
    if (parsed < 7 || addr < start || addr >= end) continue;
    info->st_size = sizeof(ModuleInfoForUnwind);
    std::strncpy(info->name, parsed >= 8 ? path : "", sizeof(info->name) - 1);
    info->name[sizeof(info->name) - 1] = '\0';
    info->eh_frame_hdr_addr = 0;
    info->eh_frame_addr = 0;
    info->eh_frame_size = 0;
    info->seg0_addr = start;
    info->seg0_size = end - start;
    return 0;
  }
  return SCE_KERNEL_ERROR_ESRCH;
#endif
}

int APS5_VABI sceKernelGetModuleInfoFromAddr(uint64_t addr, int n, ModuleInfo* r) {
 (void)addr;
 (void)n;
 (void)r;
 NotImplemented_nid_no_patch(__func__);
 return 0;
}

KernelModule APS5_VABI sceKernelLoadStartModule(const char* module_file_name, size_t args, const void* argp, uint32_t flags, const KernelLoadModuleOpt* opt, int* res) {
 (void)args;
 (void)argp;
 (void)opt;
 if (!module_file_name) return static_cast<KernelModule>(KernelEfault);
 if (flags != 0) return static_cast<KernelModule>(KernelEinval);
 void* handle = dlopen_sce_module_nid_no_patch(module_file_name, 2);
 if (!handle) return static_cast<KernelModule>(KernelEsrch);
 if (res) *res = 0;
 return static_cast<KernelModule>(reinterpret_cast<std::intptr_t>(handle));
}

int APS5_VABI sceKernelStopUnloadModule(KernelModule handle, size_t args, const void* argp, uint32_t flags, const KernelUnloadModuleOpt* opt, int* res) {
 (void)args;
 (void)argp;
 (void)opt;
 if (flags != 0) return KernelEinval;
 if (dlclose_nid_postfix(reinterpret_cast<void*>(static_cast<std::intptr_t>(handle))) != 0) return KernelEsrch;
 if (res) *res = 0;
 return 0;
}

}

extern "C" {

int APS5_VABI __elf_phdr_match_addr_nid_postfix(ModuleInfo* module, std::uint64_t address) {
    (void)module;
    (void)address;
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

// unknown signature
std::int32_t APS5_VABI sceKernelInternalMemoryGetModuleSegmentInfo_nid_postfix(void* result) {
    (void)result;
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

}
