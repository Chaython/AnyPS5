#include "Hmd2.hpp"

#include <atomic>

namespace {

std::atomic<bool> initialized{false};

std::int32_t RequireInitialized() {
    return initialized.load(std::memory_order_acquire) ? 0 : Hmd2::NotInitialized;
}

std::int32_t NoDevice() {
    const auto state = RequireInitialized();
    return state == 0 ? Hmd2::DeviceDisconnected : state;
}

void FillDisconnected(Hmd2::DeviceInformation* info) {
    *info = {};
    info->status = Hmd2::DeviceStatus::NotDetected;
}

}

extern "C" {

std::int32_t APS5_VABI sceHmd2Initialize(const Hmd2::InitializeParam* param) {
    if (!param) return Hmd2::ParameterNull;
    if (initialized.exchange(true, std::memory_order_acq_rel))
        return Hmd2::AlreadyInitialized;
    return 0;
}

std::int32_t APS5_VABI sceHmd2Terminate() {
    return initialized.exchange(false, std::memory_order_acq_rel)
        ? 0
        : Hmd2::NotInitialized;
}

std::int32_t APS5_VABI sceHmd2GetDeviceInformation(Hmd2::DeviceInformation* info) {
    if (!info) return Hmd2::ParameterNull;
    const auto state = RequireInitialized();
    if (state != 0) return state;
    FillDisconnected(info);
    return 0;
}

std::int32_t APS5_VABI sceHmd2GetDeviceInformationByHandle(
    const std::int32_t handle, Hmd2::DeviceInformation* info) {
    (void)handle;
    if (!info) return Hmd2::ParameterNull;
    const auto state = RequireInitialized();
    if (state != 0) return state;
    FillDisconnected(info);
    return Hmd2::InvalidHandle;
}

std::int32_t APS5_VABI sceHmd2Open(
    const std::int32_t userId, const std::int32_t type,
    const std::int32_t index, void* param) {
    (void)userId;
    (void)type;
    (void)index;
    (void)param;
    return NoDevice();
}

std::int32_t APS5_VABI sceHmd2Close(const std::int32_t handle) {
    (void)handle;
    const auto state = RequireInitialized();
    return state == 0 ? Hmd2::InvalidHandle : state;
}

std::int32_t APS5_VABI sceHmd2GetFieldOfView(
    const std::int32_t handle, Hmd2::FieldOfView* fov) {
    (void)handle;
    if (!fov) return Hmd2::ParameterNull;
    *fov = {};
    return NoDevice();
}

std::int32_t APS5_VABI sceHmd2GetFieldOfViewWithoutHandle(Hmd2::FieldOfView* fov) {
    if (!fov) return Hmd2::ParameterNull;
    *fov = {};
    return NoDevice();
}

std::int32_t APS5_VABI sceHmd2SetVibration(...) {
    return NoDevice();
}

Hmd2::SizeAlign APS5_VABI sceHmd2ReprojectionQueryBufferSizeAlign() {
    return {0, 1};
}

Hmd2::SizeAlign APS5_VABI sceHmd2ReprojectionQueryDisplayBufferSizeAlign() {
    return {0, 1};
}

Hmd2::SizeAlign APS5_VABI sceHmd2ReprojectionQuerySeeThroughBufferSizeAlign() {
    return {0, 1};
}

Hmd2::SizeAlign APS5_VABI sceHmd2ReprojectionGetMirroringWorkMemorySizeAlign() {
    return {0, 1};
}

Hmd2::SizeAlign APS5_VABI sceHmd2ReprojectionGetMirroringDisplayBufferSizeAlign() {
    return {0, 1};
}

std::int32_t APS5_VABI sceHmd2ReprojectionInitialize(
    const Hmd2::ReprojectionInitializeParam* param, void* reserved) {
    (void)reserved;
    const auto state = RequireInitialized();
    if (state != 0) return state;
    if (!param) return Hmd2::ParameterNull;
    return Hmd2::DeviceDisconnected;
}

std::int32_t APS5_VABI sceHmd2ReprojectionTerminate() {
    const auto state = RequireInitialized();
    return state == 0 ? Hmd2::ReprojectionNotInitialized : state;
}

std::int32_t APS5_VABI sceHmd2ReprojectionEnableVrMode(...) {
    return NoDevice();
}

std::int32_t APS5_VABI sceHmd2ReprojectionDisableVrMode(...) {
    const auto state = RequireInitialized();
    return state == 0 ? 0 : state;
}

std::int32_t APS5_VABI sceHmd2ReprojectionBeginFrame(...) {
    return NoDevice();
}

std::int32_t APS5_VABI sceHmd2ReprojectionGetStatus(...) {
    return NoDevice();
}

std::int32_t APS5_VABI sceHmd2ReprojectionGetPredictedDisplayTime(...) {
    return NoDevice();
}

std::int32_t APS5_VABI sceHmd2ReprojectionSetTiming(...) {
    const auto state = RequireInitialized();
    return state == 0 ? 0 : state;
}

std::int32_t APS5_VABI sceHmd2ReprojectionSetAllowPositionalReprojection(...) {
    const auto state = RequireInitialized();
    return state == 0 ? 0 : state;
}

std::int32_t APS5_VABI sceHmd2ReprojectionSetMirroringOption(...) {
    const auto state = RequireInitialized();
    return state == 0 ? 0 : state;
}

std::int32_t APS5_VABI sceHmd2ReprojectionEnableMirroring(...) {
    return NoDevice();
}

std::int32_t APS5_VABI sceHmd2ReprojectionDisableMirroring(...) {
    const auto state = RequireInitialized();
    return state == 0 ? 0 : state;
}

std::int32_t APS5_VABI sceHmd2ReprojectionSetUserEventStart(...) {
    const auto state = RequireInitialized();
    return state == 0 ? 0 : state;
}

std::int32_t APS5_VABI sceHmd2ReprojectionSetUserEventEnd(...) {
    const auto state = RequireInitialized();
    return state == 0 ? 0 : state;
}

std::int32_t APS5_VABI sceHmd2ReprojectionClearUserEventStart(...) {
    const auto state = RequireInitialized();
    return state == 0 ? 0 : state;
}

std::int32_t APS5_VABI sceHmd2ReprojectionClearUserEventEnd(...) {
    const auto state = RequireInitialized();
    return state == 0 ? 0 : state;
}

std::int32_t APS5_VABI sceHmd2ReprojectionSetParam(...) {
    const auto state = RequireInitialized();
    return state == 0 ? 0 : state;
}

std::int32_t APS5_VABI sceHmd2ReprojectionSetParamWithBuffer(...) {
    const auto state = RequireInitialized();
    return state == 0 ? 0 : state;
}

std::int32_t APS5_VABI sceHmd2ReprojectionSetRenderConfig(...) {
    const auto state = RequireInitialized();
    return state == 0 ? 0 : state;
}

}
