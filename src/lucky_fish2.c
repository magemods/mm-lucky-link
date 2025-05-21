#include "modding.h"
#include "global.h"
#include "recomputils.h"
#include "recompconfig.h"
#include "magemods_globals.h"
#include "overlays/actors/ovl_En_Fish2/z_en_fish2.h"

extern s32 func_80B28478(EnFish2* this);
extern void func_80B287F4(EnFish2* this, s32 arg1);
extern void func_80B289DC(EnFish2* this, PlayState* play);

static f32 D_80B2B370_4 = 0.019f;
static EnFish2* fishPtrs[2] = {NULL};

static void loser_fish_action(EnFish2* this, PlayState* play) {
    if (!func_80B28478(this)) {
        Math_ApproachF(&this->actor.speed, (D_80B2B370_4 - this->unk_330) * 1000.0f, 0.3f, 0.3f);

        if (this->actor.speed > 1.0f) {
            this->actor.speed = 1.0f;
        } else if (this->actor.speed < 0.5f) {
            this->actor.speed = 0.5f;
        }

        func_80B287F4(this, false);
        func_80B289DC(this, play);
    }
}

RECOMP_HOOK("EnFish2_Update") void on_EnFish2_Update(Actor* thisx) {
    if (!get_config_bool("lucky_fish2")) {
        return;
    }

    EnFish2* this = (EnFish2*)thisx;
    if (this->unk_344 < 2) {
        fishPtrs[this->unk_344] = this;
    }
}

RECOMP_HOOK("func_80B29250") void on_func_80B29250(EnFish2* this) {
    if (!get_config_bool("lucky_fish2")) {
        return;
    }

    EnFish2* otherFish = fishPtrs[(this->unk_344 + 1) % 2];
    if (otherFish != NULL && this->unk_2C0 < otherFish->unk_2C0) {
        this->actionFunc = loser_fish_action;
    }
}
