//
// Created by novakdominikviktor on 2026. 02. 28..
//
#include "scene.h"
#include "camera.h"
#include <math.h>
#include <GL/gl.h>
#include <math.h>

void init_camera(Camera* camera) {
    camera->x = 0.0f;
    camera->y = 0.0f;
    camera->z = 10.0f; // start behind the origin
    camera->pitch = 0.0f;
    camera->yaw = 0.0f; // look toward the origin
}

void set_view(Camera* camera) {
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    //printf("Camera pos: %f, %f, %f\n", camera->x, camera->y, camera->z);
    glRotatef(-camera->pitch, 1.0f, 0.0f, 0.0f);
    glRotatef(-camera->yaw, 0.0f, 1.0f, 0.0f);

    glTranslatef(-camera->x, -camera->y, -camera->z);
}

void update_camera_position(Camera* camera, float dx, float dy, float dz, struct World* world) {
    float newX = camera->x + dx;
    float newY = camera->y + dy;
    float newZ = camera->z + dz;

    // scene.h is included above so World is a complete type here
    for (int i = 0; i < world->count; i++) {
        Planet* p = &world->planets[i];


        float dist = sqrtf((newX - p->world_x) * (newX - p->world_x) +
                           (newY - p->world_y) * (newY - p->world_y) +
                           (newZ - p->world_z) * (newZ - p->world_z));

        if (dist < (p->size + 0.5f)) {
            return; // Collision detected: abort position update
        }
    }

    camera->x = newX;
    camera->y = newY;
    camera->z = newZ;
}