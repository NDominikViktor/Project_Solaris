//
// Created by novakdominikviktor on 2026. 02. 28..
//
#include "app.h"
#include "scene.h"
#include "hud.h"
#include "comet.h"
#include "ui.h"
#include "camera.h"
#include <GL/gl.h>
#include <GL/glu.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

bool app_init(App* app) {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("SDL error: %s\n", SDL_GetError());
        return false;
    }

    app->win_w = 1280;
    app->win_h = 720;

    app->window = SDL_CreateWindow("Solaris",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        app->win_w, app->win_h,
        SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
    if (!app->window) {
        printf("Window error: %s\n", SDL_GetError());
        return false;
    }

    SDL_SetRelativeMouseMode(SDL_FALSE);
    SDL_ShowCursor(SDL_ENABLE);
    app->gl_context = SDL_GL_CreateContext(app->window);
    glEnable(GL_DEPTH_TEST);
    glClearColor(0.0f, 0.0f, 0.05f, 1.0f);

    // Timing
    app->last_time  = SDL_GetTicks();
    app->delta_time = 0;
    app->running    = true;

    // Render flags
    app->fog_enabled          = true;
    app->show_orbits          = true;
    app->show_help            = false;
    app->sun_intensity        = 1.0f;
    app->time_scale           = 1.0f;
    app->cam_preset           = 0;
    app->target_planet_index  = -1;

    // Lighting
    glEnable(GL_LIGHTING); glEnable(GL_LIGHT0); glEnable(GL_COLOR_MATERIAL);
    glEnable(GL_FOG);
    float fogColor[] = {0.05f, 0.02f, 0.15f, 1.0f};
    glFogfv(GL_FOG_COLOR, fogColor);
    glFogf(GL_FOG_DENSITY, 0.06f);
    glHint(GL_FOG_HINT, GL_NICEST);
    glFogi(GL_FOG_MODE, GL_EXP2);
    float ambient[]  = {0.05f, 0.05f, 0.05f, 1.0f};
    float specular[] = {0.50f, 0.50f, 0.50f, 1.0f};
    glLightfv(GL_LIGHT0, GL_AMBIENT,  ambient);
    glLightfv(GL_LIGHT0, GL_SPECULAR, specular);

    // UI
    app->font = ui_init("assets/font.ttf", 15);
    app->app_state = STATE_MENU;
    memset(app->menu_btns, 0, sizeof(app->menu_btns));
    app->editor = (EditorState){-1, "NewPlanet", 3, false, false, "",
                                EDITOR_TAB_BASIC, 0.0f};

    // Scene
    init_camera(&app->camera);
    setup_projection(app->win_w, app->win_h);

    FILE* ftest = fopen("assets/custom_planets.csv", "r");
    if (ftest) { fclose(ftest); load_planets(&app->world, "assets/custom_planets.csv"); }
    else         load_planets(&app->world, "assets/planets.csv");

    init_asteroid_belt(app->asteroid_belt);
    app->skybox_texture_id = load_texture("assets/stars.jpg");

    app->comet_model = (OBJModel){NULL, NULL, 0, 0, 0};
    app->halley      = (Comet){0, 0, 0, 0, 0.0009f};
    load_asteroid_obj("assets/asteroid.obj", &app->comet_model);

    return true;
}

void app_quit(App* app) {
    for (int i = 0; i < app->world.count; i++)
        free_ring_particles(&app->world.planets[i]);
    if (app->comet_model.vertices) free(app->comet_model.vertices);
    if (app->comet_model.faces)    free(app->comet_model.faces);
    ui_quit(app->font);
    SDL_GL_DeleteContext(app->gl_context);
    SDL_DestroyWindow(app->window);
    SDL_Quit();
}

