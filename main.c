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
    // Megszüntetjük a "unused parameter" warningokat
    (void)argc;
    (void)args;

    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("SDL error: %s\n", SDL_GetError());
        return 1;
    }

    // 2. Ablak létrehozása
    SDL_Window* window = SDL_CreateWindow(
        "Solaris",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        SCREEN_WIDTH, SCREEN_HEIGHT,
        SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE
    );

    if (window == NULL) { // NULL ellenőrzés javítva
        printf("window error: %s\n", SDL_GetError());
        return 1;
    }

    // 3. OpenGL kontextus
    SDL_GLContext glContext = SDL_GL_CreateContext(window);

    // 4. Alapvető 3D beállítások
    glEnable(GL_DEPTH_TEST); // Mélységvizsgálat (hogy ne lássunk át a bolygókon)
    glClearColor(0.0f, 0.0f, 0.05f, 1.0f); // Sötétkék világűr alap

    bool running = true;
    SDL_Event event;

    Camera camera;
    init_camera(&camera);

    setup_projection();
    // Fő ciklus
    load_planets(&world, "assets/planets.csv");
    printf("Planet count: %d\n", world.count);

    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_COLOR_MATERIAL);

    float light_pos[] = {0.0f, 0.0f, 0.0f, 1.0f}; //nap helye
    float ambient[] = {0.2f, 0.2f, 0.2f, 1.0f}; //alaap szórt fény
    float diffuse[] = {1.0f, 1.0f, 1.0f, 1.0f}; // erős fehér fény

    glLightfv(GL_LIGHT0, GL_POSITION, light_pos);
    glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);

    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) running = false;
        }

        const Uint8* state = SDL_GetKeyboardState(NULL);
        float speed = 0.1f;

        if (state[SDL_SCANCODE_W]) camera.z -= speed;
        if (state[SDL_SCANCODE_S]) camera.z += speed;
        if (state[SDL_SCANCODE_A]) camera.x -= speed;
        if (state[SDL_SCANCODE_D]) camera.x += speed;
        if (state[SDL_SCANCODE_UP]) camera.pitch += 1.0f;
        if (state[SDL_SCANCODE_DOWN]) camera.pitch -= 1.0f;
        if (state[SDL_SCANCODE_LEFT]) camera.yaw -= 1.0f;
        if (state[SDL_SCANCODE_RIGHT]) camera.yaw += 1.0f;

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Frissítjük a nézetet (Kamera a +10-en van)
        set_view(&camera);
        // Végigmegyünk a betöltött bolygókon
        for (int i = 0; i < world.count; i++) {
            Planet* p = &world.planets[i];

            // 1. Számoljuk ki a bolygó aktuális pozícióját a keringés alapján
            // Az egyszerűség kedvéért körpálya: x = cos(angle)*r, z = sin(angle)*r
            float x = cosf(p->current_angle) * p->distance;
            float z = sinf(p->current_angle) * p->distance;

            // 2. Mentsük el az aktuális mátrix állapotot
            glPushMatrix();

            // 3. Toljuk el a bolygót a helyére
            glTranslatef(x, 0.0f, z);

            if (p->distance < 0.1f) {
                // nap nem kap árnyékot
                glDisable(GL_LIGHTING);
            } else {
                // a bolygóknak van árnyéka
                glEnable(GL_LIGHTING);
            }

            // 4. Rajzoljuk ki (egyelőre egy színes négyzetként/kockaként)
            glEnable(GL_TEXTURE_2D);
            glBindTexture(GL_TEXTURE_2D, p->texture_id);

            GLUquadric* quad = gluNewQuadric();
            gluQuadricTexture(quad, GL_TRUE); // Ez kényszeríti rá a képet a gömbre!
            gluSphere(quad, p->size, 32, 32);
            gluDeleteQuadric(quad);

            glDisable(GL_TEXTURE_2D);

            glPopMatrix(); // Visszaállítjuk a mátrixot a következő bolygónak

            // 5. Frissítsük az szöget a keringési sebességgel (időalapú mozgás)
            // Itt egy fix értéket adunk hozzá, de később delta_time-al szebb lesz
            p->current_angle += p->orbit_speed * 0.0001f;
        }

        SDL_GL_SwapWindow(window);
    }

    // Takarítás
    SDL_GL_DeleteContext(glContext);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}