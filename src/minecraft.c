// minecraft.c — entry point, window+gl init, camera, picking, main loop

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include <float.h>

#include "common.h"

#include "level/level.h"
#include "level/level_renderer.h"
#include "renderer/frustum.h"
#include "level/tile/tile.h"
#include "renderer/textures.h"
#include "particle/particle_engine.h"
#include "player.h"
#include "character/zombie.h"
#include "timer.h"
#include "hitresult.h"
#include "gui/font.h"

#include "dsp_renderer.h"

#define MAX_MOBS 100

static Level      level;
static LevelRenderer  levelRenderer;
static Player         player;
static Timer          timer;
static Zombie         mobs[MAX_MOBS];
static ParticleEngine particleEngine;

static int mobCount = 0;

static int prevLeft  = 0;
static int prevRight = 0;
static int prevEnter = 0;
static int prevNum1 = 0, prevNum2 = 0;
static int prevNum3 = 0, prevNum4 = 0;
static int prevNum6 = 0;
static int prevG    = 0;
static int prevY    = 0;
bool isMouseLocked = true;

static int gEditMode = 0;              // 0=destroy, 1=place
static int gYMouseAxis = 1;            // toggled by Y key (1 or -1)

static int texTerrain = 0;

static Tessellator hudTess;
static Font gFont;                     // HUD font
static int selectedTileId = 1;         // 1=rock, 3=dirt, 4=stoneBrick, 5=wood
static int gFPS = 0;                   // last computed fps per second
static int gChunkUpdatesPerSec = 0;    // last computed chunk updates per second
static int gChunkUpdatesAccum = 0;     // accumulates during the current second

static int gWinWidth  = 640;
static int gWinHeight = 480;
static int gIsFullscreen = 0;

static int      isHitNull = 1;
static HitResult hitResult;

static void tick(Player* player);

/* --- boot/shutdown ----------------------------------------------------------- */

static int init(Level *lvl, LevelRenderer *lr, Player *p) {
    if (!initWindow(gIsFullscreen, gWinWidth, gWinHeight))
        return 0;

    if (!initGraphics())
        return 0;

    /* Game initialization */
    Tile_registerAll();

    texTerrain = loadTexture("resources/terrain.png");
    Font_init(&gFont, "resources/default.gif");

    Level_init(lvl, 256, 256, 64);
    LevelRenderer_init(lr, lvl, (unsigned int)texTerrain);
    calcLightDepths(lvl, 0, 0, lvl->width, lvl->height);

    Player_init(p, lvl);

    ParticleEngine_init(&particleEngine, lvl, (unsigned int)texTerrain);

    mobCount = 0;
    for (int i = 0; i < 10 && i < MAX_MOBS; ++i) {
        Zombie_init(&mobs[mobCount], lvl, 128.0, 0.0, 129.0);
        Entity_resetPosition(&mobs[mobCount].base);
        mobCount++;
    }

    Timer_init(&timer, 20.0f);

    return 1;
}

static void destroy(Level* lvl) {
    Level_save(lvl);
    Level_destroy(lvl);
    shutdownWindow();
}

/* --- camera ------------------------------------------------------------------ */

static void moveCameraToPlayer(Player* p, float t) {
    setModel_positionOffset(0.0f, 0.0f, -0.3f); // eye offset

    setModel_rotation(p->e.xRotation, 0, 0);
    setModel_rotation(0,            p->e.yRotation, 0);

    double x = p->e.prevX + (p->e.x - p->e.prevX) * (double)t;
    double y = p->e.prevY + (p->e.y - p->e.prevY) * (double)t;
    double z = p->e.prevZ + (p->e.z - p->e.prevZ) * (double)t;

    setModel_positionOffset(-x, -y, -z);
}

static void setupCamera(Player* p, float t) {
    setupPerspective();
    moveCameraToPlayer(p, t);
}

