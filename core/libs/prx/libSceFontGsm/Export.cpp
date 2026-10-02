// Compatibility provider generated from the public PS5 NID catalog.
// The module is loadable, but unverified calls fail fast instead of returning fake success.
#include <cstdint>
#include "prx/libc/include/General.hpp"

extern "C" {

APS5_EXPORT("6zxuBlsGF0I", aps5CompatStub_0);
std::int64_t APS5_VABI aps5CompatStub_0(...) {
    NotImplemented_nid_no_patch("sceFontGraphicsSequenceGetAgcCommandsForJump");
    return -1;
}

APS5_EXPORT("CeSI7C4ihjo", aps5CompatStub_1);
std::int64_t APS5_VABI aps5CompatStub_1(...) {
    NotImplemented_nid_no_patch("sceFontGraphicsSequenceGetAgcCommandsForSubmit");
    return -1;
}

APS5_EXPORT("PwZEP8ZBK2Y", aps5CompatStub_2);
std::int64_t APS5_VABI aps5CompatStub_2(...) {
    NotImplemented_nid_no_patch("sceFontGraphicsAgcDrawupFillTextureImage");
    return -1;
}

APS5_EXPORT("XKfh4naaANE", aps5CompatStub_3);
std::int64_t APS5_VABI aps5CompatStub_3(...) {
    NotImplemented_nid_no_patch("sceFontSelectGraphicsAgc");
    return -1;
}

APS5_EXPORT("qEGeDXvL3fc", aps5CompatStub_4);
std::int64_t APS5_VABI aps5CompatStub_4(...) {
    NotImplemented_nid_no_patch("sceFontGraphicsAgcSurfaceInit");
    return -1;
}

APS5_EXPORT("z-m061OoEqQ", aps5CompatStub_5);
std::int64_t APS5_VABI aps5CompatStub_5(...) {
    NotImplemented_nid_no_patch("sceFontGraphicsAgcDrawupFillTexturePattern");
    return -1;
}

}
