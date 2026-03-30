//
// Created by novakdominikviktor on 2026. 03. 30..
//

#ifndef SOLARIS_UI_H
#define SOLARIS_UI_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <stdbool.h>
#include "scene.h"

/** @brief Top-level application states. */
typedef enum {
    STATE_MENU, /**< Main menu screen. */
    STATE_SIMULATION, /**< 3D solar system simulation. */
    STATE_EDITOR /**< Planet editor panel + live 3D preview. */
} AppState;

/** @brief A single clickable button rendered with SDL2_ttf. */
typedef struct {
    SDL_Rect rect; /**< Screen-space bounding box. */
    const char* label; /**< Text shown on the button. */
    bool hovered; /**< True when the mouse is over the button. */
} Button;

/** @brief All runtime state for the editor panel. */
typedef struct {
    int selected;  /**< Index of the currently selected planet, or -1. */
    char new_name[32]; /**< Name field buffer for a new planet. */
    int selected_texture; /**< Index into TEXTURE_FILES[] for the new planet. */
    bool editing_name; /**< True while the name text field has keyboard focus. */
} EditorState;

/* Names of texture files available for selection (must match assets/ folder). */
#define TEXTURE_COUNT 12
extern const char* TEXTURE_FILES[TEXTURE_COUNT];

/**
 * @brief Initialise SDL2_ttf and load the font.
 * @param font_path Path to a .ttf file (e.g. "assets/font.ttf").
 * @param size      Point size to load.
 * @return Loaded TTF_Font pointer, or NULL on failure.
 */
TTF_Font* ui_init(const char* font_path, int size);

/**
 * @brief Free the font and shut down SDL2_ttf.
 * @param font Font previously returned by ui_init().
 */
void ui_quit(TTF_Font* font);

/**
 * @brief Draw the main menu using SDL2 (rendered on top of an OpenGL frame).
 *
 * @param renderer  SDL_Renderer used for 2D drawing.
 * @param font      TTF font for labels.
 * @param win_w     Current window width.
 * @param win_h     Current window height.
 * @param btns      Array of 3 Button structs (Start, Editor, Quit).
 */
void ui_draw_menu(SDL_Renderer* renderer, TTF_Font* font, int win_w, int win_h, Button btns[3]);

/**
 * @brief Draw the editor side-panel (left third of the window).
 *
 * @param renderer  SDL_Renderer used for 2D drawing.
 * @param font      TTF font for labels.
 * @param world     Pointer to the World being edited (read + write).
 * @param es        Pointer to the editor UI state.
 * @param win_w     Current window width.
 * @param win_h     Current window height.
 */
void ui_draw_editor(SDL_Renderer* renderer, TTF_Font* font, World* world, EditorState* es, int win_w, int win_h);

/**
 * @brief Handle a mouse-button-down event in the editor panel.
 *
 * @param x         Mouse X coordinate.
 * @param y         Mouse Y coordinate.
 * @param world     World being edited.
 * @param es        Editor UI state.
 * @param win_w     Current window width.
 * @param win_h     Current window height.
 * @param state     Pointer to AppState (editor may switch to simulation).
 */
void ui_editor_click(int x, int y, World* world, EditorState* es, int win_w, int win_h, AppState* state);

/**
 * @brief Handle a mouse-button-down event on the main menu.
 *
 * @param x     Mouse X coordinate.
 * @param y     Mouse Y coordinate.
 * @param btns  Array of 3 Button structs.
 * @param state Pointer to AppState — updated on button hit.
 */
void ui_menu_click(int x, int y, Button btns[3], AppState* state);

/**
 * @brief Update button hover states from the current mouse position.
 *
 * @param x     Mouse X.
 * @param y     Mouse Y.
 * @param btns  Array of 3 Button structs.
 */
void ui_menu_hover(int x, int y, Button btns[3]);

/**
 * @brief Save the current World to a CSV file.
 *
 * Overwrites the file at path. Format matches the one read by load_planets().
 *
 * @param world    World to serialise.
 * @param filename Destination file path (e.g. "assets/planets.csv").
 */
void save_planets(World* world, const char* filename);


#endif //SOLARIS_UI_H