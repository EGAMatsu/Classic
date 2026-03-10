/* Shit for my crap renderer */
#include "common.h"

#include "level/level_renderer.h"
#include "hitresult.h"

#ifdef GLFW_GLEW
    #include <GL/glew.h>
    #include <GL/glu.h>
    #include <GL/glfw.h>
#endif

extern bool isMouseLocked;

extern int firePrim;
extern int fireSec;
extern int buttonSave;
extern int buttonRespawn;

void startModel_Matrix();
void setTexture(int texture);
void   endModel_Matrix();

/* Input */
void keyCallback(int key, int action);
void getMouse_xy(int *x, int *y);
void setMouse_xy(int x, int y);
int getKey(int key);

/* Model position */
void setModel_color(float r, float g, float b, float a);
void setModel_position(double ix, double iy, double iz);
void setModel_positionOffset(double ix, double iy, double iz);
void setModel_scale(float scale);
void setModel_scalePercise(float sx, float sy, float sz);
void setModel_rotation(float rx, float ry, float rz);

/* Core */
int initWindow(int fullscreen, int width, int height);
int initGraphics();
void shutdownWindow();
bool isRunning();
bool isActive();
void startOfTick();
void startOfFrame(int *width, int *height);
void endOfFrame(LevelRenderer *lr, HitResult *hitResult, int isHitNull, int gEditMode, int selectedTileId);
void getWindowSize(int *width, int *height);
void setupPerspective();
void setupGUI(int screenWidth, int screenHeight);

/* Rendering */
void setupFog(int type);