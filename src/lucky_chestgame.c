#include "modding.h"
#include "global.h"
#include "recomputils.h"
#include "recompconfig.h"
#include "magemods_globals.h"
#include "overlays/actors/ovl_Obj_Takaraya_Wall/z_obj_takaraya_wall.h"

#define TAKARAYA_WALL_ROWS 11
#define TAKARAYA_WALL_COLUMNS 8

typedef struct ObjTakarayaWallCell {
    u8 row;
    u8 column;
} ObjTakarayaWallCell;

extern ObjTakarayaWallCell sTakarayaWallPathStack[TAKARAYA_WALL_ROWS * TAKARAYA_WALL_COLUMNS];
extern f32 sTakarayaWallHeights[TAKARAYA_WALL_ROWS][TAKARAYA_WALL_COLUMNS];
extern s32 ObjTakarayaWall_PosToCell(Vec3f* pos, s32* row, s32* column);
extern s32 ObjTakarayaWall_IsCellOpen(s32 row, s32 column);

static s32 secretTimer = 0;
static bool secretOpened = false;

static void carve_easy_path() {
    ObjTakarayaWallCell chestCell = sTakarayaWallPathStack[0];
    ObjTakarayaWallCell curCell = {10, 6}; // Our starting cell
    ObjTakarayaWallCell nextCell[3]; // Array containing possible next cells
    s32 i, j, randNum;

    // Reset all cells
    for (i = 0; i < TAKARAYA_WALL_ROWS; i++) {
        for (j = 0; j < TAKARAYA_WALL_COLUMNS; j++) {
            sTakarayaWallHeights[i][j] = 0.0f;
        }
    }

    // Open chest cell, starting cell, adjacent starting cell, and secret cell
    sTakarayaWallHeights[chestCell.row][chestCell.column] = -10.0f;
    sTakarayaWallHeights[curCell.row][curCell.column] = -10.0f;
    sTakarayaWallHeights[10][5] = -10.0f;
    sTakarayaWallHeights[10][7] = -10.0f;

    do {
        // Reset possible next cells
        nextCell[0] = (ObjTakarayaWallCell){255, 255};
        nextCell[1] = (ObjTakarayaWallCell){255, 255};
        nextCell[2] = (ObjTakarayaWallCell){255, 255};

        // Check if we can move right
        if (curCell.column > 0 && curCell.row < TAKARAYA_WALL_ROWS - 1 &&
            !ObjTakarayaWall_IsCellOpen(curCell.row, curCell.column - 1) &&
            !ObjTakarayaWall_IsCellOpen(curCell.row - 1, curCell.column - 1) &&
            !ObjTakarayaWall_IsCellOpen(curCell.row + 1, curCell.column - 1)) {
            nextCell[1] = (ObjTakarayaWallCell){curCell.row, curCell.column - 1};
        }

        // Check if we can move left
        if (curCell.column < TAKARAYA_WALL_COLUMNS - 1 && curCell.row < TAKARAYA_WALL_ROWS - 1 &&
            !ObjTakarayaWall_IsCellOpen(curCell.row, curCell.column + 1) &&
            !ObjTakarayaWall_IsCellOpen(curCell.row - 1, curCell.column + 1) &&
            !ObjTakarayaWall_IsCellOpen(curCell.row + 1, curCell.column + 1)) {
            nextCell[2] = (ObjTakarayaWallCell){curCell.row, curCell.column + 1};
        }

        // Check if we can move straight
        if (curCell.row > 0 &&
            (!ObjTakarayaWall_IsCellOpen(curCell.row + 1, curCell.column) ||
             !ObjTakarayaWall_IsCellOpen(curCell.row + 2, curCell.column))) {
            nextCell[0] = (ObjTakarayaWallCell){curCell.row - 1, curCell.column};
        }

        // Once at back row, go directly to chest
        if (curCell.row == 0) {
            if (curCell.column > chestCell.column) {
                nextCell[1] = (ObjTakarayaWallCell){curCell.row, curCell.column - 1};
            } else {
                nextCell[2] = (ObjTakarayaWallCell){curCell.row, curCell.column + 1};
            }
        }

        // If all directions are null, force move straight
        if (nextCell[0].row == 255 && nextCell[1].row == 255 && nextCell[2].row == 255) {
            nextCell[0] = (ObjTakarayaWallCell){curCell.row - 1, curCell.column};
        }

        // Pick a cell at random, straight will have higher probability
        do {
            randNum = Rand_S16Offset(0, 5) % ARRAY_COUNT(nextCell);
        } while(nextCell[randNum].row == 255);

        // Open the current cell
        sTakarayaWallHeights[curCell.row][curCell.column] = -10.0f;
        curCell = nextCell[randNum];

    } while (curCell.row != chestCell.row || curCell.column != chestCell.column);
}

static void check_for_secret() {
    Player* player = GET_PLAYER(gPlayState);
    s32 playerRow;
    s32 playerColumn;
    s32 playerCellXPos;
    s8 playerOnSecretSpot = false;
    s32 i, j;

    ObjTakarayaWall_PosToCell(&player->actor.world.pos, &playerRow, &playerColumn);
    playerCellXPos = (((s32)player->actor.world.pos.x - -1800) % 120) - 60;
    playerOnSecretSpot = playerRow == 10 && playerColumn == 7 && playerCellXPos < -20;

    if (secretTimer == 60) {
        if (sTakarayaWallHeights[9][7] > 0) {
            Math_StepToF(&sTakarayaWallHeights[9][7], 0.0f, playerOnSecretSpot ? 30.0f : 15.0f);
        }
        if (sTakarayaWallHeights[9][7] <= 0) {
            secretOpened = true;
            for (i = 0; i < TAKARAYA_WALL_ROWS; i++) {
                sTakarayaWallHeights[i][TAKARAYA_WALL_COLUMNS - 1] = -10.0f;
            }
            for (j = 0; j < TAKARAYA_WALL_COLUMNS; j++) {
                sTakarayaWallHeights[0][j] = -10.0f;
            }
        }
    } else if (playerOnSecretSpot) {
        if (++secretTimer == 60) {
            Audio_PlaySfx(NA_SE_SY_CORRECT_CHIME);
        }
    } else {
        secretTimer = 0;
    }
}

RECOMP_HOOK_RETURN("ObjTakarayaWall_Init") void after_ObjTakarayaWall_Init() {
    if (!get_config_bool("lucky_chestgame")) {
        return;
    }
    secretTimer = 0;
    secretOpened = false;
    carve_easy_path();
}

RECOMP_HOOK("ObjTakarayaWall_Manage") void on_ObjTakarayaWall_Manage() {
    if (!get_config_bool("lucky_chestgame")) {
        return;
    }
    if (!secretOpened) {
        check_for_secret();
    }
}
