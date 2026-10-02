// Compatibility provider generated from the public PS5 NID catalog.
// The module is loadable, but unverified calls fail fast instead of returning fake success.
#include <cstdint>
#include "prx/libc/include/General.hpp"

extern "C" {

APS5_EXPORT("19Ec7WkMFfQ", aps5CompatStub_0);
std::int64_t APS5_VABI aps5CompatStub_0(...) {
    NotImplemented_nid_no_patch("sceNetCtlApCheckCallback");
    return -1;
}

APS5_EXPORT("4jkLJc954+Q", aps5CompatStub_1);
std::int64_t APS5_VABI aps5CompatStub_1(...) {
    NotImplemented_nid_no_patch("sceNetCtlApGetResult");
    return -1;
}

APS5_EXPORT("AKZOzsb9whc", aps5CompatStub_2);
std::int64_t APS5_VABI aps5CompatStub_2(...) {
    NotImplemented_nid_no_patch("sceNetCtlApGetState");
    return -1;
}

APS5_EXPORT("FdN+edNRtiw", aps5CompatStub_3);
std::int64_t APS5_VABI aps5CompatStub_3(...) {
    NotImplemented_nid_no_patch("sceNetCtlApInit");
    return -1;
}

APS5_EXPORT("LXADzTIzM9I", aps5CompatStub_4);
std::int64_t APS5_VABI aps5CompatStub_4(...) {
    NotImplemented_nid_no_patch("sceNetCtlApGetInfo");
    return -1;
}

APS5_EXPORT("NpTcFtaQ-0E", aps5CompatStub_5);
std::int64_t APS5_VABI aps5CompatStub_5(...) {
    NotImplemented_nid_no_patch("sceNetCtlApUnregisterCallback");
    return -1;
}

APS5_EXPORT("cv5Y2efOTeg", aps5CompatStub_6);
std::int64_t APS5_VABI aps5CompatStub_6(...) {
    NotImplemented_nid_no_patch("sceNetCtlApTerm");
    return -1;
}

APS5_EXPORT("hfkLVdXmfnU", aps5CompatStub_7);
std::int64_t APS5_VABI aps5CompatStub_7(...) {
    NotImplemented_nid_no_patch("sceNetCtlApGetConnectInfo");
    return -1;
}

APS5_EXPORT("meFMaDpdsVI", aps5CompatStub_8);
std::int64_t APS5_VABI aps5CompatStub_8(...) {
    NotImplemented_nid_no_patch("sceNetCtlApClearEvent");
    return -1;
}

APS5_EXPORT("pmjobSVHuY0", aps5CompatStub_9);
std::int64_t APS5_VABI aps5CompatStub_9(...) {
    NotImplemented_nid_no_patch("sceNetCtlApRegisterCallback");
    return -1;
}

APS5_EXPORT("r-pOyN6AhsM", aps5CompatStub_10);
std::int64_t APS5_VABI aps5CompatStub_10(...) {
    NotImplemented_nid_no_patch("sceNetCtlApStop");
    return -1;
}

}
