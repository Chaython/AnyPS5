// Compatibility provider generated from the public PS5 NID catalog.
// The module is loadable, but unverified calls fail fast instead of returning fake success.
#include <cstdint>
#include "prx/libc/include/General.hpp"

extern "C" {

APS5_EXPORT("-0jDlM2hG5k", aps5CompatStub_0);
std::int64_t APS5_VABI aps5CompatStub_0(...) {
    NotImplemented_nid_no_patch("sceAudiodecCpuInternalDecode");
    return -1;
}

APS5_EXPORT("CnY1NGmdi7I", aps5CompatStub_1);
std::int64_t APS5_VABI aps5CompatStub_1(...) {
    NotImplemented_nid_no_patch("sceAudiodecCpuInternalClearContext");
    return -1;
}

APS5_EXPORT("KkhdeVCyo6Y", aps5CompatStub_2);
std::int64_t APS5_VABI aps5CompatStub_2(...) {
    NotImplemented_nid_no_patch("sceAudiodecCpuInternalInitDecoder");
    return -1;
}

APS5_EXPORT("R8v5kdZ55mY", aps5CompatStub_3);
std::int64_t APS5_VABI aps5CompatStub_3(...) {
    NotImplemented_nid_no_patch("sceAudiodecCpuInternalQueryMemSize");
    return -1;
}

APS5_EXPORT("hAS5WH6hxrE", aps5CompatStub_4);
std::int64_t APS5_VABI aps5CompatStub_4(...) {
    NotImplemented_nid_no_patch("sceAudiodecCpuClearContext");
    return -1;
}

APS5_EXPORT("hdFsxo3MFu8", aps5CompatStub_5);
std::int64_t APS5_VABI aps5CompatStub_5(...) {
    NotImplemented_nid_no_patch("sceAudiodecCpuInitDecoder");
    return -1;
}

APS5_EXPORT("ktD2w3D4G2U", aps5CompatStub_6);
std::int64_t APS5_VABI aps5CompatStub_6(...) {
    NotImplemented_nid_no_patch("sceAudiodecCpuQueryMemSize");
    return -1;
}

APS5_EXPORT("lSVTiWV5wLc", aps5CompatStub_7);
std::int64_t APS5_VABI aps5CompatStub_7(...) {
    NotImplemented_nid_no_patch("sceAudiodecCpuDecode");
    return -1;
}

}
