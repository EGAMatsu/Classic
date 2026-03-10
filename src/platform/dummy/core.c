#include <math.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include "../../dsp_renderer.h"

int firePrim = 100;
int fireSec = 101;
int buttonSave = 102;
int buttonRespawn = 103;
int forwardButton[2] = {104, 105};
int backwardButton[2] = {106, 107};
int strafeLeftButton[2] = {108, 109};
int strafeRightButton[2] = {110, 111};
int buttonJump = 112;

static int wdth, hght;

void getFrustumCone(float proj[16], float modl[16]) {
}

void keyCallback(int key, int action) {
    if (action == 1 && key == 27) {
        printf("Escape (might have been) pressed\n");
    }
}

void getMouse_xy(int *x, int *y) {
    *x = 0; *y = 0;
}

void setMouse_xy(int x, int y) {}
void showMouse() {}
void hideMouse() {}

int getKey(int key) {
    return 0;
}

int initWindow(int fullscreen, int width, int height) {
    printf("Window Init: %dx%d\n", width, height);
    wdth = width;
    hght = height;
    return 1;
}

int initGraphics() {
    printf("Graphics Init\n");
    return 1;
}

void shutdownWindow() {
    printf("Shutdown\n");
}

bool isRunning() {
    return true;
}

bool isActive() {
    return true;
}

void startOfTick() {}

void startOfFrame(int *width, int *height) {
    width[0] = wdth;
    height[0] = hght;
}

void endOfFrame(LevelRenderer *lr, HitResult *hitResult, int isHitNull, int gEditMode, int selectedTileId) {
    if (!isHitNull) {
        printf("Rendering hit at %d %d %d\n", hitResult->x, hitResult->y, hitResult->z);
    }
}

void getWindowSize(int *width, int *height) {
    width[0] = wdth;
    height[0] = hght;
}

void setupPerspective() {}
void setupGUI(int sw, int sh) {}

void setupFog(int type) {
    if (type == 0) {
        printf("Fog: Daylight\n");
    } else {
        printf("Fog: Shadow\n");
    }
}

extern Tessellator TESSELLATOR;
void LevelRenderer_renderHit(LevelRenderer* r, HitResult* h, int mode, int tileId) {
    if (!h) return;
    printf("Hit Mode %d on Tile %d\n", mode, tileId);
}