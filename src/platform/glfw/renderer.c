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
void startOfFrame(int width, int height) {
    glfwGetWindowSize(&width, &height); 
    glViewport(0, 0, width, height);

    if (!glfwGetWindowParam(GLFW_ACTIVE)) {
        glfwEnable(GLFW_MOUSE_CURSOR);
    }
}

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