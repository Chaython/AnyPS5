// Compatibility provider generated from the public PS5 NID catalog.
// The module is loadable, but unverified calls fail fast instead of returning fake success.
#include <cstdint>
#include "prx/libc/include/General.hpp"

extern "C" {

APS5_EXPORT("NOAMxY2EGS0", aps5CompatStub_0);
std::int64_t APS5_VABI aps5CompatStub_0(...) {
    NotImplemented_nid_no_patch("scePlayGoDialogGetStatus");
    return -1;
}

APS5_EXPORT("Yb60K7BST48", aps5CompatStub_1);
std::int64_t APS5_VABI aps5CompatStub_1(...) {
    NotImplemented_nid_no_patch("scePlayGoDialogUpdateStatus");
    return -1;
}

APS5_EXPORT("fECamTJKpsM", aps5CompatStub_2);
std::int64_t APS5_VABI aps5CompatStub_2(...) {
    NotImplemented_nid_no_patch("scePlayGoDialogInitialize");
    return -1;
}

APS5_EXPORT("fbigNQiZpm0", aps5CompatStub_3);
std::int64_t APS5_VABI aps5CompatStub_3(...) {
    NotImplemented_nid_no_patch("scePlayGoDialogClose");
    return -1;
}

APS5_EXPORT("kHd72ukqbxw", aps5CompatStub_4);
std::int64_t APS5_VABI aps5CompatStub_4(...) {
    NotImplemented_nid_no_patch("scePlayGoDialogOpen");
    return -1;
}

APS5_EXPORT("okgIGdr5Iz0", aps5CompatStub_5);
std::int64_t APS5_VABI aps5CompatStub_5(...) {
    NotImplemented_nid_no_patch("scePlayGoDialogTerminate");
    return -1;
}

APS5_EXPORT("wx9TDplJKB4", aps5CompatStub_6);
std::int64_t APS5_VABI aps5CompatStub_6(...) {
    NotImplemented_nid_no_patch("scePlayGoDialogGetResult");
    return -1;
}

}
