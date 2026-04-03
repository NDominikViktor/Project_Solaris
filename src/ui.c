//
// Created by novakdominikviktor on 2026. 02. 28..
//

#include "ui.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <GL/glu.h>

const char* TEXTURE_FILES[TEXTURE_COUNT] = {
    "sun.jpg",     "mercury.jpg", "venus.jpg",  "earth.jpg",
    "moon.jpg",    "mars.jpg",    "jupiter.jpg","saturn.jpg",
    "uranus.jpg",  "neptune.jpg", "stars.jpg",  "asteroid.jpg"
};

// ─────────────────────────────────────────────────────────────────────────────
// Internal: 2D ortho setup / restore
// ─────────────────────────────────────────────────────────────────────────────

static void begin_2d(int win_w, int win_h) {
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    glOrtho(0, win_w, win_h, 0, -1, 1);
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_LIGHTING);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

static void end_2d(void) {
    glDisable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
}

// Draw a filled quad in screen space (top-left origin).
static void gl_fill_rect(float x, float y, float w, float h,
                          float r, float g, float b, float a) {
    glColor4f(r, g, b, a);
    glBegin(GL_QUADS);
    glVertex2f(x,     y);
    glVertex2f(x + w, y);
    glVertex2f(x + w, y + h);
    glVertex2f(x,     y + h);
    glEnd();
}

// Draw a rect outline.
static void gl_draw_rect(float x, float y, float w, float h,
                          float r, float g, float b, float a) {
    glColor4f(r, g, b, a);
    glBegin(GL_LINE_LOOP);
    glVertex2f(x,     y);
    glVertex2f(x + w, y);
    glVertex2f(x + w, y + h);
    glVertex2f(x,     y + h);
    glEnd();
}

// ─────────────────────────────────────────────────────────────────────────────
// Text rendering: SDL2_ttf → SDL_Surface → OpenGL texture → quad
// ─────────────────────────────────────────────────────────────────────────────

void ui_draw_text(TTF_Font* font, const char* text,
                  float x, float y,
                  float r, float g, float b, float a,
                  int win_w, int win_h) {
    if (!font || !text || text[0] == '\0') return;
    (void)win_w; (void)win_h;

    SDL_Color col = {
        (Uint8)(r * 255),
        (Uint8)(g * 255),
        (Uint8)(b * 255),
        (Uint8)(a * 255)
    };

    SDL_Surface* surf = TTF_RenderUTF8_Blended(font, text, col);
    if (!surf) return;

    // Convert to RGBA if needed
    SDL_Surface* rgba = SDL_ConvertSurfaceFormat(surf, SDL_PIXELFORMAT_RGBA32, 0);
    SDL_FreeSurface(surf);
    if (!rgba) return;

    GLuint tex;
    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D, tex);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA,
                 rgba->w, rgba->h, 0,
                 GL_RGBA, GL_UNSIGNED_BYTE, rgba->pixels);

    float tw = (float)rgba->w;
    float th = (float)rgba->h;
    SDL_FreeSurface(rgba);

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, tex);
    glColor4f(1.0f, 1.0f, 1.0f, a);
    glBegin(GL_QUADS);
    glTexCoord2f(0, 0); glVertex2f(x,      y);
    glTexCoord2f(1, 0); glVertex2f(x + tw, y);
    glTexCoord2f(1, 1); glVertex2f(x + tw, y + th);
    glTexCoord2f(0, 1); glVertex2f(x,      y + th);
    glEnd();
    glDisable(GL_TEXTURE_2D);

    glDeleteTextures(1, &tex);
}

// Centered text inside a rect.
static void draw_text_centered(TTF_Font* font, const char* text,
                                float rx, float ry, float rw, float rh,
                                float r, float g, float b, float a,
                                int win_w, int win_h) {
    if (!font) return;
    int tw, th;
    TTF_SizeUTF8(font, text, &tw, &th);
    float tx = rx + (rw - tw) / 2.0f;
    float ty = ry + (rh - th) / 2.0f;
    ui_draw_text(font, text, tx, ty, r, g, b, a, win_w, win_h);
}

