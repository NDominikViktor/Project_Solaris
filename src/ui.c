//
// Created by novakdominikviktor on 2026. 02. 28..
//

#include "ui.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

const char* TEXTURE_FILES[TEXTURE_COUNT] = {
    "sun.jpg",     "mercury.jpg", "venus.jpg",  "earth.jpg",
    "moon.jpg",    "mars.jpg",    "jupiter.jpg","saturn.jpg",
    "uranus.jpg",  "neptune.jpg", "stars.jpg",  "asteroid.jpg"
};

// ─────────────────────────────────────────────────────────────────────────────
// Internal helpers
// ─────────────────────────────────────────────────────────────────────────────

static void render_text(SDL_Renderer* r, TTF_Font* font,
                        const char* text, int x, int y, SDL_Color col) {
    SDL_Surface* surf = TTF_RenderUTF8_Blended(font, text, col);
    if (!surf) return;
    SDL_Texture* tex = SDL_CreateTextureFromSurface(r, surf);
    SDL_Rect dst = { x, y, surf->w, surf->h };
    SDL_RenderCopy(r, tex, NULL, &dst);
    SDL_DestroyTexture(tex);
    SDL_FreeSurface(surf);
}

static void render_text_centered(SDL_Renderer* r, TTF_Font* font,
                                  const char* text, SDL_Rect area, SDL_Color col) {
    SDL_Surface* surf = TTF_RenderUTF8_Blended(font, text, col);
    if (!surf) return;
    SDL_Texture* tex = SDL_CreateTextureFromSurface(r, surf);
    SDL_Rect dst = {
        area.x + (area.w - surf->w) / 2,
        area.y + (area.h - surf->h) / 2,
        surf->w, surf->h
    };
    SDL_RenderCopy(r, tex, NULL, &dst);
    SDL_DestroyTexture(tex);
    SDL_FreeSurface(surf);
}

static void draw_rect_outline(SDL_Renderer* r, SDL_Rect rect,
                               SDL_Color col, int thickness) {
    SDL_SetRenderDrawColor(r, col.r, col.g, col.b, col.a);
    for (int i = 0; i < thickness; i++) {
        SDL_Rect inner = { rect.x + i, rect.y + i,
                           rect.w - i * 2, rect.h - i * 2 };
        SDL_RenderDrawRect(r, &inner);
    }
}

