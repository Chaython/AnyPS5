// Compatibility provider generated from the public PS5 NID catalog.
// The module is loadable, but unverified calls fail fast instead of returning fake success.
#include <cstdint>
#include "prx/libc/include/General.hpp"

extern "C" {

APS5_EXPORT("3QtqTA0uMmU", aps5CompatStub_0);
std::int64_t APS5_VABI aps5CompatStub_0(...) {
    NotImplemented_nid_no_patch("sceNetCtlApDialogClose");
    return -1;
}

APS5_EXPORT("5X+QFaVtiKg", aps5CompatStub_1);
std::int64_t APS5_VABI aps5CompatStub_1(...) {
    NotImplemented_nid_no_patch("sceNetCtlApDialogTerminate");
    return -1;
}

APS5_EXPORT("SKJQwgudGFg", aps5CompatStub_2);
std::int64_t APS5_VABI aps5CompatStub_2(...) {
    NotImplemented_nid_no_patch("sceNetCtlApDialogOpen");
    return -1;
}

APS5_EXPORT("ZaEVF6WqBP8", aps5CompatStub_3);
std::int64_t APS5_VABI aps5CompatStub_3(...) {
    NotImplemented_nid_no_patch("sceNetCtlApDialogGetResult");
    return -1;
}

APS5_EXPORT("eBWS-HorSWY", aps5CompatStub_4);
std::int64_t APS5_VABI aps5CompatStub_4(...) {
    NotImplemented_nid_no_patch("sceNetCtlApDialogGetStatus");
    return -1;
}

APS5_EXPORT("py8-vzIY-RY", aps5CompatStub_5);
std::int64_t APS5_VABI aps5CompatStub_5(...) {
    NotImplemented_nid_no_patch("sceNetCtlApDialogInitialize");
    return -1;
}

APS5_EXPORT("ys1JAOGzNG0", aps5CompatStub_6);
std::int64_t APS5_VABI aps5CompatStub_6(...) {
    NotImplemented_nid_no_patch("sceNetCtlApDialogUpdateStatus");
    return -1;
}

}
