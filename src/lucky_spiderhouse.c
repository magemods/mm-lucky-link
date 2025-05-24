#include "modding.h"
#include "global.h"
#include "recomputils.h"
#include "recompconfig.h"
#include "magemods_globals.h"
#include "overlays/actors/ovl_Bg_Kin2_Fence/z_bg_kin2_fence.h"

static s8 origSpiderHouseMaskOrder[6];

static void store_mask_order() {
    for (s8 i = 0; i < 6; i++) {
        origSpiderHouseMaskOrder[i] = gSaveContext.save.saveInfo.spiderHouseMaskOrder[i];
    }
}

static void restore_mask_order() {
    for (s8 i = 0; i < 6; i++) {
        gSaveContext.save.saveInfo.spiderHouseMaskOrder[i] = origSpiderHouseMaskOrder[i];
    }
}

static void set_next_mask_hit(BgKin2Fence* this) {
    for (s8 i = 0; i < ARRAY_COUNT(this->colliderElements); i++) {
        if (this->collider.elements[i].base.acElemFlags & ACELEM_HIT) {
            gSaveContext.save.saveInfo.spiderHouseMaskOrder[this->masksHit] = i;
        }
    }
}

RECOMP_HOOK_RETURN("Sram_OpenSave") void after_Sram_OpenSave_spiderhouse() {
    store_mask_order();
}

RECOMP_HOOK("BgKin2Fence_HandleMaskCode") void on_BgKin2Fence_HandleMaskCode(BgKin2Fence* this) {
    if (!get_config_bool("lucky_spiderhouse")) {
        return;
    }
    if (this->collider.base.acFlags & AC_HIT) {
        set_next_mask_hit(this);
    }
}

RECOMP_HOOK_RETURN("BgKin2Fence_HandleMaskCode") void after_BgKin2Fence_HandleMaskCode() {
    restore_mask_order();
}
