//
// Created by novakdominikviktor on 2026. 02. 28..
//

#ifndef SOLARIS_CAMERA_H
#define SOLARIS_CAMERA_H
#include "scene.h"

#include <GL/gl.h>
struct World;

typedef struct {
    float x, y, z;
    float pitch; // fel-le nézésre
    float yaw; // bal-jobbra nézésre
} Camera;

void init_camera(Camera* camera);
void set_view(Camera* camera);
void update_camera_position(Camera* camera, float dx, float dy, float dz, struct World* world);

#endif //SOLARIS_CAMERA_H