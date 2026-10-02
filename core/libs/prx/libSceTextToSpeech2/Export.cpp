// Compatibility provider generated from the public PS5 NID catalog.
// The module is loadable, but unverified calls fail fast instead of returning fake success.
#include <cstdint>
#include "prx/libc/include/General.hpp"

extern "C" {

APS5_EXPORT("+352WTlGCQI", aps5CompatStub_0);
std::int64_t APS5_VABI aps5CompatStub_0(...) {
    NotImplemented_nid_no_patch("sceTextToSpeech2GetSystemStatus");
    return -1;
}

APS5_EXPORT("08JSg9p6bgQ", aps5CompatStub_1);
std::int64_t APS5_VABI aps5CompatStub_1(...) {
    NotImplemented_nid_no_patch("sceTextToSpeech2GetSpeechStatus");
    return -1;
}

APS5_EXPORT("2jiIxUmcsGo", aps5CompatStub_2);
std::int64_t APS5_VABI aps5CompatStub_2(...) {
    NotImplemented_nid_no_patch("sceTextToSpeech2Cancel");
    return -1;
}

APS5_EXPORT("8ntsRd07EQA", aps5CompatStub_3);
std::int64_t APS5_VABI aps5CompatStub_3(...) {
    NotImplemented_nid_no_patch("sceTextToSpeech2Speak");
    return -1;
}

APS5_EXPORT("LazJT1ZrQys", aps5CompatStub_4);
std::int64_t APS5_VABI aps5CompatStub_4(...) {
    NotImplemented_nid_no_patch("sceTextToSpeech2RegisterTextConversionItem");
    return -1;
}

APS5_EXPORT("SoWHuVW0gpU", aps5CompatStub_5);
std::int64_t APS5_VABI aps5CompatStub_5(...) {
    NotImplemented_nid_no_patch("sceTextToSpeech2Terminate");
    return -1;
}

APS5_EXPORT("UOjiprYwVNw", aps5CompatStub_6);
std::int64_t APS5_VABI aps5CompatStub_6(...) {
    NotImplemented_nid_no_patch("sceTextToSpeech2Initialize");
    return -1;
}

APS5_EXPORT("X0HZNbSiqyg", aps5CompatStub_7);
std::int64_t APS5_VABI aps5CompatStub_7(...) {
    NotImplemented_nid_no_patch("sceTextToSpeech2Open");
    return -1;
}

APS5_EXPORT("t4e879M-cSw", aps5CompatStub_8);
std::int64_t APS5_VABI aps5CompatStub_8(...) {
    NotImplemented_nid_no_patch("sceTextToSpeech2Close");
    return -1;
}

}
