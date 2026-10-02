// Compatibility provider generated from the public PS5 NID catalog.
// The module is loadable, but unverified calls fail fast instead of returning fake success.
#include <cstdint>
#include "prx/libc/include/General.hpp"

extern "C" {

APS5_EXPORT("-yzonNNSV8E", aps5CompatStub_0);
std::int64_t APS5_VABI aps5CompatStub_0(...) {
    NotImplemented_nid_no_patch("scePfsGetSingleBlockReadCommand");
    return -1;
}

APS5_EXPORT("FpmVtvy8tYo", aps5CompatStub_1);
std::int64_t APS5_VABI aps5CompatStub_1(...) {
    NotImplemented_nid_no_patch("scePfsGetBlockTableReadCommand");
    return -1;
}

APS5_EXPORT("LJTtb7xKzkY", aps5CompatStub_2);
std::int64_t APS5_VABI aps5CompatStub_2(...) {
    NotImplemented_nid_no_patch("scePfsPread");
    return -1;
}

APS5_EXPORT("P6Yiba6u7cc", aps5CompatStub_3);
std::int64_t APS5_VABI aps5CompatStub_3(...) {
    NotImplemented_nid_no_patch("scePfsPreadDecompressBlock");
    return -1;
}

APS5_EXPORT("QiJFZRyM1dE", aps5CompatStub_4);
std::int64_t APS5_VABI aps5CompatStub_4(...) {
    NotImplemented_nid_no_patch("scePfsGetUncompressedSize");
    return -1;
}

APS5_EXPORT("V7BQGxc2Ll0", aps5CompatStub_5);
std::int64_t APS5_VABI aps5CompatStub_5(...) {
    NotImplemented_nid_no_patch("scePfsGetWorkBufferSize");
    return -1;
}

APS5_EXPORT("YyWt34TCzw4", aps5CompatStub_6);
std::int64_t APS5_VABI aps5CompatStub_6(...) {
    NotImplemented_nid_no_patch("scePfsValidate");
    return -1;
}

APS5_EXPORT("qFujOxHUDSs", aps5CompatStub_7);
std::int64_t APS5_VABI aps5CompatStub_7(...) {
    NotImplemented_nid_no_patch("scePfsGetHeaderReadCommand");
    return -1;
}

APS5_EXPORT("smQl0Bi1Ick", aps5CompatStub_8);
std::int64_t APS5_VABI aps5CompatStub_8(...) {
    NotImplemented_nid_no_patch("scePfsGetPreadBlockRange");
    return -1;
}

}
