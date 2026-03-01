//
// Created by novakdominikviktor on 2026. 02. 28..
//
#include "scene.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define STB_IMAGE_IMPLEMENTATION
#include <GL/glu.h>

#include "stb_image.h"

GLuint load_texture(const char* filename) {
    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);

    // Textúra paraméterek (ismétlődés és simítás)
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

        char texture_name[64]; // Ideiglenes tároló a fájlnévnek
        Planet* p = &world->planets[world->count];

        // Frissített sscanf: az utolsó %s beolvassa a textúra nevét (pl. sun.jpg)
        if (sscanf(line, "%[^,],%f,%f,%f,%f,%f,%[^,], %d, %f, %f, %f, %f",
                   p->name, &p->distance, &p->size, &p->orbit_speed, &p->rotation_speed, &p->axial_tilt, texture_name, &p->has_atmosphere, &p->atmo_r, &p->atmo_g, &p->atmo_b) == 11) {

            p->current_angle = 0.0f;

            // Itt jön a lényeg: Összeillesztjük az assets/ mappát a fájlnévvel
            char full_path[128];
            sprintf(full_path, "assets/%s", texture_name);

            // Betöltjük a textúrát és elmentjük az ID-t a bolygóba
            p->texture_id = load_texture(full_path);

            world->count++;
                   }
    }
    fclose(file);
    printf("Loaded %d planets from %s\n", world->count, filename);
}

void draw_skybox (GLuint texture_id) {
    float size = 500.0f; // legyen messze
    GLboolean fog_was_enabled = glIsEnabled(GL_FOG);
    glDisable(GL_FOG);
    glDisable(GL_LIGHTING);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, texture_id);
    glColor3f(1.0f, 1.0f, 1.0f);

    glBegin(GL_QUADS);
    glTexCoord2f(0, 0); glVertex3f(-size, -size, -size);
    glTexCoord2f(1, 0); glVertex3f(size, -size, -size);
    glTexCoord2f(1, 1); glVertex3f(size, size, -size);
    glTexCoord2f(0, 1); glVertex3f(-size, size, -size);

    glTexCoord2f(0, 0); glVertex3f( size, -size,  size);
    glTexCoord2f(1, 0); glVertex3f(-size, -size,  size);
    glTexCoord2f(1, 1); glVertex3f(-size,  size,  size);
    glTexCoord2f(0, 1); glVertex3f( size,  size,  size);

    // Bal oldali fal
    glTexCoord2f(0, 0); glVertex3f(-size, -size,  size);
    glTexCoord2f(1, 0); glVertex3f(-size, -size, -size);
    glTexCoord2f(1, 1); glVertex3f(-size,  size, -size);
    glTexCoord2f(0, 1); glVertex3f(-size,  size,  size);

    // Jobb oldali fal
    glTexCoord2f(0, 0); glVertex3f( size, -size, -size);
    glTexCoord2f(1, 0); glVertex3f( size, -size,  size);
    glTexCoord2f(1, 1); glVertex3f( size,  size,  size);
    glTexCoord2f(0, 1); glVertex3f( size,  size, -size);

    // Felső fal
    glTexCoord2f(0, 0); glVertex3f(-size,  size, -size);
    glTexCoord2f(1, 0); glVertex3f( size,  size, -size);
    glTexCoord2f(1, 1); glVertex3f( size,  size,  size);
    glTexCoord2f(0, 1); glVertex3f(-size,  size,  size);

    // Alsó fal
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
    glDisable(GL_LIGHTING); // emiatt fénylik a légkör.

    glColor4f(r, g, b, alpha); // emiatt átlátszó

    GLUquadric* quad = gluNewQuadric();

    gluSphere(quad, size * 1.05f, 32, 32);
    gluDeleteQuadric(quad);

    glEnable(GL_LIGHTING);
    glDisable(GL_BLEND);
    glColor3f(1.0f, 1.0f, 1.0f); //Szín visszaállítása.
}