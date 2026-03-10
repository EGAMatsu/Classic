#include <GL/glew.h>
#include <math.h>
#include <string.h>
#include "../../renderer/tessellator.h"
#include "../../character/polygon.h"
#include "../../character/cube.h"
#include "../../common.h"

/* Flushes all the queued quads for the World/UI */
void Tessellator_flush(Tessellator* t) {
    // reset client states so stale arrays (e.g., colors) don't affect us
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
    glColor3f(1.f, 1.f, 1.f);

    const float uDiv = 63.999f;
    const float vDiv = 31.999f;

    glTexCoord2f(p->v[3].u / uDiv, p->v[3].v / vDiv); glVertex3f(p->v[3].pos.x, p->v[3].pos.y, p->v[3].pos.z);
    glTexCoord2f(p->v[2].u / uDiv, p->v[2].v / vDiv); glVertex3f(p->v[2].pos.x, p->v[2].pos.y, p->v[2].pos.z);
    glTexCoord2f(p->v[1].u / uDiv, p->v[1].v / vDiv); glVertex3f(p->v[1].pos.x, p->v[1].pos.y, p->v[1].pos.z);
    glTexCoord2f(p->v[0].u / uDiv, p->v[0].v / vDiv); glVertex3f(p->v[0].pos.x, p->v[0].pos.y, p->v[0].pos.z);
}

/* Box rendering (also for chars) */
void Cube_render(const Cube* c) {
    glPushMatrix();
    glTranslatef(c->x, c->y, c->z);
    glRotated(c->zRot * 180.0 / M_PI, 0, 0, 1);
    glRotated(c->yRot * 180.0 / M_PI, 0, 1, 0);
    glRotated(c->xRot * 180.0 / M_PI, 1, 0, 0);

    glBegin(GL_QUADS);
    for (int i = 0; i < 6; ++i) Polygon_render(&c->polys[i]);
    glEnd();

    glPopMatrix();
}

/* Start/End of model bullcrap */
void startModel_Matrix() {
    glPushMatrix();
    glEnable(GL_TEXTURE_2D);
}
void   endModel_Matrix() {
    glDisable(GL_TEXTURE_2D);
    glPopMatrix();
}

/* Model position */
void setModel_position(double ix, double iy, double iz) {
    glTranslated(ix, iy, iz);
    glScalef(1.f, -1.f, 1.f);
}

void setModel_positionOffset(double ix, double iy, double iz) {
    glTranslated(ix, iy, iz);
}

/* Model Scale */
void setModel_scale(float scale) {
    glScalef(scale, scale, scale);
}

/* Model Rotation */
void setModel_rotation(float rx, float ry, float rz) {
    glRotatef(rz, 0, 0, 1);
    glRotatef(ry, 0, 1, 0);
    glRotatef(rx, 1, 0, 0);
}