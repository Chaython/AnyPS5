// Compatibility provider generated from the public PS5 NID catalog.
// The module is loadable, but unverified calls fail fast instead of returning fake success.
#include <cstdint>
#include "prx/libc/include/General.hpp"

extern "C" {

APS5_EXPORT("+YX0z-GUSNw", aps5CompatStub_0);
std::int64_t APS5_VABI aps5CompatStub_0(...) {
    NotImplemented_nid_no_patch("sceCoredumpAttachMemoryRegion");
    return -1;
}

APS5_EXPORT("32KQRUK13kI", aps5CompatStub_1);
std::int64_t APS5_VABI aps5CompatStub_1(...) {
    NotImplemented_nid_no_patch("sceCoredumpWriteUserString");
    return -1;
}

APS5_EXPORT("5nc2gdLNsok", aps5CompatStub_2);
std::int64_t APS5_VABI aps5CompatStub_2(...) {
    NotImplemented_nid_no_patch("sceCoredumpAttachUserFile");
    return -1;
}

APS5_EXPORT("8zLSfEfW5AU", aps5CompatStub_3);
std::int64_t APS5_VABI aps5CompatStub_3(...) {
    NotImplemented_nid_no_patch("sceCoredumpRegisterCoredumpHandler");
    return -1;
}

APS5_EXPORT("Dbbkj6YHWdo", aps5CompatStub_4);
std::int64_t APS5_VABI aps5CompatStub_4(...) {
    NotImplemented_nid_no_patch("sceCoredumpWriteUserData");
    return -1;
}

APS5_EXPORT("DoKHmUw1yiQ", aps5CompatStub_5);
std::int64_t APS5_VABI aps5CompatStub_5(...) {
    NotImplemented_nid_no_patch("sceCoredumpAttachUserMemoryFile");
    return -1;
}

APS5_EXPORT("Jrs7UUkGOFo", aps5CompatStub_6);
std::int64_t APS5_VABI aps5CompatStub_6(...) {
    NotImplemented_nid_no_patch("sceCoredumpGetStopInfoGpu");
    return -1;
}

APS5_EXPORT("MEJ7tc7ThwM", aps5CompatStub_7);
std::int64_t APS5_VABI aps5CompatStub_7(...) {
    NotImplemented_nid_no_patch("sceCoredumpAttachMemoryRegionAsUserFile");
    return -1;
}

APS5_EXPORT("ShChva57wIM", aps5CompatStub_8);
std::int64_t APS5_VABI aps5CompatStub_8(...) {
    NotImplemented_nid_no_patch("sceCoredumpGetThreadContextInfo");
    return -1;
}

APS5_EXPORT("Uxqkdta7wEg", aps5CompatStub_9);
std::int64_t APS5_VABI aps5CompatStub_9(...) {
    NotImplemented_nid_no_patch("sceCoredumpSetUserDataType");
    return -1;
}

APS5_EXPORT("dei8oUx6DbU", aps5CompatStub_10);
std::int64_t APS5_VABI aps5CompatStub_10(...) {
    NotImplemented_nid_no_patch("sceCoredumpDebugTextOut");
    return -1;
}

APS5_EXPORT("fFkhOgztiCA", aps5CompatStub_11);
std::int64_t APS5_VABI aps5CompatStub_11(...) {
    NotImplemented_nid_no_patch("sceCoredumpUnregisterCoredumpHandler");
    return -1;
}

APS5_EXPORT("gzLt9Qrauk0", aps5CompatStub_12);
std::int64_t APS5_VABI aps5CompatStub_12(...) {
    NotImplemented_nid_no_patch("sceCoredumpConfigDumpMode");
    return -1;
}

APS5_EXPORT("kK0DUW1Ukgc", aps5CompatStub_13);
std::int64_t APS5_VABI aps5CompatStub_13(...) {
    NotImplemented_nid_no_patch("sceCoredumpGetStopInfoCpu");
    return -1;
}

}