static void drawGui(float partialTicks) {
    (void)partialTicks;

    int fbw, fbh;
    getWindowSize(&fbw, &fbh);
    const int baseH = 240;

    int scale = fbh / baseH;
    if (scale < 1) scale = 1;

    int screenHeight = baseH;
    int screenWidth  = fbw / scale;          

    setupGUI(screenWidth, screenHeight);
    setModel_positionOffset(0.0f, 0.0f, -200.0f);

    startModel_Matrix();
    setModel_positionOffset((double)screenWidth - 16.0, 16.0, 0.0);
    setModel_scale(16.0f);
    setModel_rotation(30.0f, 0, 0);
    setModel_rotation(0, 45.0f, 0);
    setModel_positionOffset(-1.5f, 0.5f, -0.5f);
    setModel_scalePercise(-1.0f, -1.0f, 1.0f);

    setTexture(texTerrain);

    Tessellator_init(&hudTess);
    const Tile* hand = gTiles[selectedTileId];
    if (hand && hand->render) {
        hand->render(hand, &hudTess, &level, 0, -2, 0, 0);
    }
    Tessellator_flush(&hudTess);

    endModel_Matrix();

    Font_drawShadow(&gFont, &hudTess, "0.0.11a", 2, 2, 0xFFFFFF);

    char stats[64];
    snprintf(stats, sizeof stats, "%d fps, %d chunk updates", gFPS, gChunkUpdatesPerSec);
    Font_drawShadow(&gFont, &hudTess, stats, 2, 12, 0xFFFFFF);

    int cx = screenWidth / 2;
    int cy = screenHeight / 2;

    setModel_color(1, 1, 1, 1);
    Tessellator_init(&hudTess);
    Tessellator_vertex(&hudTess, (float)(cx + 1), (float)(cy - 4), 0.0f);
    Tessellator_vertex(&hudTess, (float)(cx + 0), (float)(cy - 4), 0.0f);
    Tessellator_vertex(&hudTess, (float)(cx + 0), (float)(cy + 5), 0.0f);
    Tessellator_vertex(&hudTess, (float)(cx + 1), (float)(cy + 5), 0.0f);
    
    Tessellator_vertex(&hudTess, (float)(cx + 5), (float)(cy + 0), 0.0f);
    Tessellator_vertex(&hudTess, (float)(cx - 4), (float)(cy + 0), 0.0f);
    Tessellator_vertex(&hudTess, (float)(cx - 4), (float)(cy + 1), 0.0f);
    Tessellator_vertex(&hudTess, (float)(cx + 5), (float)(cy + 1), 0.0f);
    Tessellator_flush(&hudTess);
}

/* --- picking ----------------------------------------------------------------- */

static void get_look_dir(const Player* p, double* dx, double* dy, double* dz) {
    const double yaw   = p->e.yRotation * M_PI / 180.0;
    const double pitch = p->e.xRotation * M_PI / 180.0;
    const double cp = cos(pitch), sp = sin(pitch);
    const double cy = cos(yaw),   sy = sin(yaw);
    *dx =  sy * cp;
    *dy = -sp;
    *dz = -cy * cp;
}

static int raycast_block(const Level* lvl,
                         double ox, double oy, double oz,
                         double dx, double dy, double dz,
                         double maxDist,
                         HitResult* out) {
    int x = (int)floor(ox);
    int y = (int)floor(oy);
    int z = (int)floor(oz);

    const int stepX = (dx > 0) - (dx < 0);
    const int stepY = (dy > 0) - (dy < 0);
    const int stepZ = (dz > 0) - (dz < 0);

    double tMaxX = (dx != 0.0) ? (((stepX > 0 ? (x + 1) : x) - ox) / dx) : DBL_MAX;
    double tMaxY = (dy != 0.0) ? (((stepY > 0 ? (y + 1) : y) - oy) / dy) : DBL_MAX;
    double tMaxZ = (dz != 0.0) ? (((stepZ > 0 ? (z + 1) : z) - oz) / dz) : DBL_MAX;

    double tDeltaX = (dx != 0.0) ? fabs(1.0 / dx) : DBL_MAX;
    double tDeltaY = (dy != 0.0) ? fabs(1.0 / dy) : DBL_MAX;
    double tDeltaZ = (dz != 0.0) ? fabs(1.0 / dz) : DBL_MAX;

    int face = -1;
    double t = 0.0;

    while (t <= maxDist) {
        if (x < 0 || y < 0 || z < 0 || x >= lvl->width || y >= lvl->depth || z >= lvl->height)
            return 0;

        int id = Level_getTile(lvl, x, y, z);
        if (id != 0) {
            if (out) hitresult_create(out, x, y, z, 0, (face < 0 ? 0 : face));
            return 1;
        }

        if (tMaxX < tMaxY) {
            if (tMaxX < tMaxZ) {
                x += stepX; t = tMaxX; tMaxX += tDeltaX;
                face = (stepX > 0) ? 4 : 5;
            } else {
                z += stepZ; t = tMaxZ; tMaxZ += tDeltaZ;
                face = (stepZ > 0) ? 2 : 3;
            }
        } else {
            if (tMaxY < tMaxZ) {
                y += stepY; t = tMaxY; tMaxY += tDeltaY;
                face = (stepY > 0) ? 0 : 1;
            } else {
                z += stepZ; t = tMaxZ; tMaxZ += tDeltaZ;
                face = (stepZ > 0) ? 2 : 3;
            }
        }
    }
    return 0;
}

