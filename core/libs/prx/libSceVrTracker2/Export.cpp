// Compatibility provider generated from the public PS5 NID catalog.
// The module is loadable, but unverified calls fail fast instead of returning fake success.
#include <cstdint>
#include "prx/libc/include/General.hpp"

extern "C" {

APS5_EXPORT("6Jy73SRfG-o", aps5CompatStub_0);
std::int64_t APS5_VABI aps5CompatStub_0(...) {
    NotImplemented_nid_no_patch("sceVrTracker2Initialize");
    return -1;
}

APS5_EXPORT("DSTergmOvvE", aps5CompatStub_1);
std::int64_t APS5_VABI aps5CompatStub_1(...) {
    NotImplemented_nid_no_patch("sceVrTracker2CheckDeviceIsInsidePlayAreaBoundary");
    return -1;
}

APS5_EXPORT("Dog+g25QYjw", aps5CompatStub_2);
std::int64_t APS5_VABI aps5CompatStub_2(...) {
    NotImplemented_nid_no_patch("sceVrTracker2RegisterDevice");
    return -1;
}

APS5_EXPORT("HDlD-xE1Xuk", aps5CompatStub_3);
std::int64_t APS5_VABI aps5CompatStub_3(...) {
    NotImplemented_nid_no_patch("sceVrTracker2ResetLocalCoordinateWithPose");
    return -1;
}

APS5_EXPORT("IGdE6nsMTAY", aps5CompatStub_4);
std::int64_t APS5_VABI aps5CompatStub_4(...) {
    NotImplemented_nid_no_patch("sceVrTracker2CheckPointIsInsidePlayAreaBoundary");
    return -1;
}

APS5_EXPORT("IQ3UD6SZbXo", aps5CompatStub_5);
std::int64_t APS5_VABI aps5CompatStub_5(...) {
    NotImplemented_nid_no_patch("sceVrTracker2Finalize");
    return -1;
}

APS5_EXPORT("IdI2f+xHIeA", aps5CompatStub_6);
std::int64_t APS5_VABI aps5CompatStub_6(...) {
    NotImplemented_nid_no_patch("sceVrTracker2ResetLocalCoordinate");
    return -1;
}

APS5_EXPORT("J4Vh3VVX0iU", aps5CompatStub_7);
std::int64_t APS5_VABI aps5CompatStub_7(...) {
    NotImplemented_nid_no_patch("sceVrTracker2GetResult");
    return -1;
}

APS5_EXPORT("SCph4ZbkqzU", aps5CompatStub_8);
std::int64_t APS5_VABI aps5CompatStub_8(...) {
    NotImplemented_nid_no_patch("sceVrTracker2GetPlayAreaBoundaryGeometry");
    return -1;
}

APS5_EXPORT("TwqZnaIjWv4", aps5CompatStub_9);
std::int64_t APS5_VABI aps5CompatStub_9(...) {
    NotImplemented_nid_no_patch("sceVrTracker2QueryMemory");
    return -1;
}

APS5_EXPORT("UVCMLmS-Eas", aps5CompatStub_10);
std::int64_t APS5_VABI aps5CompatStub_10(...) {
    NotImplemented_nid_no_patch("sceVrTracker2SetCoordinateSystem");
    return -1;
}

APS5_EXPORT("Y-3JCiU9bbU", aps5CompatStub_11);
std::int64_t APS5_VABI aps5CompatStub_11(...) {
    NotImplemented_nid_no_patch("sceVrTracker2GetCoordinateSystem");
    return -1;
}

APS5_EXPORT("f7G97dWnEis", aps5CompatStub_12);
std::int64_t APS5_VABI aps5CompatStub_12(...) {
    NotImplemented_nid_no_patch("sceVrTracker2LocateCoordinateSystem");
    return -1;
}

APS5_EXPORT("kFt4MB3SUEk", aps5CompatStub_13);
std::int64_t APS5_VABI aps5CompatStub_13(...) {
    NotImplemented_nid_no_patch("sceVrTracker2UnregisterDevice");
    return -1;
}

APS5_EXPORT("snYs7Nf-RKk", aps5CompatStub_14);
std::int64_t APS5_VABI aps5CompatStub_14(...) {
    NotImplemented_nid_no_patch("sceVrTracker2GetPlayAreaOrientedBoundingBox");
    return -1;
}

}
