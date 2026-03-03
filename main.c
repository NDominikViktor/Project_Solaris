#include <stdio.h>
#include <stdbool.h>
#include <SDL2/SDL.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include "camera.h"
#include <math.h>
#include "scene.h"
#include <string.h>


World world;

const int SCREEN_WIDTH = 1280;
const int SCREEN_HEIGHT = 720;

float sun_intensity = 1.0f;

bool show_help = false;
GLuint help_texture_id;
GLuint skybox_texture_id;

GLuint load_texture(const char* filename);
void draw_skybox(GLuint texure_id);
void draw_atmosphere(float size, float r, float g, float b, float alpha);

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
    SDL_SetRelativeMouseMode(SDL_TRUE);

    SDL_GLContext glContext = SDL_GL_CreateContext(window);

    glEnable(GL_DEPTH_TEST);
    glClearColor(0.0f, 0.0f, 0.05f, 1.0f);

    bool running = true;
    bool fog_enabled = true;
    SDL_Event event;

    Camera camera;
    init_camera(&camera);


    setup_projection();
    load_planets(&world, "assets/planets.csv");
    printf("Planet count: %d\n", world.count);
    help_texture_id = load_texture("assets/help.png");
    skybox_texture_id = load_texture("assets/stars.jpg");
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_COLOR_MATERIAL);

    glEnable(GL_FOG);
    float fogColor[] = {0.05f, 0.02f, 0.15f, 1.0f}; // sötét lila/kék köd

    glFogfv(GL_FOG_COLOR, fogColor);
    glFogf(GL_FOG_DENSITY, 0.02f);
    glHint(GL_FOG_HINT, GL_NICEST);
    glFogi(GL_FOG_MODE, GL_EXP2);

    float r = 1.0f;
    float g = sun_intensity > 1.0f ? 1.0f : sun_intensity;
    float b = sun_intensity > 1.5f ? 1.0f : (sun_intensity < 1.0f ? sun_intensity * 0.5f : sun_intensity - 0.5f);

    // Light properties - set once, position set every frame
    float ambient[] = {0.05f, 0.05f, 0.05f, 1.0f}; // very dark ambient so night side is dark
    float diffuse[] = {r * sun_intensity, g * sun_intensity, b * sun_intensity, 1.0f};
    float specular[]= {0.5f,  0.5f,  0.5f,  1.0f};

    glLightfv(GL_LIGHT0, GL_AMBIENT,  ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE,  diffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, specular);

    int target_planet_index = -1; // -1: szabad mozgás, 0-8: bolygó követése



    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) running = false;

            if (event.type == SDL_KEYDOWN) {
                // F gomb kezelése sym-mel a scancode helyett
                if (event.key.keysym.sym == SDLK_f) {
                    fog_enabled = !fog_enabled;
                    if (fog_enabled) {
                        glEnable(GL_FOG);
                        printf("Kod: BE\n");
                    } else {
                        glDisable(GL_FOG);
                        printf("Kod: KI\n");
                    }
                }
                if (event.key.keysym.sym == SDLK_ESCAPE) {
                    running = false;
                }

                if (event.key.keysym.sym >= SDLK_1 && event.key.keysym.sym <= SDLK_9) {
                    target_planet_index = event.key.keysym.sym - SDLK_1;
                    if (target_planet_index >= world.count) target_planet_index = -1;
                }
                if (event.key.keysym.sym == SDLK_0) target_planet_index = -1; // Szabad kamera visszaállítása
            }

            if (event.type == SDL_MOUSEMOTION) {
                float sensitivity = 0.1f;
                camera.yaw -= event.motion.xrel * sensitivity;
                camera.pitch -= event.motion.yrel * sensitivity;

                // korlátozzuk a függőleges nézést ne forduljon át a kamera fejjel lefele.
                if (camera.pitch > 89.0f) camera.pitch = 89.0f;
                if (camera.pitch < -89.0f) camera.pitch = -89.0f;
            }
        }

        const Uint8* state = SDL_GetKeyboardState(NULL);
        float speed = 0.1f;
        float rad_yaw = camera.yaw * M_PI / 180.0f;

        if (state[SDL_SCANCODE_W]) {
            update_camera_position(&camera, -sinf(rad_yaw) * speed, 0, -cosf(rad_yaw) * speed, &world);
        }
        if (state[SDL_SCANCODE_S]) {
            update_camera_position(&camera, sinf(rad_yaw) * speed, 0, cosf(rad_yaw) * speed, &world);
        }
        if (state[SDL_SCANCODE_A]) {
            update_camera_position(&camera, -cosf(rad_yaw) * speed, 0, sinf(rad_yaw) * speed, &world);
        }
        if (state[SDL_SCANCODE_D]) {
            update_camera_position(&camera, cosf(rad_yaw) * speed, 0, -sinf(rad_yaw) * speed, &world);
        }
        if (state[SDL_SCANCODE_UP])    camera.pitch += 1.0f;
        if (state[SDL_SCANCODE_DOWN])  camera.pitch -= 1.0f;
        if (state[SDL_SCANCODE_LEFT])  camera.yaw += 1.0f;
        if (state[SDL_SCANCODE_RIGHT]) camera.yaw -= 1.0f;

        if (state[SDL_SCANCODE_ESCAPE]) {
            running = false;
        }

        // 1. FÉNYERŐ GOMBOK KEZELÉSE
    if (state[SDL_SCANCODE_KP_PLUS] || state[SDL_SCANCODE_EQUALS]) {
        sun_intensity += 0.01f;
        if (sun_intensity > 2.0f) sun_intensity = 2.0f;
    }
    if (state[SDL_SCANCODE_KP_MINUS] || state[SDL_SCANCODE_MINUS]) {
        sun_intensity -= 0.01f;
        if (sun_intensity < 0.1f) sun_intensity = 0.1f;
    }

    // 2. SZÍN SZÁMÍTÁSA (Minden frame-ben újra, hogy változzon!)
    float r = 1.0f;
    float g = sun_intensity > 1.0f ? 1.0f : sun_intensity;
    float b = sun_intensity > 1.5f ? 1.0f : (sun_intensity < 1.0f ? sun_intensity * 0.5f : sun_intensity - 0.5f);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    set_view(&camera);
    glPushMatrix();
        glTranslatef(camera.x, camera.y, camera.z),
        draw_skybox(skybox_texture_id);
        glPopMatrix();


    // 3. FÉNY FRISSÍTÉSE
    float ambient[] = {0.05f, 0.05f, 0.05f, 1.0f};
    float diffuse[] = {r * sun_intensity, g * sun_intensity, b * sun_intensity, 1.0f};
    glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);
    glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);

    float light_pos[] = {0.0f, 0.0f, 0.0f, 1.0f};
    glLightfv(GL_LIGHT0, GL_POSITION, light_pos);
        if (target_planet_index != -1) {
            Planet* p = &world.planets[target_planet_index];
            float px = cosf(p->current_angle) * p->distance;
            float pz = sinf(p->current_angle) * p->distance;
            camera.x = px + p->size * 3.0f;
            camera.y = p->size * 2.0f;
            camera.z = pz + p->size * 3.0f;
        }

    for (int i = 0; i < world.count; i++) {
        Planet* p = &world.planets[i];
        float x = cosf(p->current_angle) * p->distance;
        float z = sinf(p->current_angle) * p->distance;



        glPushMatrix();
        glTranslatef(x, 0.0f, z);

        // Helyes sorrend: előbb létrehozzuk a quad-ot!
        GLUquadric* quad = gluNewQuadric();
        gluQuadricTexture(quad, GL_TRUE);

        if (p->distance < 0.1f) {
            glDisable(GL_LIGHTING); // A Nap saját fénye miatt ne kapjon árnyékot
            glEnable(GL_TEXTURE_2D);
            glBindTexture(GL_TEXTURE_2D, p->texture_id);

            // Itt számoljuk ki a dinamikus méretet
            float current_sun_size = p->size;
            if (sun_intensity < 0.8f) {
                current_sun_size *= (1.0f + (0.8f - sun_intensity) * 2.0f); // Felfúvódik
            } else if (sun_intensity > 1.5f) {
                current_sun_size *= (1.0f - (sun_intensity - 1.5f) * 1.5f); // Összehúzódik
                if (current_sun_size < 0.2f) current_sun_size = 0.2f;
            }

            // Színbeállítás az intenzitás alapján
            glColor3f(r, g, b);

            // CSAK EGYETLEN gömböt rajzolunk a kiszámolt mérettel!
            GLUquadric* sunQuad = gluNewQuadric();
            gluQuadricTexture(sunQuad, GL_TRUE);
            gluSphere(sunQuad, current_sun_size, 32, 32);
            gluDeleteQuadric(sunQuad);

            glColor3f(1.0f, 1.0f, 1.0f); // Szín visszaállítása
            glDisable(GL_TEXTURE_2D);
            glEnable(GL_LIGHTING);
        } else {
            glEnable(GL_LIGHTING);

            // Bolygó rajzolása (Forgatásokkal)
            glPushMatrix();
            glRotatef(-90.0f, 1.0f, 0.0f, 0.0f);
            glRotatef(p->axial_tilt, 1.0f, 0.0f, 0.0f);
            glRotatef(p->rotation_angle, 0.0f, 0.0f, 1.0f);

            glEnable(GL_TEXTURE_2D);
            glBindTexture(GL_TEXTURE_2D, p->texture_id);
            gluSphere(quad, p->size, 32, 32);

           if (p->has_atmosphere) {
               draw_atmosphere(p->size, p->atmo_r, p->atmo_g, p->atmo_b, 0.25f);
           }
            // Szaturnusz és Uránusz gyűrű (közös logika)
            if (strcmp(p->name, "Szaturnusz") == 0 || strcmp(p->name, "Uranusz") == 0) {
                glEnable(GL_BLEND);
                glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
                glDisable(GL_CULL_FACE);

                glColor4f(1.0f, 1.0f, 1.0f, 0.6f);

                GLUquadric* ringQuad = gluNewQuadric();
                gluQuadricTexture(ringQuad, GL_TRUE);
                // Az Uránusznak vékonyabb gyűrűt adunk
                float inner = (strcmp(p->name, "Uranusz") == 0) ? 1.5f : 1.3f;
                float outer = (strcmp(p->name, "Uranusz") == 0) ? 1.7f : 2.1f;
                gluDisk(ringQuad, p->size * inner, p->size * outer, 64, 1);
                gluDeleteQuadric(ringQuad);
                glDisable(GL_CULL_FACE);
                glEnable(GL_CULL_FACE);
            }
            glDisable(GL_TEXTURE_2D);
            glPopMatrix();
        }

        gluDeleteQuadric(quad);
        glPopMatrix();

        p->current_angle  += p->orbit_speed * 0.001f;
        p->rotation_angle += p->rotation_speed;
    }
        if (show_help) {
            glMatrixMode(GL_PROJECTION);
            glPushMatrix();
            glLoadIdentity();
            // A 0,0 a bal felső sarok, SCREEN_WIDTH/HEIGHT a jobb alsó
            glOrtho(0, SCREEN_WIDTH, SCREEN_HEIGHT, 0, -1, 1);

            glMatrixMode(GL_MODELVIEW);
            glPushMatrix();
            glLoadIdentity();

            glDisable(GL_LIGHTING);
            glDisable(GL_DEPTH_TEST);
            glDisable(GL_CULL_FACE);

            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            glEnable(GL_TEXTURE_2D);
            glBindTexture(GL_TEXTURE_2D, help_texture_id);

            glColor3f(1.0f, 1.0f, 1.0f); // Fehér szín, hogy a textúra eredeti színei látsszanak

            glBegin(GL_QUADS);
            glTexCoord2f(0, 0); glVertex2f(100, 100);
            glTexCoord2f(1, 0); glVertex2f(SCREEN_WIDTH - 100, 100);
            glTexCoord2f(1, 1); glVertex2f(SCREEN_WIDTH - 100, SCREEN_HEIGHT - 100);
            glTexCoord2f(0, 1); glVertex2f(100, SCREEN_HEIGHT - 100);
            glEnd();

            glDisable(GL_BLEND);
            glDisable(GL_TEXTURE_2D);
            glEnable(GL_DEPTH_TEST);
            glEnable(GL_CULL_FACE);

            glPopMatrix();
            glMatrixMode(GL_PROJECTION);
            glPopMatrix();
            glMatrixMode(GL_MODELVIEW);
        }

    SDL_GL_SwapWindow(window);
}

    SDL_GL_DeleteContext(glContext);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}