// particle/particle_engine.c

#include "particle_engine.h"
#include <stdlib.h>
#include <math.h>
#include "../renderer/textures.h"
#include "../player.h"

static void ensure_capacity(ParticleEngine* pe, int need) {
    if (pe->capacity >= need) return;
    int newcap = pe->capacity ? pe->capacity * 2 : 64;
    if (newcap < need) newcap = need;
    pe->items = (Particle*)realloc(pe->items, (size_t)newcap * sizeof(Particle));
    pe->capacity = newcap;
}

void ParticleEngine_init(ParticleEngine* pe, Level* level, unsigned int terrainTex) {
    pe->level = level;
    pe->texture = terrainTex;
    pe->items = NULL;
    pe->count = pe->capacity = 0;
    Tessellator_init(&pe->tess);
}

void ParticleEngine_add(ParticleEngine* pe, const Particle* p) {
    ensure_capacity(pe, pe->count + 1);
    pe->items[pe->count++] = *p;
}

void ParticleEngine_onTick(ParticleEngine* pe) {
    for (int i = 0; i < pe->count; ) {
        Particle_onTick(&pe->items[i]);
        if (pe->items[i].base.removed) {
            pe->items[i] = pe->items[pe->count - 1];
            pe->count--;
        } else {
            i++;
        }
    }
}

void ParticleEngine_render(ParticleEngine* pe, const Player* player, float partialTicks, int layer) {
    if (pe->count == 0) return;

    setTexture(pe->texture);

    meshArray_disableColor();
    bool cullWasEnabled = doingCulling();
    doCulling(!cullWasEnabled);

    // --- NEW camera vectors (match Java commit) ---
    float yawRad   = player->e.yRotation * (float)M_PI / 180.0f;
    float pitchRad = player->e.xRotation * (float)M_PI / 180.0f;

    float cameraX = -cosf(yawRad);
    float cameraY =  cosf(pitchRad);
    float cameraZ = -sinf(yawRad);

    float cameraXWithY = -cameraZ * sinf(pitchRad);
    float cameraZWithY =  cameraX * sinf(pitchRad);
    // ----------------------------------------------

    //setModel_color(0.8f, 0.8f, 0.8f, 1.0f);

    Tessellator_init(&pe->tess);
    for (int i = 0; i < pe->count; ++i) {
        Particle* p = &pe->items[i];
        int lit = Entity_isLit(&p->base) ? 1 : 0;
        int inLayer = (layer == 1) ? 1 : 0;
        if ((lit ^ inLayer) == 1) {
            Particle_render(p, &pe->tess, partialTicks,
                            cameraX, cameraY, cameraZ,
                            cameraXWithY, cameraZWithY);
        }
    }
    Tessellator_flush(&pe->tess);

    doCulling(cullWasEnabled);
    disableTexture();
}