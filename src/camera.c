//
// Created by novakdominikviktor on 2026. 02. 28..
//
#include "camera.h"
#include <math.h>
#include <GL/gl.h>

void init_camera(Camera* camera) {
    camera->x = 0.0f;
    camera->y = 0.0f;
    camera->z = 10.0f; // középonttól hátrébb állunk
    camera->pitch = 0.0f;
    camera->yaw = 0.0f; // felfele legyen a középpont
}

void set_view(Camera* camera) {
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    //printf("Camera pos: %f, %f, %f\n", camera->x, camera->y, camera->z);
    glRotatef(-camera->pitch, 1.0f, 0.0f, 0.0f);
    glRotatef(-camera->yaw, 0.0f, 1.0f, 0.0f);

    glTranslatef(-camera->x, -camera->y, -camera->z);

}