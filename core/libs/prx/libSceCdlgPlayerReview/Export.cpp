#include <atomic>
#include <cstdint>
#include <cstring>
#include "prx/libc/include/General.hpp"

namespace {

constexpr std::int32_t StatusNone = 0;
constexpr std::int32_t StatusInitialized = 1;
constexpr std::int32_t StatusRunning = 2;
constexpr std::int32_t StatusFinished = 3;
constexpr std::int32_t ErrorArgumentNull = static_cast<std::int32_t>(0x80B8000Du);

std::atomic<std::int32_t> g_status{StatusNone};

}

extern "C" {

std::int32_t APS5_VABI scePlayerReviewDialogInitialize() {
    g_status.store(StatusInitialized, std::memory_order_release);
    return 0;
}

std::int32_t APS5_VABI scePlayerReviewDialogOpen(const void* param) {
    (void)param;
    // There is no PlayStation shell review surface on the host. Completing
    // immediately is the offline/no-review path and avoids guest poll loops.
    g_status.store(StatusFinished, std::memory_order_release);
    return 0;
}

std::int32_t APS5_VABI scePlayerReviewDialogUpdateStatus() {
    return g_status.load(std::memory_order_acquire);
}

std::int32_t APS5_VABI scePlayerReviewDialogGetStatus() {
    return g_status.load(std::memory_order_acquire);
}

std::int32_t APS5_VABI scePlayerReviewDialogGetResult(void* result) {
    if (!result) return ErrorArgumentNull;
    // Public reverse-engineering sources agree on an 8-byte result block for
    // the currently observed ABI. Zero means no review action was submitted.
    std::memset(result, 0, 8);
    return 0;
}

std::int32_t APS5_VABI scePlayerReviewDialogClose() {
    if (g_status.load(std::memory_order_acquire) != StatusNone)
        g_status.store(StatusFinished, std::memory_order_release);
    return 0;
}

std::int32_t APS5_VABI scePlayerReviewDialogTerminate() {
    g_status.store(StatusNone, std::memory_order_release);
    return 0;
}

}
