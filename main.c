#include <stdio.h>
#include <stdbool.h>
#include <SDL2/SDL.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include "camera.h"
#include <math.h>
#include "scene.h"


World world;



const int SCREEN_WIDTH = 1280;
const int SCREEN_HEIGHT = 720;

void setup_projection() {
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    float aspect = (float)SCREEN_WIDTH / (float)SCREEN_HEIGHT;

    float fov = 45.0f;
    float zNear = 0.1f;
    float zFar = 1000.0f;
    float fH = tanf(fov / 360.0f * 3.14159f) * zNear;
    float fW = fH * aspect;

    glFrustum(-fW, fW, -fH, fH, zNear, zFar);

    glMatrixMode(GL_MODELVIEW);
}

int main(int argc, char* args[]) {
    (void)argc;
    (void)args;

    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("SDL error: %s\n", SDL_GetError());
        return 1;
    }

    SDL_Window* window = SDL_CreateWindow(
        "Solaris",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        SCREEN_WIDTH, SCREEN_HEIGHT,
        SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE
    );

    if (window == NULL) {
        printf("window error: %s\n", SDL_GetError());
        return 1;
    }

    SDL_GLContext glContext = SDL_GL_CreateContext(window);

    glEnable(GL_DEPTH_TEST);
    glClearColor(0.0f, 0.0f, 0.05f, 1.0f);

    bool running = true;
    SDL_Event event;

    Camera camera;
    init_camera(&camera);

    setup_projection();
    load_planets(&world, "assets/planets.csv");
    printf("Planet count: %d\n", world.count);

    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_COLOR_MATERIAL);

    float light_pos[] = {0.0f, 0.0f, 0.0f, 1.0f};
    float ambient[]   = {0.2f, 0.2f, 0.2f, 1.0f};
    float diffuse[]   = {1.0f, 1.0f, 1.0f, 1.0f};

    glLightfv(GL_LIGHT0, GL_POSITION, light_pos);
    glLightfv(GL_LIGHT0, GL_AMBIENT,  ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE,  diffuse);

    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) running = false;
        }

        const Uint8* state = SDL_GetKeyboardState(NULL);
        float speed = 0.1f;

        if (state[SDL_SCANCODE_W])     camera.z -= speed;
        if (state[SDL_SCANCODE_S])     camera.z += speed;
        if (state[SDL_SCANCODE_A])     camera.x -= speed;
        if (state[SDL_SCANCODE_D])     camera.x += speed;
        if (state[SDL_SCANCODE_UP])    camera.pitch += 1.0f;
        if (state[SDL_SCANCODE_DOWN])  camera.pitch -= 1.0f;
        if (state[SDL_SCANCODE_LEFT])  camera.yaw -= 1.0f;
        if (state[SDL_SCANCODE_RIGHT]) camera.yaw += 1.0f;

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        set_view(&camera);

        // Végigmegyünk a betöltött bolygókon
        for (int i = 0; i < world.count; i++) {
            Planet* p = &world.planets[i];

            // 1. Pozíció kiszámítása
            float x = cosf(p->current_angle) * p->distance;
            float z = sinf(p->current_angle) * p->distance;

            glPushMatrix(); // Mátrix mentése

            // 2. Mozgatás a pályára
            glTranslatef(x, 0.0f, z);

            // 3. Fénykezelés
            if (p->distance < 0.1f) glDisable(GL_LIGHTING);
            else glEnable(GL_LIGHTING);

            // 4. FORGATÁSOK - Ez a lényeg!
            // Alap dőlés, hogy a sarkok felül legyenek
            glRotatef(-90.0f, 1.0f, 0.0f, 0.0f);

            // ÚJ: Egyedi tengelyferdeség beállítása (CSV-ből jön)
            glRotatef(p->axial_tilt, 0.0f, 1.0f, 0.0f);

            // Saját tengely körüli pörgés
            glRotatef(p->rotation_angle, 0.0f, 0.0f, 1.0f);

            // 5. Rajzolás
            glEnable(GL_TEXTURE_2D);
            glBindTexture(GL_TEXTURE_2D, p->texture_id);

            GLUquadric* quad = gluNewQuadric();
            gluQuadricTexture(quad, GL_TRUE);
            gluSphere(quad, p->size, 32, 32);
            gluDeleteQuadric(quad);

            glDisable(GL_TEXTURE_2D);
            glPopMatrix(); // Mátrix visszaállítása

            // 6. Szögek frissítése
            p->current_angle += p->orbit_speed * 0.001f;
            p->rotation_angle += p->rotation_speed;
        }

        SDL_GL_SwapWindow(window);
    }

    SDL_GL_DeleteContext(glContext);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}