void app_handle_events(App* app) {
    SDL_Event event;
    World*       world        = &app->world;
    Camera*      camera_ptr   = &app->camera;
    AppState*    app_state    = &app->app_state;
    Button*      menu_btns    = app->menu_btns;
    int*         win_w        = &app->win_w;
    int*         win_h        = &app->win_h;
    float*       sun_intensity      = &app->sun_intensity;
    bool*        show_help          = &app->show_help;
    bool*        fog_enabled        = &app->fog_enabled;
    bool*        show_orbits        = &app->show_orbits;
    bool*        running            = &app->running;
    float*       time_scale         = &app->time_scale;
    int*         target_planet_index = &app->target_planet_index;
    int*         cam_preset          = &app->cam_preset;

    while (SDL_PollEvent(&event)) {
        // ── Quit ──────────────────────────────────────────────────────────────
        if (event.type == SDL_QUIT) { *running = false; break; }

        // ── Window resize ─────────────────────────────────────────────────────
        if (event.type == SDL_WINDOWEVENT &&
            event.window.event == SDL_WINDOWEVENT_RESIZED) {
            *win_w = event.window.data1;
            *win_h = event.window.data2;
            setup_projection(*win_w, *win_h);
        }

        // ── Menu events ───────────────────────────────────────────────────────
        if (app->app_state == STATE_MENU) {
            if (event.type == SDL_MOUSEMOTION)
                ui_menu_hover(event.motion.x, event.motion.y, menu_btns);
            if (event.type == SDL_MOUSEBUTTONDOWN &&
                event.button.button == SDL_BUTTON_LEFT)
                ui_menu_click(event.button.x, event.button.y,
                              menu_btns, app_state, running);
            continue;
        }

        // ── Editor events ─────────────────────────────────────────────────────
        if (app->app_state == STATE_EDITOR) {
            // Scroll
            if (event.type == SDL_MOUSEWHEEL) {
                app->editor.scroll_y -= event.wheel.y * 20.0f;
                if (app->editor.scroll_y < 0) app->editor.scroll_y = 0;
            }
            // Panel click or planet pick
            if (event.type == SDL_MOUSEBUTTONDOWN &&
                event.button.button == SDL_BUTTON_LEFT) {
                int cx = event.button.x, cy = event.button.y;
                if (cx <= PANEL_W) {
                    ui_editor_click(cx, cy, world, &app->editor,
                                    *win_w, *win_h, app_state);
                } else {
                    int hit = pick_planet(cx - PANEL_W, cy, camera_ptr, world);
                    if (hit != -1) app->editor.selected = hit;
                }
            }
            // Keyboard for text fields
            if (event.type == SDL_KEYDOWN) {
                SDL_Keycode k = event.key.keysym.sym;
                if (app->editor.editing_name && app->editor.selected >= 0) {
                    if (k == SDLK_RETURN || k == SDLK_KP_ENTER)
                        { app->editor.editing_name = false; SDL_StopTextInput(); }
                    else if (k == SDLK_BACKSPACE) {
                        char* nm = world->planets[app->editor.selected].name;
                        int len = (int)strlen(nm); if (len > 0) nm[len-1] = '\0';
                    } else if (k == SDLK_ESCAPE) {
                        app->editor.editing_name = false; SDL_StopTextInput();
                        app->app_state = STATE_MENU;
                    }
                } else if (app->editor.editing_custom_tex) {
                    if (k == SDLK_RETURN || k == SDLK_KP_ENTER)
                        { app->editor.editing_custom_tex = false; SDL_StopTextInput(); }
                    else if (k == SDLK_BACKSPACE) {
                        int len = (int)strlen(app->editor.custom_tex_buf);
                        if (len > 0) app->editor.custom_tex_buf[len-1] = '\0';
                    } else if (k == SDLK_ESCAPE) {
                        app->editor.editing_custom_tex = false; SDL_StopTextInput();
                    }
                } else {
                    if (k == SDLK_ESCAPE) app->app_state = STATE_MENU;
                }
            }
            if (event.type == SDL_TEXTINPUT) {
                if (app->editor.editing_name && app->editor.selected >= 0) {
                    strncat(world->planets[app->editor.selected].name,
                            event.text.text,
                            sizeof(world->planets[app->editor.selected].name)
                            - strlen(world->planets[app->editor.selected].name) - 1);
                } else if (app->editor.editing_custom_tex) {
                    strncat(app->editor.custom_tex_buf, event.text.text,
                            sizeof(app->editor.custom_tex_buf)
                            - strlen(app->editor.custom_tex_buf) - 1);
                }
            }
            continue;
        }

        // ── Simulation events ─────────────────────────────────────────────────

        // Left click
        if (event.type == SDL_MOUSEBUTTONDOWN &&
            event.button.button == SDL_BUTTON_LEFT) {
            int mx = event.button.x, my = event.button.y;

            // Back to menu button (top-left, matches hud.c drawing)
            if (mx >= 12 && mx <= 92 && my >= 12 && my <= 38) {
                app->app_state = STATE_MENU;
                app->show_help = false;
                continue;
            }

            // Camera preset buttons (bottom-left HUD)
            float ts_y = (float)(*win_h) - 85.0f;
            float cb_y = ts_y - 32.0f;
            if (my >= (int)cb_y && my <= (int)(cb_y + 24)) {
                if      (mx >= 20 && mx <= 66)  { *cam_preset=0; init_camera(camera_ptr); }
                else if (mx >= 90 && mx <= 136) { *cam_preset=1; camera_ptr->x=0; camera_ptr->y=80.0f; camera_ptr->z=0;    camera_ptr->pitch=-89.0f; camera_ptr->yaw=0; }
                else if (mx >= 160 && mx <= 206){ *cam_preset=2; camera_ptr->x=0; camera_ptr->y=0;     camera_ptr->z=80.0f; camera_ptr->pitch=0;     camera_ptr->yaw=0; }
                else if (mx >= 230 && mx <= 276){ *time_scale = (*time_scale > 0.0f) ? 0.0f : 1.0f; }
            }

            // Time scale bar click
            if (mx >= 20 && mx <= 220 && my >= (int)ts_y && my <= (int)(ts_y + 16)) {
                *time_scale = ((float)(mx - 20) / 200.0f) * 10.0f;
                if (*time_scale < 0)  *time_scale = 0;
                if (*time_scale > 10) *time_scale = 10;
            }

            // Planet picking
            int hit = pick_planet(mx, my, camera_ptr, world);
            if (hit != -1) *target_planet_index = hit;
        }

        // Right mouse drag — look around
        if (event.type == SDL_MOUSEMOTION &&
            (SDL_GetMouseState(NULL, NULL) & SDL_BUTTON(SDL_BUTTON_RIGHT))) {
            camera_ptr->yaw   -= event.motion.xrel * 0.2f;
            camera_ptr->pitch -= event.motion.yrel * 0.2f;
            if (camera_ptr->pitch >  89.0f) camera_ptr->pitch =  89.0f;
            if (camera_ptr->pitch < -89.0f) camera_ptr->pitch = -89.0f;
        }

        // Keyboard shortcuts
        if (event.type == SDL_KEYDOWN) {
            SDL_Keycode k = event.key.keysym.sym;

            if (k == SDLK_F1 || k == SDLK_h) *show_help = !(*show_help);
            if (k == SDLK_ESCAPE) { app->app_state = STATE_MENU; app->show_help = false; }

            if (k == SDLK_f) {
                *fog_enabled = !(*fog_enabled);
                if (*fog_enabled) glEnable(GL_FOG); else glDisable(GL_FOG);
            }
            if (k == SDLK_o) *show_orbits = !(*show_orbits);

            // Planet jump
            if (k >= SDLK_1 && k <= SDLK_9) {
                int idx = k - SDLK_1;
                *target_planet_index = (idx < world->count) ? idx : -1;
            }
            if (k == SDLK_0) *target_planet_index = -1;

            // Time scale
            if (k == SDLK_p) *time_scale = (*time_scale > 0.0f) ? 0.0f : 1.0f;
            if (k == SDLK_LEFTBRACKET)  { *time_scale -= 0.25f; if (*time_scale < 0)   *time_scale = 0; }
            if (k == SDLK_RIGHTBRACKET) { *time_scale += 0.25f; if (*time_scale > 10)  *time_scale = 10; }

            // Camera presets
            if (k == SDLK_t) { *cam_preset=1; camera_ptr->x=0; camera_ptr->y=80.0f; camera_ptr->z=0;    camera_ptr->pitch=-89.0f; camera_ptr->yaw=0; }
            if (k == SDLK_y) { *cam_preset=2; camera_ptr->x=0; camera_ptr->y=0;     camera_ptr->z=80.0f; camera_ptr->pitch=0;     camera_ptr->yaw=0; }
            if (k == SDLK_u) { *cam_preset=0; init_camera(camera_ptr); }
        }
    }
}

