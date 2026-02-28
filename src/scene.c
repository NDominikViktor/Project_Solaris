//
// Created by novakdominikviktor on 2026. 02. 28..
//
#include "scene.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

        Planet* p = &world->planets[world->count];
        sscanf(line, "%[^,],%f,%f,%f", p->name, &p->distance, &p->size, &p->orbit_speed);

        p->current_angle = 0.0f;
        world->count++;
    }
    fclose(file);
    printf("Loaded %d planets from %s\n", world->count, filename);
}