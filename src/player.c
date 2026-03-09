// player.c — player base

#include "player.h"
#include <GL/glfw.h>
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

    if (glfwGetKey('R') == GLFW_PRESS) {
        Entity_resetPosition(&p->e);
    }

    if (glfwGetKey('W') == GLFW_PRESS || glfwGetKey(GLFW_KEY_UP)    == GLFW_PRESS) forward -= 1.0f;
    if (glfwGetKey('S') == GLFW_PRESS || glfwGetKey(GLFW_KEY_DOWN)  == GLFW_PRESS) forward += 1.0f;
    if (glfwGetKey('A') == GLFW_PRESS || glfwGetKey(GLFW_KEY_LEFT)  == GLFW_PRESS) strafe  -= 1.0f;
    if (glfwGetKey('D') == GLFW_PRESS || glfwGetKey(GLFW_KEY_RIGHT) == GLFW_PRESS) strafe  += 1.0f;

    if (glfwGetKey(GLFW_KEY_SPACE) == GLFW_PRESS && p->e.onGround) {
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