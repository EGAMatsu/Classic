// character/polygon.c

#include "polygon.h"
#include <GL/glew.h>

void Polygon_init_uv(Polygon* p, Vertex a, Vertex b, Vertex c, Vertex d,
                     int minU, int minV, int maxU, int maxV) {
    p->v[0] = Vertex_remap(a, maxU, minV);
    p->v[1] = Vertex_remap(b, minU, minV);
    p->v[2] = Vertex_remap(c, minU, maxV);
    p->v[3] = Vertex_remap(d, maxU, maxV);
}