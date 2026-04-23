//
// Created by novakdominikviktor on 2026. 02. 28..
//

#ifndef SOLARIS_COMET_H
#define SOLARIS_COMET_H

#include <GL/gl.h>

/** @brief A single vertex loaded from a Wavefront OBJ file. */
typedef struct { float x, y, z; } Vertex;

/** @brief A triangular face referencing three vertex indices. */
typedef struct { int v[3]; } Face;

/**
 * @brief A simple triangle-mesh model loaded from a Wavefront OBJ file.
 *
 * Only vertex positions (v) and triangular faces (f) are supported.
 */
typedef struct {
    Vertex* vertices;   /**< Heap-allocated vertex array. */
    Face*   faces;      /**< Heap-allocated face array. */
    int     vertex_count;
    int     face_count;
    int     initialized; /**< Non-zero after a successful load. */
} OBJModel;

#define MAX_TRAIL_POINTS 128

/** @brief A single point in the comet's particle trail. */
typedef struct {
    float x, y, z;
    float life; // 0.0 - 1.0, fades out
} TrailPoint;

/**
 * @brief A comet that follows a fixed elliptical orbit around the Sun.
 */
typedef struct {
    float x, y, z;    /**< Current world-space position. */
    float angle;       /**< Current orbital angle in radians. */
    float speed;       /**< Orbital angular speed in radians per second. */

    // Trail data
    TrailPoint trail[MAX_TRAIL_POINTS];
    int trail_head;
    int trail_length;
} Comet;

/**
 * @brief Load a Wavefront OBJ file into an OBJModel.
 *
 * Only triangular faces are supported. Quads and higher-order polygons
 * are ignored. The model is scaled at draw time, not here.
 *
 * @param filename Path to the .obj file.
 * @param model    Target OBJModel to populate.
 */
void load_asteroid_obj(const char* filename, OBJModel* model);

/**
 * @brief Draw an OBJModel as solid triangles.
 * @param model Pointer to an initialised OBJModel.
 * @param scale Uniform scale factor applied before drawing.
 */
void draw_obj_model(OBJModel* model, float scale);

/**
 * @brief Advance and draw the comet including its particle trail.
 *
 * Updates the comet position, appends it to the trail ring-buffer,
 * draws the fading quad-strip tail, then draws the OBJ rock core.
 *
 * @param c          Pointer to the Comet state.
 * @param delta_time Frame time in seconds.
 * @param model      OBJ model used for the rock core.
 */
void draw_comet(Comet* c, float delta_time, OBJModel* model);

#endif /* SOLARIS_COMET_H */
