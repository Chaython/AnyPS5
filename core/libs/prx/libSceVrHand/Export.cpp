// Compatibility provider generated from the public PS5 NID catalog.
// The module is loadable, but unverified calls fail fast instead of returning fake success.
#include <cstdint>
#include "prx/libc/include/General.hpp"

extern "C" {

APS5_EXPORT("-iqDAXQlXew", aps5CompatStub_0);
std::int64_t APS5_VABI aps5CompatStub_0(...) {
    NotImplemented_nid_no_patch("sceVrHandInitialize");
    return -1;
}

APS5_EXPORT("-wppX82q8dk", aps5CompatStub_1);
std::int64_t APS5_VABI aps5CompatStub_1(...) {
    NotImplemented_nid_no_patch("sceVrHandFinalize");
    return -1;
}

APS5_EXPORT("EG4LeoZJ+mY", aps5CompatStub_2);
std::int64_t APS5_VABI aps5CompatStub_2(...) {
    NotImplemented_nid_no_patch("sceVrHandQueryMemory");
    return -1;
}

APS5_EXPORT("HK2JUFv2RJE", aps5CompatStub_3);
std::int64_t APS5_VABI aps5CompatStub_3(...) {
    NotImplemented_nid_no_patch("sceVrHandGetMeshSize");
    return -1;
}

APS5_EXPORT("ZRe6rlaz1wY", aps5CompatStub_4);
std::int64_t APS5_VABI aps5CompatStub_4(...) {
    NotImplemented_nid_no_patch("sceVrHandStart");
    return -1;
}

APS5_EXPORT("kqcnvHb39ls", aps5CompatStub_5);
std::int64_t APS5_VABI aps5CompatStub_5(...) {
    NotImplemented_nid_no_patch("sceVrHandGetMesh");
    return -1;
}

APS5_EXPORT("lt1WXVXcyxk", aps5CompatStub_6);
std::int64_t APS5_VABI aps5CompatStub_6(...) {
    NotImplemented_nid_no_patch("sceVrHandGetResult");
    return -1;
}

APS5_EXPORT("z1EUL+LMJ+8", aps5CompatStub_7);
std::int64_t APS5_VABI aps5CompatStub_7(...) {
    NotImplemented_nid_no_patch("sceVrHandStop");
    return -1;
}

}
