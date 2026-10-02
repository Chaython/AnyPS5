#include "prx/libSceHmd2/Hmd2.hpp"

int main() {
    Hmd2::InitializeParam param{};
    Hmd2::DeviceInformation info{};

    if (sceHmd2Initialize(nullptr) != Hmd2::ParameterNull) return 1;
    if (sceHmd2Initialize(&param) != 0) return 2;
    if (sceHmd2Initialize(&param) != Hmd2::AlreadyInitialized) return 3;

    if (sceHmd2GetDeviceInformation(&info) != 0) return 4;
    if (info.status != Hmd2::DeviceStatus::NotDetected) return 5;
    if (sceHmd2Open(0, 0, 0, nullptr) != Hmd2::DeviceDisconnected) return 6;
    if (sceHmd2ReprojectionSetAllowPositionalReprojection() != 0) return 7;

    const auto size = sceHmd2ReprojectionQueryBufferSizeAlign();
    if (size.size != 0 || size.align == 0) return 8;

    if (sceHmd2Terminate() != 0) return 9;
    if (sceHmd2GetDeviceInformation(&info) != Hmd2::NotInitialized) return 10;
    return 0;
}
