#include <math.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include "../../dsp_renderer.h"
#include "../../renderer/tessellator.h"
#include "../../renderer/textures.h"
#include "../../character/polygon.h"
#include "../../character/cube.h"
#include "../../common.h"

/* Flushes all the queued quads for the World/UI */
void Tessellator_flush(Tessellator* t) {
}

/* Polygon rendering for chars */
void Polygon_render(const Polygon* p) {
}

/* Box rendering (also for chars) */
void Cube_render(const Cube* c) {
}

/* Start/End of model bullcrap */
void startModel_Matrix() {
}
void setTexture(int texture) {
}
void   endModel_Matrix() {
}

/* Model position */
void setModel_color(float r, float g, float b, float a) {
}

void setModel_position(double ix, double iy, double iz) {
}

void setModel_positionOffset(double ix, double iy, double iz) {
}

/* Model Scale */
void setModel_scale(float scale) {
}

void setModel_scalePercise(float sx, float sy, float sz) {
}

/* Model Rotation */
void setModel_rotation(float rx, float ry, float rz) {
}