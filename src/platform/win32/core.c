#include <windows.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <math.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include "../../dsp_renderer.h"

typedef BOOL (WINAPI *PFNWGLSWAPINTERVALEXTPROC)(int interval);

static GLfloat fogColorDaylight[4] = { 254.0f/255.0f, 251.0f/255.0f, 250.0f/255.0f, 1.0f };
static GLfloat fogColorShadow  [4] = {  14.0f/255.0f,  11.0f/255.0f,  10.0f/255.0f, 1.0f };

/* Input Defaults */
#define DEVICE_KB 0
#define DEVICE_MOUSE 4096
#define DEVICE_CONTROLLER0 8192

int firePrim = 0x01 + DEVICE_MOUSE;
int fireSec  = 0x02 + DEVICE_MOUSE;
int buttonSave = VK_RETURN;
int buttonRespawn = 'R';

int     forwardButton[2] = {'W', VK_UP};
int    backwardButton[2] = {'S', VK_DOWN};
int   strafeLeftButton[2] = {'A', VK_LEFT};
int strafeRightButton[2] = {'D', VK_RIGHT};
int buttonJump           = VK_SPACE;

static HWND g_hWnd = NULL;
static HDC g_hDC = NULL;
static HGLRC g_hRC = NULL;
static bool g_windowActive = true;
static bool g_windowOpen = false;
static int g_mouseX = 0, g_mouseY = 0;
static int g_mouseDeltaX = 0, g_mouseDeltaY = 0;
static bool g_mouseLocked = false;

LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
        case WM_ACTIVATE:
            g_windowActive = LOWORD(wParam) != WA_INACTIVE;
            if (!g_windowActive) {
                g_mouseLocked = false;
                ShowCursor(TRUE);
            }
            return 0;
        case WM_CLOSE:
            g_windowOpen = false;
            return 0;
        case WM_KEYDOWN:
            if (wParam == VK_ESCAPE) {
                g_mouseLocked = false;
                ShowCursor(TRUE);
            }
            return 0;
        case WM_MOUSEMOVE:
            if (!g_mouseLocked) {
                g_mouseX = LOWORD(lParam);
                g_mouseY = HIWORD(lParam);
            }
            return 0;
        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
    }
    return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

/* Frustum */
void getFrustumCone(float proj[16], float modl[16]) {
    glGetFloatv(GL_PROJECTION_MATRIX, proj);
    glGetFloatv(GL_MODELVIEW_MATRIX,  modl);
}

/* Input */
/* Input */
void getMouse_xy(int *x, int *y) {
    if (g_mouseLocked) {
        // Return the accumulated delta since the last reset
        *x = g_mouseDeltaX;
        *y = g_mouseDeltaY;
        // Reset deltas so we don't spin forever
        g_mouseDeltaX = 0;
        g_mouseDeltaY = 0;
    } else {
        // Return absolute position for GUI/Menus
        *x = g_mouseX;
        *y = g_mouseY;
    }
}

void setMouse_xy(int x, int y) {
    POINT pt = {x, y};
    ClientToScreen(g_hWnd, &pt);
    SetCursorPos(pt.x, pt.y);
}

void showMouse() {
    g_mouseLocked = false;
    ShowCursor(TRUE);
}

void hideMouse() {
    g_mouseLocked = true;
    ShowCursor(FALSE);
    
    RECT rect;
    GetClientRect(g_hWnd, &rect);
    setMouse_xy(rect.right / 2, rect.bottom / 2);
}

int getKey(int key) {
    if (key >= DEVICE_MOUSE && key < DEVICE_CONTROLLER0) {
        int mouseBtn = key - DEVICE_MOUSE;
        int vk = (mouseBtn == 1) ? VK_LBUTTON : VK_RBUTTON;
        return (GetAsyncKeyState(vk) & 0x8000) != 0;
    } else if (key >= DEVICE_CONTROLLER0) {
        return 0;
    } else {
        return (GetAsyncKeyState(key) & 0x8000) != 0;
    }
}

/* Window creation and management */
int initWindow(int fullscreen, int width, int height) {
    HINSTANCE hInstance = GetModuleHandle(NULL);
    WNDCLASS wc = {0};
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.lpszClassName = "classicWindow";
    RegisterClass(&wc);

    DWORD dwStyle = WS_OVERLAPPEDWINDOW;
    DWORD dwExStyle = WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;

    if (fullscreen) {
        DEVMODE dmScreenSettings = {0};
        dmScreenSettings.dmSize = sizeof(dmScreenSettings);
        dmScreenSettings.dmPelsWidth = width;
        dmScreenSettings.dmPelsHeight = height;
        dmScreenSettings.dmBitsPerPel = 32;
        dmScreenSettings.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;
        ChangeDisplaySettings(&dmScreenSettings, CDS_FULLSCREEN);
        dwStyle = WS_POPUP;
        dwExStyle = WS_EX_APPWINDOW;
    }

    RECT rec = {0, 0, width, height};
    AdjustWindowRectEx(&rec, dwStyle, FALSE, dwExStyle);

    g_hWnd = CreateWindowEx(dwExStyle, "classicWindow", "Minecraft 0.0.11a",
                            dwStyle | WS_CLIPSIBLINGS | WS_CLIPCHILDREN,
                            0, 0, rec.right - rec.left, rec.bottom - rec.top,
                            NULL, NULL, hInstance, NULL);

    if (!g_hWnd) return 0;

    g_hDC = GetDC(g_hWnd);
    PIXELFORMATDESCRIPTOR pfd = { sizeof(PIXELFORMATDESCRIPTOR), 1,
        PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER,
        PFD_TYPE_RGBA, 32, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 24, 8, 0,
        PFD_MAIN_PLANE, 0, 0, 0, 0 };

    int pf = ChoosePixelFormat(g_hDC, &pfd);
    SetPixelFormat(g_hDC, pf, &pfd);
    g_hRC = wglCreateContext(g_hDC);
    wglMakeCurrent(g_hDC, g_hRC);

    ShowWindow(g_hWnd, SW_SHOW);
    SetForegroundWindow(g_hWnd);
    SetFocus(g_hWnd);

    g_windowOpen = true;
    return 1;
}

