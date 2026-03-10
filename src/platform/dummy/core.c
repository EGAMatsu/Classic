/* The main class for the renderer, window handling, input, etc... */
/* Seperate from the meshRenderer, as this is more general bullcrap than that is. */

#include <math.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include "../../dsp_renderer.h"

/* Input */
void keyCallback(int key, int action) {
}

void getMouse_xy(int *x, int *y) {
}
void setMouse_xy(int x, int y) {
}

int getKey(int key) {
}

/* Window creation and management */
int initWindow(int fullscreen, int width, int height) {
    return 1;
}

int initGraphics() {
    return 1;
}

void shutdownWindow() {
}

bool isRunning() {
    return 1;
}
bool isActive() {
    return 1;
}

void startOfTick() {
}
void startOfFrame(int *width, int *height) {
}
void endOfFrame(LevelRenderer *lr, HitResult *hitResult, int isHitNull, int gEditMode, int selectedTileId) {
}

void getWindowSize(int *width, int *height) {
}

void setupPerspective() {
}

void setupGUI(int screenWidth, int screenHeight) {
}

/* General rendering crap */
void setupFog(int type) {
}