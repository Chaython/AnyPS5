#include <cstdint>

extern "C" {
std::int32_t scePlayerReviewDialogInitialize();
std::int32_t scePlayerReviewDialogOpen(const void*);
std::int32_t scePlayerReviewDialogUpdateStatus();
std::int32_t scePlayerReviewDialogGetStatus();
std::int32_t scePlayerReviewDialogGetResult(void*);
std::int32_t scePlayerReviewDialogClose();
std::int32_t scePlayerReviewDialogTerminate();
}

int main() {
    if (scePlayerReviewDialogGetStatus() != 0) return 1;
    if (scePlayerReviewDialogInitialize() != 0) return 2;
    if (scePlayerReviewDialogUpdateStatus() != 1) return 3;
    if (scePlayerReviewDialogOpen(nullptr) != 0) return 4;
    if (scePlayerReviewDialogGetStatus() != 3) return 5;

    unsigned char result[8];
    for (auto& byte : result) byte = 0xff;
    if (scePlayerReviewDialogGetResult(result) != 0) return 6;
    for (auto byte : result) if (byte != 0) return 7;

    if (scePlayerReviewDialogClose() != 0) return 8;
    if (scePlayerReviewDialogTerminate() != 0) return 9;
    if (scePlayerReviewDialogGetStatus() != 0) return 10;
    return 0;
}
