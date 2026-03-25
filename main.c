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
GLuint sun_glow_texture_id;
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
    glPushMatrix();
    glTranslatef(x, y, 0);
    glScalef(1.2f, 1.2f, 1.0f);
    glLineWidth(2.0f);
    glBegin(GL_LINES);
    for (int i = 0; text[i] != '\0'; i++) {
        float ox = i * 12.0f;
        char c = text[i];
        if (c >= 'a' && c <= 'z') c -= 32; // Kisbetűből nagybetűt csinálunk

        // --- BETŰK (A-Z) ---
        if (c == 'A') { glVertex2f(ox,10); glVertex2f(ox+4,0); glVertex2f(ox+4,0); glVertex2f(ox+8,10); glVertex2f(ox+2,5); glVertex2f(ox+6,5); }
        else if (c == 'B') { glVertex2f(ox,0); glVertex2f(ox,10); glVertex2f(ox,0); glVertex2f(ox+6,2); glVertex2f(ox+6,2); glVertex2f(ox,5); glVertex2f(ox,5); glVertex2f(ox+7,7); glVertex2f(ox+7,7); glVertex2f(ox,10); }
        else if (c == 'C') { glVertex2f(ox+8,2); glVertex2f(ox+4,0); glVertex2f(ox+4,0); glVertex2f(ox,4); glVertex2f(ox,4); glVertex2f(ox,6); glVertex2f(ox,6); glVertex2f(ox+4,10); glVertex2f(ox+4,10); glVertex2f(ox+8,8); }
        else if (c == 'D') { glVertex2f(ox,0); glVertex2f(ox,10); glVertex2f(ox,0); glVertex2f(ox+6,0); glVertex2f(ox+6,0); glVertex2f(ox+8,5); glVertex2f(ox+8,5); glVertex2f(ox+6,10); glVertex2f(ox+6,10); glVertex2f(ox,10); }
        else if (c == 'E') { glVertex2f(ox,0); glVertex2f(ox,10); glVertex2f(ox,0); glVertex2f(ox+8,0); glVertex2f(ox,5); glVertex2f(ox+6,5); glVertex2f(ox,10); glVertex2f(ox+8,10); }
        else if (c == 'F') { glVertex2f(ox,0); glVertex2f(ox,10); glVertex2f(ox,0); glVertex2f(ox+8,0); glVertex2f(ox,5); glVertex2f(ox+6,5); }
        else if (c == 'G') { glVertex2f(ox+8,2); glVertex2f(ox+4,0); glVertex2f(ox+4,0); glVertex2f(ox,4); glVertex2f(ox,4); glVertex2f(ox,10); glVertex2f(ox,10); glVertex2f(ox+8,10); glVertex2f(ox+8,10); glVertex2f(ox+8,5); glVertex2f(ox+8,5); glVertex2f(ox+4,5); }
        else if (c == 'H') { glVertex2f(ox,0); glVertex2f(ox,10); glVertex2f(ox+8,0); glVertex2f(ox+8,10); glVertex2f(ox,5); glVertex2f(ox+8,5); }
        else if (c == 'I') { glVertex2f(ox+4,0); glVertex2f(ox+4,10); glVertex2f(ox+1,0); glVertex2f(ox+7,0); glVertex2f(ox+1,10); glVertex2f(ox+7,10); }
        else if (c == 'J') { glVertex2f(ox+8,0); glVertex2f(ox+8,8); glVertex2f(ox+8,8); glVertex2f(ox+4,10); glVertex2f(ox+4,10); glVertex2f(ox,8); }
        else if (c == 'K') { glVertex2f(ox,0); glVertex2f(ox,10); glVertex2f(ox,5); glVertex2f(ox+8,0); glVertex2f(ox,5); glVertex2f(ox+8,10); }
        else if (c == 'L') { glVertex2f(ox,0); glVertex2f(ox,10); glVertex2f(ox,10); glVertex2f(ox+8,10); }
        else if (c == 'M') { glVertex2f(ox,10); glVertex2f(ox,0); glVertex2f(ox,0); glVertex2f(ox+4,5); glVertex2f(ox+4,5); glVertex2f(ox+8,0); glVertex2f(ox+8,0); glVertex2f(ox+8,10); }
        else if (c == 'N') { glVertex2f(ox,10); glVertex2f(ox,0); glVertex2f(ox,0); glVertex2f(ox+8,10); glVertex2f(ox+8,10); glVertex2f(ox+8,0); }
        else if (c == 'O') { glVertex2f(ox,0); glVertex2f(ox+8,0); glVertex2f(ox+8,0); glVertex2f(ox+8,10); glVertex2f(ox+8,10); glVertex2f(ox,10); glVertex2f(ox,10); glVertex2f(ox,0); }
        else if (c == 'P') { glVertex2f(ox,0); glVertex2f(ox,10); glVertex2f(ox,0); glVertex2f(ox+8,0); glVertex2f(ox+8,0); glVertex2f(ox+8,5); glVertex2f(ox+8,5); glVertex2f(ox,5); }
        else if (c == 'Q') { glVertex2f(ox,0); glVertex2f(ox+8,0); glVertex2f(ox+8,0); glVertex2f(ox+8,10); glVertex2f(ox+8,10); glVertex2f(ox,10); glVertex2f(ox,10); glVertex2f(ox,0); glVertex2f(ox+4,6); glVertex2f(ox+8,10); }
        else if (c == 'R') { glVertex2f(ox,0); glVertex2f(ox,10); glVertex2f(ox,0); glVertex2f(ox+8,0); glVertex2f(ox+8,0); glVertex2f(ox+8,5); glVertex2f(ox+8,5); glVertex2f(ox,5); glVertex2f(ox+4,5); glVertex2f(ox+8,10); }
        else if (c == 'S') { glVertex2f(ox+8,0); glVertex2f(ox,0); glVertex2f(ox,0); glVertex2f(ox,5); glVertex2f(ox,5); glVertex2f(ox+8,5); glVertex2f(ox+8,5); glVertex2f(ox+8,10); glVertex2f(ox+8,10); glVertex2f(ox,10); }
        else if (c == 'T') { glVertex2f(ox+4,0); glVertex2f(ox+4,10); glVertex2f(ox,0); glVertex2f(ox+8,0); }
        else if (c == 'U') { glVertex2f(ox,0); glVertex2f(ox,10); glVertex2f(ox,10); glVertex2f(ox+8,10); glVertex2f(ox+8,10); glVertex2f(ox+8,0); }
        else if (c == 'V') { glVertex2f(ox,0); glVertex2f(ox+4,10); glVertex2f(ox+4,10); glVertex2f(ox+8,0); }
        else if (c == 'W') { glVertex2f(ox,0); glVertex2f(ox,10); glVertex2f(ox,10); glVertex2f(ox+4,5); glVertex2f(ox+4,5); glVertex2f(ox+8,10); glVertex2f(ox+8,10); glVertex2f(ox+8,0); }
        else if (c == 'X') { glVertex2f(ox,0); glVertex2f(ox+8,10); glVertex2f(ox+8,0); glVertex2f(ox,10); }
        else if (c == 'Y') { glVertex2f(ox,0); glVertex2f(ox+4,5); glVertex2f(ox+8,0); glVertex2f(ox+4,5); glVertex2f(ox+4,5); glVertex2f(ox+4,10); }
        else if (c == 'Z') { glVertex2f(ox,0); glVertex2f(ox+8,0); glVertex2f(ox+8,0); glVertex2f(ox,10); glVertex2f(ox,10); glVertex2f(ox+8,10); }

        // --- SZÁMOK (0-9) ---
        else if (c == '0') { glVertex2f(ox,0); glVertex2f(ox+8,0); glVertex2f(ox+8,0); glVertex2f(ox+8,10); glVertex2f(ox+8,10); glVertex2f(ox,10); glVertex2f(ox,10); glVertex2f(ox,0); glVertex2f(ox+8,0); glVertex2f(ox,10); }
        else if (c == '1') { glVertex2f(ox+4,0); glVertex2f(ox+4,10); glVertex2f(ox,2); glVertex2f(ox+4,0); }
        else if (c == '2') { glVertex2f(ox,0); glVertex2f(ox+8,0); glVertex2f(ox+8,0); glVertex2f(ox+8,5); glVertex2f(ox+8,5); glVertex2f(ox,5); glVertex2f(ox,5); glVertex2f(ox,10); glVertex2f(ox,10); glVertex2f(ox+8,10); }
        else if (c == '3') { glVertex2f(ox,0); glVertex2f(ox+8,0); glVertex2f(ox+8,0); glVertex2f(ox+8,10); glVertex2f(ox+8,10); glVertex2f(ox,10); glVertex2f(ox+4,5); glVertex2f(ox+8,5); }
        else if (c == '4') { glVertex2f(ox,0); glVertex2f(ox,5); glVertex2f(ox,5); glVertex2f(ox+8,5); glVertex2f(ox+6,0); glVertex2f(ox+6,10); }
        else if (c == '5') { glVertex2f(ox+8,0); glVertex2f(ox,0); glVertex2f(ox,0); glVertex2f(ox,5); glVertex2f(ox,5); glVertex2f(ox+8,5); glVertex2f(ox+8,5); glVertex2f(ox+8,10); glVertex2f(ox+8,10); glVertex2f(ox,10); }
        else if (c == '6') { glVertex2f(ox,0); glVertex2f(ox,10); glVertex2f(ox,0); glVertex2f(ox+8,0); glVertex2f(ox,5); glVertex2f(ox+8,5); glVertex2f(ox+8,5); glVertex2f(ox+8,10); glVertex2f(ox+8,10); glVertex2f(ox,10); }
        else if (c == '7') { glVertex2f(ox,0); glVertex2f(ox+8,0); glVertex2f(ox+8,0); glVertex2f(ox+4,10); }
        else if (c == '8') { glVertex2f(ox,0); glVertex2f(ox+8,0); glVertex2f(ox+8,0); glVertex2f(ox+8,10); glVertex2f(ox+8,10); glVertex2f(ox,10); glVertex2f(ox,10); glVertex2f(ox,0); glVertex2f(ox,5); glVertex2f(ox+8,5); }
        else if (c == '9') { glVertex2f(ox,0); glVertex2f(ox+8,0); glVertex2f(ox+8,0); glVertex2f(ox+8,10); glVertex2f(ox,5); glVertex2f(ox+8,5); glVertex2f(ox,0); glVertex2f(ox,5); }

        // --- ÍRÁSJELEK ---
        else if (c == '.') { glVertex2f(ox+3, 9); glVertex2f(ox+5, 9); glVertex2f(ox+3, 10); glVertex2f(ox+5, 10); }
        else if (c == ':') { glVertex2f(ox+4, 2); glVertex2f(ox+4, 3); glVertex2f(ox+4, 7); glVertex2f(ox+4, 8); }
        else if (c == '-') { glVertex2f(ox+2, 5); glVertex2f(ox+6, 5); }
    }
    glEnd();
    glLineWidth(1.0f);
    glPopMatrix();
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

