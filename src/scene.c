//
// Created by novakdominikviktor on 2026. 02. 28..
//
#include "scene.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#define STB_IMAGE_IMPLEMENTATION
#include <GL/glu.h>
#include "camera.h"

#include "stb_image.h"

GLuint load_texture(const char* filename) {
    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    int width, height, nrChannels;
    // Force 4 channels (RGBA) to avoid buffer overread with grayscale images
    unsigned char *data = stbi_load(filename, &width, &height, &nrChannels, 4);
    if (data) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        stbi_image_free(data);
    } else {
        printf("Texture loading error: %s\n", filename);
    }
    return textureID;
}

void load_planets(World* world, const char* filename) {
    FILE* file = fopen(filename, "r");
    if (!file) {
        printf("Error: I can't find the %s file!\n", filename);
        return;
    }
    char line[256];
    world->count = 0;
    while (fgets(line, sizeof(line), file) && world->count < 20) {
        if (line[0] == '#') continue;

        char texture_name[64];
        char parent_name[32] = "";
        Planet* p = &world->planets[world->count];

        // Remove newline at the end of line if present
        line[strcspn(line, "\r\n")] = 0;

        // name,distance,size,orbit_speed,rotation_speed,axial_tilt,texture,has_atmo,atmo_r,atmo_g,atmo_b,has_rings,parent
        int fields = sscanf(line, "%[^,],%f,%f,%f,%f,%f,%[^,],%d,%f,%f,%f,%d,%[^,\n]",
                   p->name, &p->distance, &p->size, &p->orbit_speed, &p->rotation_speed,
                   &p->axial_tilt, texture_name, &p->has_atmosphere,
                   &p->atmo_r, &p->atmo_g, &p->atmo_b, &p->has_rings, parent_name);

        if (fields >= 11) {
            p->current_angle  = 0.0f;
            p->rotation_angle = 0.0f;
            p->ring_particles = NULL;
            p->particle_count = 0;
            p->world_x = 0.0f;
            p->world_y = 0.0f;
            p->world_z = 0.0f;

            // Handle legacy format or missing fields
            if (fields < 12) {
                // Hardcoded fallback for Saturn and Uranus if field is missing
                if (strcmp(p->name, "Saturn") == 0 || strcmp(p->name, "Uranus") == 0) {
                    p->has_rings = 1;
                } else {
                    p->has_rings = 0;
                }
            }

            char full_path[128];
            sprintf(full_path, "assets/%s", texture_name);
            p->texture_id = load_texture(full_path);
            strncpy(p->texture_name, texture_name, sizeof(p->texture_name)-1);

            // Resolve parent index
            p->parent_index = -1;
            const char* actual_parent = (fields >= 13) ? parent_name : (fields == 12 ? parent_name : "");
            if (strlen(actual_parent) > 0) {
                for (int j = 0; j < world->count; j++) {
                    if (strcmp(world->planets[j].name, actual_parent) == 0) {
                        p->parent_index = j;
                        break;
                    }
                }
            }

            if (p->has_rings) {
                init_ring_particles(p);
            }

            world->count++;
        }
    }
    fclose(file);
    printf("Loaded %d planets from %s\n", world->count, filename);
}

