// Compatibility provider generated from the public PS5 NID catalog.
// The module is loadable, but unverified calls fail fast instead of returning fake success.
#include <cstdint>
#include "prx/libc/include/General.hpp"

extern "C" {

APS5_EXPORT("+L5ArV1tPGA", aps5CompatStub_0);
std::int64_t APS5_VABI aps5CompatStub_0(...) {
    NotImplemented_nid_no_patch("sceVdecswCreateDecoder");
    return -1;
}

APS5_EXPORT("0moTubWCsTM", aps5CompatStub_1);
std::int64_t APS5_VABI aps5CompatStub_1(...) {
    NotImplemented_nid_no_patch("sceVdecswQueryComputeMemoryInfo");
    return -1;
}

APS5_EXPORT("5Y6nZqIZvBg", aps5CompatStub_2);
std::int64_t APS5_VABI aps5CompatStub_2(...) {
    NotImplemented_nid_no_patch("sceVdecswFinalizeDecodeSequence");
    return -1;
}

APS5_EXPORT("A+2M7EivuOU", aps5CompatStub_3);
std::int64_t APS5_VABI aps5CompatStub_3(...) {
    NotImplemented_nid_no_patch("sceVdecswQueryDecoderMemoryInfo");
    return -1;
}

APS5_EXPORT("AAMM-Q1X0g0", aps5CompatStub_4);
std::int64_t APS5_VABI aps5CompatStub_4(...) {
    NotImplemented_nid_no_patch("sceVdecswSyncDecodeInput");
    return -1;
}

APS5_EXPORT("FzECy3Wxxas", aps5CompatStub_5);
std::int64_t APS5_VABI aps5CompatStub_5(...) {
    NotImplemented_nid_no_patch("sceVdecswGetPictureInfo");
    return -1;
}

APS5_EXPORT("LEqH+dkMszo", aps5CompatStub_6);
std::int64_t APS5_VABI aps5CompatStub_6(...) {
    NotImplemented_nid_no_patch("sceVdecswGetVp9PictureInfo");
    return -1;
}

APS5_EXPORT("PzF+L5zXoyg", aps5CompatStub_7);
std::int64_t APS5_VABI aps5CompatStub_7(...) {
    NotImplemented_nid_no_patch("sceVdecswGetHevcPictureInfo");
    return -1;
}

APS5_EXPORT("aqMiF0AgUYI", aps5CompatStub_8);
std::int64_t APS5_VABI aps5CompatStub_8(...) {
    NotImplemented_nid_no_patch("sceVdecswSetDecodeInput");
    return -1;
}

APS5_EXPORT("ecUtPX+dBYk", aps5CompatStub_9);
std::int64_t APS5_VABI aps5CompatStub_9(...) {
    NotImplemented_nid_no_patch("sceVdecswDeleteDecoder");
    return -1;
}

APS5_EXPORT("fX-zOOefbbs", aps5CompatStub_10);
std::int64_t APS5_VABI aps5CompatStub_10(...) {
    NotImplemented_nid_no_patch("sceVdecswReleaseComputeQueue");
    return -1;
}

APS5_EXPORT("hIgrg5h4V6s", aps5CompatStub_11);
std::int64_t APS5_VABI aps5CompatStub_11(...) {
    NotImplemented_nid_no_patch("sceVdecswAllocateComputeQueue");
    return -1;
}

APS5_EXPORT("ihNT-uuEAr4", aps5CompatStub_12);
std::int64_t APS5_VABI aps5CompatStub_12(...) {
    NotImplemented_nid_no_patch("sceVdecswGetAvcPictureInfo");
    return -1;
}

APS5_EXPORT("kMBw37oH8nI", aps5CompatStub_13);
std::int64_t APS5_VABI aps5CompatStub_13(...) {
    NotImplemented_nid_no_patch("sceVdecswTrySyncDecodeOutput");
    return -1;
}

APS5_EXPORT("l4sQYy5wPkc", aps5CompatStub_14);
std::int64_t APS5_VABI aps5CompatStub_14(...) {
    NotImplemented_nid_no_patch("sceVdecswTrySyncDecodeInput");
    return -1;
}

APS5_EXPORT("rgtMCOpyBSc", aps5CompatStub_15);
std::int64_t APS5_VABI aps5CompatStub_15(...) {
    NotImplemented_nid_no_patch("sceVdecswSetDecodeOutput");
    return -1;
}

APS5_EXPORT("tWiSgXov8GM", aps5CompatStub_16);
std::int64_t APS5_VABI aps5CompatStub_16(...) {
    NotImplemented_nid_no_patch("sceVdecswSyncDecodeOutput");
    return -1;
}

APS5_EXPORT("veb-YBrOqo0", aps5CompatStub_17);
std::int64_t APS5_VABI aps5CompatStub_17(...) {
    NotImplemented_nid_no_patch("sceVdecswResetDecoder");
    return -1;
}

}
