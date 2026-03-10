/* The main class for the renderer, window handling, input, etc... */
/* Seperate from the meshRenderer, as this is more general bullcrap than that is. */
#include <GL/glew.h>
#include <GL/glfw.h>
#include <math.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include "../../dsp_renderer.h"

static GLfloat fogColorDaylight[4] = { 254.0f/255.0f, 251.0f/255.0f, 250.0f/255.0f, 1.0f };
static GLfloat fogColorShadow  [4] = {  14.0f/255.0f,  11.0f/255.0f,  10.0f/255.0f, 1.0f };

/* Input Defaults */
#define DEVICE_KB -1
#define DEVICE_MOUSE -2
#define DEVICE_CONTROLLER0 0

int8_t firePrim_dev = DEVICE_MOUSE;
int firePrim = GLFW_MOUSE_BUTTON_LEFT;
int8_t  fireSec_dev = DEVICE_MOUSE;
int fireSec  = GLFW_MOUSE_BUTTON_RIGHT;

/* Input */
void keyCallback(int key, int action) {
    if (action == 1 && key == GLFW_KEY_ESC) {
        if (glfwGetWindowParam(GLFW_OPENED)) {
            int ww, wh; glfwGetWindowSize(&ww, &wh);
            glfwEnable(GLFW_MOUSE_CURSOR);
            isMouseLocked = 0;
            glfwSetMousePos(ww / 2, wh / 2);
        }
    }
}

void getMouse_xy(int *x, int *y) {
    glfwGetMousePos(x, y);
}
void setMouse_xy(int x, int y) {
    glfwSetMousePos(x, y);
}

int getKey(int key) {
    return glfwGetKey(key);
}

/* Window creation and management */
int initWindow(int fullscreen, int width, int height) {
    if (!glfwInit()) {
        fprintf(stderr, "Failed to initialize GLFW\n");
        return 0;
    }

    GLFWvidmode mode;
    glfwGetDesktopMode(&mode);

    int res;
    if (fullscreen) {
        res = glfwOpenWindow(mode.Width, mode.Height, 8,8,8,8, 24,0, GLFW_FULLSCREEN);
    } else {
        res = glfwOpenWindow(width, height, 8,8,8,8, 24,0, GLFW_WINDOW);
    }

    if (!res) {
        fprintf(stderr, "Failed to create GLFW window\n");
        glfwTerminate();
        return 0;
    }

    glfwSetWindowTitle("Minecraft 0.0.11a");
    glfwSwapInterval(0);

    return 1;
}

int initGraphics() {
    GLenum err = glewInit();
    if (err != GLEW_OK) {
        fprintf(stderr, "GLEW init failed: %s\n", glewGetErrorString(err));
        return 0;
    }

    glEnable(GL_TEXTURE_2D);
    glShadeModel(GL_SMOOTH);
    glDisable(GL_LIGHTING);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glClearColor(0.5f, 0.8f, 1.0f, 0.0f);

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);

    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);

    glEnable(GL_ALPHA_TEST);
    glAlphaFunc(GL_GREATER, 0.5f);
    
    int ww, wh;
    glfwGetWindowSize(&ww, &wh);
    glfwDisable(GLFW_MOUSE_CURSOR);
    glfwSetMousePos(ww / 2, wh / 2);
    glfwSetKeyCallback(keyCallback);

    return 1;
}

void shutdownWindow() {
    glfwTerminate();
}

bool isRunning() {
    return glfwGetWindowParam(GLFW_OPENED);
}
bool isActive() {
    return glfwGetWindowParam(GLFW_ACTIVE);
}

void startOfTick() {
    glfwPollEvents();
}
void startOfFrame(int *width, int *height) {
    glfwSwapBuffers();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    getWindowSize(width, height); 
    glViewport(0, 0, width[0], height[0]);

    if (!glfwGetWindowParam(GLFW_ACTIVE)) {
        glfwEnable(GLFW_MOUSE_CURSOR);
    }
}
void endOfFrame(LevelRenderer *lr, HitResult *hitResult, int isHitNull, int gEditMode, int selectedTileId) {
    glDisable(GL_LIGHTING);
    glDisable(GL_TEXTURE_2D);
    glDisable(GL_FOG);

    if (!isHitNull) {
        GLboolean wasAlpha = glIsEnabled(GL_ALPHA_TEST);
        if (wasAlpha) glDisable(GL_ALPHA_TEST);
        LevelRenderer_renderHit(lr, hitResult, gEditMode, selectedTileId);
        if (wasAlpha) glEnable(GL_ALPHA_TEST);
    }
}

void getWindowSize(int *width, int *height) {
    glfwGetWindowSize(width, height); 
}

void setupPerspective() {
    int fbw, fbh; glfwGetWindowSize(&fbw, &fbh);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(70.0, (double)fbw / (double)fbh, 0.05, 1000.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

void setupGUI(int screenWidth, int screenHeight) {
    glClear(GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0.0, (double)screenWidth, (double)screenHeight, 0.0, 100.0, 300.0);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

/* General rendering crap */
void setupFog(int type) {
    glEnable(GL_FOG);
    glFogi(GL_FOG_MODE, GL_EXP);

    if (type == 0) { // daylight
        glFogf(GL_FOG_DENSITY, 0.001f);
        glFogfv(GL_FOG_COLOR, fogColorDaylight);
        glDisable(GL_LIGHTING);
        glDisable(GL_COLOR_MATERIAL);
    } else {         // shadow
        glFogf(GL_FOG_DENSITY, 0.01f);
        glFogfv(GL_FOG_COLOR, fogColorShadow);
        glEnable(GL_LIGHTING);
        glEnable(GL_COLOR_MATERIAL);

        GLfloat ambient[4] = { 0.6f, 0.6f, 0.6f, 1.0f };
        glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambient);
    }
}