void draw_skybox(GLuint texture_id) {
    float size = 500.0f;
    GLboolean fog_was_enabled = glIsEnabled(GL_FOG);
    glDisable(GL_FOG);
    glDisable(GL_LIGHTING);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, texture_id);
    glColor3f(1.0f, 1.0f, 1.0f);

    glBegin(GL_QUADS);
    glTexCoord2f(0, 0); glVertex3f(-size, -size, -size);
    glTexCoord2f(1, 0); glVertex3f( size, -size, -size);
    glTexCoord2f(1, 1); glVertex3f( size,  size, -size);
    glTexCoord2f(0, 1); glVertex3f(-size,  size, -size);

    glTexCoord2f(0, 0); glVertex3f( size, -size,  size);
    glTexCoord2f(1, 0); glVertex3f(-size, -size,  size);
    glTexCoord2f(1, 1); glVertex3f(-size,  size,  size);
    glTexCoord2f(0, 1); glVertex3f( size,  size,  size);

    glTexCoord2f(0, 0); glVertex3f(-size, -size,  size);
    glTexCoord2f(1, 0); glVertex3f(-size, -size, -size);
    glTexCoord2f(1, 1); glVertex3f(-size,  size, -size);
    glTexCoord2f(0, 1); glVertex3f(-size,  size,  size);

    glTexCoord2f(0, 0); glVertex3f( size, -size, -size);
    glTexCoord2f(1, 0); glVertex3f( size, -size,  size);
    glTexCoord2f(1, 1); glVertex3f( size,  size,  size);
    glTexCoord2f(0, 1); glVertex3f( size,  size, -size);

    glTexCoord2f(0, 0); glVertex3f(-size,  size, -size);
    glTexCoord2f(1, 0); glVertex3f( size,  size, -size);
    glTexCoord2f(1, 1); glVertex3f( size,  size,  size);
    glTexCoord2f(0, 1); glVertex3f(-size,  size,  size);

    glTexCoord2f(0, 0); glVertex3f(-size, -size,  size);
    glTexCoord2f(1, 0); glVertex3f( size, -size,  size);
    glTexCoord2f(1, 1); glVertex3f( size, -size, -size);
    glTexCoord2f(0, 1); glVertex3f(-size, -size, -size);
    glEnd();

    glEnable(GL_LIGHTING);
    if (fog_was_enabled) glEnable(GL_FOG);
}

void draw_atmosphere(float size, float r, float g, float b, float alpha) {
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDisable(GL_LIGHTING);

    glColor4f(r, g, b, alpha);

    GLUquadric* quad = gluNewQuadric();
    gluSphere(quad, size * 1.05f, 32, 32);
    gluDeleteQuadric(quad);

    glEnable(GL_LIGHTING);
    glDisable(GL_BLEND);
    glColor3f(1.0f, 1.0f, 1.0f);
}

void init_ring_particles(Planet* p) {
    if (p->ring_particles) free(p->ring_particles);
    p->particle_count = 2000;
    p->ring_particles = malloc(p->particle_count * sizeof(Particle));
    if (!p->ring_particles) {
        printf("Error: failed to allocate ring particles for %s\n", p->name);
        p->particle_count = 0;
        return;
    }

    float inner = (strcmp(p->name, "Uranus") == 0) ? 1.5f : 1.3f;
    float outer = (strcmp(p->name, "Uranus") == 0) ? 1.7f : 2.1f;

    for (int i = 0; i < p->particle_count; i++) {
        Particle* part = &p->ring_particles[i];

        part->angle    = (float)(rand() % 360);
        float r        = ((float)rand() / (float)RAND_MAX) * (outer - inner) + inner;
        part->distance = p->size * r;
        part->size     = 0.02f;
        part->speed    = 0.1f + ((float)rand() / (float)RAND_MAX) * 0.2f;

        part->y = ((float)rand() / (float)RAND_MAX) * 0.04f - 0.02f;
    }
}

void free_ring_particles(Planet* p) {
    if (p->ring_particles) {
        free(p->ring_particles);
        p->ring_particles = NULL;
        p->particle_count = 0;
    }
}

void draw_ring_particles(Planet* p) {
    if (!p->ring_particles || p->particle_count == 0) return;

    glDisable(GL_TEXTURE_2D);
    glDisable(GL_LIGHTING);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glPointSize(2.0f);

    int is_uranus = (strcmp(p->name, "Uranus") == 0);

    glBegin(GL_POINTS);
    for (int i = 0; i < p->particle_count; i++) {
        Particle* part = &p->ring_particles[i];
        float rad = part->angle * (float)M_PI / 180.0f;
        float px  = cosf(rad) * part->distance;
        float pz  = sinf(rad) * part->distance;

        if (is_uranus)
            glColor4f(0.75f, 0.85f, 0.9f, 0.55f);
        else
            glColor4f(0.9f, 0.80f, 0.55f, 0.65f);

        glVertex3f(px, part->y, pz);
    }
    glEnd();

    glPointSize(1.0f);
    glEnable(GL_LIGHTING);
    glDisable(GL_BLEND);
    glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
}

