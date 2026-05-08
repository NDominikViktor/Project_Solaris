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
    // Convenience aliases so the extracted code needs minimal edits
    SDL_Event event;
    World*       world      = &app->world;
    Camera*      camera_ptr = &app->camera;
    Camera       camera     = app->camera;  // local copy for read-only use
    AppState*    app_state  = &app->app_state;
    EditorState* editor_ptr = &app->editor;
    EditorState  editor     = app->editor;
    Button*      menu_btns  = app->menu_btns;
    int*         win_w      = &app->win_w;
    int*         win_h      = &app->win_h;
    float*       sun_intensity     = &app->sun_intensity;
    bool*        show_help         = &app->show_help;
    bool*        fog_enabled       = &app->fog_enabled;
    bool*        show_orbits       = &app->show_orbits;
    bool*        running           = &app->running;
    float*       time_scale        = &app->time_scale;
    int*         target_planet_index = &app->target_planet_index;
    int*         cam_preset          = &app->cam_preset;
    SDL_Window*  window            = app->window;

    (void)camera; (void)editor;

    while (SDL_PollEvent(&event)) {
        // ── Quit ──────────────────────────────────────────────────────────────
        if (event.type == SDL_QUIT) { *running = false; break; }

        // ── ESC → back to menu (not quit) ─────────────────────────────────────
        if (event.type == SDL_KEYDOWN &&
            event.key.keysym.sym == SDLK_ESCAPE &&
            app->app_state != STATE_MENU) {
            app->app_state = STATE_MENU;
            app->show_help = false;
            continue;
        }

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
                ui_menu_hover(event.motion.x, event.motion.y, app->menu_btns);
            if (event.type == SDL_MOUSEBUTTONDOWN &&
                event.button.button == SDL_BUTTON_LEFT)
                ui_menu_click(event.button.x, event.button.y,
                              app->menu_btns, &app->app_state, running);
            continue;
        }

        // ── Editor text input ─────────────────────────────────────────────────
        if (app->app_state == STATE_EDITOR) {
            if (event.type == SDL_TEXTINPUT) {
                if (app->editor.editing_name && app->editor.selected >= 0) {
                    char* nm = app->world.planets[app->editor.selected].name;
                    if (strlen(nm) < 30) strncat(nm, event.text.text, 30 - strlen(nm));
                } else if (app->editor.editing_custom_tex) {
                    strncat(app->editor.custom_tex_buf, event.text.text,
                            sizeof(app->editor.custom_tex_buf)
                            - strlen(app->editor.custom_tex_buf) - 1);
                }
            }
            if (event.type == SDL_KEYDOWN) {
                SDL_Keycode k = event.key.keysym.sym;
                if (app->editor.editing_name && app->editor.selected >= 0) {
                    if (k == SDLK_RETURN || k == SDLK_KP_ENTER)
                        { app->editor.editing_name = false; SDL_StopTextInput(); }
                    else if (k == SDLK_BACKSPACE) {
                        char* nm = app->world.planets[app->editor.selected].name;
                        int len = (int)strlen(nm); if (len > 0) nm[len-1] = '\0';
                    }
                } else if (app->editor.editing_custom_tex) {
                    if (k == SDLK_RETURN || k == SDLK_KP_ENTER)
                        { app->editor.editing_custom_tex = false; SDL_StopTextInput(); }
                    else if (k == SDLK_BACKSPACE) {
                        int len = (int)strlen(app->editor.custom_tex_buf);
                        if (len > 0) app->editor.custom_tex_buf[len-1] = '\0';
                    }
                }
            }
        }

        // ── Keyboard shortcuts (simulation + editor) ──────────────────────────
        if (event.type == SDL_KEYDOWN &&
            !app->editor.editing_name && !app->editor.editing_custom_tex) {
            SDL_Keycode k = event.key.keysym.sym;

            // Planet jump
            if (k >= SDLK_1 && k <= SDLK_9)
                { int idx = k - SDLK_1; *target_planet_index = (idx < world->count) ? idx : -1; }
            if (k == SDLK_0) *target_planet_index = -1;

            // Time scale
            if (k == SDLK_p) *time_scale = (*time_scale > 0.0f) ? 0.0f : 1.0f;
            if (k == SDLK_LEFTBRACKET)  { *time_scale -= 0.25f; if (*time_scale < 0) *time_scale = 0; }
            if (k == SDLK_RIGHTBRACKET) { *time_scale += 0.25f; if (*time_scale > 10) *time_scale = 10; }

            // Sun intensity
            if (k == SDLK_PLUS  || k == SDLK_KP_PLUS)  { *sun_intensity += 0.1f; if (*sun_intensity > 2.0f) *sun_intensity = 2.0f; }
            if (k == SDLK_MINUS || k == SDLK_KP_MINUS)  { *sun_intensity -= 0.1f; if (*sun_intensity < 0.1f) *sun_intensity = 0.1f; }

            // Fog
            if (k == SDLK_f) {
                *fog_enabled = !(*fog_enabled);
                if (*fog_enabled) glEnable(GL_FOG); else glDisable(GL_FOG);
            }

            // Orbits
            if (k == SDLK_o) *show_orbits = !(*show_orbits);

            // Help
            if (k == SDLK_F1 || k == SDLK_h) *show_help = !(*show_help);

            // Camera presets
            if (k == SDLK_t) { *cam_preset=1; camera_ptr->x=0; camera_ptr->y=80.0f; camera_ptr->z=0;    camera_ptr->pitch=-89.0f; camera_ptr->yaw=0; }
            if (k == SDLK_y) { *cam_preset=2; camera_ptr->x=0; camera_ptr->y=0;     camera_ptr->z=80.0f; camera_ptr->pitch=0;     camera_ptr->yaw=0; }
            if (k == SDLK_u) { *cam_preset=0; init_camera(camera_ptr); }
        }

        // ── Mouse button down ─────────────────────────────────────────────────
        if (event.type == SDL_MOUSEBUTTONDOWN &&
            event.button.button == SDL_BUTTON_LEFT) {
            int mx = event.button.x, my = event.button.y;

            // Back to menu button (top-left: 12..92 x 12..38)
            if (mx >= 12 && mx <= 92 && my >= 12 && my <= 38) {
                app->app_state = STATE_MENU;
                app->show_help = false;
                continue;
            }

            // Editor panel clicks
            if (app->app_state == STATE_EDITOR && mx <= PANEL_W) {
                ui_editor_click(mx, my, world, &app->editor,
                                *win_w, *win_h, &app->app_state);
                continue;
            }

            // Camera preset buttons (bottom-left HUD)
            {
                float ts_y = (float)(*win_h) - 85.0f;
                float cb_y = ts_y - 30.0f;
                if (my >= (int)cb_y && my <= (int)(cb_y + 24)) {
                    if      (mx>=20&&mx<=66 ) { *cam_preset=0; init_camera(camera_ptr); }
                    else if (mx>=70&&mx<=116 ) { *cam_preset=1; camera_ptr->x=0; camera_ptr->y=80.0f; camera_ptr->z=0;    camera_ptr->pitch=-89.0f; camera_ptr->yaw=0; }
                    else if (mx>=120&&mx<=166) { *cam_preset=2; camera_ptr->x=0; camera_ptr->y=0;     camera_ptr->z=80.0f; camera_ptr->pitch=0;     camera_ptr->yaw=0; }
                    else if (mx>=170&&mx<=216) { *time_scale = (*time_scale > 0.0f) ? 0.0f : 1.0f; }
                    continue;
                }
                // Time scale bar click
                if (mx>=20&&mx<=220&&my>=(int)ts_y&&my<=(int)(ts_y+16)) {
                    *time_scale = ((float)(mx-20)/200.0f)*10.0f;
                    if (*time_scale < 0) *time_scale = 0;
                    if (*time_scale > 10) *time_scale = 10;
                    continue;
                }
            }

            // Planet picking
            {
                int hit = pick_planet(mx, my, camera_ptr, world);
                if (hit != -1) *target_planet_index = hit;
            }
        }

        // ── Right mouse drag — look around ────────────────────────────────────
        if (event.type == SDL_MOUSEMOTION &&
            (SDL_GetMouseState(NULL, NULL) & SDL_BUTTON(SDL_BUTTON_RIGHT))) {
            float sensitivity = 0.1f;
            camera_ptr->yaw   += event.motion.xrel * sensitivity;
            camera_ptr->pitch -= event.motion.yrel * sensitivity;
            if (camera_ptr->pitch >  89.0f) camera_ptr->pitch =  89.0f;
            if (camera_ptr->pitch < -89.0f) camera_ptr->pitch = -89.0f;
        }
    }

    // Write back local copies that event handlers may have modified
    app->camera = *camera_ptr;
    app->editor = *editor_ptr;
}

