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
    unsigned char *data = stbi_load(filename, &width, &height, &nrChannels, 0);
    if (data) {
        GLenum format = (nrChannels == 4) ? GL_RGBA : GL_RGB;
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
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
    char line[128];
    world->count = 0;
    while (fgets(line, sizeof(line), file) && world->count < 10) {
        if (line[0] == '#') continue;

        char texture_name[64];
        Planet* p = &world->planets[world->count];

        if (sscanf(line, "%[^,],%f,%f,%f,%f,%f,%[^,], %d, %f, %f, %f, %f",
                   p->name, &p->distance, &p->size, &p->orbit_speed, &p->rotation_speed,
                   &p->axial_tilt, texture_name, &p->has_atmosphere,
                   &p->atmo_r, &p->atmo_g, &p->atmo_b) == 11) {

            p->current_angle  = 0.0f;
            p->ring_particles = NULL;   // safe default
            p->particle_count = 0;

            char full_path[128];
            sprintf(full_path, "assets/%s", texture_name);
            p->texture_id = load_texture(full_path);

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

// -------------------------------------------------------
// RING PARTICLE SYSTEM
// -------------------------------------------------------

void init_ring_particles(Planet* p) {  // fixed typo
    p->particle_count = 2000;
    p->ring_particles = malloc(p->particle_count * sizeof(Particle));
    if (!p->ring_particles) {
        printf("Error: failed to allocate ring particles for %s\n", p->name);
        p->particle_count = 0;
        return;
    }

    float inner = (strcmp(p->name, "Uranusz") == 0) ? 1.5f : 1.3f;
    float outer = (strcmp(p->name, "Uranusz") == 0) ? 1.7f : 2.1f;

    for (int i = 0; i < p->particle_count; i++) {
        Particle* part = &p->ring_particles[i];

        part->angle    = (float)(rand() % 360);
        float r        = ((float)rand() / (float)RAND_MAX) * (outer - inner) + inner;
        part->distance = p->size * r;
        part->size     = 0.02f;
        part->speed    = 0.1f + ((float)rand() / (float)RAND_MAX) * 0.2f;

        // slight vertical scatter so the ring isn't perfectly flat
        part->y = ((float)rand() / (float)RAND_MAX) * 0.04f - 0.02f;
    }
}

void draw_ring_particles(Planet* p) {
    if (!p->ring_particles || p->particle_count == 0) return;

    glDisable(GL_TEXTURE_2D);
    glDisable(GL_LIGHTING);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glPointSize(2.0f);

    // Saturn: warm golden/brown tint  Uranus: pale blue-grey tint
    int is_uranus = (strcmp(p->name, "Uranusz") == 0);

    glBegin(GL_POINTS);
    for (int i = 0; i < p->particle_count; i++) {
        Particle* part = &p->ring_particles[i];
        float rad = part->angle * (float)M_PI / 180.0f;
        float px  = cosf(rad) * part->distance;
        float pz  = sinf(rad) * part->distance;

        if (is_uranus)
            glColor4f(0.75f, 0.85f, 0.9f, 0.55f);  // icy blue-grey
        else
            glColor4f(0.9f, 0.80f, 0.55f, 0.65f);   // warm golden

        glVertex3f(px, part->y, pz);
    }
    glEnd();

    glPointSize(1.0f);
    glEnable(GL_LIGHTING);
    glDisable(GL_BLEND);
    glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
}