void draw_sun_glow(float size, float r, float g, float b) {
    glPushMatrix();
    glDisable(GL_LIGHTING);
    glDisable(GL_TEXTURE_2D);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE); // Izzó hatás
    glDisable(GL_DEPTH_TEST);
    glDepthMask(GL_FALSE);

    int layers = 8;
    for (int i = 1; i <= layers; i++) {
        // Kifelé növő körök, de sokkal gyorsabban csökkenő átlátszóság
        float current_size = size * (1.0f + (float)i * 0.25f);
        float alpha = 0.12f * (1.0f - (float)i / layers);

        glColor4f(r, g, b, alpha);

        // Billboard mátrix: hogy mindig felénk nézzen
        float mv[16];
        glGetFloatv(GL_MODELVIEW_MATRIX, mv);
        for(int k=0; k<3; k++) for(int j=0; j<3; j++) {
            if(k==j) mv[k*4+j] = 1.0f; else mv[k*4+j] = 0.0f;
        }
        glLoadMatrixf(mv);

        glBegin(GL_TRIANGLE_FAN);
        glVertex3f(0, 0, 0);
        // 2 fokonként rajzolunk (180 pont egy körhöz), ettől lesz sima a széle
        for (int angle = 0; angle <= 360; angle += 2) {
            float rad = angle * 3.14159f / 180.0f;
            glVertex3f(cosf(rad) * current_size, sinf(rad) * current_size, 0);
        }
        glEnd();
    }

    glDepthMask(GL_TRUE);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
    glDisable(GL_BLEND);
    glPopMatrix();
}

