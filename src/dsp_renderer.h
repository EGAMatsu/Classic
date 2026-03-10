/* Shit for my crap renderer */
#include "common.h"
extern bool isMouseLocked;

void startModel_Matrix();
void   endModel_Matrix();

/* Model position */
void setModel_position(double ix, double iy, double iz);
void setModel_positionOffset(double ix, double iy, double iz);
void setModel_scale(float scale);
void setModel_rotation(float rx, float ry, float rz);

/* Core */
int initWindow(int fullscreen, int width, int height);
int initGraphics();
void shutdownWindow();
void keyCallback(int key, int action);
bool isRunning();
bool isActive();
void startOfTick();
void startOfFrame(int width, int height);

/* Rendering */
void setupFog(int type);