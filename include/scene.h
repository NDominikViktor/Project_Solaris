//
// Created by novakdominikviktor on 2026. 02. 28..
//

#ifndef SCENE_H
#define SCENE_H

#include <math.h>
#include <GL/gl.h>
#include <stdbool.h>

struct Camera;

/** @brief A single ring particle orbiting a planet. */
typedef struct {
    float x, y, z;      /**< Current world-space position (computed each frame). */
    float size;          /**< Visual radius of the particle. */
    float angle;         /**< Current orbital angle in degrees. */
    float distance;      /**< Orbital radius from the planet centre. */
    float speed;         /**< Angular speed in degrees per frame. */
} Particle;

/** @brief All data describing one planet (or moon) in the simulation. */
typedef struct {
    char  name[32];          /**< Display name, also used for special-case logic. */
    float distance;          /**< Orbital radius from the parent body. */
    float size;              /**< Sphere radius used for rendering and collision. */
    float orbit_speed;       /**< Orbital angular speed (radians per update). */
    float rotation_speed;    /**< Self-rotation speed (degrees per update). */
    float current_angle;     /**< Current orbital angle in radians. */
    float rotation_angle;    /**< Current self-rotation angle in degrees. */
    float axial_tilt;        /**< Axial tilt in degrees. */
    GLuint texture_id;       /**< OpenGL texture handle. */
    int   has_atmosphere;    /**< Non-zero if an atmosphere halo should be drawn. */
    float atmo_r, atmo_g, atmo_b; /**< Atmosphere colour (RGB, 0-1). */
    Particle* ring_particles;     /**< Heap-allocated ring particle array, or NULL. */
    int   particle_count;    /**< Number of elements in ring_particles. */
    int   parent_index;      /**< Index of parent in World.planets, or -1 for root. */
    float world_x, world_y, world_z; /**< Absolute world-space position (computed each frame). */
} Planet;

/** @brief Container for all planets and moons in the scene. */
typedef struct World {
    Planet planets[20]; /**< Fixed-size array of planets/moons. */
    int    count;       /**< Number of active entries in planets[]. */
} World;

/** @brief One asteroid in the belt. */
typedef struct {
    float x, y, z;              /**< World-space position; y is the vertical offset from the ecliptic. */
    float size;                  /**< Sphere radius. */
    float angle;                 /**< Current orbital angle in degrees. */
    float distance;              /**< Orbital radius from the Sun. */
    float orbit_speed;           /**< Orbital angular speed in degrees per frame. */
    float scale_x, scale_y, scale_z; /**< Per-axis scale for an irregular shape. */
    float rot_angle;             /**< Current self-rotation angle in degrees. */
    float rot_speed;             /**< Self-rotation speed in degrees per frame. */
    float rot_axis_x, rot_axis_y, rot_axis_z; /**< Self-rotation axis (unit vector). */
    float color_r, color_g, color_b;           /**< Surface colour (RGB, 0-1). */
    float orbit_eccentricity;    /**< Scales the Z radius to create an elliptical orbit. */
} Asteroid;

/** @brief A generic 3D vector used for ray casting. */
typedef struct {
    float x, y, z;
} Vec3;

#define MAX_ASTEROID 500

/**
 * @brief Load planet definitions from a CSV file into the World.
 * @param world    Target World struct.
 * @param filename Path to the CSV file (e.g. "assets/planets.csv").
 */
void load_planets(World* world, const char* filename);

/**
 * @brief Allocate and initialise ring particles for a planet.
 *
 * Must be called once after load_planets() for any planet that should
 * display a particle ring (Saturn, Uranus).
 *
 * @param p Pointer to the Planet that owns the ring.
 */
void init_ring_particles(Planet* p);

/**
 * @brief Draw all ring particles belonging to a planet.
 *
 * Assumes the current matrix is already translated to the planet centre.
 *
 * @param p Pointer to the Planet whose ring should be drawn.
 */
void draw_ring_particles(Planet* p);

/**
 * @brief Select a planet by casting a ray through the given screen pixel.
 *
 * @param mouseX    Screen X coordinate of the click.
 * @param mouseY    Screen Y coordinate of the click.
 * @param cam_ptr   Pointer to the active Camera (passed as void* to avoid circular include).
 * @param world_ptr Pointer to the World (passed as void*).
 * @return Index of the hit planet in World.planets, or -1 if nothing was hit.
 */
int pick_planet(int mouseX, int mouseY, void* cam_ptr, void* world_ptr);

/**
 * @brief Test whether a ray intersects a sphere.
 * @param origin     Ray origin.
 * @param dir        Ray direction (must be normalised).
 * @param sphere_pos Centre of the sphere.
 * @param radius     Radius of the sphere.
 * @return true if the ray intersects the sphere, false otherwise.
 */
bool ray_sphere_intersection(Vec3 origin, Vec3 dir, Vec3 sphere_pos, float radius);

/**
 * @brief Randomise and initialise all asteroids in the belt array.
 * @param asteroid_belt Pointer to an array of MAX_ASTEROID Asteroid elements.
 */
void init_asteroid_belt(Asteroid* asteroid_belt);

/**
 * @brief Animate and draw all asteroids in the belt array.
 * @param asteroid_belt Pointer to an array of MAX_ASTEROID Asteroid elements.
 */
void draw_asteroid_belt(Asteroid* asteroid_belt);

#endif /* SCENE_H */