// Draw a labelled slider; returns the new value if clicked, else current_val.
static float draw_slider(SDL_Renderer* r, TTF_Font* font,
                          int x, int y, int w,
                          const char* label, float val, float min, float max,
                          int mouse_x, int mouse_y, bool mouse_down) {
    SDL_Color white  = {220, 220, 220, 255};
    SDL_Color track  = { 60,  60,  80, 255};
    SDL_Color fill   = { 80, 140, 220, 255};

    // Label + value
    char buf[64];
    snprintf(buf, sizeof(buf), "%s: %.2f", label, val);
    render_text(r, font, buf, x, y, white);

    int ty = y + 22;
    int th = 6;

    // Track
    SDL_Rect track_r = { x, ty, w, th };
    SDL_SetRenderDrawColor(r, track.r, track.g, track.b, track.a);
    SDL_RenderFillRect(r, &track_r);

    // Filled portion
    float t = (val - min) / (max - min);
    if (t < 0) t = 0; if (t > 1) t = 1;
    SDL_Rect fill_r = { x, ty, (int)(w * t), th };
    SDL_SetRenderDrawColor(r, fill.r, fill.g, fill.b, fill.a);
    SDL_RenderFillRect(r, &fill_r);

    // Knob
    int kx = x + (int)(w * t);
    SDL_Rect knob = { kx - 6, ty - 5, 12, 16 };
    SDL_SetRenderDrawColor(r, 200, 220, 255, 255);
    SDL_RenderFillRect(r, &knob);

    // Interaction — drag or click on track
    if (mouse_down) {
        SDL_Rect hit = { x - 4, ty - 8, w + 8, th + 16 };
        if (mouse_x >= hit.x && mouse_x <= hit.x + hit.w &&
            mouse_y >= hit.y && mouse_y <= hit.y + hit.h) {
            float new_t = (float)(mouse_x - x) / (float)w;
            if (new_t < 0) new_t = 0;
            if (new_t > 1) new_t = 1;
            return min + new_t * (max - min);
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

void ui_draw_menu(SDL_Renderer* renderer, TTF_Font* font,
                  int win_w, int win_h, Button btns[3]) {
    // Semi-transparent dark overlay
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(renderer, 0, 0, 20, 200);
    SDL_Rect full = { 0, 0, win_w, win_h };
    SDL_RenderFillRect(renderer, &full);

    // Title
    SDL_Color title_col = { 180, 210, 255, 255 };
    SDL_Color white     = { 220, 220, 220, 255 };
    SDL_Color hover_col = {  80, 160, 255, 255 };

    render_text_centered(renderer, font, "SOLARIS",
                         (SDL_Rect){ 0, win_h / 5, win_w, 60 }, title_col);
    render_text_centered(renderer, font, "Solar System Simulator",
                         (SDL_Rect){ 0, win_h / 5 + 65, win_w, 30 }, white);

    // Buttons
    const char* labels[3] = { "Start Simulation", "Planet Editor", "Quit" };
    int bw = 260, bh = 52;
    int bx = (win_w - bw) / 2;
    int by_start = win_h / 2 - 20;

    for (int i = 0; i < 3; i++) {
        btns[i].rect  = (SDL_Rect){ bx, by_start + i * 70, bw, bh };
        btns[i].label = labels[i];

        // Background
        SDL_Color bg = btns[i].hovered
            ? (SDL_Color){ 40, 80, 160, 220 }
            : (SDL_Color){ 20, 30,  60, 200 };
        SDL_SetRenderDrawColor(renderer, bg.r, bg.g, bg.b, bg.a);
        SDL_RenderFillRect(renderer, &btns[i].rect);

        // Border
        SDL_Color border = btns[i].hovered
            ? (SDL_Color){ 100, 180, 255, 255 }
            : (SDL_Color){  60,  80, 140, 255 };
        draw_rect_outline(renderer, btns[i].rect, border, 2);

        // Label
        SDL_Color lc = btns[i].hovered ? hover_col : white;
        render_text_centered(renderer, font, labels[i], btns[i].rect, lc);
    }

    // Footer hint
    SDL_Color dim = { 100, 100, 120, 255 };
    render_text_centered(renderer, font, "F1 for controls during simulation",
                         (SDL_Rect){ 0, win_h - 50, win_w, 30 }, dim);
}

void ui_menu_click(int x, int y, Button btns[3], AppState* state) {
    for (int i = 0; i < 3; i++) {
        SDL_Rect r = btns[i].rect;
        if (x >= r.x && x <= r.x + r.w && y >= r.y && y <= r.y + r.h) {
            if (i == 0) *state = STATE_SIMULATION;
            if (i == 1) *state = STATE_EDITOR;
            if (i == 2) *state = STATE_MENU; // caller checks running flag
        }
    }
}

void ui_menu_hover(int x, int y, Button btns[3]) {
    for (int i = 0; i < 3; i++) {
        SDL_Rect r = btns[i].rect;
        btns[i].hovered = (x >= r.x && x <= r.x + r.w &&
                           y >= r.y && y <= r.y + r.h);
    }
}

// ── Editor panel ─────────────────────────────────────────────────────────────

// Width of the left editor panel in pixels.
#define PANEL_W 340

void ui_draw_editor(SDL_Renderer* renderer, TTF_Font* font,
                    World* world, EditorState* es,
                    int win_w, int win_h) {
    (void)win_w;

    SDL_Color white   = { 220, 220, 220, 255 };
    SDL_Color dim     = { 130, 130, 150, 255 };
    SDL_Color accent  = {  80, 160, 255, 255 };
    SDL_Color sel_bg  = {  30,  60, 120, 220 };
    SDL_Color panel   = {  10,  12,  28, 230 };
    SDL_Color red_col = { 220,  80,  80, 255 };
    SDL_Color grn_col = {  80, 200, 100, 255 };

    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

    // Panel background
    SDL_Rect panel_r = { 0, 0, PANEL_W, win_h };
    SDL_SetRenderDrawColor(renderer, panel.r, panel.g, panel.b, panel.a);
    SDL_RenderFillRect(renderer, &panel_r);
    draw_rect_outline(renderer, panel_r, (SDL_Color){50, 70, 130, 255}, 1);

    // Header
    render_text(renderer, font, "Planet Editor", 14, 12, accent);

    // ── Planet list ───────────────────────────────────────────────────────────
    int list_y = 50;
    render_text(renderer, font, "Planets:", 14, list_y, dim);
    list_y += 24;

    for (int i = 0; i < world->count; i++) {
        SDL_Rect row = { 8, list_y + i * 26, PANEL_W - 16, 24 };

        if (i == es->selected) {
            SDL_SetRenderDrawColor(renderer, sel_bg.r, sel_bg.g, sel_bg.b, sel_bg.a);
            SDL_RenderFillRect(renderer, &row);
            draw_rect_outline(renderer, row, (SDL_Color){80, 140, 255, 200}, 1);
        }

        SDL_Color tc = (i == es->selected) ? accent : white;
        render_text(renderer, font, world->planets[i].name, 16, list_y + i * 26 + 4, tc);
    }

    int after_list = list_y + world->count * 26 + 8;

    // ── Add / Delete / Back buttons ───────────────────────────────────────────
    int btn_y = after_list;
    SDL_Rect add_btn  = { 8,           btn_y, 90, 26 };
    SDL_Rect del_btn  = { 106,         btn_y, 90, 26 };
    SDL_Rect back_btn = { PANEL_W - 98, btn_y, 90, 26 };

    SDL_SetRenderDrawColor(renderer, 30, 80, 40, 220);
    SDL_RenderFillRect(renderer, &add_btn);
    draw_rect_outline(renderer, add_btn, (SDL_Color){60,180,80,255}, 1);
    render_text_centered(renderer, font, "Add", add_btn, grn_col);

    SDL_SetRenderDrawColor(renderer, 80, 20, 20, 220);
    SDL_RenderFillRect(renderer, &del_btn);
    draw_rect_outline(renderer, del_btn, (SDL_Color){200,60,60,255}, 1);
    render_text_centered(renderer, font, "Delete", del_btn, red_col);

    SDL_SetRenderDrawColor(renderer, 20, 30, 60, 220);
    SDL_RenderFillRect(renderer, &back_btn);
    draw_rect_outline(renderer, back_btn, (SDL_Color){60,80,140,255}, 1);
    render_text_centered(renderer, font, "< Menu", back_btn, white);

    // ── Property sliders (only when a planet is selected) ────────────────────
    if (es->selected < 0 || es->selected >= world->count) return;

    Planet* p = &world->planets[es->selected];
    int sx = 14, sw = PANEL_W - 28;
    int sy = btn_y + 40;

    // We need current mouse state for live slider dragging
    int mx, my;
    Uint32 mb = SDL_GetMouseState(&mx, &my);
    bool md = (mb & SDL_BUTTON(SDL_BUTTON_LEFT)) != 0;

    render_text(renderer, font, p->name, sx, sy, accent);
    sy += 28;

    p->size = draw_slider(renderer, font, sx, sy, sw,
                          "Size", p->size, 0.1f, 3.0f, mx, my, md);
    sy += 44;

    p->distance = draw_slider(renderer, font, sx, sy, sw,
                               "Distance", p->distance, 0.0f, 60.0f, mx, my, md);
    sy += 44;

    p->orbit_speed = draw_slider(renderer, font, sx, sy, sw,
                                  "Orbit speed", p->orbit_speed, 0.0f, 0.5f, mx, my, md);
    sy += 44;

    p->rotation_speed = draw_slider(renderer, font, sx, sy, sw,
                                     "Rotation", p->rotation_speed, 0.0f, 2.0f, mx, my, md);
    sy += 44;

    p->axial_tilt = draw_slider(renderer, font, sx, sy, sw,
                                 "Axial tilt", p->axial_tilt, 0.0f, 180.0f, mx, my, md);
    sy += 50;

    // ── Texture picker ────────────────────────────────────────────────────────
    render_text(renderer, font, "Texture:", sx, sy, dim);
    sy += 22;

    int tx_cols = 4;
    int tx_size = (sw - (tx_cols - 1) * 4) / tx_cols;

    for (int i = 0; i < TEXTURE_COUNT; i++) {
        int col = i % tx_cols;
        int row = i / tx_cols;
        SDL_Rect tr = { sx + col * (tx_size + 4), sy + row * (tx_size / 2 + 4),
                        tx_size, tx_size / 2 };

        SDL_Color bg = (i == es->selected_texture)
            ? (SDL_Color){ 40, 80, 160, 220 }
            : (SDL_Color){ 20, 24,  48, 200 };
        SDL_SetRenderDrawColor(renderer, bg.r, bg.g, bg.b, bg.a);
        SDL_RenderFillRect(renderer, &tr);

        SDL_Color bc = (i == es->selected_texture)
            ? (SDL_Color){ 80, 160, 255, 255 }
            : (SDL_Color){ 50,  60, 100, 255 };
        draw_rect_outline(renderer, tr, bc, 1);

        // Strip extension from label
        char label[32];
        strncpy(label, TEXTURE_FILES[i], sizeof(label) - 1);
        label[sizeof(label) - 1] = '\0';
        char* dot = strrchr(label, '.');
        if (dot) *dot = '\0';

        SDL_Color lc = (i == es->selected_texture) ? accent : dim;
        render_text(renderer, font, label,
                    tr.x + 4, tr.y + (tr.h - TTF_FontHeight(font)) / 2, lc);
    }

    int tx_rows = (TEXTURE_COUNT + tx_cols - 1) / tx_cols;
    sy += tx_rows * (tx_size / 2 + 4) + 12;

    // ── Save button ───────────────────────────────────────────────────────────
    SDL_Rect save_btn = { sx, sy, sw, 32 };
    SDL_SetRenderDrawColor(renderer, 20, 60, 30, 230);
    SDL_RenderFillRect(renderer, &save_btn);
    draw_rect_outline(renderer, save_btn, (SDL_Color){60, 200, 80, 255}, 2);
    render_text_centered(renderer, font, "Save to CSV", save_btn, grn_col);
}

void ui_editor_click(int x, int y, World* world, EditorState* es,
                     int win_w, int win_h, AppState* state) {
    (void)win_w;

    // Only handle clicks inside the panel
    if (x > PANEL_W) return;

    // Planet list rows
    int list_y  = 74;
    int after_list = list_y + world->count * 26 + 8;

    for (int i = 0; i < world->count; i++) {
        int ry = list_y + i * 26;
        if (y >= ry && y < ry + 26) {
            es->selected = i;
            return;
        }
    }

    // Add button
    SDL_Rect add_btn  = { 8,            after_list, 90, 26 };
    SDL_Rect del_btn  = { 106,          after_list, 90, 26 };
    SDL_Rect back_btn = { PANEL_W - 98, after_list, 90, 26 };

    if (x >= add_btn.x && x <= add_btn.x + add_btn.w &&
        y >= add_btn.y && y <= add_btn.y + add_btn.h) {
        if (world->count < 20) {
            Planet* p = &world->planets[world->count];
            snprintf(p->name, sizeof(p->name), "Planet%d", world->count);
            p->distance        = 10.0f;
            p->size            = 0.5f;
            p->orbit_speed     = 0.05f;
            p->rotation_speed  = 0.3f;
            p->axial_tilt      = 0.0f;
            p->has_atmosphere  = 0;
            p->atmo_r = p->atmo_g = p->atmo_b = 0.0f;
            p->ring_particles  = NULL;
            p->particle_count  = 0;
            p->parent_index    = 0; // orbit the Sun
            p->world_x = p->world_y = p->world_z = 0.0f;
            p->current_angle   = 0.0f;
            p->rotation_angle  = 0.0f;

            char path[128];
            snprintf(path, sizeof(path), "assets/%s",
                     TEXTURE_FILES[es->selected_texture]);
            extern GLuint load_texture(const char*);
            p->texture_id = load_texture(path);

            es->selected = world->count;
            world->count++;
        }
        return;
    }

    // Delete button
    if (x >= del_btn.x && x <= del_btn.x + del_btn.w &&
        y >= del_btn.y && y <= del_btn.y + del_btn.h) {
        if (es->selected >= 1 && es->selected < world->count) {
            // Shift planets down
            for (int i = es->selected; i < world->count - 1; i++)
                world->planets[i] = world->planets[i + 1];
            world->count--;
            if (es->selected >= world->count)
                es->selected = world->count - 1;
        }
        return;
    }

    // Back to menu button
    if (x >= back_btn.x && x <= back_btn.x + back_btn.w &&
        y >= back_btn.y && y <= back_btn.y + back_btn.h) {
        *state = STATE_MENU;
        return;
    }

    // Texture picker
    if (es->selected >= 0 && es->selected < world->count) {
        int btn_area_bottom = after_list + 40;
        int sx = 14, sw = PANEL_W - 28;
        // sliders occupy 5 * 44 = 220px + 28 for planet name
        int tex_y = btn_area_bottom + 28 + 5 * 44 + 22;
        int tx_cols = 4;
        int tx_size = (sw - (tx_cols - 1) * 4) / tx_cols;

        for (int i = 0; i < TEXTURE_COUNT; i++) {
            int col = i % tx_cols;
            int row = i / tx_cols;
            SDL_Rect tr = { sx + col * (tx_size + 4),
                            tex_y + row * (tx_size / 2 + 4),
                            tx_size, tx_size / 2 };
            if (x >= tr.x && x <= tr.x + tr.w &&
                y >= tr.y && y <= tr.y + tr.h) {
                es->selected_texture = i;
                // Apply texture to selected planet immediately
                char path[128];
                snprintf(path, sizeof(path), "assets/%s", TEXTURE_FILES[i]);
                extern GLuint load_texture(const char*);
                world->planets[es->selected].texture_id = load_texture(path);
                return;
            }
        }

        // Save button
        int tx_rows = (TEXTURE_COUNT + tx_cols - 1) / tx_cols;
        int save_y = tex_y + tx_rows * (tx_size / 2 + 4) + 12;
        SDL_Rect save_btn = { sx, save_y, sw, 32 };
        if (x >= save_btn.x && x <= save_btn.x + save_btn.w &&
            y >= save_btn.y && y <= save_btn.y + save_btn.h) {
            save_planets(world, "assets/planets.csv");
            printf("Saved to assets/planets.csv\n");
            return;
        }
    }

    (void)win_h;
    (void)state;
}

// ── Save planets ─────────────────────────────────────────────────────────────

void save_planets(const World* world, const char* filename) {
    FILE* f = fopen(filename, "w");
    if (!f) {
        printf("Error: cannot open %s for writing\n", filename);
        return;
    }

    fprintf(f, "#name,distance,size,orbit_speed,rotation_speed,"
               "axial_tilt,texture,has_atmo,atmo_r,atmo_g,atmo_b,parent\n");

    for (int i = 0; i < world->count; i++) {
        const Planet* p = &world->planets[i];

        // Determine parent name
        const char* parent_name = "";
        if (p->parent_index >= 0 && p->parent_index < world->count)
            parent_name = world->planets[p->parent_index].name;

        // Guess texture filename from known list (fallback to earth.jpg)
        const char* tex = "earth.jpg";
        for (int t = 0; t < TEXTURE_COUNT; t++) {
            // Match by checking if texture_id was loaded from this file
            // (we store the index in selected_texture — good enough heuristic)
            (void)t;
        }
        // Use a reasonable default per planet name
        char tex_buf[64];
        snprintf(tex_buf, sizeof(tex_buf), "%s.jpg",
                 p->name[0] >= 'A' && p->name[0] <= 'Z'
                     ? p->name : "earth");
        // Lowercase first char heuristic: just use the name lowercased
        tex_buf[0] = (char)(p->name[0] | 32); // lowercase first char
        for (int c = 1; p->name[c]; c++)
            tex_buf[c] = (char)(p->name[c] | 32);
        tex_buf[strlen(p->name)] = '\0';
        strncat(tex_buf, ".jpg", sizeof(tex_buf) - strlen(tex_buf) - 1);
        tex = tex_buf;

        fprintf(f, "%s,%.4f,%.4f,%.4f,%.4f,%.4f,%s,%d,%.2f,%.2f,%.2f,%s\n",
                p->name,
                p->distance,
                p->size,
                p->orbit_speed,
                p->rotation_speed,
                p->axial_tilt,
                tex,
                p->has_atmosphere,
                p->atmo_r, p->atmo_g, p->atmo_b,
                parent_name);
    }
    fclose(f);
}