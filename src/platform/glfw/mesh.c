#include <GL/glew.h>
#include <math.h>
#include <string.h>
#include "../../renderer/tessellator.h"
#include "../../renderer/textures.h"
#include "../../character/polygon.h"
#include "../../character/cube.h"
#include "../../common.h"

/* Transparency (mostly for the block shit when placing/breaking) */
void enableTranslucency(int mode) {
    if (!mode) {
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE);
    } else {
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    }
}
void disableTranslucency() {
    glDisable(GL_BLEND);
}
void doCulling(bool doIt) {
    if (doIt) {
        glEnable(GL_CULL_FACE);
    } else {
        glDisable(GL_CULL_FACE);
    }
}
bool doingCulling() {
    return glIsEnabled(GL_CULL_FACE);
}

/* Lists */
unsigned int model_genList(unsigned int n) {
    return glGenLists(n);
}
void startMeshList(unsigned int layer, unsigned int texture) {
    glNewList(layer, GL_COMPILE);
    setTexture(texture);
}
void endMeshList() {
    glDisable(GL_TEXTURE_2D);
    glEndList();
}
void renderMeshList(unsigned int list) {
    glCallList(list);
}

/* Mesh Array crap */
void meshArray_disableColor() {
    glDisableClientState(GL_COLOR_ARRAY);
}

/* Flushes all the queued quads for the World/UI */
void Tessellator_flush(Tessellator* t) {
    glDisableClientState(GL_COLOR_ARRAY);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);

    glVertexPointer(3, GL_FLOAT, 0, t->vertexBuffer);
    glEnableClientState(GL_VERTEX_ARRAY);

    if (t->hasTexture) {
        glTexCoordPointer(2, GL_FLOAT, 0, t->texBuffer);
        glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    }
    if (t->hasColor) {
        glColorPointer(3, GL_FLOAT, 0, t->colorBuffer);
        glEnableClientState(GL_COLOR_ARRAY);
    }

    glDrawArrays(GL_QUADS, 0, t->vertices);
    Tessellator_clear(t);
}

/* Polygon rendering for chars */
void Polygon_render(const Polygon* p) {
    glColor3f(1.0f, 1.0f, 1.0f);

    const float uDiv = 63.999f;
    const float vDiv = 31.999f;

    glBegin(GL_QUADS);
    glTexCoord2f(p->v[3].u / uDiv, p->v[3].v / vDiv); glVertex3f(p->v[3].pos.x, p->v[3].pos.y, p->v[3].pos.z);
    glTexCoord2f(p->v[2].u / uDiv, p->v[2].v / vDiv); glVertex3f(p->v[2].pos.x, p->v[2].pos.y, p->v[2].pos.z);
    glTexCoord2f(p->v[1].u / uDiv, p->v[1].v / vDiv); glVertex3f(p->v[1].pos.x, p->v[1].pos.y, p->v[1].pos.z);
    glTexCoord2f(p->v[0].u / uDiv, p->v[0].v / vDiv); glVertex3f(p->v[0].pos.x, p->v[0].pos.y, p->v[0].pos.z);
    glEnd();
}

/* Box rendering (also for chars) */
void Cube_render(const Cube* c) {
    glPushMatrix();
    glTranslatef(c->x, c->y, c->z);
    glRotatef((float)(c->zRot * 180.0 / M_PI), 0.0f, 0.0f, 1.0f);
    glRotatef((float)(c->yRot * 180.0 / M_PI), 0.0f, 1.0f, 0.0f);
    glRotatef((float)(c->xRot * 180.0 / M_PI), 1.0f, 0.0f, 0.0f);

    for (int i = 0; i < 6; ++i) Polygon_render(&c->polys[i]);

    glPopMatrix();
}

/* Start/End of model bullcrap */
void startModel_Matrix() {
    glPushMatrix();
    glEnable(GL_TEXTURE_2D);
}
void setTexture(int texture) {
    glEnable(GL_TEXTURE_2D);
    texture_bind(texture);
}
void disableTexture() {
    glDisable(GL_TEXTURE_2D);
}
void   endModel_Matrix() {
    glDisable(GL_TEXTURE_2D);
    glPopMatrix();
}

/* Model position */
void setModel_color(float r, float g, float b, float a) {
    glColor4f(r, g, b, a);
}
void setModel_color_rgb(float r, float g, float b) {
    glColor3f(r, g, b);
}

void setModel_position(double ix, double iy, double iz) {
    glTranslated(ix, iy, iz);
    glScalef(1.0f, -1.0f, 1.0f);
}

void setModel_positionOffset(double ix, double iy, double iz) {
    glTranslated(ix, iy, iz);
}

/* Model Scale */
void setModel_scale(float scale) {
    glScalef(scale, scale, scale);
}

void setModel_scalePercise(float sx, float sy, float sz) {
    glScalef(sx, sy, sz);
}

/* Model Rotation */
void setModel_rotation(float rx, float ry, float rz) {
    glRotatef(rz, 0.0f, 0.0f, 1.0f);
    glRotatef(ry, 0.0f, 1.0f, 0.0f);
    glRotatef(rx, 1.0f, 0.0f, 0.0f);
}