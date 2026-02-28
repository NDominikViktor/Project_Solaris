//
// Created by novakdominikviktor on 2026. 02. 28..
//

#ifndef SCENE_H
#define SCENE_H
#include <GL/gl.h>

typedef struct {
    char name[32];
    float distance;
    float size;
    float orbit_speed;
    float rotation_speed;
    float current_angle;
    float rotation_angle; // saját maga körrüli forgás
    float axial_tilt;  // Tengelyferdeség foka
    GLuint texture_id;
} Planet;

typedef struct {
    Planet planets[10];
    int count;
} World;

void load_planets(World* world, const char* filename);

#endif //SOLARIS_SCENE_H