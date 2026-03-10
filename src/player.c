// player.c — player base

#include "player.h"
#include "dsp_renderer.h"
#include <math.h>

void Player_init(Player* p, Level* level) {
    Entity_init(&p->e, level);
    p->e.heightOffset = 1.62f;
}

void Player_turn(Player* p, float dx, float dy) {
    // Entity_turn handles the rotation math
    Entity_turn(&p->e, dx, dy);
}

void Player_onTick(Player* p) {
    Entity_onTick(&p->e);

    float forward = 0.0f, strafe = 0.0f;

    if (getKey(buttonRespawn)) {
        Entity_resetPosition(&p->e);
    }

    if (getKey(    forwardButton[0]) || getKey(    forwardButton[1])    ) forward -= 1.0f;
    if (getKey(   backwardButton[0]) || getKey(   backwardButton[1])    ) forward += 1.0f;
    if (getKey( strafeLeftButton[0]) || getKey( strafeLeftButton[1])    ) strafe  -= 1.0f;
    if (getKey(strafeRightButton[0]) || getKey(strafeRightButton[1])    ) strafe  += 1.0f;

    if (getKey(buttonJump) && p->e.onGround) {
        p->e.motionY = 0.5f;
    }

    // Movement physics
    Entity_moveRelative(&p->e, strafe, forward, p->e.onGround ? 0.1f : 0.02f);

    p->e.motionY -= 0.08f; // gravity

    Entity_move(&p->e, p->e.motionX, p->e.motionY, p->e.motionZ);

    p->e.motionX *= 0.91f;
    p->e.motionY *= 0.98f;
    p->e.motionZ *= 0.91f;

    if (p->e.onGround) {
        p->e.motionX *= 0.7f;
        p->e.motionZ *= 0.7f;
    }
}