// ─────────────────────────────────────────────────────────────────────────────
// Slider
// ─────────────────────────────────────────────────────────────────────────────

static float draw_slider(TTF_Font* font,
                          float x, float y, float w,
                          const char* label, float val, float vmin, float vmax,
                          int win_w, int win_h) {
    char buf[64];
    snprintf(buf, sizeof(buf), "%s: %.2f", label, val);
    ui_draw_text(font, buf, x, y, 0.8f, 0.8f, 0.8f, 1.0f, win_w, win_h);

    float ty = y + 22.0f;

    // Track
    gl_fill_rect(x, ty, w, 6, 0.23f, 0.23f, 0.31f, 1.0f);

    // Fill
    float t = (val - vmin) / (vmax - vmin);
    if (t < 0) t = 0; if (t > 1) t = 1;
    gl_fill_rect(x, ty, w * t, 6, 0.31f, 0.55f, 0.86f, 1.0f);

    // Knob
    float kx = x + w * t;
    gl_fill_rect(kx - 6, ty - 5, 12, 16, 0.78f, 0.86f, 1.0f, 1.0f);

    // Interaction
    int mx, my;
    Uint32 mb = SDL_GetMouseState(&mx, &my);
    if (mb & SDL_BUTTON(SDL_BUTTON_LEFT)) {
        if ((float)mx >= x - 4 && (float)mx <= x + w + 4 &&
            (float)my >= ty - 8 && (float)my <= ty + 14) {
            float new_t = ((float)mx - x) / w;
            if (new_t < 0) new_t = 0;
            if (new_t > 1) new_t = 1;
            return vmin + new_t * (vmax - vmin);
        }
    }
    return val;
}

// ─────────────────────────────────────────────────────────────────────────────
// Public API
// ─────────────────────────────────────────────────────────────────────────────

TTF_Font* ui_init(const char* font_path, int size) {
    if (TTF_Init() < 0) {
        printf("TTF_Init error: %s\n", TTF_GetError());
        return NULL;
    }
    TTF_Font* font = TTF_OpenFont(font_path, size);
    if (!font)
        printf("Font load error (%s): %s\n", font_path, TTF_GetError());
    return font;
}

void ui_quit(TTF_Font* font) {
    if (font) TTF_CloseFont(font);
    TTF_Quit();
}

// ── Main menu ────────────────────────────────────────────────────────────────

void ui_draw_menu(TTF_Font* font, int win_w, int win_h, Button btns[3]) {
    begin_2d(win_w, win_h);

    // Dark overlay
    gl_fill_rect(0, 0, (float)win_w, (float)win_h, 0.0f, 0.0f, 0.08f, 0.92f);

    // Title
    if (font) {
        int tw, th; TTF_SizeUTF8(font, "SOLARIS", &tw, &th);
        ui_draw_text(font, "SOLARIS",
                     (win_w - tw) / 2.0f, win_h * 0.18f,
                     0.7f, 0.82f, 1.0f, 1.0f, win_w, win_h);

        int sw, sh; TTF_SizeUTF8(font, "Solar System Simulator", &sw, &sh);
        ui_draw_text(font, "Solar System Simulator",
                     (win_w - sw) / 2.0f, win_h * 0.18f + th + 8,
                     0.6f, 0.6f, 0.7f, 1.0f, win_w, win_h);
    }

    // Buttons
    const char* labels[3] = { "Start Simulation", "Planet Editor", "Quit" };
    float bw = 260, bh = 52;
    float bx = (win_w - bw) / 2.0f;
    float by_start = win_h * 0.45f;

    for (int i = 0; i < 3; i++) {
        btns[i].x = bx;
        btns[i].y = by_start + i * 70.0f;
        btns[i].w = bw;
        btns[i].h = bh;
        btns[i].label = labels[i];

        if (btns[i].hovered)
            gl_fill_rect(bx, btns[i].y, bw, bh, 0.16f, 0.31f, 0.63f, 0.9f);
        else
            gl_fill_rect(bx, btns[i].y, bw, bh, 0.08f, 0.12f, 0.24f, 0.85f);

        float br = btns[i].hovered ? 0.39f : 0.24f;
        float bg = btns[i].hovered ? 0.71f : 0.31f;
        float bb = btns[i].hovered ? 1.0f  : 0.55f;
        gl_draw_rect(bx, btns[i].y, bw, bh, br, bg, bb, 1.0f);

        if (font)
            draw_text_centered(font, labels[i],
                               bx, btns[i].y, bw, bh,
                               btns[i].hovered ? 0.55f : 0.86f,
                               btns[i].hovered ? 0.78f : 0.86f,
                               1.0f, 1.0f, win_w, win_h);
    }

    // Footer
    if (font) {
        int fw, fh; TTF_SizeUTF8(font, "F1 for controls", &fw, &fh);
        ui_draw_text(font, "F1 for controls",
                     (win_w - fw) / 2.0f, win_h - 44.0f,
                     0.39f, 0.39f, 0.47f, 1.0f, win_w, win_h);
    }

    end_2d();
}

