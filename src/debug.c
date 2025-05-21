#include "modding.h"
#include "global.h"
#include "recomputils.h"
#include "recompconfig.h"
#include "magemods_globals.h"

/*
RECOMP_HOOK_RETURN("Sram_OpenSave") void after_Sram_OpenSave() {
    recomp_printf("Bombers Code: ");
    for (s8 i = 0; i < 5; i++) {
        recomp_printf("%d", gSaveContext.save.saveInfo.bomberCode[i]);
    }
    recomp_printf("\n");

    for (s8 i = 0; i < 3; i++) {
        recomp_printf("Lottery %d: ", i+1);
        for (s8 j = 0; j < 3; j++) {
            recomp_printf("%d", gSaveContext.save.saveInfo.lotteryCodes[i][j]);
        }
        recomp_printf("\n");
    }

    recomp_printf("Spider House Code: ");
    for (s8 i = 0; i < 6; i++) {
        recomp_printf("%d", gSaveContext.save.saveInfo.spiderHouseMaskOrder[i]);
    }
    recomp_printf("\n");

    // Skip goron cutscene
    SET_WEEKEVENTREG(WEEKEVENTREG_12_02);
}
//*/
