//
// Created by novakdominikviktor on 2026. 02. 28..
//

#ifndef SCENE_H
#define SCENE_H
#include <math.h>
#include <GL/gl.h>
#include <stdbool.h>


struct Camera;

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

typedef struct {
    float x, y, z;
    float size;
    float angle;
    float distance;
    float orbit_speed;
} Asteroid;

typedef struct {
    float x, y, z;
} Vec3;


#define MAX_ASTEROID 500
extern Asteroid asteroid_belt[MAX_ASTEROID];

void load_planets(World* world, const char* filename);
void init_ring_particles(Planet* p);
void draw_ring_particles(Planet* p);
// scene.h-ban így nézzen ki:
void pick_planet(int mouseX, int mouseY, void* cam_ptr, void* world_ptr);
bool ray_sphere_intersection(Vec3 origin, Vec3 dir, Vec3 sphere_pos, float radius);
void init_asteroid_belt();
void draw_asteroid_belt();

#endif //SOLARIS_SCENE_H