void init_asteroid_belt(Asteroid* asteroid_belt) {
    for (int i = 0; i < MAX_ASTEROID; i++) {
        float r = 14.5f + ((float)rand() / (float)RAND_MAX) * 4.5f;
        asteroid_belt[i].distance = r;
        asteroid_belt[i].angle = (float)(rand() % 360);
        asteroid_belt[i].size = 0.02f + ((float)rand() / (float)RAND_MAX) * 0.05f;
        asteroid_belt[i].orbit_speed = 0.005f + ((float)rand() / (float)RAND_MAX) * 0.01f;
        asteroid_belt[i].y = ((float)rand() / (float)RAND_MAX) * 0.4f - 0.2f;

        // Flattened shape
        asteroid_belt[i].scale_x = 0.5f + ((float)rand() / (float)RAND_MAX) * 1.0f;
        asteroid_belt[i].scale_y = 0.5f + ((float)rand() / (float)RAND_MAX) * 1.0f;
        asteroid_belt[i].scale_z = 0.5f + ((float)rand() / (float)RAND_MAX) * 1.0f;

        // Self-rotation
        asteroid_belt[i].rot_angle = (float)(rand() % 360);
        asteroid_belt[i].rot_speed = 0.2f + ((float)rand() / (float)RAND_MAX) * 0.8f;
        asteroid_belt[i].rot_axis_x = ((float)rand() / (float)RAND_MAX);
        asteroid_belt[i].rot_axis_y = ((float)rand() / (float)RAND_MAX);
        asteroid_belt[i].rot_axis_z = ((float)rand() / (float)RAND_MAX);

        // Color: grey, brownish, reddish
        float base = 0.35f + ((float)rand() / (float)RAND_MAX) * 0.3f;
        int color_type = rand() % 3;
        if (color_type == 0) {
            // grey
            asteroid_belt[i].color_r = base;
            asteroid_belt[i].color_g = base;
            asteroid_belt[i].color_b = base;
        } else if (color_type == 1) {
            // brownish
            asteroid_belt[i].color_r = base + 0.15f;
            asteroid_belt[i].color_g = base * 0.8f;
            asteroid_belt[i].color_b = base * 0.6f;
        } else {
            // reddish
            asteroid_belt[i].color_r = base + 0.2f;
            asteroid_belt[i].color_g = base * 0.6f;
            asteroid_belt[i].color_b = base * 0.5f;
        }

        // Elliptical orbit
        asteroid_belt[i].orbit_eccentricity = 0.8f + ((float)rand() / (float)RAND_MAX) * 0.4f;
    }
}

void draw_asteroid_belt(Asteroid* asteroid_belt) {
    for (int i = 0; i < MAX_ASTEROID; i++) {
        glPushMatrix();

        float rad = asteroid_belt[i].angle * (M_PI / 180.0f);
        float x = cosf(rad) * asteroid_belt[i].distance;
        float z = sinf(rad) * asteroid_belt[i].distance * asteroid_belt[i].orbit_eccentricity;

        glTranslatef(x, asteroid_belt[i].y, z);

        // Self-rotation
        glRotatef(asteroid_belt[i].rot_angle,
                  asteroid_belt[i].rot_axis_x,
                  asteroid_belt[i].rot_axis_y,
                  asteroid_belt[i].rot_axis_z);

        // Flattened shape
        glScalef(asteroid_belt[i].scale_x,
                 asteroid_belt[i].scale_y,
                 asteroid_belt[i].scale_z);

        glColor3f(asteroid_belt[i].color_r,
                  asteroid_belt[i].color_g,
                  asteroid_belt[i].color_b);

        GLUquadric* q = gluNewQuadric();
        gluSphere(q, asteroid_belt[i].size, 6, 6);
        gluDeleteQuadric(q);
        glPopMatrix();

        asteroid_belt[i].angle += asteroid_belt[i].orbit_speed;
        asteroid_belt[i].rot_angle += asteroid_belt[i].rot_speed;
    }
}