int initGraphics() {
    PFNWGLSWAPINTERVALEXTPROC wglSwapIntervalEXT = (PFNWGLSWAPINTERVALEXTPROC)wglGetProcAddress("wglSwapIntervalEXT");
    if (wglSwapIntervalEXT) wglSwapIntervalEXT(1);

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

    hideMouse();

    return 1;
}

void shutdownWindow() {
    if (g_hRC) {
        wglMakeCurrent(NULL, NULL);
        wglDeleteContext(g_hRC);
    }
    if (g_hDC) ReleaseDC(g_hWnd, g_hDC);
    if (g_hWnd) DestroyWindow(g_hWnd);
    ChangeDisplaySettings(NULL, 0);
}

bool isRunning() {
    return g_windowOpen;
}

bool isActive() {
    return g_windowActive && g_mouseLocked;
}

void startOfTick() {
    MSG msg;
    while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
        if (msg.message == WM_QUIT) g_windowOpen = false;
    }

    if (g_mouseLocked && g_windowActive) {
        POINT p;
        GetCursorPos(&p);
        ScreenToClient(g_hWnd, &p);

        RECT rect;
        GetClientRect(g_hWnd, &rect);
        int cx = rect.right / 2;
        int cy = rect.bottom / 2;

        g_mouseDeltaX = p.x;
        g_mouseDeltaY = p.y;

        if (g_mouseDeltaX != 0 || g_mouseDeltaY != 0) {
            setMouse_xy(cx, cy);
        }
    }
}

void startOfFrame(int *width, int *height) {
    SwapBuffers(g_hDC);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    getWindowSize(width, height);
    glViewport(0, 0, width[0], height[0]);

    if (!g_windowActive && g_mouseLocked) {
        showMouse();
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
    RECT rect;
    GetClientRect(g_hWnd, &rect);
    *width = rect.right;
    *height = rect.bottom;
}

void setupPerspective() {
    int fbw, fbh;
    getWindowSize(&fbw, &fbh);
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

/* Block hit */
extern Tessellator TESSELLATOR;
void LevelRenderer_renderHit(LevelRenderer* r, HitResult* h, int mode, int tileId) {
    if (!h) return;

    if (mode == 0) {
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE);
        float a = (float)(sin((double)GetTickCount() / 100.0) * 0.2 + 0.4) * 0.5f;
        glColor4f(1.0f, 1.0f, 1.0f, a);

        Tessellator_init(&TESSELLATOR);
        for (int face = 0; face < 6; ++face) {
            Face_render(&TESSELLATOR, h->x, h->y, h->z, face);
        }
        Tessellator_flush(&TESSELLATOR);

        glDisable(GL_BLEND);
        return;
    }

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    float br = (float)(sin((double)GetTickCount() / 100.0) * 0.2 + 0.8);
    float al = (float)(sin((double)GetTickCount() / 200.0) * 0.2 + 0.5);
    glColor4f(br, br, br, al);

    int nx = 0, ny = 0, nz = 0;
    switch (h->f) {
        case 0: ny = -1; break; // bottom
        case 1: ny =  1; break; // top
        case 2: nz = -1; break; // -Z
        case 3: nz =  1; break; // +Z
        case 4: nx = -1; break; // -X
        case 5: nx =  1; break; // +X
    }
    const int x = h->x + nx, y = h->y + ny, z = h->z + nz;

    const Tile* t = (tileId >= 0 && tileId < 256) ? gTiles[tileId] : NULL;
    if (t && t->render) {
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, r->terrainTex);

        Tessellator_setIgnoreColor(&TESSELLATOR, 1);
        Tessellator_init(&TESSELLATOR);
        t->render(t, &TESSELLATOR, r->level, 0, x, y, z);
        t->render(t, &TESSELLATOR, r->level, 1, x, y, z);
        Tessellator_flush(&TESSELLATOR);
        Tessellator_setIgnoreColor(&TESSELLATOR, 0);

        glDisable(GL_TEXTURE_2D);
    }

    glDisable(GL_BLEND);
}