void app_render(App* app) {
    World*  world = &app->world;
    Camera* cam   = &app->camera;

    // Update timing
    Uint32 current_time = SDL_GetTicks();
    app->delta_time = (current_time - app->last_time) / 1000.0f;
    app->last_time  = current_time;
    float delta_time = app->delta_time;

    // ── Menu state ────────────────────────────────────────────────────────────
    if (app->app_state == STATE_MENU) {
        glViewport(0, 0, app->win_w, app->win_h);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        ui_draw_menu(app->font, app->win_w, app->win_h, app->menu_btns);
        SDL_GL_SwapWindow(app->window);
        return;
    }

    // ── Continuous keyboard (sun intensity, movement) ─────────────────────────
    const Uint8* keys = SDL_GetKeyboardState(NULL);

    if (keys[SDL_SCANCODE_KP_PLUS]  || keys[SDL_SCANCODE_EQUALS]) { app->sun_intensity += 0.01f; if (app->sun_intensity > 2.0f) app->sun_intensity = 2.0f; }
    if (keys[SDL_SCANCODE_KP_MINUS] || keys[SDL_SCANCODE_MINUS])  { app->sun_intensity -= 0.01f; if (app->sun_intensity < 0.1f) app->sun_intensity = 0.1f; }

    float speed = 0.2f;
    if (keys[SDL_SCANCODE_LSHIFT]) speed *= 3.0f;

    float rad_yaw   = cam->yaw   * (float)M_PI / 180.0f;
    float rad_pitch = cam->pitch * (float)M_PI / 180.0f;

    if (keys[SDL_SCANCODE_W] || keys[SDL_SCANCODE_S] ||
        keys[SDL_SCANCODE_A] || keys[SDL_SCANCODE_D] ||
        keys[SDL_SCANCODE_E] || keys[SDL_SCANCODE_Q])
        app->target_planet_index = -1;

    if (keys[SDL_SCANCODE_W])
        update_camera_position(cam, -sinf(rad_yaw)*cosf(rad_pitch)*speed,  sinf(rad_pitch)*speed, -cosf(rad_yaw)*cosf(rad_pitch)*speed, world);
    if (keys[SDL_SCANCODE_S])
        update_camera_position(cam,  sinf(rad_yaw)*cosf(rad_pitch)*speed, -sinf(rad_pitch)*speed,  cosf(rad_yaw)*cosf(rad_pitch)*speed, world);
    if (keys[SDL_SCANCODE_A])
        update_camera_position(cam, -cosf(rad_yaw)*speed, 0,  sinf(rad_yaw)*speed, world);
    if (keys[SDL_SCANCODE_D])
        update_camera_position(cam,  cosf(rad_yaw)*speed, 0, -sinf(rad_yaw)*speed, world);
    if (keys[SDL_SCANCODE_E]) cam->y += speed;
    if (keys[SDL_SCANCODE_Q]) cam->y -= speed;
    if (keys[SDL_SCANCODE_UP])    cam->pitch += 1.0f;
    if (keys[SDL_SCANCODE_DOWN])  cam->pitch -= 1.0f;
    if (keys[SDL_SCANCODE_LEFT])  cam->yaw   += 1.0f;
    if (keys[SDL_SCANCODE_RIGHT]) cam->yaw   -= 1.0f;

    // ── Update orbital positions ──────────────────────────────────────────────
    for (int i = 0; i < world->count; i++) {
        world->planets[i].current_angle  += world->planets[i].orbit_speed    * delta_time * 10.0f * app->time_scale;
        world->planets[i].rotation_angle += world->planets[i].rotation_speed * delta_time * 50.0f * app->time_scale;
        Planet* p = &world->planets[i];
        float lx = cosf(p->current_angle) * p->distance;
        float lz = sinf(p->current_angle) * p->distance;
        if (p->parent_index != -1) {
            Planet* parent = &world->planets[p->parent_index];
            p->world_x = parent->world_x + lx;
            p->world_y = parent->world_y;
            p->world_z = parent->world_z + lz;
        } else {
            p->world_x = lx;
            p->world_y = 0.0f;
            p->world_z = lz;
        }
    }

    // ── Camera follow ─────────────────────────────────────────────────────────
    if (app->target_planet_index >= 0 && app->target_planet_index < world->count) {
        Planet* tp = &world->planets[app->target_planet_index];
        cam->x = tp->world_x + tp->size * 3.0f;
        cam->y = tp->world_y + tp->size * 2.0f;
        cam->z = tp->world_z + tp->size * 3.0f;
    }

    // ── Projection & clear ────────────────────────────────────────────────────
    if (app->app_state == STATE_EDITOR)
        setup_projection_editor(app->win_w, app->win_h);
    else
        setup_projection(app->win_w, app->win_h);

    // Sun color derived from intensity (matches old_main.c)
    float r = 1.0f;
    float g = app->sun_intensity > 1.0f ? 1.0f : app->sun_intensity;
    float b = app->sun_intensity > 1.5f ? 1.0f
            : (app->sun_intensity < 1.0f ? app->sun_intensity * 0.5f
                                         : app->sun_intensity - 0.5f);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    set_view(cam);

    // ── Skybox (translated to camera position) ────────────────────────────────
    glPushMatrix();
        glTranslatef(cam->x, cam->y, cam->z);
        draw_skybox(app->skybox_texture_id);
    glPopMatrix();

    // ── Orbits & asteroid belt (no lighting) ─────────────────────────────────
    glDisable(GL_LIGHTING);
    if (app->show_orbits) draw_orbit_paths(world);
    draw_asteroid_belt(app->asteroid_belt);
    glEnable(GL_LIGHTING);

    // ── Sun light ─────────────────────────────────────────────────────────────
    float diffuse[]   = {r * app->sun_intensity, g * app->sun_intensity,
                         b * app->sun_intensity, 1.0f};
    float light_pos[] = {0.0f, 0.0f, 0.0f, 1.0f};
    glLightfv(GL_LIGHT0, GL_DIFFUSE,  diffuse);
    glLightfv(GL_LIGHT0, GL_POSITION, light_pos);

    // ── Planets ───────────────────────────────────────────────────────────────
    for (int i = 0; i < world->count; i++) {
        Planet* p = &world->planets[i];
        glPushMatrix();
        glTranslatef(p->world_x, p->world_y, p->world_z);
        GLUquadric* quad = gluNewQuadric();
        gluQuadricTexture(quad, GL_TRUE);

        if (p->obj_type == OBJ_STAR) {
            glDisable(GL_LIGHTING);
            glEnable(GL_TEXTURE_2D);
            glBindTexture(GL_TEXTURE_2D, p->texture_id);
            float ss = p->size;
            if (app->sun_intensity < 0.8f)       ss *= 1.0f + (0.8f - app->sun_intensity) * 2.0f;
            else if (app->sun_intensity > 1.5f) { ss *= 1.0f - (app->sun_intensity - 1.5f) * 1.5f; if (ss < 0.2f) ss = 0.2f; }
            glColor3f(r, g, b);
            GLUquadric* sq = gluNewQuadric(); gluQuadricTexture(sq, GL_TRUE);
            gluSphere(sq, ss, 32, 32); gluDeleteQuadric(sq);
            draw_sun_glow(ss * 1.5f, r, g, b);
            glEnable(GL_BLEND); glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            glColor4f(1.0f, 0.9f, 0.3f, 0.08f);
            GLUquadric* gq = gluNewQuadric();
            gluSphere(gq, ss * 1.6f, 32, 32); gluDeleteQuadric(gq);
            glDisable(GL_BLEND);
            glEnable(GL_LIGHTING); glColor3f(1,1,1); glDisable(GL_TEXTURE_2D);
        } else {
            glEnable(GL_LIGHTING);
            glPushMatrix();
            glRotatef(-90.0f, 1, 0, 0);
            glRotatef(p->axial_tilt,     1, 0, 0);
            glRotatef(p->rotation_angle, 0, 0, 1);
            glEnable(GL_TEXTURE_2D);
            glBindTexture(GL_TEXTURE_2D, p->texture_id);
            gluSphere(quad, p->size, 32, 32);
            if (p->has_atmosphere)
                draw_atmosphere(p->size, p->atmo_r, p->atmo_g, p->atmo_b, 0.25f);
            glDisable(GL_TEXTURE_2D);
            glPopMatrix();

            if (p->has_rings && p->ring_particles) {
                for (int j = 0; j < p->particle_count; j++) {
                    p->ring_particles[j].angle += p->ring_particles[j].speed;
                    if (p->ring_particles[j].angle >= 360.0f)
                        p->ring_particles[j].angle -= 360.0f;
                }
                glPushMatrix();
                glRotatef(p->ring_tilt, 1, 0, 0);
                draw_ring_particles(p);
                glPopMatrix();
            }
        }
        gluDeleteQuadric(quad);
        glPopMatrix();
    }

    draw_moon_shadows(world);

    // ── Comet ─────────────────────────────────────────────────────────────────
    app->halley.angle += 0.002f * app->time_scale;
    if (app->halley.angle > 6.28f) app->halley.angle = 0;
    draw_comet(&app->halley, delta_time, &app->comet_model);

    // ── HUD ───────────────────────────────────────────────────────────────────
    draw_hud(app->target_planet_index, app->sun_intensity, world,
             app->win_w, app->win_h, app->show_help,
             app->time_scale, app->cam_preset, app->font);

    // ── Help overlay ──────────────────────────────────────────────────────────
    if (app->show_help && app->font) {
        glMatrixMode(GL_PROJECTION); glPushMatrix(); glLoadIdentity();
        glOrtho(0, app->win_w, app->win_h, 0, -1, 1);
        glMatrixMode(GL_MODELVIEW); glPushMatrix(); glLoadIdentity();
        glDisable(GL_LIGHTING); glDisable(GL_DEPTH_TEST); glDisable(GL_CULL_FACE);
        glEnable(GL_BLEND); glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        float hx = 100, hy = 80;
        float hw = (float)app->win_w - 200;
        float hh = (float)app->win_h - 160;

        // Background
        glColor4f(0.02f, 0.05f, 0.15f, 0.94f);
        glBegin(GL_QUADS);
            glVertex2f(hx,    hy);    glVertex2f(hx+hw, hy);
            glVertex2f(hx+hw, hy+hh); glVertex2f(hx,    hy+hh);
        glEnd();
        // Border
        glColor4f(0.20f, 0.55f, 1.0f, 1.0f);
        glBegin(GL_LINE_LOOP);
            glVertex2f(hx,    hy);    glVertex2f(hx+hw, hy);
            glVertex2f(hx+hw, hy+hh); glVertex2f(hx,    hy+hh);
        glEnd();
        // Title underline
        glColor4f(0.20f, 0.55f, 1.0f, 0.35f);
        glBegin(GL_LINES);
            glVertex2f(hx+12, hy+40); glVertex2f(hx+hw-12, hy+40);
        glEnd();

        float tx   = hx + 20;
        float ty   = hy + 12;
        float lh   = 26.0f;
        float col2 = hx + hw / 2.0f;

        ui_draw_text(app->font, "Controls",      tx,  ty,  0.40f,0.82f,1.0f,1.0f, app->win_w, app->win_h); ty += 44;

        ui_draw_text(app->font, "MOVEMENT",      tx,  ty,  0.40f,0.60f,0.90f,1.0f, app->win_w, app->win_h); ty += lh;
        ui_draw_text(app->font, "W/A/S/D      Move forward/back/left/right", tx, ty, 0.80f,0.85f,0.90f,1.0f, app->win_w, app->win_h); ty += lh;
        ui_draw_text(app->font, "Q/E          Move down/up",                 tx, ty, 0.80f,0.85f,0.90f,1.0f, app->win_w, app->win_h); ty += lh;
        ui_draw_text(app->font, "Arrow keys   Rotate camera",                tx, ty, 0.80f,0.85f,0.90f,1.0f, app->win_w, app->win_h); ty += lh;
        ui_draw_text(app->font, "Right mouse  Look around",                  tx, ty, 0.80f,0.85f,0.90f,1.0f, app->win_w, app->win_h); ty += lh;
        ui_draw_text(app->font, "Shift        Sprint (3x speed)",            tx, ty, 0.80f,0.85f,0.90f,1.0f, app->win_w, app->win_h); ty += lh + 6;

        ui_draw_text(app->font, "CAMERA PRESETS", tx, ty, 0.40f,0.60f,0.90f,1.0f, app->win_w, app->win_h); ty += lh;
        ui_draw_text(app->font, "T            Top-down view",                tx, ty, 0.80f,0.85f,0.90f,1.0f, app->win_w, app->win_h); ty += lh;
        ui_draw_text(app->font, "Y            Side view",                    tx, ty, 0.80f,0.85f,0.90f,1.0f, app->win_w, app->win_h); ty += lh;
        ui_draw_text(app->font, "U            Free camera (reset)",          tx, ty, 0.80f,0.85f,0.90f,1.0f, app->win_w, app->win_h); ty += lh + 6;

        ui_draw_text(app->font, "SELECTION",     tx, ty, 0.40f,0.60f,0.90f,1.0f, app->win_w, app->win_h); ty += lh;
        ui_draw_text(app->font, "Left click   Select planet",                tx, ty, 0.80f,0.85f,0.90f,1.0f, app->win_w, app->win_h); ty += lh;
        ui_draw_text(app->font, "1-9          Jump to planet",               tx, ty, 0.80f,0.85f,0.90f,1.0f, app->win_w, app->win_h); ty += lh;
        ui_draw_text(app->font, "0            Release follow",               tx, ty, 0.80f,0.85f,0.90f,1.0f, app->win_w, app->win_h);

        // Column 2
        ty = hy + 56;
        ui_draw_text(app->font, "TIME CONTROL",  col2, ty, 0.40f,0.60f,0.90f,1.0f, app->win_w, app->win_h); ty += lh;
        ui_draw_text(app->font, "P            Pause / resume",               col2, ty, 0.80f,0.85f,0.90f,1.0f, app->win_w, app->win_h); ty += lh;
        ui_draw_text(app->font, "[            Slow down (0.25x)",            col2, ty, 0.80f,0.85f,0.90f,1.0f, app->win_w, app->win_h); ty += lh;
        ui_draw_text(app->font, "]            Speed up (0.25x, max 10x)",    col2, ty, 0.80f,0.85f,0.90f,1.0f, app->win_w, app->win_h); ty += lh;
        ui_draw_text(app->font, "Click time bar  Set speed directly",        col2, ty, 0.80f,0.85f,0.90f,1.0f, app->win_w, app->win_h); ty += lh + 6;

        ui_draw_text(app->font, "DISPLAY",       col2, ty, 0.40f,0.60f,0.90f,1.0f, app->win_w, app->win_h); ty += lh;
        ui_draw_text(app->font, "+/-          Sun intensity",                col2, ty, 0.80f,0.85f,0.90f,1.0f, app->win_w, app->win_h); ty += lh;
        ui_draw_text(app->font, "F            Toggle fog",                   col2, ty, 0.80f,0.85f,0.90f,1.0f, app->win_w, app->win_h); ty += lh;
        ui_draw_text(app->font, "O            Toggle orbit paths",           col2, ty, 0.80f,0.85f,0.90f,1.0f, app->win_w, app->win_h); ty += lh + 6;

        ui_draw_text(app->font, "EDITOR",        col2, ty, 0.40f,0.60f,0.90f,1.0f, app->win_w, app->win_h); ty += lh;
        ui_draw_text(app->font, "Main menu -> Planet Editor",                col2, ty, 0.80f,0.85f,0.90f,1.0f, app->win_w, app->win_h); ty += lh;
        ui_draw_text(app->font, "Add/Delete planets, set type, parent",      col2, ty, 0.80f,0.85f,0.90f,1.0f, app->win_w, app->win_h); ty += lh;
        ui_draw_text(app->font, "Set textures, rings, atmosphere",           col2, ty, 0.80f,0.85f,0.90f,1.0f, app->win_w, app->win_h); ty += lh;
        ui_draw_text(app->font, "Save custom solar system to CSV",           col2, ty, 0.80f,0.85f,0.90f,1.0f, app->win_w, app->win_h); ty += lh + 6;

        ui_draw_text(app->font, "OTHER",         col2, ty, 0.40f,0.60f,0.90f,1.0f, app->win_w, app->win_h); ty += lh;
        ui_draw_text(app->font, "F1/H         Toggle this help",             col2, ty, 0.80f,0.85f,0.90f,1.0f, app->win_w, app->win_h); ty += lh;
        ui_draw_text(app->font, "ESC          Quit",                         col2, ty, 0.80f,0.85f,0.90f,1.0f, app->win_w, app->win_h);

        glDisable(GL_BLEND);
        glEnable(GL_DEPTH_TEST); glEnable(GL_CULL_FACE);
        glPopMatrix(); glMatrixMode(GL_PROJECTION); glPopMatrix();
        glMatrixMode(GL_MODELVIEW);
    }

    // ── Editor panel ──────────────────────────────────────────────────────────
    if (app->app_state == STATE_EDITOR) {
        glViewport(0, 0, app->win_w, app->win_h);
        ui_draw_editor(app->font, world, &app->editor, app->win_w, app->win_h);
    }

    SDL_GL_SwapWindow(app->window);
}