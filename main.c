#include <stdio.h>
#include <stdbool.h>
#include <SDL2/SDL.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include "camera.h"
#include <math.h>
#include "scene.h"
#include <string.h>
#include <stdlib.h>



World world;

const int SCREEN_WIDTH = 1280;
const int SCREEN_HEIGHT = 720;

float sun_intensity = 1.0f;

bool show_help = false;
GLuint help_texture_id;
GLuint skybox_texture_id;

int selected_planet_index = 0;

GLuint load_texture(const char* filename);
void draw_skybox(GLuint texture_id);
void draw_atmosphere(float size, float r, float g, float b, float alpha);

void setup_projection() {
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    float aspect = (float)SCREEN_WIDTH / (float)SCREEN_HEIGHT;

    float fov   = 45.0f;
    float zNear = 0.1f;
    float zFar  = 1000.0f;
    float fH    = tanf(fov / 360.0f * 3.14159f) * zNear;
    float fW    = fH * aspect;

    glFrustum(-fW, fW, -fH, fH, zNear, zFar);
    glMatrixMode(GL_MODELVIEW);
}

void draw_text_simple(float x, float y, const char* text) {
    glRasterPos2f(x, y);
    for (int i = 0; text[i] != '\0'; i++) {
        // Minden karaktert kis téglalapként rajzol ki
        float cx = x + i * 8.0f;
        glBegin(GL_QUADS);
        glVertex2f(cx,       y);
        glVertex2f(cx + 6,   y);
        glVertex2f(cx + 6,   y + 10);
        glVertex2f(cx,       y + 10);
        glEnd();
    }
}

