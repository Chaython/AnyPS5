#include "VrSetupDialog.hpp"

#include <atomic>

namespace {

std::atomic<VrSetupDialog::Status> status{VrSetupDialog::Status::None};

}

extern "C" {

std::int32_t APS5_VABI sceVrSetupDialogInitialize() {
    status.store(VrSetupDialog::Status::Initialized, std::memory_order_release);
    return VrSetupDialog::Ok;
}

std::int32_t APS5_VABI sceVrSetupDialogOpen(const void* param) {
    (void)param;
    // No host PS VR2 setup surface exists yet. Complete immediately instead of
    // leaving a title polling forever, while HMD2 itself continues to report no
    // connected headset.
    status.store(VrSetupDialog::Status::Finished, std::memory_order_release);
    return VrSetupDialog::Ok;
}

std::int32_t APS5_VABI sceVrSetupDialogUpdateStatus() {
    return static_cast<std::int32_t>(status.load(std::memory_order_acquire));
}

std::int32_t APS5_VABI sceVrSetupDialogGetStatus() {
    return static_cast<std::int32_t>(status.load(std::memory_order_acquire));
}

std::int32_t APS5_VABI sceVrSetupDialogGetResult(void* result) {
    // The PS5 result structure is not publicly documented. Do not guess its
    // layout or overwrite guest memory; callers normally pass a zeroed result
    // structure, which truthfully represents that no VR setup occurred.
    (void)result;
    return VrSetupDialog::Ok;
}

std::int32_t APS5_VABI sceVrSetupDialogClose() {
    status.store(VrSetupDialog::Status::None, std::memory_order_release);
    return VrSetupDialog::Ok;
}

std::int32_t APS5_VABI sceVrSetupDialogTerminate() {
    return sceVrSetupDialogClose();
}

}
