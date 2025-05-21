#include "modding.h"
#include "global.h"
#include "recomputils.h"
#include "recompconfig.h"
#include "magemods_globals.h"
#include "overlays/actors/ovl_En_Bombers2/z_en_bombers2.h"

static s8 origBomberCode[5];

static void store_bombers_code() {
    for (s8 i = 0; i < 6; i++) {
        origBomberCode[i] = gSaveContext.save.saveInfo.bomberCode[i];
    }
}

static void restore_bombers_code() {
    for (s8 i = 0; i < 5; i++) {
        gSaveContext.save.saveInfo.bomberCode[i] = origBomberCode[i];
    }
}

static void set_bombers_code() {
    for (s8 i = 0; i < 5; i++) {
        gSaveContext.save.saveInfo.bomberCode[i] = i + 1;
    }
}

RECOMP_HOOK_RETURN("Sram_OpenSave") void after_Sram_OpenSave_bombers() {
    store_bombers_code();
}

RECOMP_HOOK("func_809C4BC4") void on_func_809C4BC4() {
    if (!get_config_bool("lucky_bombers")) {
        return;
    }
    set_bombers_code();
}

RECOMP_HOOK("Message_BombersNotebookQueueEvent") void on_Message_BombersNotebookQueueEvent(int, u8 event) {
    if (!get_config_bool("lucky_bombers")) {
        return;
    }
    if (event != BOMBERS_NOTEBOOK_EVENT_LEARNED_SECRET_CODE) {
        return;
    }
    restore_bombers_code();
}

RECOMP_HOOK("func_80C04D8C") void on_func_80C04D8C(EnBombers2* this) {
    if (!get_config_bool("lucky_bombers")) {
        return;
    }
    if (this->talkState != TEXT_STATE_INPUT_BOMBER_CODE || !Message_ShouldAdvance(gPlayState)) {
        return;
    }
    // Make any entered code correct
    for (s8 i = 0; i < ARRAY_COUNT(this->correctDigitSlots); i++) {
        gPlayState->msgCtx.unk12054[i] = gSaveContext.save.saveInfo.bomberCode[i];
    }
}
