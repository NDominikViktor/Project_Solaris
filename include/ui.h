//
// Created by novakdominikviktor on 2026. 02. 28..
//

#ifndef SOLARIS_UI_H
#define SOLARIS_UI_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <GL/gl.h>
#include <stdbool.h>
#include "scene.h"

/** @brief Top-level application states. */
typedef enum {
    STATE_MENU,
    STATE_SIMULATION,
    STATE_EDITOR
} AppState;

/** @brief A single clickable button. */
typedef struct {
    float x, y, w, h;   /**< Position and size in screen pixels (top-left origin). */
    const char* label;
    bool hovered;
} Button;

/** @brief Which section of the editor is currently expanded. */
typedef enum {
    EDITOR_TAB_BASIC = 0, /**< Name, type, parent, size, distance, speed. */
    EDITOR_TAB_VISUAL,    /**< Texture, atmosphere, rings. */
} EditorTab;

/** @brief Runtime state for the editor panel. */
typedef struct {
    int  selected;           /**< Index of the selected planet, or -1. */
    char new_name[32];
    int  selected_texture;
    bool editing_name;
    EditorTab active_tab;    /**< Current tab in the editor. */
} EditorState;

#define TEXTURE_COUNT 12
extern const char* TEXTURE_FILES[TEXTURE_COUNT];

/**
 * @brief Initialise SDL2_ttf and load the font.
 * @return Loaded TTF_Font*, or NULL on failure.
 */
TTF_Font* ui_init(const char* font_path, int size);

/** @brief Close the font and shut down SDL2_ttf. */
void ui_quit(TTF_Font* font);

/**
 * @brief Draw text at (x,y) using SDL2_ttf baked into an OpenGL texture.
 * The coordinate system is screen pixels, top-left origin.
 */
void ui_draw_text(TTF_Font* font, const char* text,
                  float x, float y,
                  float r, float g, float b, float a,
                  int win_w, int win_h);

/**
 * @brief Draw the main menu (pure OpenGL, no SDL_Renderer).
 */
void ui_draw_menu(TTF_Font* font, int win_w, int win_h, Button btns[3]);

/**
 * @brief Draw the editor side-panel (pure OpenGL).
 */
void ui_draw_editor(TTF_Font* font, World* world, EditorState* es,
                    int win_w, int win_h);

/** @brief Handle a click on the menu. */
void ui_menu_click(int x, int y, Button btns[3], AppState* state, bool* running);

/** @brief Update button hover state. */
void ui_menu_hover(int x, int y, Button btns[3]);

/** @brief Handle a click inside the editor panel. */
void ui_editor_click(int x, int y, World* world, EditorState* es,
                     int win_w, int win_h, AppState* state);

/** @brief Save the World to a CSV file. */
void save_planets(World* world, const char* filename);

#endif /* SOLARIS_UI_H */