typedef struct {
    float x, y, z;
} Vertex;

typedef struct {
    int v[3];
} Face;

typedef struct {
    Vertex* vertices;
    Face* faces;
    int vertex_count;
    int face_count;
    int initialized;
} OBJModel;

typedef struct {
    float x, y, z;
    float angle;
    float speed;
} Comet;

// Globális példányok
OBJModel comet_model = {NULL, NULL, 0, 0, 0};
Comet halley = {0, 0, 0, 0, 0.0009f}; // Ez oldja meg a "halley undeclared" hibát

void load_asteroid_obj(const char* filename, OBJModel* model) {
    FILE* file = fopen(filename, "r");
    if (!file) {
        printf("HIBA: Nem talalhato a fajl: %s\n", filename);
        model->initialized = 0;
        return;
    }

    char line[256];
    model->vertex_count = 0;
    model->face_count = 0;

    // 1. Kör: Megszámoljuk az adatokat
    while (fgets(line, sizeof(line), file)) {
        if (line[0] == 'v' && line[1] == ' ') model->vertex_count++;
        if (line[0] == 'f' && line[1] == ' ') model->face_count++;
    }

    // Memória foglalás
    model->vertices = (Vertex*)malloc(sizeof(Vertex) * model->vertex_count);
    model->faces = (Face*)malloc(sizeof(Face) * model->face_count);

    if (!model->vertices || !model->faces) {
        printf("HIBA: Nincs eleg memoria az OBJ-nek!\n");
        fclose(file);
        return;
    }

    rewind(file);

    int v_idx = 0, f_idx = 0;
    // 2. Kör: Beolvasás
    while (fgets(line, sizeof(line), file)) {
        if (line[0] == 'v' && line[1] == ' ') {
            sscanf(line, "v %f %f %f", &model->vertices[v_idx].x, &model->vertices[v_idx].y, &model->vertices[v_idx].z);
            v_idx++;
        } else if (line[0] == 'f' && line[1] == ' ') {
            // Beolvassuk az indexeket (kezeljük a v/vt/vn formátumot is az atoi-val)
            char* p = line + 2;
            for (int i = 0; i < 3; i++) {
                model->faces[f_idx].v[i] = atoi(p) - 1;
                while (*p && *p != ' ') p++;
                while (*p && *p == ' ') p++;
            }
            f_idx++;
        }
    }
    fclose(file);
    model->initialized = 1;
    printf("Siker: %s betoltve (%d vertex).\n", filename, model->vertex_count);
}