void ui_menu_click(int x, int y, Button btns[3], AppState* state, bool* running) {
    for (int i = 0; i < 3; i++) {
        if ((float)x >= btns[i].x && (float)x <= btns[i].x + btns[i].w &&
            (float)y >= btns[i].y && (float)y <= btns[i].y + btns[i].h) {
            if (i == 0) *state  = STATE_SIMULATION;
            if (i == 1) *state  = STATE_EDITOR;
            if (i == 2) *running = false;
        }
    }
}

void ui_menu_hover(int x, int y, Button btns[3]) {
    for (int i = 0; i < 3; i++)
        btns[i].hovered = ((float)x >= btns[i].x && (float)x <= btns[i].x + btns[i].w &&
                           (float)y >= btns[i].y && (float)y <= btns[i].y + btns[i].h);
}

// ── Editor panel ─────────────────────────────────────────────────────────────

#define PANEL_W 340.0f

void ui_draw_editor(TTF_Font* font, World* world, EditorState* es,
                    int win_w, int win_h) {
    begin_2d(win_w, win_h);

    // Panel background
    gl_fill_rect(0, 0, PANEL_W, (float)win_h, 0.04f, 0.05f, 0.11f, 0.92f);
    gl_draw_rect(0, 0, PANEL_W, (float)win_h, 0.20f, 0.27f, 0.51f, 1.0f);

    if (font)
        ui_draw_text(font, "Planet Editor", 14, 12,
                     0.31f, 0.63f, 1.0f, 1.0f, win_w, win_h);

    // Planet list
    float list_y = 50.0f;
    if (font)
        ui_draw_text(font, "Planets:", 14, list_y,
                     0.51f, 0.51f, 0.59f, 1.0f, win_w, win_h);
    list_y += 24.0f;

    for (int i = 0; i < world->count; i++) {
        float ry = list_y + i * 26.0f;
        if (i == es->selected) {
            gl_fill_rect(8, ry, PANEL_W - 16, 24, 0.12f, 0.24f, 0.47f, 0.9f);
            gl_draw_rect(8, ry, PANEL_W - 16, 24, 0.31f, 0.55f, 1.0f, 0.8f);
        }
        float tc = (i == es->selected) ? 1.0f : 0.86f;
        float tg = (i == es->selected) ? 0.78f : 0.86f;
        if (font)
            ui_draw_text(font, world->planets[i].name, 16, ry + 4,
                         0.31f * (1 - tc) + tc, tg, 1.0f * tc + 0.86f * (1 - tc),
                         1.0f, win_w, win_h);
    }

    float after_list = list_y + world->count * 26.0f + 8.0f;

    // Row 1: Add / Delete / Back
    float btn_y = after_list;
    gl_fill_rect(8,           btn_y, 90, 26, 0.12f, 0.31f, 0.16f, 0.9f);
    gl_draw_rect(8,           btn_y, 90, 26, 0.24f, 0.71f, 0.31f, 1.0f);
    if (font) draw_text_centered(font, "Add",    8,           btn_y, 90, 26, 0.31f, 0.78f, 0.39f, 1.0f, win_w, win_h);
    gl_fill_rect(106,         btn_y, 90, 26, 0.31f, 0.08f, 0.08f, 0.9f);
    gl_draw_rect(106,         btn_y, 90, 26, 0.78f, 0.24f, 0.24f, 1.0f);
    if (font) draw_text_centered(font, "Delete", 106,         btn_y, 90, 26, 0.86f, 0.31f, 0.31f, 1.0f, win_w, win_h);
    gl_fill_rect(PANEL_W-98,  btn_y, 90, 26, 0.08f, 0.12f, 0.24f, 0.9f);
    gl_draw_rect(PANEL_W-98,  btn_y, 90, 26, 0.24f, 0.31f, 0.55f, 1.0f);
    if (font) draw_text_centered(font, "< Menu", PANEL_W-98,  btn_y, 90, 26, 0.86f, 0.86f, 0.86f, 1.0f, win_w, win_h);
    // Row 2: New System / Load Default
    float btn_y2 = btn_y + 30.0f;
    float half_w = (PANEL_W - 24.0f) / 2.0f;
    gl_fill_rect(8,            btn_y2, half_w, 24, 0.20f, 0.10f, 0.30f, 0.9f);
    gl_draw_rect(8,            btn_y2, half_w, 24, 0.60f, 0.30f, 0.90f, 1.0f);
    if (font) draw_text_centered(font, "New System",  8,           btn_y2, half_w, 24, 0.80f, 0.60f, 1.0f, 1.0f, win_w, win_h);
    gl_fill_rect(16+half_w,    btn_y2, half_w, 24, 0.08f, 0.18f, 0.28f, 0.9f);
    gl_draw_rect(16+half_w,    btn_y2, half_w, 24, 0.24f, 0.50f, 0.78f, 1.0f);
    if (font) draw_text_centered(font, "Load Default", 16+half_w,  btn_y2, half_w, 24, 0.60f, 0.80f, 1.0f, 1.0f, win_w, win_h);

    // Property sliders
    if (es->selected < 0 || es->selected >= world->count) { end_2d(); return; }

    Planet* p = &world->planets[es->selected];
    float sx = 14.0f, sw = PANEL_W - 28.0f;
    float sy = btn_y + 70.0f;   // btn_y + row1(26) + row2(24) + padding(20)

    // Planet name — editable text field
    {
        float field_col_r = es->editing_name ? 0.10f : 0.06f;
        float field_col_g = es->editing_name ? 0.16f : 0.08f;
        float field_col_b = es->editing_name ? 0.28f : 0.14f;
        gl_fill_rect(sx, sy, sw, 24, field_col_r, field_col_g, field_col_b, 0.9f);
        float bdr = es->editing_name ? 0.31f : 0.20f;
        float bdg = es->editing_name ? 0.63f : 0.27f;
        float bdb = es->editing_name ? 1.00f : 0.51f;
        gl_draw_rect(sx, sy, sw, 24, bdr, bdg, bdb, 1.0f);
        // Show name + cursor when editing
        char display[36];
        if (es->editing_name) {
            snprintf(display, sizeof(display), "%s|", p->name);
        } else {
            snprintf(display, sizeof(display), "%s  (click to rename)", p->name);
        }
        if (font) ui_draw_text(font, display, sx + 4, sy + 4,
                               0.31f, 0.63f, 1.0f, 1.0f, win_w, win_h);
    }
    sy += 32.0f;

    p->size          = draw_slider(font, sx, sy, sw, "Size",         p->size,          0.1f,  3.0f,  win_w, win_h); sy += 44.0f;
    p->distance      = draw_slider(font, sx, sy, sw, "Distance",     p->distance,      0.0f, 60.0f,  win_w, win_h); sy += 44.0f;
    p->orbit_speed   = draw_slider(font, sx, sy, sw, "Orbit speed",  p->orbit_speed,   0.0f,  0.5f,  win_w, win_h); sy += 44.0f;
    p->rotation_speed= draw_slider(font, sx, sy, sw, "Rotation",     p->rotation_speed,0.0f,  2.0f,  win_w, win_h); sy += 44.0f;
    p->axial_tilt    = draw_slider(font, sx, sy, sw, "Axial tilt",   p->axial_tilt,    0.0f,180.0f,  win_w, win_h); sy += 44.0f;

    // Rings toggle button
    {
        float btn_fill_r = p->has_rings ? 0.12f : 0.08f;
        float btn_fill_g = p->has_rings ? 0.31f : 0.09f;
        float btn_fill_b = p->has_rings ? 0.16f : 0.19f;
        gl_fill_rect(sx, sy, sw, 26, btn_fill_r, btn_fill_g, btn_fill_b, 0.9f);
        float btn_br = p->has_rings ? 0.24f : 0.31f;
        float btn_bg = p->has_rings ? 0.71f : 0.55f;
        float btn_bb = p->has_rings ? 0.31f : 0.86f;
        gl_draw_rect(sx, sy, sw, 26, btn_br, btn_bg, btn_bb, 1.0f);
        char ring_buf[32];
        snprintf(ring_buf, sizeof(ring_buf), "Rings: %s  (click to toggle)", p->has_rings ? "ON" : "OFF");
        if (font) draw_text_centered(font, ring_buf, sx, sy, sw, 26,
                                     p->has_rings ? 0.31f : 0.51f,
                                     p->has_rings ? 0.78f : 0.51f,
                                     p->has_rings ? 0.39f : 0.59f,
                                     1.0f, win_w, win_h);
    }
    sy += 34.0f;

    // Texture picker
    if (font) ui_draw_text(font, "Texture:", sx, sy, 0.51f, 0.51f, 0.59f, 1.0f, win_w, win_h);
    sy += 22.0f;

    int tx_cols = 4;
    float tx_w = (sw - (tx_cols - 1) * 4.0f) / tx_cols;
    float tx_h = 20.0f;

    for (int i = 0; i < TEXTURE_COUNT; i++) {
        float col = (float)(i % tx_cols);
        float row = (float)(i / tx_cols);
        float tx = sx + col * (tx_w + 4);
        float ty = sy + row * (tx_h + 4);

        if (i == es->selected_texture)
            gl_fill_rect(tx, ty, tx_w, tx_h, 0.16f, 0.31f, 0.63f, 0.9f);
        else
            gl_fill_rect(tx, ty, tx_w, tx_h, 0.08f, 0.09f, 0.19f, 0.85f);

        float bc = (i == es->selected_texture) ? 1.0f : 0.39f;
        gl_draw_rect(tx, ty, tx_w, tx_h,
                     0.31f * (1 - bc) + bc * 0.31f,
                     0.55f * bc + 0.24f * (1 - bc),
                     bc, 1.0f);

        char label[32];
        strncpy(label, TEXTURE_FILES[i], sizeof(label) - 1);
        label[sizeof(label) - 1] = '\0';
        char* dot = strrchr(label, '.');
        if (dot) *dot = '\0';

        if (font)
            ui_draw_text(font, label, tx + 2, ty + 2,
                         i == es->selected_texture ? 0.55f : 0.51f,
                         i == es->selected_texture ? 0.78f : 0.51f,
                         i == es->selected_texture ? 1.0f  : 0.59f,
                         1.0f, win_w, win_h);
    }

    int tx_rows = (TEXTURE_COUNT + tx_cols - 1) / tx_cols;
    sy += tx_rows * (tx_h + 4) + 12.0f;

    // Save button
    gl_fill_rect(sx, sy, sw, 32, 0.08f, 0.24f, 0.12f, 0.9f);
    gl_draw_rect(sx, sy, sw, 32, 0.24f, 0.78f, 0.31f, 1.0f);
    if (font) draw_text_centered(font, "Save Custom", sx, sy, sw, 32,
                                  0.31f, 0.78f, 0.39f, 1.0f, win_w, win_h);

    end_2d();
}

