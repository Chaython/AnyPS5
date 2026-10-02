#include <cstdint>
#include "prx/libc/include/General.hpp"

namespace {
constexpr std::int32_t SCE_SYSTEM_SERVICE_ERROR_UNAVAILABLE =
    static_cast<std::int32_t>(0x80A10002u);
}

extern "C" {

std::int32_t APS5_VABI sceSystemServiceOpenTournamentOccurrence(...) {
    // Tournament shell UI is unavailable on the host. Return the platform's
    // documented/observed "unavailable" service error instead of crashing.
    return SCE_SYSTEM_SERVICE_ERROR_UNAVAILABLE;
}

}