// Calculate distance between two 3D points
float dist3D(float x1, float y1, float z1, float x2, float y2, float z2) {
    return sqrtf(powf(x2 - x1, 2) + powf(y2 - y1, 2) + powf(z2 - z1, 2));
}

// Ray-sphere intersection test
bool ray_sphere_intersection(Vec3 origin, Vec3 dir, Vec3 sphere_pos, float radius) {
    Vec3 oc = {origin.x - sphere_pos.x, origin.y - sphere_pos.y, origin.z - sphere_pos.z};
    float b = 2.0f * (oc.x * dir.x + oc.y * dir.y + oc.z * dir.z);
    float c = (oc.x * oc.x + oc.y * oc.y + oc.z * oc.z) - radius * radius;
    float discriminant = b * b - 4 * c;
    return (discriminant > 0);
}

int pick_planet(int mouseX, int mouseY, void* cam_ptr, void* world_ptr) {
    // Cast void pointers back to their concrete types
    Camera* cam = (Camera*)cam_ptr;
    World* world = (World*)world_ptr;

    GLint viewport[4];
    GLdouble modelview[16];
    GLdouble projection[16];
    GLdouble posX, posY, posZ, farX, farY, farZ;

    glGetIntegerv(GL_VIEWPORT, viewport);
    glGetDoublev(GL_MODELVIEW_MATRIX, modelview);
    glGetDoublev(GL_PROJECTION_MATRIX, projection);

    float winX = (float)mouseX;
    float winY = (float)viewport[3] - (float)mouseY;

    gluUnProject(winX, winY, 0.0, modelview, projection, viewport, &posX, &posY, &posZ);
    gluUnProject(winX, winY, 1.0, modelview, projection, viewport, &farX, &farY, &farZ);

    Vec3 ray_origin = { (float)posX, (float)posY, (float)posZ };
    Vec3 ray_dir = { (float)(farX - posX), (float)(farY - posY), (float)(farZ - posZ) };

    float len = sqrtf(ray_dir.x * ray_dir.x + ray_dir.y * ray_dir.y + ray_dir.z * ray_dir.z);
    if (len > 0) {
        ray_dir.x /= len; ray_dir.y /= len; ray_dir.z /= len;
    }


    for (int i = 0; i < world->count; i++) {
        Vec3 planet_pos = { world->planets[i].world_x, world->planets[i].world_y, world->planets[i].world_z };
        // Inflate radius by 1.5x to make picking easier
        if (ray_sphere_intersection(ray_origin, ray_dir, planet_pos, world->planets[i].size * 1.5f)) {
            printf("Planet selected: %s", world->planets[i].name);
            return i;
        }
    }
    return -1;
}