void app_render(App* app) {
    // Convenience aliases
    World*  world      = &app->world;
    Camera* cam        = &app->camera;
    float   delta_time = app->delta_time;

    // Update timing
    Uint32 current_time = SDL_GetTicks();
    app->delta_time = (current_time - app->last_time) / 1000.0f;
    app->last_time  = current_time;

    // ── Menu state: just draw menu and return ─────────────────────────────────
    if (app->app_state == STATE_MENU) {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        ui_draw_menu(app->font, app->win_w, app->win_h, app->menu_btns);
        SDL_GL_SwapWindow(app->window);
        return;
    }

    // ── Update orbital positions ──────────────────────────────────────────────
    for (int i = 0; i < world->count; i++) {
        world->planets[i].current_angle  += world->planets[i].orbit_speed
                                             * delta_time * 10.0f * app->time_scale;
        world->planets[i].rotation_angle += world->planets[i].rotation_speed
                                             * delta_time * 50.0f * app->time_scale;

        Planet* p = &world->planets[i];
        if (p->parent_index >= 0) {
            Planet* par = &world->planets[p->parent_index];
            p->world_x = par->world_x + cosf(p->current_angle) * p->distance;
            p->world_y = par->world_y;
            p->world_z = par->world_z + sinf(p->current_angle) * p->distance;
        } else {
            p->world_x = cosf(p->current_angle) * p->distance;
            p->world_y = 0;
            p->world_z = sinf(p->current_angle) * p->distance;
        }
    }

    // ── Camera follow ─────────────────────────────────────────────────────────
    if (app->target_planet_index >= 0 && app->target_planet_index < world->count) {
        Planet* tp = &world->planets[app->target_planet_index];
        cam->x = tp->world_x;
        cam->y = tp->world_y + tp->size * 3.0f;
        cam->z = tp->world_z + tp->size * 5.0f;
    }

    // ── Keyboard movement (simulation + editor) ───────────────────────────────
    const Uint8* keys = SDL_GetKeyboardState(NULL);
    float speed = 0.2f;
    if (keys[SDL_SCANCODE_LSHIFT]) speed *= 3.0f;

    float yaw_r = cam->yaw * (float)M_PI / 180.0f;
    float fwd_x = -sinf(yaw_r), fwd_z = -cosf(yaw_r);

    if (keys[SDL_SCANCODE_W]) update_camera_position(cam,  fwd_x*speed,  0, fwd_z*speed, world);
    if (keys[SDL_SCANCODE_S]) update_camera_position(cam, -fwd_x*speed,  0,-fwd_z*speed, world);
    if (keys[SDL_SCANCODE_A]) update_camera_position(cam,  fwd_z*speed,  0,-fwd_x*speed, world);
    if (keys[SDL_SCANCODE_D]) update_camera_position(cam, -fwd_z*speed,  0, fwd_x*speed, world);
    if (keys[SDL_SCANCODE_Q]) update_camera_position(cam, 0, -speed, 0, world);
    if (keys[SDL_SCANCODE_E]) update_camera_position(cam, 0,  speed, 0, world);

    if (keys[SDL_SCANCODE_UP])    { cam->pitch += 1.0f; if(cam->pitch> 89)cam->pitch= 89; }
    if (keys[SDL_SCANCODE_DOWN])  { cam->pitch -= 1.0f; if(cam->pitch<-89)cam->pitch=-89; }
    if (keys[SDL_SCANCODE_LEFT])  cam->yaw -= 1.0f;
    if (keys[SDL_SCANCODE_RIGHT]) cam->yaw += 1.0f;

    // ── Render ────────────────────────────────────────────────────────────────
    if (app->app_state == STATE_EDITOR)
        setup_projection_editor(app->win_w, app->win_h);
    else
        setup_projection(app->win_w, app->win_h);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    set_view(cam);

    // Lighting
    float diffuse[] = {app->sun_intensity, app->sun_intensity * 0.95f,
                       app->sun_intensity * 0.85f, 1.0f};
    float light_pos[] = {0.0f, 0.0f, 0.0f, 1.0f};
    glLightfv(GL_LIGHT0, GL_DIFFUSE,  diffuse);
    glLightfv(GL_LIGHT0, GL_POSITION, light_pos);

    draw_skybox(app->skybox_texture_id);
    if (app->show_orbits) draw_orbit_paths(world);

    // Draw planets
    for (int i = 0; i < world->count; i++) {
        Planet* p = &world->planets[i];
        glPushMatrix();
        glTranslatef(p->world_x, p->world_y, p->world_z);
        glRotatef(p->axial_tilt, 0, 0, 1);
        glRotatef(p->rotation_angle, 0, 1, 0);

        if (p->obj_type == OBJ_STAR) {
            glDisable(GL_LIGHTING);
            float ss = p->size;
            float sr = (app->sun_intensity < 1.0f) ? 0.5f + app->sun_intensity * 0.5f : 1.0f;
            float sg = (app->sun_intensity < 1.0f) ? 0.3f + app->sun_intensity * 0.2f
                     : (app->sun_intensity > 1.5f) ? 0.6f : 0.9f;
            float sb = (app->sun_intensity > 1.5f) ? 0.1f : 0.3f;
            glColor3f(sr, sg, sb);
            GLUquadric* sq = gluNewQuadric(); gluQuadricTexture(sq, GL_TRUE);
            glEnable(GL_TEXTURE_2D); glBindTexture(GL_TEXTURE_2D, p->texture_id);
            int segs = lod_segments(cam->x, cam->y, cam->z,
                                    p->world_x, p->world_y, p->world_z, ss);
            gluSphere(sq, ss, segs, segs); gluDeleteQuadric(sq);
            glDisable(GL_TEXTURE_2D);
            draw_sun_glow(ss, sr, sg, sb);
            glEnable(GL_LIGHTING);
        } else {
            glEnable(GL_TEXTURE_2D); glBindTexture(GL_TEXTURE_2D, p->texture_id);
            GLUquadric* quad = gluNewQuadric(); gluQuadricTexture(quad, GL_TRUE);
            int segs = lod_segments(cam->x, cam->y, cam->z,
                                    p->world_x, p->world_y, p->world_z, p->size);
            gluSphere(quad, p->size, segs, segs);
            glDisable(GL_TEXTURE_2D);
            if (p->has_atmosphere)
                draw_atmosphere(p->size, p->atmo_r, p->atmo_g, p->atmo_b, 0.25f);
            if (p->has_rings && p->ring_particles) {
                for (int j = 0; j < p->particle_count; j++) {
                    p->ring_particles[j].angle += p->ring_particles[j].speed;
                    if (p->ring_particles[j].angle >= 360.0f)
                        p->ring_particles[j].angle -= 360.0f;
                }
                glPushMatrix();
                if (p->axial_tilt > 45.0f) glRotatef(p->axial_tilt, 1, 0, 0);
                draw_ring_particles(p);
                glPopMatrix();
            }
            gluDeleteQuadric(quad);
        }
        glPopMatrix();
    }

    draw_moon_shadows(world);
    app->halley.angle += 0.002f * app->time_scale;
    if (app->halley.angle > 6.28f) app->halley.angle = 0;
    draw_comet(&app->halley, delta_time * app->time_scale, &app->comet_model);
    draw_asteroid_belt(app->asteroid_belt);

    // ── HUD and editor overlay ────────────────────────────────────────────────
    glViewport(0, 0, app->win_w, app->win_h);
    setup_projection(app->win_w, app->win_h);
    draw_hud(app->target_planet_index, app->sun_intensity, world,
             app->win_w, app->win_h, app->show_help,
             app->time_scale, app->cam_preset, app->font);

    if (app->app_state == STATE_EDITOR)
        ui_draw_editor(app->font, world, &app->editor, app->win_w, app->win_h);

    // Help overlay
    if (app->show_help && app->font) {
        // Help is drawn inside draw_hud when show_help is true
    }

    SDL_GL_SwapWindow(app->window);
}