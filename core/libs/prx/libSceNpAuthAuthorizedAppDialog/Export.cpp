// Compatibility provider generated from the public PS5 NID catalog.
// The module is loadable, but unverified calls fail fast instead of returning fake success.
#include <cstdint>
#include "prx/libc/include/General.hpp"

extern "C" {

APS5_EXPORT("2SlriPR9TMc", aps5CompatStub_0);
std::int64_t APS5_VABI aps5CompatStub_0(...) {
    NotImplemented_nid_no_patch("sceNpAuthAuthorizedAppDialogTerminate");
    return -1;
}

APS5_EXPORT("9CiV7Shy6qQ", aps5CompatStub_1);
std::int64_t APS5_VABI aps5CompatStub_1(...) {
    NotImplemented_nid_no_patch("sceNpAuthAuthorizedAppDialogInitialize");
    return -1;
}

APS5_EXPORT("L5iGQ06WlTo", aps5CompatStub_2);
std::int64_t APS5_VABI aps5CompatStub_2(...) {
    NotImplemented_nid_no_patch("sceNpAuthAuthorizedAppDialogUpdateStatus");
    return -1;
}

APS5_EXPORT("bZlJPa2hPWk", aps5CompatStub_3);
std::int64_t APS5_VABI aps5CompatStub_3(...) {
    NotImplemented_nid_no_patch("sceNpAuthAuthorizedAppDialogGetResult");
    return -1;
}

APS5_EXPORT("cTvSsJRn30w", aps5CompatStub_4);
std::int64_t APS5_VABI aps5CompatStub_4(...) {
    NotImplemented_nid_no_patch("sceNpAuthAuthorizedAppDialogOpen");
    return -1;
}

APS5_EXPORT("umZaRUshsKc", aps5CompatStub_5);
std::int64_t APS5_VABI aps5CompatStub_5(...) {
    NotImplemented_nid_no_patch("sceNpAuthAuthorizedAppDialogClose");
    return -1;
}

APS5_EXPORT("wQdH6qOwqhE", aps5CompatStub_6);
std::int64_t APS5_VABI aps5CompatStub_6(...) {
    NotImplemented_nid_no_patch("sceNpAuthAuthorizedAppDialogGetStatus");
    return -1;
}

}
