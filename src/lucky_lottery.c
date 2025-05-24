#include "modding.h"
#include "global.h"
#include "recomputils.h"
#include "recompconfig.h"
#include "magemods_globals.h"
#include "overlays/actors/ovl_En_Kujiya/z_en_kujiya.h"

static s8 origLotteryCode[3];

static void store_lottery_code() {
    for (s8 i = 0; i < 3; i++) {
        origLotteryCode[i] = gSaveContext.save.saveInfo.lotteryCodes[CURRENT_DAY - 1][i];
    }
}

static void restore_lottery_code() {
    for (s8 i = 0; i < 3; i++) {
        gSaveContext.save.saveInfo.lotteryCodes[CURRENT_DAY - 1][i] = origLotteryCode[i];
    }
}

static void set_lottery_code() {
    gSaveContext.save.saveInfo.lotteryCodes[CURRENT_DAY - 1][0] = (HS_GET_LOTTERY_CODE_GUESS() & 0xF00) >> 8;
    gSaveContext.save.saveInfo.lotteryCodes[CURRENT_DAY - 1][1] = (HS_GET_LOTTERY_CODE_GUESS() & 0xF0) >> 4;
    gSaveContext.save.saveInfo.lotteryCodes[CURRENT_DAY - 1][2] = (HS_GET_LOTTERY_CODE_GUESS() & 0xF);
}

RECOMP_HOOK_RETURN("Sram_OpenSave") void after_Sram_OpenSave_lottery() {
    store_lottery_code();
}

RECOMP_HOOK("EnKujiya_UnsetBoughtTicket") void on_EnKujiya_UnsetBoughtTicket() {
    if (!get_config_bool("lucky_lottery")) {
        return;
    }
    set_lottery_code();
}

RECOMP_HOOK("EnKujiya_GivePrize") void on_EnKujiya_GivePrize() {
    restore_lottery_code();
}