static void pick(float t) {
    double x = player.e.prevX + (player.e.x - player.e.prevX) * (double)t;
    double y = player.e.prevY + (player.e.y - player.e.prevY) * (double)t;
    double z = player.e.prevZ + (player.e.z - player.e.prevZ) * (double)t;

    double dx, dy, dz;
    get_look_dir(&player, &dx, &dy, &dz);

    x += dx * 0.3; y += dy * 0.3; z += dz * 0.3;

    const int reachBlocks = 3; 

    HitResult hr;
    if (raycast_block(&level, x, y, z, dx, dy, dz, 100.0, &hr)) {
        int px = (int)floor(player.e.x);
        int py = (int)floor(player.e.y);
        int pz = (int)floor(player.e.z);
        if (abs(hr.x - px) <= reachBlocks &&
            abs(hr.y - py) <= reachBlocks &&
            abs(hr.z - pz) <= reachBlocks) {
            hitResult = hr;
            isHitNull = 0;
            return;
        }
    }
    isHitNull = 1;
}

/* --- input actions ----------------------------------------------------------- */

static void handleGameplayKeys() {
    int enter = getKey(buttonSave);
    if (enter == 1 && prevEnter == 0) {
        Level_save(&level);
    }
    prevEnter = enter;

    int n1 = getKey('1');
    int n2 = getKey('2');
    int n3 = getKey('3');
    int n4 = getKey('4');
    int n6 = getKey('6');
    if (n1 == 1 && prevNum1 == 0) selectedTileId = 1; 
    if (n2 == 1 && prevNum2 == 0) selectedTileId = 3; 
    if (n3 == 1 && prevNum3 == 0) selectedTileId = 4; 
    if (n4 == 1 && prevNum4 == 0) selectedTileId = 5; 
    if (n6 == 1 && prevNum6 == 0) selectedTileId = 6; 
    prevNum1 = n1; prevNum2 = n2; prevNum3 = n3; prevNum4 = n4; prevNum6 = n6;

    int g = getKey('G');
    if (g == 1 && prevG == 0 && mobCount < MAX_MOBS) {
        Zombie_init(&mobs[mobCount++], &level, player.e.x, player.e.y, player.e.z);
    }
    prevG = g;

    int kY = getKey('Y');
    if (kY == 1 && prevY == 0) {
        gYMouseAxis *= -1;
    }
    prevY = kY;
}

static bool wasGrabbed = false;
static void handleBlockClicks() {
    int left  = getKey(firePrim);
    int right = getKey(fireSec);

    if (!isMouseLocked && (left || right)) {
        isMouseLocked = 1;
        return;
    }
    if (wasGrabbed != isMouseLocked) {
        wasGrabbed = isMouseLocked;
        return;
    }

    if (isRunning() && !isActive() &&
        (left == 1 || right == 1)) {
        int ww, wh; getWindowSize(&ww, &wh);
        hideMouse();
        setMouse_xy(ww / 2, wh / 2);
        prevLeft = left;
        prevRight = right;
        return;
    }

    if (right == 1 && prevRight == 0) {
        gEditMode = (gEditMode + 1) % 2;
    }
    prevRight = right;

    if (left == 1 && prevLeft == 0 && !isHitNull) {
        if (gEditMode == 0) {
            int id = Level_getTile(&level, hitResult.x, hitResult.y, hitResult.z);
            const Tile* t = (id >= 0 && id < 256) ? gTiles[id] : NULL;
            bool changed = level_setTile(&level, hitResult.x, hitResult.y, hitResult.z, 0);
            if (t && changed) {
                Tile_onDestroy(t, &level, hitResult.x, hitResult.y, hitResult.z, &particleEngine);
            }
        } else {
            int nx=0, ny=0, nz=0;
            switch (hitResult.f) {
                case 0: ny = -1; break; 
                case 1: ny =  1; break; 
                case 2: nz = -1; break; 
                case 3: nz =  1; break; 
                case 4: nx = -1; break; 
                case 5: nx =  1; break; 
            }
            int x = hitResult.x + nx;
            int y = hitResult.y + ny;
            int z = hitResult.z + nz;

            AABB aabb = Level_getTilePickAABB(&level, x, y, z);
            if (!AABB_intersects(&player.e.boundingBox, &aabb)) {
                bool blocked = false;
                for (int i = 0; i < mobCount; ++i) {
                    if (AABB_intersects(&mobs[i].base.boundingBox, &aabb)) { blocked = true; break; }
                }
                if (!blocked) {
                    level_setTile(&level, x, y, z, selectedTileId);
                }
            }
        }
    }
    prevLeft = left;
}

