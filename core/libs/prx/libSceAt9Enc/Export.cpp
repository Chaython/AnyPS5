// Compatibility provider generated from the public PS5 NID catalog.
// The module is loadable, but unverified calls fail fast instead of returning fake success.
#include <cstdint>
#include "prx/libc/include/General.hpp"

extern "C" {

APS5_EXPORT("1iDLkHJ4gbA", aps5CompatStub_0);
std::int64_t APS5_VABI aps5CompatStub_0(...) {
    NotImplemented_nid_no_patch("sceAt9EncQueryMemSize");
    return -1;
}

APS5_EXPORT("WBgTbIYvODM", aps5CompatStub_1);
std::int64_t APS5_VABI aps5CompatStub_1(...) {
    NotImplemented_nid_no_patch("sceAt9EncEncode");
    return -1;
}

APS5_EXPORT("ZE8DFcSverk", aps5CompatStub_2);
std::int64_t APS5_VABI aps5CompatStub_2(...) {
    NotImplemented_nid_no_patch("sceAt9EncClearContext");
    return -1;
}

APS5_EXPORT("gujP4afE9dQ", aps5CompatStub_3);
std::int64_t APS5_VABI aps5CompatStub_3(...) {
    NotImplemented_nid_no_patch("sceAt9EncCreateEncoder");
    return -1;
}

APS5_EXPORT("jOnUkmU6pyQ", aps5CompatStub_4);
std::int64_t APS5_VABI aps5CompatStub_4(...) {
    NotImplemented_nid_no_patch("sceAt9EncFlush");
    return -1;
}

}
