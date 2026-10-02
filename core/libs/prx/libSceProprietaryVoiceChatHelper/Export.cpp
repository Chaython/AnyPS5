// Compatibility provider generated from the public PS5 NID catalog.
// The module is loadable, but unverified calls fail fast instead of returning fake success.
#include <cstdint>
#include "prx/libc/include/General.hpp"

extern "C" {

APS5_EXPORT("Asuucx2vvaE", aps5CompatStub_0);
std::int64_t APS5_VABI aps5CompatStub_0(...) {
    NotImplemented_nid_no_patch("sceProprietaryVoiceChatHelperGetVoiceChatUsageState");
    return -1;
}

APS5_EXPORT("LDjk9ULlN34", aps5CompatStub_1);
std::int64_t APS5_VABI aps5CompatStub_1(...) {
    NotImplemented_nid_no_patch("sceProprietaryVoiceChatHelperInitialize");
    return -1;
}

APS5_EXPORT("QJy6V9QAXXE", aps5CompatStub_2);
std::int64_t APS5_VABI aps5CompatStub_2(...) {
    NotImplemented_nid_no_patch("sceProprietaryVoiceChatHelperSetVoiceChatState");
    return -1;
}

APS5_EXPORT("lrswogWNZyM", aps5CompatStub_3);
std::int64_t APS5_VABI aps5CompatStub_3(...) {
    NotImplemented_nid_no_patch("sceProprietaryVoiceChatHelperTerminate");
    return -1;
}

}