/* --- frame ------------------------------------------------------------------- */
static void render(Level* lvl, LevelRenderer* lr, Player* p, float t) {
    (void)lvl;
    int fbw, fbh;
    startOfFrame(&fbw, &fbh);

    int grabbed = isActive();
    if (grabbed && isMouseLocked) {
        int mx, my;
        getMouse_xy(&mx, &my);

        int ww, wh; 
        getWindowSize(&ww, &wh);
        int cx = ww / 2, cy = wh / 2;

        float dx = (float)(mx - cx) * t;
        float dy = (float)(my - cy) * t;

        dy = -dy * (float)gYMouseAxis;

        Player_turn(p, dx, dy);
        //setMouse_xy(cx, cy);
    }

    setupCamera(p, t);

    setupFog(0);
    LevelRenderer_render(lr, 0);

    for (int i = 0; i < mobCount; ++i) {
        const Zombie* z = &mobs[i];
        if (Entity_isLit(&z->base) && frustum_isVisible(&z->base.boundingBox)) {
            Zombie_render(z, t);
        }
    }

    ParticleEngine_render(&particleEngine, p, t, 0);

    setupFog(1);
    LevelRenderer_render(lr, 1);

    for (int i = 0; i < mobCount; ++i) {
        const Zombie* z = &mobs[i];
        if (!Entity_isLit(&z->base) && frustum_isVisible(&z->base.boundingBox)) {
            Zombie_render(z, t);
        }
    }

    ParticleEngine_render(&particleEngine, p, t, 1);

    endOfFrame(lr, &hitResult, isHitNull, gEditMode, selectedTileId);
    drawGui(t);
}

/* --- main loop --------------------------------------------------------------- */
float dt;
long long last;
static void run(Level* lvl, LevelRenderer* lr, Player* p) {
    if (!init(lvl, lr, p)) {
        fprintf(stderr, "Failed to initialize game\n");
        destroy(lvl);
        exit(EXIT_FAILURE);
    }

    int frames = 0;
    last = currentTimeMillis();

    while (isRunning()) {
        startOfTick();

        Timer_advanceTime(&timer);

        for (int i = 0; i < timer.ticks; ++i) tick(p);

        pick(timer.partialTicks);
        handleBlockClicks();
        handleGameplayKeys();

        int built = LevelRenderer_updateDirtyChunks(lr, p);
        gChunkUpdatesAccum += built;

        render(lvl, lr, p, timer.partialTicks);

        frames++;
        while (currentTimeMillis() >= last + 1000LL) {
            gFPS = frames;
            gChunkUpdatesPerSec = gChunkUpdatesAccum;
        #ifndef NDEBUG
            printf("%d fps, %d chunk updates\n", gFPS, gChunkUpdatesPerSec);
        #endif
            last += 1000LL;
            frames = 0;
            gChunkUpdatesAccum = 0;
        }
    }

    destroy(lvl);
}

int main(void) {
    run(&level, &levelRenderer, &player);
    return EXIT_SUCCESS;
}

static void tick(Player* p) {
    Level_onTick(&level);
    ParticleEngine_onTick(&particleEngine);
    Player_onTick(p);

    for (int i = 0; i < mobCount; ) {
        Zombie_onTick(&mobs[i]);
        if (mobs[i].base.removed) {
            mobs[i] = mobs[mobCount - 1];
            mobCount--;
            continue;
        }
        i++;
    }
}