void draw_hud(int target_index, float intensity, World* w) {
    // Állapotok mentése, hogy a HUD ne rontsa el a 3D renderelést
    GLboolean lighting_was_on = glIsEnabled(GL_LIGHTING);
    GLboolean cull_face_was_on = glIsEnabled(GL_CULL_FACE);

    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    glOrtho(0, SCREEN_WIDTH, SCREEN_HEIGHT, 0, -1, 1);
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    glDisable(GL_LIGHTING);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // --- 1. INTENZITÁS SÁV (Bal alsó sarok - Fényerő) ---
    glColor4f(1.0f, 1.0f, 1.0f, 0.2f);
    glBegin(GL_QUADS);
        glVertex2f(20, SCREEN_HEIGHT - 50);
        glVertex2f(220, SCREEN_HEIGHT - 50);
        glVertex2f(220, SCREEN_HEIGHT - 20);
        glVertex2f(20, SCREEN_HEIGHT - 20);
    glEnd();

    float fill = ((intensity - 0.1f) / 1.9f) * 200.0f;
    glColor4f(1.0f, 0.7f, 0.0f, 0.8f);
    glBegin(GL_QUADS);
        glVertex2f(20, SCREEN_HEIGHT - 50);
        glVertex2f(20 + fill, SCREEN_HEIGHT - 50);
        glVertex2f(20 + fill, SCREEN_HEIGHT - 20);
        glVertex2f(20, SCREEN_HEIGHT - 20);
    glEnd();

    // --- 2. BOLYGÓ INFO PANEL (Jobb felső sarok) ---
    // Csak akkor rajzoljuk, ha van kijelölt bolygó
    if (target_index != -1 && target_index < w->count) {
        Planet* p = &w->planets[target_index];

        // Panel háttér (sötétkék, áttetsző)
        glColor4f(0.0f, 0.1f, 0.2f, 0.7f);
        glBegin(GL_QUADS);
            glVertex2f(SCREEN_WIDTH - 280, 20);
            glVertex2f(SCREEN_WIDTH - 20, 20);
            glVertex2f(SCREEN_WIDTH - 20, 160);
            glVertex2f(SCREEN_WIDTH - 280, 160);
        glEnd();

        // Panel keret (neon kék)
        glColor4f(0.0f, 0.8f, 1.0f, 1.0f);
        glBegin(GL_LINE_LOOP);
            glVertex2f(SCREEN_WIDTH - 280, 20);
            glVertex2f(SCREEN_WIDTH - 20, 20);
            glVertex2f(SCREEN_WIDTH - 20, 160);
            glVertex2f(SCREEN_WIDTH - 280, 160);
        glEnd();

        // Szöveges adatok kiírása a draw_text_simple segítségével
        char buffer[64];

        // Név kiírása (Fehérrel)
        glColor3f(1.0f, 1.0f, 1.0f);
        draw_text_simple(SCREEN_WIDTH - 260, 40, p->name);

        // Távolság
        sprintf(buffer, "TAV: %.1f", p->distance);
        draw_text_simple(SCREEN_WIDTH - 260, 75, buffer);

        // Méret
        sprintf(buffer, "MERET: %.2f", p->size);
        draw_text_simple(SCREEN_WIDTH - 260, 110, buffer);

        // Keringési sebesség (opcionális extra)
        sprintf(buffer, "SEB: %.3f", p->orbit_speed);
        draw_text_simple(SCREEN_WIDTH - 260, 140, buffer);
    }

    // --- 3. HELP IKON / KERET (Bal felső sarok) ---
    if (!show_help) {
        glColor4f(0.0f, 0.8f, 1.0f, 0.5f);
        glBegin(GL_LINE_LOOP);
            glVertex2f(20, 20); glVertex2f(100, 20);
            glVertex2f(100, 50); glVertex2f(20, 50);
        glEnd();

        glBegin(GL_LINES);
            glVertex2f(60, 30); glVertex2f(60, 32); // 'i' pontja
            glVertex2f(60, 37); glVertex2f(60, 45); // 'i' szára
        glEnd();
    }

    // Visszaállítjuk az eredeti állapotokat
    glDisable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);

    if (lighting_was_on) glEnable(GL_LIGHTING);
    if (cull_face_was_on) glEnable(GL_CULL_FACE);

    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
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

    if (!window) {
        printf("window error: %s\n", SDL_GetError());
        return 1;
    }
    SDL_SetRelativeMouseMode(SDL_FALSE);
    SDL_ShowCursor(SDL_ENABLE);

    SDL_GLContext glContext = SDL_GL_CreateContext(window);

    glEnable(GL_DEPTH_TEST);
    glClearColor(0.0f, 0.0f, 0.05f, 1.0f);

    Uint32 last_time = SDL_GetTicks();
    float delta_time = 0;
    bool running     = true;
    bool fog_enabled = true;
    SDL_Event event;

    Camera camera;
    init_camera(&camera);

    setup_projection();
    load_planets(&world, "assets/planets.csv");
    printf("Planet count: %d\n", world.count);

    init_asteroid_belt();

    // ---------------------------------------------------------
    // NEW: initialise ring particles for Saturn and Uranus
    // ---------------------------------------------------------
    for (int i = 0; i < world.count; i++) {
        if (strcmp(world.planets[i].name, "Szaturnusz") == 0 ||
            strcmp(world.planets[i].name, "Uranusz")    == 0) {
            init_ring_particles(&world.planets[i]);
        }
    }

    help_texture_id   = load_texture("assets/help.png");
    skybox_texture_id = load_texture("assets/stars.jpg");

    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_COLOR_MATERIAL);

    glEnable(GL_FOG);
    float fogColor[] = {0.05f, 0.02f, 0.15f, 1.0f};
    glFogfv(GL_FOG_COLOR, fogColor);
    glFogf(GL_FOG_DENSITY, 0.02f);
    glHint(GL_FOG_HINT, GL_NICEST);
    glFogi(GL_FOG_MODE, GL_EXP2);

    float ambient[]  = {0.05f, 0.05f, 0.05f, 1.0f};
    float specular[] = {0.5f,  0.5f,  0.5f,  1.0f};
    glLightfv(GL_LIGHT0, GL_AMBIENT,  ambient);
    glLightfv(GL_LIGHT0, GL_SPECULAR, specular);

    int target_planet_index = -1;

    while (running) {

        Uint32 current_time = SDL_GetTicks();
        delta_time = (current_time - last_time) / 1000.0f;
        last_time = current_time;

        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) running = false;

            if (event.type == SDL_MOUSEBUTTONDOWN) {
                if (event.button.button == SDL_BUTTON_LEFT) {
                    // Itt hívjuk meg a kijelölést
                    pick_planet(event.button.x, event.button.y, &camera, &world);
                    // Átadjuk a kijelölt indexet a HUD-nak is, ha akarjuk
                    target_planet_index = selected_planet_index;
                }
            }

            // Kamera forgatása (csak ha a jobb gomb le van nyomva)
            if (event.type == SDL_MOUSEMOTION) {
                if (SDL_GetMouseState(NULL, NULL) & SDL_BUTTON(SDL_BUTTON_RIGHT)) {
                    float sensitivity = 0.2f;
                    camera.yaw   -= event.motion.xrel * sensitivity;
                    camera.pitch -= event.motion.yrel * sensitivity;

                    if (camera.pitch >  89.0f) camera.pitch =  89.0f;
                    if (camera.pitch < -89.0f) camera.pitch = -89.0f;
                }
            }

            if (event.type == SDL_KEYDOWN) {
                if (event.key.keysym.sym == SDLK_F1 || event.key.keysym.sym == SDLK_h){
                    show_help = !show_help;
                }

                if (event.key.keysym.sym == SDLK_f) {
                    fog_enabled = !fog_enabled;
                    if (fog_enabled) { glEnable(GL_FOG);  printf("Kod: BE\n"); }
                    else             { glDisable(GL_FOG); printf("Kod: KI\n"); }
                }
                if (event.key.keysym.sym == SDLK_ESCAPE) running = false;

                if (event.key.keysym.sym >= SDLK_1 && event.key.keysym.sym <= SDLK_9) {
                    target_planet_index = event.key.keysym.sym - SDLK_1;
                    if (target_planet_index >= world.count) target_planet_index = -1;
                }
                if (event.key.keysym.sym == SDLK_0) target_planet_index = -1;
            }

            if (event.type == SDL_MOUSEMOTION) {
                float sensitivity = 0.1f;
                camera.yaw   -= event.motion.xrel * sensitivity;
                camera.pitch -= event.motion.yrel * sensitivity;
                if (camera.pitch >  89.0f) camera.pitch =  89.0f;
                if (camera.pitch < -89.0f) camera.pitch = -89.0f;
            }
        }

        const Uint8* state = SDL_GetKeyboardState(NULL);
        float speed   = 0.2f;
        if (state[SDL_SCANCODE_LSHIFT]) speed *= 3.0f;

        float rad_yaw = camera.yaw * (float)M_PI / 180.0f;
        float rad_pitch = camera.pitch * (float)M_PI / 180.0f;

        if (state[SDL_SCANCODE_W] || state[SDL_SCANCODE_S] ||
        state[SDL_SCANCODE_A] || state[SDL_SCANCODE_D] ||
        state[SDL_SCANCODE_E] || state[SDL_SCANCODE_Q]) {
            target_planet_index = -1;
        }

        if (state[SDL_SCANCODE_W]) {
            float dx = -sinf(rad_yaw) * cosf(rad_pitch) * speed;
            float dy =  sinf(rad_pitch) * speed;
            float dz = -cosf(rad_yaw) * cosf(rad_pitch) * speed;
            update_camera_position(&camera, dx, dy, dz, &world);
        }
        if (state[SDL_SCANCODE_S])
        {
            float dx =  sinf(rad_yaw) * cosf(rad_pitch) * speed;
            float dy = -sinf(rad_pitch) * speed;
            float dz =  cosf(rad_yaw) * cosf(rad_pitch) * speed;
            update_camera_position(&camera, dx, dy, dz, &world);
        }
        if (state[SDL_SCANCODE_A])
        {
            update_camera_position(&camera, -cosf(rad_yaw) * speed, 0, sinf(rad_yaw) * speed, &world);
        }
        if (state[SDL_SCANCODE_D])
        {
            update_camera_position(&camera,  cosf(rad_yaw) * speed, 0, -sinf(rad_yaw) * speed, &world);
        }

        if (state[SDL_SCANCODE_E]) camera.y += speed;
        if (state[SDL_SCANCODE_Q]) camera.y -= speed;

        if (state[SDL_SCANCODE_UP])    camera.pitch += 1.0f;
        if (state[SDL_SCANCODE_DOWN])  camera.pitch -= 1.0f;
        if (state[SDL_SCANCODE_LEFT])  camera.yaw   += 1.0f;
        if (state[SDL_SCANCODE_RIGHT]) camera.yaw   -= 1.0f;
        if (state[SDL_SCANCODE_ESCAPE]) running = false;

        // Sun intensity controls
        if (state[SDL_SCANCODE_KP_PLUS]  || state[SDL_SCANCODE_EQUALS]) {
            sun_intensity += 0.01f;
            if (sun_intensity > 2.0f) sun_intensity = 2.0f;
        }
        if (state[SDL_SCANCODE_KP_MINUS] || state[SDL_SCANCODE_MINUS]) {
            sun_intensity -= 0.01f;
            if (sun_intensity < 0.1f) sun_intensity = 0.1f;
        }

        // Dynamic sun colour
        float r = 1.0f;
        float g = sun_intensity > 1.0f ? 1.0f : sun_intensity;
        float b = sun_intensity > 1.5f ? 1.0f :
                  (sun_intensity < 1.0f ? sun_intensity * 0.5f : sun_intensity - 0.5f);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        set_view(&camera);



        glPushMatrix();
            glTranslatef(camera.x, camera.y, camera.z);
            draw_skybox(skybox_texture_id);
        glPopMatrix();

        glDisable(GL_LIGHTING); // Kikapcsoljuk a fényt, hogy a kövek saját színe látsszon
        draw_asteroid_belt();
        glEnable(GL_LIGHTING);  // Visszakapcsoljuk a bolygókhoz


        // Update light each frame
        float diffuse[] = {r * sun_intensity, g * sun_intensity, b * sun_intensity, 1.0f};
        glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);
        float light_pos[] = {0.0f, 0.0f, 0.0f, 1.0f};
        glLightfv(GL_LIGHT0, GL_POSITION, light_pos);

        // --- NEW: Calculate planet positions hierarchically ---
        for (int i = 0; i < world.count; i++) {
            world.planets[i].current_angle += world.planets[i].orbit_speed * delta_time * 10.0f;
            world.planets[i].rotation_angle += world.planets[i].rotation_speed * delta_time * 50.0f;
            Planet* p = &world.planets[i];
            float lx = cosf(p->current_angle) * p->distance;
            float lz = sinf(p->current_angle) * p->distance;
            if (p->parent_index != -1) {
                Planet* parent = &world.planets[p->parent_index];
                p->world_x = parent->world_x + lx;
                p->world_y = parent->world_y;
                p->world_z = parent->world_z + lz;
            } else {
                p->world_x = lx;
                p->world_y = 0.0f;
                p->world_z = lz;
            }
        }

        // Planet-follow camera
        if (target_planet_index != -1) {
            Planet* p = &world.planets[target_planet_index];
            camera.x = p->world_x + p->size * 3.0f;
            camera.y = p->world_y + p->size * 2.0f;
            camera.z = p->world_z + p->size * 3.0f;
        }

        // Draw planets
        for (int i = 0; i < world.count; i++) {
            Planet* p = &world.planets[i];

            glPushMatrix();
            glTranslatef(p->world_x, p->world_y, p->world_z);

            GLUquadric* quad = gluNewQuadric();
            gluQuadricTexture(quad, GL_TRUE);

            if (p->distance < 0.1f && p->parent_index == -1) { // Check for Sun
                // --- SUN ---
                glDisable(GL_LIGHTING);
                glEnable(GL_TEXTURE_2D);
                glBindTexture(GL_TEXTURE_2D, p->texture_id);

                float current_sun_size = p->size;
                if (sun_intensity < 0.8f)
                    current_sun_size *= (1.0f + (0.8f - sun_intensity) * 2.0f);
                else if (sun_intensity > 1.5f) {
                    current_sun_size *= (1.0f - (sun_intensity - 1.5f) * 1.5f);
                    if (current_sun_size < 0.2f) current_sun_size = 0.2f;
                }

                glColor3f(r, g, b);
                GLUquadric* sunQuad = gluNewQuadric();
                gluQuadricTexture(sunQuad, GL_TRUE);
                gluSphere(sunQuad, current_sun_size, 32, 32);
                gluDeleteQuadric(sunQuad);

                glColor3f(1.0f, 1.0f, 1.0f);
                glDisable(GL_TEXTURE_2D);
                glEnable(GL_LIGHTING);

            } else {
                // --- PLANET or MOON ---
                glEnable(GL_LIGHTING);

                glPushMatrix();
                glRotatef(-90.0f, 1.0f, 0.0f, 0.0f);
                glRotatef(p->axial_tilt,      1.0f, 0.0f, 0.0f);
                glRotatef(p->rotation_angle,  0.0f, 0.0f, 1.0f);

                glEnable(GL_TEXTURE_2D);
                glBindTexture(GL_TEXTURE_2D, p->texture_id);
                gluSphere(quad, p->size, 32, 32);

                if (p->has_atmosphere)
                    draw_atmosphere(p->size, p->atmo_r, p->atmo_g, p->atmo_b, 0.25f);

                glDisable(GL_TEXTURE_2D);
                glPopMatrix();  // pop the tilt/rotation matrix

                // ---------------------------------------------------------
                // RINGS: particle system replaces gluDisk for Saturn/Uranus
                // ---------------------------------------------------------
                if (strcmp(p->name, "Szaturnusz") == 0 ||
                    strcmp(p->name, "Uranusz")    == 0) {

                    // ----- animate particles -----
                    for (int j = 0; j < p->particle_count; j++) {
                        p->ring_particles[j].angle += p->ring_particles[j].speed;
                        if (p->ring_particles[j].angle >= 360.0f)
                            p->ring_particles[j].angle -= 360.0f;
                    }

                    // ----- draw particles -----
                    // Uranus: rings are nearly perpendicular to the orbital plane
                    glPushMatrix();
                    if (strcmp(p->name, "Uranusz") == 0)
                        glRotatef(97.0f, 1.0f, 0.0f, 0.0f); // match axial tilt
                    draw_ring_particles(p);
                    glPopMatrix();
                }
            }

            gluDeleteQuadric(quad);
            glPopMatrix();

            p->current_angle  += p->orbit_speed * 0.001f;
            p->rotation_angle += p->rotation_speed;
        }

        draw_hud(target_planet_index, sun_intensity, &world);
        // Help overlay
        if (show_help) {
            glMatrixMode(GL_PROJECTION);
            glPushMatrix();
            glLoadIdentity();
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
            glColor3f(1.0f, 1.0f, 1.0f);

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

    // ---------------------------------------------------------
    // NEW: free ring particle memory before exit
    // ---------------------------------------------------------
    for (int i = 0; i < world.count; i++) {
        if (world.planets[i].ring_particles) {
            free(world.planets[i].ring_particles);
            world.planets[i].ring_particles = NULL;
        }
    }

    SDL_GL_DeleteContext(glContext);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}