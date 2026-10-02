#ifndef CORE_LIBS_PRX_LIBSCEVRSETUPDIALOG_VRSETUPDIALOG_HPP
#define CORE_LIBS_PRX_LIBSCEVRSETUPDIALOG_VRSETUPDIALOG_HPP

#include "prx/libc/include/general/VabiMacros.hpp"

#include <cstdint>

namespace VrSetupDialog {

inline constexpr std::int32_t Ok = 0;

enum class Status : std::int32_t {
    None = 0,
    Initialized = 1,
    Running = 2,
    Finished = 3,
};

}

extern "C" {
std::int32_t APS5_VABI sceVrSetupDialogInitialize();
std::int32_t APS5_VABI sceVrSetupDialogOpen(const void* param);
std::int32_t APS5_VABI sceVrSetupDialogUpdateStatus();
std::int32_t APS5_VABI sceVrSetupDialogGetStatus();
std::int32_t APS5_VABI sceVrSetupDialogGetResult(void* result);
std::int32_t APS5_VABI sceVrSetupDialogClose();
std::int32_t APS5_VABI sceVrSetupDialogTerminate();
}

#endif
