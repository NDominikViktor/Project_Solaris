//
// Created by novakdominikviktor on 2026. 04. 29..
//

#ifndef SOLARIS_APP_H
#define SOLARIS_APP_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <stdbool.h>
#include <scene.h>
#include <camera.h>
#include <ui.h>
#include <comet.h>

/**
 * @brief All runtime state for the application, passed between subsystems.
 *
 * Centralising state here keeps main() free of variable declarations
 * and makes every subsystem's dependencies explicit.
 */

typedef struct {
    // Window / GL
    SDL_Window* window; /**< Main SDL window. */
    SDL_GLContext gl_context; /**< OpenGL rendering context. */
    int win_w; /**< Current window width in pixels. */
    int win_h; /**< Current window width in pixels. */

    // Timing
    Uint32 last_time;  /**< Current window height in pixels. */
    float delta_time;   /**< Seconds elapsed since last frame. */

    // Simulation
    World world;  /**< All planets, moons and stars. */
    Camera camera; /**< First-person camera. */
    Asteroid asteroid_belt[MAX_ASTEROID];
    OBJModel comet_model; /**< OBJ mesh for Halley's comet core. */
    Comet halley;   /**< Halley's comet state. */
    GLuint skybox_texture_id; /**< OpenGL texture handle for the star-field skybox. */

    // Rendering flags
    float sun_intensity; /**< Sunlight intensity multiplier (0.1 – 2.0). */
    bool fog_enabled;   /**< True while GL_FOG is active. */
    bool show_orbits;   /**< True while orbit path circles are drawn. */
    bool show_help;    /**< True while the F1 help overlay is shown. */
    float time_scale;   /**< Simulation speed multiplier (0 = paused). */
    int cam_preset;    /**< Active camera preset: 0=free, 1=top, 2=side. */
    int target_planet_index;    /**< Selected planet index, or -1 */

    // UI
    TTF_Font* font; /**< TTF font used for all SDL2_ttf text. */
    AppState app_state; /**< Current top-level application state. */
    Button menu_btns[3]; /**< Main menu button layout. */
    EditorState editor; /**< Editor panel runtime state. */

    bool running; /**< Set to false to exit the main loop. */
} App;

/**
 * @brief Initialise SDL, OpenGL, load assets and populate the App struct.
 * @param app Pointer to an uninitialised App struct.
 * @return true on success, false on any fatal error.
 */
bool app_init(App* app);

/**
 * @brief Initialise SDL, OpenGL, load assets and populate the App struct.
 * @param app Pointer to an uninitialised App struct.
 * @return true on success, false on any fatal error.
 */
void app_handle_events(App* app);

/**
 * @brief Process all pending SDL events for one frame.
 * @param app Pointer to the active App struct.
 */
void app_render(App* app);
/**
 * @brief Free all resources and shut down SDL / TTF.
 * @param app Pointer to the App struct to clean up.
 */
void app_quit(App* app);

#endif //SOLARIS_APP_H