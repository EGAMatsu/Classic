// particle/particle_engine.h

#ifndef PARTICLE_ENGINE_H
#define PARTICLE_ENGINE_H

struct Level;   typedef struct Level Level;
struct Player;  typedef struct Player Player;

#include "particle.h"
#include "../renderer/tessellator.h"

typedef struct ParticleEngine {
    Level* level;
    unsigned int texture;
    Particle* items;
    int count, capacity;
    Tessellator tess;
} ParticleEngine;

void ParticleEngine_init  (ParticleEngine* pe, Level* level, unsigned int terrainTex);
void ParticleEngine_onTick(ParticleEngine* pe);
void ParticleEngine_render(ParticleEngine* pe, const Player* player, float partialTicks, int layer);

void ParticleEngine_add(ParticleEngine* pe, const Particle* p);

#endif