void draw_moon_shadows(World* world) {
    // For each moon: project a soft shadow disc onto the parent planet surface.
    // The shadow position is found by intersecting the Sun->moon ray with the
    // parent sphere. Only draws if the moon is actually between the Sun and planet.

    glDisable(GL_LIGHTING);
    glDisable(GL_TEXTURE_2D);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDepthMask(GL_FALSE);

    for (int i = 0; i < world->count; i++) {
        Planet* moon = &world->planets[i];
        if (moon->parent_index < 0) continue;
        Planet* parent = &world->planets[moon->parent_index];
        // Skip if parent is the Sun (distance ~0, no parent)
        if (parent->parent_index < 0 && parent->distance < 0.1f) continue;

        // Sun is at world origin.
        // Ray from Sun through moon: origin=0, dir = normalize(moon_pos)
        float mx = moon->world_x, my = moon->world_y, mz = moon->world_z;
        float md = sqrtf(mx*mx + my*my + mz*mz);
        if (md < 0.001f) continue;
        float dx = mx/md, dy = my/md, dz = mz/md;

        // Intersect this ray with the parent sphere.
        // Ray: P(t) = t * (dx,dy,dz)
        // Sphere: |P - C|^2 = R^2   where C = parent centre
        float cx = parent->world_x, cy = parent->world_y, cz = parent->world_z;
        float R  = parent->size;

        // Quadratic: t^2 - 2t(d.C) + (|C|^2 - R^2) = 0
        float b2 = dx*cx + dy*cy + dz*cz;   // half b
        float c  = cx*cx + cy*cy + cz*cz - R*R;
        float disc = b2*b2 - c;
        if (disc < 0) continue;             // ray misses planet

        // We want the intersection closer to the Sun (smaller t)
        float t = b2 - sqrtf(disc);
        if (t < 0) t = b2 + sqrtf(disc);
        if (t < 0) continue;

        // Shadow only makes sense if the moon is farther from Sun than the hit point
        // i.e. the moon is on the far side (past the planet from the Sun)
        if (t > md) continue;

        // Shadow centre on the sphere
        float sx2 = t*dx, sy2 = t*dy, sz2 = t*dz;

        // Normal at shadow centre (pointing outward from planet centre)
        float nx = sx2 - cx, ny = sy2 - cy, nz = sz2 - cz;
        float nl = sqrtf(nx*nx + ny*ny + nz*nz);
        if (nl < 0.001f) continue;
        nx /= nl; ny /= nl; nz /= nl;

        // Push slightly above surface to avoid z-fighting
        float ox = sx2 + nx * 0.02f;
        float oy = sy2 + ny * 0.02f;
        float oz = sz2 + nz * 0.02f;

        // Build two tangent vectors for the disc
        float ux = 0.0f, uy = 1.0f, uz = 0.0f;
        if (fabsf(ny) > 0.9f) { ux = 1.0f; uy = 0.0f; }
        // tangent1 = cross(normal, up)
        float t1x = ny*uz - nz*uy, t1y = nz*ux - nx*uz, t1z = nx*uy - ny*ux;
        float t1l = sqrtf(t1x*t1x + t1y*t1y + t1z*t1z);
        if (t1l < 0.001f) continue;
        t1x /= t1l; t1y /= t1l; t1z /= t1l;
        // tangent2 = cross(normal, tangent1)
        float t2x = ny*t1z - nz*t1y;
        float t2y = nz*t1x - nx*t1z;
        float t2z = nx*t1y - ny*t1x;

        // Shadow radius scales with moon size; fade over 3 soft layers
        float base_r = moon->size * 0.85f;
        int   steps  = 40;

        for (int layer = 2; layer >= 0; layer--) {
            float lr    = base_r * (1.0f + layer * 0.5f);
            float alpha = 0.50f * (1.0f - layer * 0.30f);
            glColor4f(0.0f, 0.0f, 0.0f, alpha);
            glBegin(GL_TRIANGLE_FAN);
            glVertex3f(ox, oy, oz);
            for (int s = 0; s <= steps; s++) {
                float a  = s * 2.0f * (float)M_PI / steps;
                float px = ox + lr * (cosf(a)*t1x + sinf(a)*t2x);
                float py = oy + lr * (cosf(a)*t1y + sinf(a)*t2y);
                float pz = oz + lr * (cosf(a)*t1z + sinf(a)*t2z);
                glVertex3f(px, py, pz);
            }
            glEnd();
        }
    }

    glDepthMask(GL_TRUE);
    glEnable(GL_LIGHTING);
    glDisable(GL_BLEND);
    glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
}