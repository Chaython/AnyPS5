// Compatibility provider generated from the public PS5 NID catalog.
// The module is loadable, but unverified calls fail fast instead of returning fake success.
#include <cstdint>
#include "prx/libc/include/General.hpp"

extern "C" {

APS5_EXPORT("1ZWN9dK5-Sc", aps5CompatStub_0);
std::int64_t APS5_VABI aps5CompatStub_0(...) {
    NotImplemented_nid_no_patch("sceSulphaMessage");
    return -1;
}

APS5_EXPORT("6x06HaJWiDQ", aps5CompatStub_1);
std::int64_t APS5_VABI aps5CompatStub_1(...) {
    NotImplemented_nid_no_patch("sceSulphaGetNeededMemory");
    return -1;
}

APS5_EXPORT("SqfL0YjlBiA", aps5CompatStub_2);
std::int64_t APS5_VABI aps5CompatStub_2(...) {
    NotImplemented_nid_no_patch("sceSulphaGetDefaultConfig");
    return -1;
}

APS5_EXPORT("fpFCQLbTB1w", aps5CompatStub_3);
std::int64_t APS5_VABI aps5CompatStub_3(...) {
    NotImplemented_nid_no_patch("sceSulphaInit");
    return -1;
}

APS5_EXPORT("gLjLCP6tKuw", aps5CompatStub_4);
std::int64_t APS5_VABI aps5CompatStub_4(...) {
    NotImplemented_nid_no_patch("sceSulphaShutdown");
    return -1;
}

APS5_EXPORT("miCg4z-c1Gc", aps5CompatStub_5);
std::int64_t APS5_VABI aps5CompatStub_5(...) {
    NotImplemented_nid_no_patch("sceSulphaSetBookmark");
    return -1;
}

}
