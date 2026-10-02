#include "prx/libSceVrSetupDialog/VrSetupDialog.hpp"

int main() {
    if (sceVrSetupDialogGetStatus() !=
        static_cast<std::int32_t>(VrSetupDialog::Status::None)) return 1;
    if (sceVrSetupDialogInitialize() != VrSetupDialog::Ok) return 2;
    if (sceVrSetupDialogUpdateStatus() !=
        static_cast<std::int32_t>(VrSetupDialog::Status::Initialized)) return 3;
    if (sceVrSetupDialogOpen(nullptr) != VrSetupDialog::Ok) return 4;
    if (sceVrSetupDialogGetStatus() !=
        static_cast<std::int32_t>(VrSetupDialog::Status::Finished)) return 5;
    if (sceVrSetupDialogGetResult(nullptr) != VrSetupDialog::Ok) return 6;
    if (sceVrSetupDialogClose() != VrSetupDialog::Ok) return 7;
    if (sceVrSetupDialogGetStatus() !=
        static_cast<std::int32_t>(VrSetupDialog::Status::None)) return 8;
    if (sceVrSetupDialogTerminate() != VrSetupDialog::Ok) return 9;
    return 0;
}
