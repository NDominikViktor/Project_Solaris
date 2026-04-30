//
// Created by novakdominikviktor on 2026. 02. 28..
//

#ifndef SOLARIS_HUD_H
#define SOLARIS_HUD_H

#include <SDL2/SDL_ttf.h>
#include <stdbool.h>
#include "scene.h"

/**
 * @brief Set up the OpenGL projection matrix for the full window.
 * @param width  Current window width in pixels.
 * @param height Current window height in pixels.
 */
void setup_projection(int width, int height);

/**
 * @brief Set up the OpenGL projection matrix leaving room for the editor panel.
 * @param win_w Current window width in pixels.
 * @param win_h Current window height in pixels.
 */
void setup_projection_editor(int win_w, int win_h);

/**
 * @brief Draw a single line of text using GL_LINES stroked glyphs.
 *
 * No external font required. Supports A-Z, 0-9 and basic punctuation.
 * Coordinate system: screen pixels, top-left origin (2D ortho context assumed).
 *
 * @param x    Left edge of the first character.
 * @param y    Top edge of the first character.
 * @param text Null-terminated ASCII string to render.
 */
void draw_text_simple(float x, float y, const char* text);

/**
 * @brief Draw the orbit path circle for every non-star planet.
 * @param world Pointer to the active World.
 */
void draw_orbit_paths(World* world);

/**
 * @brief Draw the in-simulation HUD overlay.
 *
 * Renders the sun-intensity bar, time-scale bar, camera-preset buttons,
 * the planet info panel, and (optionally) the full help overlay.
 *
 * @param target_index       Index of the selected planet, or -1.
 * @param intensity          Current sun intensity (0.1 – 2.0).
 * @param w                  Pointer to the active World.
 * @param scr_w              Current window width in pixels.
 * @param scr_h              Current window height in pixels.
 * @param help_visible       True while the F1 help overlay is shown.
 * @param time_scale         Current simulation time multiplier.
 * @param cam_preset         Active camera preset index (0=free, 1=top, 2=side).
 * @param font               TTF font used for the planet info panel and help text.
 */
void draw_hud(int target_index, float intensity, World* w,
              int scr_w, int scr_h, bool help_visible,
              float time_scale, int cam_preset, TTF_Font* font);

/**
 * @brief Return appropriate gluSphere segment count based on camera distance.
 * @param cam_x  Camera X world position.
 * @param cam_y  Camera Y world position.
 * @param cam_z  Camera Z world position.
 * @param px     Planet centre X.
 * @param py     Planet centre Y.
 * @param pz     Planet centre Z.
 * @param size   Planet sphere radius.
 * @return Segment count: 64 / 32 / 16 / 8 based on distance.
 */
int lod_segments(float cam_x, float cam_y, float cam_z,
                 float px, float py, float pz, float size);

/**
 * @brief Draw a multi-layer additive glow disc around the sun.
 * @param size Radius of the innermost glow layer.
 * @param r    Red component of the glow colour (0-1).
 * @param g    Green component.
 * @param b    Blue component.
 */
void draw_sun_glow(float size, float r, float g, float b);

#endif /* SOLARIS_HUD_H */