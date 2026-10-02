// Compatibility provider generated from the public PS5 NID catalog.
// The module is loadable, but unverified calls fail fast instead of returning fake success.
#include <cstdint>
#include "prx/libc/include/General.hpp"

extern "C" {

APS5_EXPORT("+MdgHI0TYOw", aps5CompatStub_0);
std::int64_t APS5_VABI aps5CompatStub_0(...) {
    NotImplemented_nid_no_patch("sceM4aacEncFlush");
    return -1;
}

APS5_EXPORT("Fqn-wkJCOes", aps5CompatStub_1);
std::int64_t APS5_VABI aps5CompatStub_1(...) {
    NotImplemented_nid_no_patch("sceM4aacEncEncode");
    return -1;
}

APS5_EXPORT("U2iepfgrDMg", aps5CompatStub_2);
std::int64_t APS5_VABI aps5CompatStub_2(...) {
    NotImplemented_nid_no_patch("sceM4aacEncCreateEncoder");
    return -1;
}

APS5_EXPORT("uPqxvaoLkbM", aps5CompatStub_3);
std::int64_t APS5_VABI aps5CompatStub_3(...) {
    NotImplemented_nid_no_patch("sceM4aacEncClearContext");
    return -1;
}

APS5_EXPORT("z394AzuRItE", aps5CompatStub_4);
std::int64_t APS5_VABI aps5CompatStub_4(...) {
    NotImplemented_nid_no_patch("sceM4aacEncDeleteEncoder");
    return -1;
}

}
