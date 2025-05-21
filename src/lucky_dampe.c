#include "modding.h"
#include "global.h"
#include "recomputils.h"
#include "recompconfig.h"
#include "magemods_globals.h"
#include "overlays/actors/ovl_En_Bigpo/z_en_bigpo.h"
#include "overlays/actors/ovl_En_Tk/z_en_tk.h"

// Coordinates of all dirt patches, last two normally never spawn flames
static Vec3f dirtPatches[6] = {{ 320.0f, 7.0f, 361.0f }, { -320.0f, 247.0f, 360.0f },
                               { 500.0f, 207.0f, -360.0f }, { 0.0f, 8.0f, 161.0f },
                               { 244.0f, 7.0f, 18.0f }, { -244.0f, 7.0f, 18.0f }};

static void ensure_dampe_digs_flame(EnTk* this) {
    Actor* flamePtr = NULL;
    Actor* enemyPtr = NULL;

    // Loop through all "big poe" actors and find only the flames that are still hidden
    enemyPtr = SubS_FindActor(gPlayState, enemyPtr, ACTORCAT_PROP, ACTOR_EN_BIGPO);
    while (enemyPtr != NULL) {
        if (enemyPtr->params == BIG_POE_TYPE_CHOSEN_FIRE) {
            // This flame is still hidden, store a reference
            flamePtr = enemyPtr;

            // If this flame is near Dampe, he will find it so exit
            if (Actor_WorldDistXYZToActor(&this->actor, flamePtr) < 80.0f) {
                return;
            }
        }
        enemyPtr = enemyPtr->next;
    }

    // Sanity check, this shouldn't happen
    if (flamePtr == NULL) {
        return;
    }

    // Loop through all dirt patches, find the one closest to Dampe, and move hidden flame
    for (u8 i = 0; i < 6; i++) {
        if (Actor_WorldDistXYZToPoint(&this->actor, &dirtPatches[i]) < 80.0f) {
            Math_Vec3f_Copy(&flamePtr->world.pos, &dirtPatches[i]);
            return;
        }
    }
}

RECOMP_HOOK("func_80AEE4D0") void on_func_80AEE4D0(EnTk* this) {
    if (!get_config_bool("lucky_dampe")) {
        return;
    }
    if ((this->unk_2CA & 0x20) || !Animation_OnFrame(&this->skelAnime, 37.0f)) {
        return;
    }

    ensure_dampe_digs_flame(this);
}
