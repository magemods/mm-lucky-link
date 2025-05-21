#include "modding.h"
#include "global.h"
#include "recomputils.h"
#include "recompconfig.h"
#include "magemods_globals.h"
#include "overlays/actors/ovl_En_Aob_01/z_en_aob_01.h"
#include "overlays/actors/ovl_En_Dg/z_en_dg.h"
#include "overlays/actors/ovl_En_Racedog/z_en_racedog.h"

static DogColor racedogColors[] = {
    DOG_COLOR_BEIGE, DOG_COLOR_WHITE, DOG_COLOR_BLUE, DOG_COLOR_GRAY, DOG_COLOR_BROWN,
    DOG_COLOR_GRAY, DOG_COLOR_BEIGE, DOG_COLOR_WHITE, DOG_COLOR_WHITE, DOG_COLOR_GOLD,
    DOG_COLOR_GRAY, DOG_COLOR_BEIGE, DOG_COLOR_WHITE, DOG_COLOR_BROWN,
};

typedef struct {
    EnRacedog* dogSelected;
    EnRacedog* dogGold;
    EnRacedog* dogBlue;
    s16 furthestPoint[RACEDOG_COUNT];
    f32 targetSpeed[RACEDOG_COUNT];
    f32 averageSpeed;
    f32 maxSpeed;
    s8 currentPlace;
} RaceDogStats;

static RaceDogStats racedog_collect_stats() {
    RaceDogStats stats = { NULL, NULL, NULL, {-1}, {0}, 0, 0, 1 };
    u8 i;
    Actor* actorPtr = NULL;
    EnRacedog* dogPtr = NULL;

    actorPtr = SubS_FindActor(gPlayState, actorPtr, ACTORCAT_NPC, ACTOR_EN_RACEDOG);
    while (actorPtr != NULL) {
        dogPtr = (EnRacedog*)actorPtr;
        i = dogPtr->index;

        if (i == dogPtr->selectedDogIndex) {
            stats.dogSelected = dogPtr;
        }
        if (racedogColors[i] == DOG_COLOR_GOLD) {
            stats.dogGold = dogPtr;
        }
        if (racedogColors[i] == DOG_COLOR_BLUE) {
            stats.dogBlue = dogPtr;
        }

        stats.furthestPoint[i] = dogPtr->currentPoint;
        stats.targetSpeed[i] = dogPtr->targetSpeed;

        actorPtr = actorPtr->next;
    }

    if (stats.dogSelected == NULL) {
        return stats;
    }

    for (i = 0; i < RACEDOG_COUNT; i++) {
        if (i == stats.dogSelected->index) {
            continue;
        }
        stats.averageSpeed += stats.targetSpeed[i];
        if (stats.targetSpeed[i] > stats.maxSpeed) {
            stats.maxSpeed = stats.targetSpeed[i];
        }
        if (stats.furthestPoint[i] >= stats.dogSelected->currentPoint) {
            stats.currentPlace++;
        }
    }
    stats.averageSpeed /= (RACEDOG_COUNT - 1);

    return stats;
}

static void racedog_update_speed() {
    RaceDogStats stats = racedog_collect_stats();
    EnRacedog* this = stats.dogSelected;

    if (this == NULL || this->raceStatus == RACEDOG_RACE_STATUS_FINISHED) {
        return;
    }

    f32 pct = (float)this->currentPoint / (float)this->path->count;

    // Do nothing for the first part of race
    if (pct < 0.25f) {
        return;
    }

    // Determine the place our dog should currently be at
    s8 targetPlace = CLAMP_MIN(6 - 5.0f * pct, 1);

    // If we are behind our target place, calculate our sprint speed multiplier
    // 0.1 for each place behind, plus up to 0.3 when closer to the end
    f32 sprintSpeedMultiplier = CLAMP(1.0f + (stats.currentPlace - targetPlace + pct * 3.0f) / 10.0f, 1.0f, 1.5f);

    // Apply speed boost if we are below targeted place
    if (stats.currentPlace > targetPlace) {
        this->targetSpeed = stats.maxSpeed * sprintSpeedMultiplier;
    } else {
        this->targetSpeed = stats.averageSpeed;
    }

    Math_ApproachF(&this->actor.speed, this->targetSpeed, 0.5f, 3.0f);

    // Only cap speed if we are not in first
    if (stats.currentPlace == 1 && this->actor.speed > 7.5f) {
        this->actor.speed = 7.5f;
    }

    // Slow down golden dog if not chosen
    if (stats.dogGold != this) {
        stats.dogGold->targetSpeed = stats.averageSpeed;
        Math_ApproachF(&stats.dogGold->actor.speed, stats.dogGold->targetSpeed, 0.5f, 3.0f);
    }
}

RECOMP_HOOK_RETURN("EnRacedog_UpdateSpeed") void after_EnRacedog_UpdateSpeed() {
    if (!get_config_bool("lucky_dograce")) {
        return;
    }

    static u8 i = 0;
    if (++i == RACEDOG_COUNT) {
        i = 0;
        racedog_update_speed();
    }
}