// Helper: add a blank planet to the world
static void add_blank_planet(World* world, EditorState* es) {
    if (world->count >= 20) return;
    Planet* p = &world->planets[world->count];
    snprintf(p->name, sizeof(p->name), "NewObj%d", world->count);
    p->distance = 10.0f; p->size = 0.5f;
    p->orbit_speed = 0.05f; p->rotation_speed = 0.3f;
    p->axial_tilt = 0.0f; p->has_atmosphere = 0;
    p->atmo_r = p->atmo_g = p->atmo_b = 0.0f;
    p->ring_particles = NULL; p->particle_count = 0;
    p->has_rings = 0; p->parent_index = 0;
    p->world_x = p->world_y = p->world_z = 0.0f;
    p->current_angle = p->rotation_angle = 0.0f;
    strncpy(p->texture_name, TEXTURE_FILES[es->selected_texture], sizeof(p->texture_name)-1);
    char path[128];
    snprintf(path, sizeof(path), "assets/%s", p->texture_name);
    extern GLuint load_texture(const char*);
    p->texture_id = load_texture(path);
    es->selected = world->count++;
}

void ui_editor_click(int x, int y, World* world, EditorState* es,
                     int win_w, int win_h, AppState* state) {
    (void)win_w; (void)win_h;
    if ((float)x > PANEL_W) return;

    // ── Layout constants (must match ui_draw_editor exactly) ────────────────
    float list_y     = 74.0f;
    float after_list = list_y + world->count * 26.0f + 8.0f;
    float btn_y      = after_list;
    float btn_y2     = btn_y + 30.0f;
    float half_w     = (PANEL_W - 24.0f) / 2.0f;
    float sx = 14.0f, sw = PANEL_W - 28.0f;
    // sliders start here (must match draw)
    float sy = btn_y + 70.0f;

    // ── Planet list ───────────────────────────────────────────────────────────
    for (int i = 0; i < world->count; i++) {
        float ry = list_y + i * 26.0f;
        if ((float)y >= ry && (float)y < ry + 26.0f) {
            es->selected = i;
            // sync texture selector to current planet
            for (int t = 0; t < TEXTURE_COUNT; t++) {
                if (strcmp(world->planets[i].texture_name, TEXTURE_FILES[t]) == 0) {
                    es->selected_texture = t; break;
                }
            }
            return;
        }
    }

    // ── Row 1 buttons ─────────────────────────────────────────────────────────
    // Add
    if ((float)x >= 8 && (float)x <= 98 && (float)y >= btn_y && (float)y <= btn_y + 26) {
        add_blank_planet(world, es); return;
    }
    // Delete
    if ((float)x >= 106 && (float)x <= 196 && (float)y >= btn_y && (float)y <= btn_y + 26) {
        if (es->selected >= 0 && es->selected < world->count) {
            free_ring_particles(&world->planets[es->selected]);
            for (int i = es->selected; i < world->count - 1; i++)
                world->planets[i] = world->planets[i + 1];
            world->count--;
            if (es->selected >= world->count) es->selected = world->count - 1;
        }
        return;
    }
    // Back to menu
    if ((float)x >= PANEL_W-98 && (float)x <= PANEL_W-8 &&
        (float)y >= btn_y && (float)y <= btn_y + 26) {
        *state = STATE_MENU; return;
    }

    // ── Row 2 buttons ─────────────────────────────────────────────────────────
    // New System
    if ((float)x >= 8 && (float)x <= 8+half_w &&
        (float)y >= btn_y2 && (float)y <= btn_y2 + 24) {
        // Free all ring particles first
        for (int i = 0; i < world->count; i++)
            free_ring_particles(&world->planets[i]);
        world->count = 0;
        es->selected = -1;
        // Add a default star
        add_blank_planet(world, es);
        strncpy(world->planets[0].name, "Star", 32);
        world->planets[0].distance = 0.0f;
        world->planets[0].size     = 2.0f;
        world->planets[0].orbit_speed = 0.0f;
        world->planets[0].parent_index = -1;
        es->selected = 0;
        return;
    }
    // Load Default
    if ((float)x >= 16+half_w && (float)x <= 16+half_w*2 &&
        (float)y >= btn_y2 && (float)y <= btn_y2 + 24) {
        for (int i = 0; i < world->count; i++)
            free_ring_particles(&world->planets[i]);
        extern void load_planets(World*, const char*);
        load_planets(world, "assets/planets.csv");
        es->selected = -1;
        return;
    }

    // ── Property area ─────────────────────────────────────────────────────────
    if (es->selected < 0 || es->selected >= world->count) return;

    // Planet name field: drawn at sy..sy+24, then sy advances by 32
    if ((float)x >= sx && (float)x <= sx + sw &&
        (float)y >= sy && (float)y <= sy + 24.0f) {
        es->editing_name = true;
        SDL_StartTextInput();
        return;
    }

    // Name field occupies sy..sy+24, then sy advances 32px (matches draw)
    float sy_sliders = sy + 32.0f;
    // Sliders: 5 sliders * 44px each
    float sy_after_sliders = sy_sliders + 5 * 44.0f;

    // Rings toggle button
    float sy_rings = sy_after_sliders;  // = sy+32 + 5*44
    if ((float)x >= sx && (float)x <= sx + sw &&
        (float)y >= sy_rings && (float)y <= sy_rings + 26) {
        Planet* p = &world->planets[es->selected];
        p->has_rings = !p->has_rings;
        if (p->has_rings) init_ring_particles(p);
        else              free_ring_particles(p);
        return;
    }

    float sy_tex = sy_rings + 34.0f + 22.0f;
    int tx_cols = 4;
    float tx_w = (sw - (tx_cols - 1) * 4.0f) / tx_cols;
    float tx_h = 20.0f;

    // Texture picker
    for (int i = 0; i < TEXTURE_COUNT; i++) {
        float col = (float)(i % tx_cols);
        float row = (float)(i / tx_cols);
        float tx = sx + col * (tx_w + 4);
        float ty = sy_tex + row * (tx_h + 4);
        if ((float)x >= tx && (float)x <= tx + tx_w &&
            (float)y >= ty && (float)y <= ty + tx_h) {
            es->selected_texture = i;
            Planet* p = &world->planets[es->selected];
            strncpy(p->texture_name, TEXTURE_FILES[i], sizeof(p->texture_name)-1);
            char path[128];
            snprintf(path, sizeof(path), "assets/%s", TEXTURE_FILES[i]);
            extern GLuint load_texture(const char*);
            p->texture_id = load_texture(path);
            return;
        }
    }

    // Save button
    int tx_rows = (TEXTURE_COUNT + tx_cols - 1) / tx_cols;
    float save_y = sy_tex + tx_rows * (tx_h + 4) + 12.0f;
    if ((float)x >= sx && (float)x <= sx + sw &&
        (float)y >= save_y && (float)y <= save_y + 32) {
        save_planets(world, "assets/custom_planets.csv");
        printf("Saved to assets/custom_planets.csv\n");
        return;
    }
}

// ── Save ─────────────────────────────────────────────────────────────────────

void save_planets(World* world, const char* filename) {
    FILE* f = fopen(filename, "w");
    if (!f) { printf("Error: cannot open %s for writing\n", filename); return; }

    fprintf(f, "#name,distance,size,orbit_speed,rotation_speed,axial_tilt,texture,has_atmo,atmo_r,atmo_g,atmo_b,has_rings,parent\n");

    for (int i = 0; i < world->count; i++) {
        const Planet* p = &world->planets[i];
        const char* parent_name = (p->parent_index >= 0 && p->parent_index < world->count)
            ? world->planets[p->parent_index].name : "";

        fprintf(f, "%s,%.4f,%.4f,%.4f,%.4f,%.4f,%s,%d,%.2f,%.2f,%.2f,%d,%s\n",
                p->name, p->distance, p->size,
                p->orbit_speed, p->rotation_speed, p->axial_tilt,
                p->texture_name, p->has_atmosphere,
                p->atmo_r, p->atmo_g, p->atmo_b, p->has_rings, parent_name);
    }
    fclose(f);
}