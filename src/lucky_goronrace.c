#include "modding.h"
#include "global.h"
#include "recomputils.h"
#include "recompconfig.h"
#include "magemods_globals.h"
#include "overlays/actors/ovl_En_Mt_tag/z_en_mt_tag.h"
#include "overlays/actors/ovl_En_Rg/z_en_rg.h"

static EnMttag* enMttag = NULL;

static void nerf_gorons() {
    EnRg* goronPtr;
    for (s32 i = 1; i < ARRAY_COUNT(enMttag->raceGorons) + 1; i++) {
        goronPtr = enMttag->raceGorons[i - 1];

        // If numCheckpointsAheadOfPlayer is -2, there is very strong rubberbanding, so clamp to -1
        goronPtr->numCheckpointsAheadOfPlayer = CLAMP_MIN(goronPtr->numCheckpointsAheadOfPlayer, -1);

        // Remove AT flag so gorons cannot hit Link
        goronPtr->collider2.base.atFlags = AT_NONE;
    }
}

RECOMP_HOOK("EnMttag_Init") void on_EnMttag_Init(Actor* thisx) {
    enMttag = (EnMttag*)thisx;
}

RECOMP_HOOK_RETURN("EnMttag_UpdateCheckpoints") void after_EnMttag_UpdateCheckpoints() {
    if (!get_config_bool("lucky_goronrace")) {
        return;
    }
    nerf_gorons();
}