void draw_obj_model(OBJModel* model, float scale) {
    if (!model->initialized) return;
    glPushMatrix();
    glScalef(scale, scale, scale);
    glBegin(GL_TRIANGLES);
    for (int i = 0; i < model->face_count; i++) {
        for (int j = 0; j < 3; j++) {
            int idx = model->faces[i].v[j];
            glNormal3f(model->vertices[idx].x, model->vertices[idx].y, model->vertices[idx].z);
            glVertex3f(model->vertices[idx].x, model->vertices[idx].y, model->vertices[idx].z);
        }
    }
    glEnd();
    glPopMatrix();
}

void draw_comet(Comet* c, float delta_time) {
    c->angle += c->speed * delta_time;
    c->x = cosf(c->angle) * 40.0f;
    c->z = sinf(c->angle) * 15.0f;
    c->y = sinf(c->angle * 0.5f) * 10.0f;

    glPushMatrix();
    glTranslatef(c->x, c->y, c->z);

    // Szikla (OBJ)
    glEnable(GL_LIGHTING);
    glColor3f(0.8f, 0.8f, 0.9f);
    draw_obj_model(&comet_model, 0.6f);

    // Csóva (Rikító kék)
    glDisable(GL_LIGHTING);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);
    glColor4f(0.5f, 0.8f, 1.0f, 0.4f);

    // Itt a korábban megírt glBegin(GL_TRIANGLES) csóva kódod helye...

    glDisable(GL_BLEND);
    glPopMatrix();
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
/*
    halley.angle = 0;
    halley.speed = 0.5;
*/


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
    load_asteroid_obj("assets/asteroid.obj", &comet_model);

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
                // Nap glow - csak EGY finom réteg
                draw_sun_glow(current_sun_size * 1.5f, r, g, b);
                glDisable(GL_LIGHTING);
                glEnable(GL_BLEND);
                glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
                glColor4f(1.0f, 0.9f, 0.3f, 0.08f);
                GLUquadric* glowQuad = gluNewQuadric();
                gluSphere(glowQuad, current_sun_size * 1.6f, 32, 32);
                gluDeleteQuadric(glowQuad);
                glDisable(GL_BLEND);
                glEnable(GL_LIGHTING);
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


        halley.angle += 0.002f;
        if (halley.angle > 6.28f) halley.angle = 0;

        // Rajzolás
        draw_comet(&halley, delta_time);

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