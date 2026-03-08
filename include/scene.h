//
// Created by novakdominikviktor on 2026. 02. 28..
//

#ifndef SCENE_H
#define SCENE_H
#include <GL/gl.h>

typedef struct {
    float x, y, z;
    float size;
    float angle; // a gyűrű helyzete
    float distance; // a távolsága a bolygó közepétől
    float speed; // keringési sebesség a gyűrűn belül
} Particle;

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
    int has_atmosphere;
    float atmo_r, atmo_g, atmo_b;
    Particle* ring_particles;
    int particle_count;
    int parent_index; // Index of the parent planet in the world.planets array
    float world_x, world_y, world_z; // Absolute world coordinates
} Planet;

typedef struct World{
    Planet planets[20]; // Increased size to accommodate moons
    int count;
} World;

void load_planets(World* world, const char* filename);
void init_ring_particles(Planet* p);
void draw_ring_particles(Planet* p);

#endif //SOLARIS_SCENE_H