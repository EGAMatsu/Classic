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

void enableTranslucency(int mode) {
    if (!mode) printf("Blend: Additive\n");
    else printf("Blend: Alpha\n");
}

void disableTranslucency() {
    printf("Blend: Disabled\n");
}

void doCulling(bool doIt) {
    printf("Culling: %s\n", doIt ? "On" : "Off");
}

bool doingCulling() {
    return true;
}

unsigned int model_genList(unsigned int n) {
    return 1;
}

void setTexture(int texture) {
    printf("Texture Bound: %d\n", texture);
}

void startMeshList(unsigned int layer, unsigned int texture) {
    printf("Start Mesh List: %u\n", layer);
    setTexture(texture);
}

void endMeshList() {
    printf("End Mesh List\n");
}

void renderMeshList(unsigned int list) {
    printf("Call Mesh List: %u\n", list);
}

void meshArray_disableColor() {}

void Tessellator_flush(Tessellator* t) {
    printf("Flushing Tessellator: %d vertices\n", t->vertices);
    Tessellator_clear(t);
}

void Polygon_render(const Polygon* p) {
    printf("  Render Poly\n");
}

void Cube_render(const Cube* c) {
    printf("Render Cube at %.2f, %.2f, %.2f\n", c->x, c->y, c->z);
}

void startModel_Matrix() {
    printf("Push Matrix\n");
}

void disableTexture() {
    printf("Texture: Off\n");
}

void endModel_Matrix() {
    printf("Pop Matrix\n");
}

void setModel_color(float r, float g, float b, float a) {
    printf("Color: %.2f, %.2f, %.2f, %.2f\n", r, g, b, a);
}

void setModel_color_rgb(float r, float g, float b) {
    printf("Color: %.2f, %.2f, %.2f\n", r, g, b);
}

void setModel_position(double ix, double iy, double iz) {
    printf("Pos: %.2f, %.2f, %.2f\n", ix, iy, iz);
}

void setModel_positionOffset(double ix, double iy, double iz) {
    printf("Offset: %.2f, %.2f, %.2f\n", ix, iy, iz);
}

void setModel_scale(float scale) {
    printf("Scale: %.2f\n", scale);
}

void setModel_scalePercise(float sx, float sy, float sz) {
    printf("Scale: %.2f, %.2f, %.2f\n", sx, sy, sz);
}

void setModel_rotation(float rx, float ry, float rz) {
    printf("Rot: %.2f, %.2f, %.2f\n", rx, ry, rz);
}