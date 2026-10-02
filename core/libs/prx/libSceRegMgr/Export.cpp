#include <cstdint>
#include "prx/libc/include/General.hpp"

namespace {
constexpr std::int32_t SCE_REGMGR_ERROR_ENTRY_NOT_FOUND =
    static_cast<std::int32_t>(0x80060002u);
}

extern "C" {

std::int32_t APS5_VABI sceRegMgrNonSysGetInt(std::uint32_t key, std::uint64_t value) {
    (void)key;
    (void)value;
    // Match the console's "setting was never written" state. Callers then
    // retain their own defaults instead of consuming a fabricated value.
    return SCE_REGMGR_ERROR_ENTRY_NOT_FOUND;
}

}
