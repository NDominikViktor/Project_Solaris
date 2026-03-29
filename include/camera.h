//
// Created by novakdominikviktor on 2026. 02. 28..
//

#ifndef SOLARIS_CAMERA_H
#define SOLARIS_CAMERA_H

#include "scene.h"
#include <GL/gl.h>

struct World;

/**
 * @brief First-person camera in 3D world space.
 *
 * Position is stored in world coordinates.
 * Orientation is expressed as yaw (left/right) and pitch (up/down) in degrees.
 */
typedef struct {
    float x, y, z;   /**< World-space position of the camera. */
    float pitch;      /**< Vertical rotation angle in degrees (clamped to +-89). */
    float yaw;        /**< Horizontal rotation angle in degrees. */
} Camera;

/**
 * @brief Set the camera to its default position and orientation.
 *
 * Places the camera slightly behind the origin, looking toward it.
 *
 * @param camera Pointer to the Camera to initialise.
 */
void init_camera(Camera* camera);

/**
 * @brief Apply the camera transform to the OpenGL MODELVIEW matrix.
 *
 * Must be called once per frame before any scene geometry is drawn.
 *
 * @param camera Pointer to the active Camera.
 */
void set_view(Camera* camera);

/**
 * @brief Move the camera by (dx, dy, dz), respecting planet collision.
 *
 * If the resulting position would overlap any planet's bounding sphere
 * the move is cancelled and the camera stays in place.
 *
 * @param camera Pointer to the Camera to move.
 * @param dx     Displacement along the X axis.
 * @param dy     Displacement along the Y axis.
 * @param dz     Displacement along the Z axis.
 * @param world  Pointer to the World used for collision testing.
 */
void update_camera_position(Camera* camera, float dx, float dy, float dz, struct World* world);

#endif /* SOLARIS_CAMERA_H */