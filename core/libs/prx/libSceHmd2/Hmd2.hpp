#ifndef CORE_LIBS_PRX_LIBSCEHMD2_HMD2_HPP
#define CORE_LIBS_PRX_LIBSCEHMD2_HMD2_HPP

#include "prx/libc/include/general/VabiMacros.hpp"

#include <cstddef>
#include <cstdint>

namespace Hmd2 {

inline constexpr std::int32_t AlreadyInitialized =
    static_cast<std::int32_t>(0x8a720001u);
inline constexpr std::int32_t NotInitialized =
    static_cast<std::int32_t>(0x8a720002u);
inline constexpr std::int32_t InvalidHandle =
    static_cast<std::int32_t>(0x8a720003u);
inline constexpr std::int32_t DeviceDisconnected =
    static_cast<std::int32_t>(0x8a720004u);
inline constexpr std::int32_t ParameterNull =
    static_cast<std::int32_t>(0x8a720008u);
inline constexpr std::int32_t ParameterInvalid =
    static_cast<std::int32_t>(0x8a720009u);
inline constexpr std::int32_t ReprojectionNotInitialized =
    static_cast<std::int32_t>(0x8a72000bu);
inline constexpr std::int32_t ReprojectionAlreadyInitialized =
    static_cast<std::int32_t>(0x8a72000cu);
inline constexpr std::int32_t ReprojectionInVrMode =
    static_cast<std::int32_t>(0x8a72001cu);

enum class DeviceStatus : std::uint32_t {
    Ready = 0,
    NotReady = 1,
    NotDetected = 2,
    HmuDisconnected = 3,
};

struct InitializeParam {
    void* reserved0;
    std::uint8_t reserved[8];
};

struct DeviceInformation {
    DeviceStatus status;
    std::uint32_t reserved0;
    struct {
        struct {
            std::uint32_t width;
            std::uint32_t height;
        } panelResolution;
        struct {
            std::uint16_t refreshRate90Hz;
            std::uint16_t refreshRate120Hz;
        } flipToDisplayLatency;
    } deviceInfo;
    std::uint8_t hmuMount;
    std::uint8_t lensSeparationDistance;
    std::uint8_t reserved1[2];
    float virtualImageDistance;
};

struct FieldOfView {
    float tanOut;
    float tanIn;
    float tanTop;
    float tanBottom;
};

struct SizeAlign {
    std::uint64_t size;
    std::uint64_t align;
};

struct ReprojectionInitializeParam {
    void* reprojectionBuffer;
    void* displayBuffer;
    std::int32_t threadPriority;
    std::int32_t cpuAffinityMask;
    std::uint32_t pipeId;
    std::uint32_t queueId;
    std::uint32_t type;
    void* seeThroughBuffer;
    std::int32_t reprojectionTiming;
    std::uint32_t reserved[5];
};

static_assert(sizeof(InitializeParam) == 16);
static_assert(sizeof(DeviceInformation) == 28);
static_assert(offsetof(DeviceInformation, deviceInfo) == 8);
static_assert(offsetof(DeviceInformation, hmuMount) == 20);
static_assert(offsetof(DeviceInformation, virtualImageDistance) == 24);
static_assert(sizeof(FieldOfView) == 16);
static_assert(sizeof(SizeAlign) == 16);
static_assert(sizeof(ReprojectionInitializeParam) == 72);

}

extern "C" {
std::int32_t APS5_VABI sceHmd2Initialize(const Hmd2::InitializeParam* param);
std::int32_t APS5_VABI sceHmd2Terminate();
std::int32_t APS5_VABI sceHmd2GetDeviceInformation(Hmd2::DeviceInformation* info);
std::int32_t APS5_VABI sceHmd2GetDeviceInformationByHandle(
    std::int32_t handle, Hmd2::DeviceInformation* info);
std::int32_t APS5_VABI sceHmd2Open(
    std::int32_t userId, std::int32_t type, std::int32_t index, void* param);
std::int32_t APS5_VABI sceHmd2Close(std::int32_t handle);
std::int32_t APS5_VABI sceHmd2GetFieldOfView(
    std::int32_t handle, Hmd2::FieldOfView* fov);
std::int32_t APS5_VABI sceHmd2GetFieldOfViewWithoutHandle(Hmd2::FieldOfView* fov);
std::int32_t APS5_VABI sceHmd2SetVibration(...);

Hmd2::SizeAlign APS5_VABI sceHmd2ReprojectionQueryBufferSizeAlign();
Hmd2::SizeAlign APS5_VABI sceHmd2ReprojectionQueryDisplayBufferSizeAlign();
Hmd2::SizeAlign APS5_VABI sceHmd2ReprojectionQuerySeeThroughBufferSizeAlign();
Hmd2::SizeAlign APS5_VABI sceHmd2ReprojectionGetMirroringWorkMemorySizeAlign();
Hmd2::SizeAlign APS5_VABI sceHmd2ReprojectionGetMirroringDisplayBufferSizeAlign();

std::int32_t APS5_VABI sceHmd2ReprojectionInitialize(
    const Hmd2::ReprojectionInitializeParam* param, void* reserved);
std::int32_t APS5_VABI sceHmd2ReprojectionTerminate();
std::int32_t APS5_VABI sceHmd2ReprojectionEnableVrMode(...);
std::int32_t APS5_VABI sceHmd2ReprojectionDisableVrMode(...);
std::int32_t APS5_VABI sceHmd2ReprojectionBeginFrame(...);
std::int32_t APS5_VABI sceHmd2ReprojectionGetStatus(...);
std::int32_t APS5_VABI sceHmd2ReprojectionGetPredictedDisplayTime(...);
std::int32_t APS5_VABI sceHmd2ReprojectionSetTiming(...);
std::int32_t APS5_VABI sceHmd2ReprojectionSetAllowPositionalReprojection(...);
std::int32_t APS5_VABI sceHmd2ReprojectionSetMirroringOption(...);
std::int32_t APS5_VABI sceHmd2ReprojectionEnableMirroring(...);
std::int32_t APS5_VABI sceHmd2ReprojectionDisableMirroring(...);
std::int32_t APS5_VABI sceHmd2ReprojectionSetUserEventStart(...);
std::int32_t APS5_VABI sceHmd2ReprojectionSetUserEventEnd(...);
std::int32_t APS5_VABI sceHmd2ReprojectionClearUserEventStart(...);
std::int32_t APS5_VABI sceHmd2ReprojectionClearUserEventEnd(...);
std::int32_t APS5_VABI sceHmd2ReprojectionSetParam(...);
std::int32_t APS5_VABI sceHmd2ReprojectionSetParamWithBuffer(...);
std::int32_t APS5_VABI sceHmd2ReprojectionSetRenderConfig(...);
}

#endif
