//
// Created by novakdominikviktor on 2026. 02. 28..
//
#include <stdio.h>
#include <stdbool.h>
#include <SDL2/SDL.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>
#include "camera.h"
#include "scene.h"
#include "ui.h"
#include "hud.h"
#include "comet.h"

const int SCREEN_WIDTH  = 1280;
const int SCREEN_HEIGHT = 720;

int main(int argc, char* args[]) {
    (void)argc;(void)args;
    if(SDL_Init(SDL_INIT_VIDEO)<0){printf("SDL error: %s\n",SDL_GetError());return 1;}

    SDL_Window* window=SDL_CreateWindow("Solaris",
        SDL_WINDOWPOS_CENTERED,SDL_WINDOWPOS_CENTERED,
        SCREEN_WIDTH,SCREEN_HEIGHT,
        SDL_WINDOW_OPENGL|SDL_WINDOW_RESIZABLE);
    if(!window){printf("window error: %s\n",SDL_GetError());return 1;}

    SDL_SetRelativeMouseMode(SDL_FALSE);
    SDL_ShowCursor(SDL_ENABLE);
    SDL_GLContext glContext=SDL_GL_CreateContext(window);
    glEnable(GL_DEPTH_TEST);
    glClearColor(0.0f,0.0f,0.05f,1.0f);

    Uint32 last_time=SDL_GetTicks();
    float  delta_time=0;
    bool   running=true;
    bool   fog_enabled=true;
    bool   show_orbits=true;
    SDL_Event event;

    World   world;
    float   sun_intensity=1.0f;
    bool    show_help=false;
    GLuint  skybox_texture_id=0;
    OBJModel comet_model={NULL,NULL,0,0,0};
    Comet    halley={0,0,0,0,0.0009f};
    Asteroid asteroid_belt[MAX_ASTEROID];

    Camera camera;
    init_camera(&camera);

    int win_w=SCREEN_WIDTH, win_h=SCREEN_HEIGHT;

    AppState    app_state=STATE_MENU;
    Button      menu_btns[3]={0};
    EditorState editor={-1,"NewPlanet",3,false,false,"",EDITOR_TAB_BASIC,0.0f};

    TTF_Font* font=ui_init("assets/font.ttf",15);
    setup_projection(win_w,win_h);

    FILE* ftest=fopen("assets/custom_planets.csv","r");
    if(ftest){fclose(ftest);load_planets(&world,"assets/custom_planets.csv");}
    else       load_planets(&world,"assets/planets.csv");

    init_asteroid_belt(asteroid_belt);
    skybox_texture_id=load_texture("assets/stars.jpg");
    load_asteroid_obj("assets/asteroid.obj",&comet_model);

    glEnable(GL_LIGHTING);glEnable(GL_LIGHT0);glEnable(GL_COLOR_MATERIAL);
    glEnable(GL_FOG);
    float fogColor[]={0.05f,0.02f,0.15f,1.0f};
    glFogfv(GL_FOG_COLOR,fogColor);glFogf(GL_FOG_DENSITY,0.06f);
    glHint(GL_FOG_HINT,GL_NICEST);glFogi(GL_FOG_MODE,GL_EXP2);
    float ambient[]={0.05f,0.05f,0.05f,1.0f}, specular[]={0.5f,0.5f,0.5f,1.0f};
    glLightfv(GL_LIGHT0,GL_AMBIENT,ambient);glLightfv(GL_LIGHT0,GL_SPECULAR,specular);

    int   target_planet_index=-1;
    float time_scale=1.0f;      // 0=paused, 1=normal, up to 10x
    int   cam_preset=0;         // 0=free, 1=top, 2=side

    while(running){
        Uint32 current_time=SDL_GetTicks();
        delta_time=(current_time-last_time)/1000.0f;
        last_time=current_time;

        while(SDL_PollEvent(&event)){
            if(event.type==SDL_QUIT){running=false;break;}

            if(app_state==STATE_MENU){
                if(event.type==SDL_MOUSEMOTION)
                    ui_menu_hover(event.motion.x,event.motion.y,menu_btns);
                if(event.type==SDL_MOUSEBUTTONDOWN&&event.button.button==SDL_BUTTON_LEFT)
                    ui_menu_click(event.button.x,event.button.y,menu_btns,&app_state,&running);
                continue;
            }

            if(app_state==STATE_EDITOR){
                if(event.type==SDL_MOUSEWHEEL){
                    editor.scroll_y-=event.wheel.y*20.0f;
                    if(editor.scroll_y<0)editor.scroll_y=0;
                }
                if(event.type==SDL_MOUSEBUTTONDOWN&&event.button.button==SDL_BUTTON_LEFT){
                    int cx=event.button.x,cy=event.button.y;
                    if(cx<=PANEL_W)
                        ui_editor_click(cx,cy,&world,&editor,win_w,win_h,&app_state);
                    else{
                        int hit=pick_planet(cx-PANEL_W,cy,&camera,&world);
                        if(hit!=-1)editor.selected=hit;
                    }
                }
                if(event.type==SDL_KEYDOWN){
                    SDL_Keycode k=event.key.keysym.sym;
                    if(editor.editing_name&&editor.selected>=0){
                        if(k==SDLK_RETURN||k==SDLK_KP_ENTER){
                            editor.editing_name=false; SDL_StopTextInput();
                        } else if(k==SDLK_BACKSPACE){
                            char* nm=world.planets[editor.selected].name;
                            int len=(int)strlen(nm); if(len>0) nm[len-1]='\0';
                        } else if(k==SDLK_ESCAPE){
                            editor.editing_name=false; SDL_StopTextInput();
                            app_state=STATE_MENU;
                        }
                    } else if(editor.editing_custom_tex){
                        if(k==SDLK_RETURN||k==SDLK_KP_ENTER){
                            editor.editing_custom_tex=false; SDL_StopTextInput();
                        } else if(k==SDLK_BACKSPACE){
                            int len=(int)strlen(editor.custom_tex_buf);
                            if(len>0) editor.custom_tex_buf[len-1]='\0';
                        } else if(k==SDLK_ESCAPE){
                            editor.editing_custom_tex=false; SDL_StopTextInput();
                        }
                    } else {
                        if(k==SDLK_ESCAPE) app_state=STATE_MENU;
                    }
                }
                if(event.type==SDL_TEXTINPUT){
                    if(editor.editing_name&&editor.selected>=0){
                        strncat(world.planets[editor.selected].name, event.text.text,
                                sizeof(world.planets[editor.selected].name)
                                -strlen(world.planets[editor.selected].name)-1);
                    } else if(editor.editing_custom_tex){
                        strncat(editor.custom_tex_buf, event.text.text,
                                sizeof(editor.custom_tex_buf)
                                -strlen(editor.custom_tex_buf)-1);
                    }
                }
                continue;
            }

            // Simulation events
            if(event.type==SDL_WINDOWEVENT&&event.window.event==SDL_WINDOWEVENT_RESIZED){
                win_w=event.window.data1;win_h=event.window.data2;
                setup_projection(win_w,win_h);
            }
            if(event.type==SDL_MOUSEBUTTONDOWN&&event.button.button==SDL_BUTTON_LEFT){
                // Check camera preset button clicks (bottom-left HUD)
                {
                    int mx=event.button.x, my=event.button.y;
                    float ts_y=(float)win_h-85.0f;
                    float cb_y=ts_y-32.0f;
                    if(my>=cb_y && my<=cb_y+24) {
                        // 4 buttons at x=20,90,160,230 each 46px wide
                        if     (mx>=20 &&mx<=66 ){ cam_preset=0; init_camera(&camera); }
                        else if(mx>=90 &&mx<=136){ cam_preset=1; camera.x=0;camera.y=80.0f;camera.z=0; camera.pitch=-89.0f;camera.yaw=0; }
                        else if(mx>=160&&mx<=206){ cam_preset=2; camera.x=0;camera.y=0;camera.z=80.0f; camera.pitch=0;camera.yaw=0; }
                        else if(mx>=230&&mx<=276){ time_scale=(time_scale>0.0f)?0.0f:1.0f; }
                    }
                }
                // Check time scale bar click (drag)
                {
                    int mx=event.button.x, my=event.button.y;
                    float ts_y=(float)win_h-85.0f;
                    if(mx>=20&&mx<=220&&my>=(int)ts_y&&my<=(int)(ts_y+16)) {
                        time_scale=((float)(mx-20)/200.0f)*10.0f;
                        if(time_scale<0)time_scale=0;
                        if(time_scale>10)time_scale=10;
                    }
                }
                int hit=pick_planet(event.button.x,event.button.y,&camera,&world);
                if(hit!=-1){ target_planet_index=hit; }
            }
            if(event.type==SDL_MOUSEMOTION&&
               (SDL_GetMouseState(NULL,NULL)&SDL_BUTTON(SDL_BUTTON_RIGHT))){
                camera.yaw  -=event.motion.xrel*0.2f;
                camera.pitch-=event.motion.yrel*0.2f;
                if(camera.pitch> 89.0f)camera.pitch= 89.0f;
                if(camera.pitch<-89.0f)camera.pitch=-89.0f;
            }
            if(event.type==SDL_KEYDOWN){
                if(event.key.keysym.sym==SDLK_F1||event.key.keysym.sym==SDLK_h)show_help=!show_help;
                if(event.key.keysym.sym==SDLK_f){
                    fog_enabled=!fog_enabled;
                    if(fog_enabled)glEnable(GL_FOG);else glDisable(GL_FOG);
                }
                if(event.key.keysym.sym==SDLK_o)show_orbits=!show_orbits;
                if(event.key.keysym.sym==SDLK_ESCAPE)running=false;
                if(event.key.keysym.sym>=SDLK_1&&event.key.keysym.sym<=SDLK_9){
                    target_planet_index=event.key.keysym.sym-SDLK_1;
                    if(target_planet_index>=world.count)target_planet_index=-1;
                }
                if(event.key.keysym.sym==SDLK_0) target_planet_index=-1;
                // Time scale: [ = slower, ] = faster, P = pause/resume
                if(event.key.keysym.sym==SDLK_p){
                    time_scale=(time_scale>0.0f)?0.0f:1.0f;
                }
                if(event.key.keysym.sym==SDLK_LEFTBRACKET){
                    time_scale-=0.25f; if(time_scale<0.0f)time_scale=0.0f;
                }
                if(event.key.keysym.sym==SDLK_RIGHTBRACKET){
                    time_scale+=0.25f; if(time_scale>10.0f)time_scale=10.0f;
                }
                // Camera presets: T=top, Y=side, U=free
                if(event.key.keysym.sym==SDLK_t){
                    cam_preset=1;
                    camera.x=0; camera.y=80.0f; camera.z=0;
                    camera.pitch=-89.0f; camera.yaw=0;
                }
                if(event.key.keysym.sym==SDLK_y){
                    cam_preset=2;
                    camera.x=0; camera.y=0; camera.z=80.0f;
                    camera.pitch=0; camera.yaw=0;
                }
                if(event.key.keysym.sym==SDLK_u){
                    cam_preset=0;
                    init_camera(&camera);
                }

            }
        }

        if(app_state==STATE_MENU){
            glViewport(0,0,win_w,win_h);
            glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
            ui_draw_menu(font,win_w,win_h,menu_btns);
            SDL_GL_SwapWindow(window);
            continue;
        }

        const Uint8* keys=SDL_GetKeyboardState(NULL);
        float speed=0.2f;
        if(keys[SDL_SCANCODE_LSHIFT])speed*=3.0f;
        float rad_yaw  =camera.yaw  *(float)M_PI/180.0f;
        float rad_pitch=camera.pitch*(float)M_PI/180.0f;

        if(keys[SDL_SCANCODE_W]||keys[SDL_SCANCODE_S]||
           keys[SDL_SCANCODE_A]||keys[SDL_SCANCODE_D]||
           keys[SDL_SCANCODE_E]||keys[SDL_SCANCODE_Q])
            target_planet_index=-1;

        if(keys[SDL_SCANCODE_W])
            update_camera_position(&camera,-sinf(rad_yaw)*cosf(rad_pitch)*speed, sinf(rad_pitch)*speed,-cosf(rad_yaw)*cosf(rad_pitch)*speed,&world);
        if(keys[SDL_SCANCODE_S])
            update_camera_position(&camera, sinf(rad_yaw)*cosf(rad_pitch)*speed,-sinf(rad_pitch)*speed, cosf(rad_yaw)*cosf(rad_pitch)*speed,&world);
        if(keys[SDL_SCANCODE_A])
            update_camera_position(&camera,-cosf(rad_yaw)*speed,0, sinf(rad_yaw)*speed,&world);
        if(keys[SDL_SCANCODE_D])
            update_camera_position(&camera, cosf(rad_yaw)*speed,0,-sinf(rad_yaw)*speed,&world);
        if(keys[SDL_SCANCODE_E])camera.y+=speed;
        if(keys[SDL_SCANCODE_Q])camera.y-=speed;
        if(keys[SDL_SCANCODE_UP])   camera.pitch+=1.0f;
        if(keys[SDL_SCANCODE_DOWN]) camera.pitch-=1.0f;
        if(keys[SDL_SCANCODE_LEFT]) camera.yaw  +=1.0f;
        if(keys[SDL_SCANCODE_RIGHT])camera.yaw  -=1.0f;

        if(keys[SDL_SCANCODE_KP_PLUS] ||keys[SDL_SCANCODE_EQUALS]){sun_intensity+=0.01f;if(sun_intensity>2.0f)sun_intensity=2.0f;}
        if(keys[SDL_SCANCODE_KP_MINUS]||keys[SDL_SCANCODE_MINUS]) {sun_intensity-=0.01f;if(sun_intensity<0.1f)sun_intensity=0.1f;}

        if(app_state==STATE_EDITOR) setup_projection_editor(win_w,win_h);
        else                        setup_projection(win_w,win_h);

        float r=1.0f;
        float g=sun_intensity>1.0f?1.0f:sun_intensity;
        float b=sun_intensity>1.5f?1.0f:(sun_intensity<1.0f?sun_intensity*0.5f:sun_intensity-0.5f);

        glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
        set_view(&camera);

        glPushMatrix();
            glTranslatef(camera.x,camera.y,camera.z);
            draw_skybox(skybox_texture_id);
        glPopMatrix();

        glDisable(GL_LIGHTING);
        if(show_orbits)draw_orbit_paths(&world);
        draw_asteroid_belt(asteroid_belt);
        glEnable(GL_LIGHTING);

        float diffuse[]  ={r*sun_intensity,g*sun_intensity,b*sun_intensity,1.0f};
        float light_pos[]={0.0f,0.0f,0.0f,1.0f};
        glLightfv(GL_LIGHT0,GL_DIFFUSE, diffuse);
        glLightfv(GL_LIGHT0,GL_POSITION,light_pos);

        for(int i=0;i<world.count;i++){
            world.planets[i].current_angle +=world.planets[i].orbit_speed *delta_time*10.0f*time_scale;
            world.planets[i].rotation_angle+=world.planets[i].rotation_speed*delta_time*50.0f*time_scale;
            Planet*p=&world.planets[i];
            float lx=cosf(p->current_angle)*p->distance;
            float lz=sinf(p->current_angle)*p->distance;
            if(p->parent_index!=-1){
                Planet*parent=&world.planets[p->parent_index];
                p->world_x=parent->world_x+lx;
                p->world_y=parent->world_y;
                p->world_z=parent->world_z+lz;
            }else{p->world_x=lx;p->world_y=0.0f;p->world_z=lz;}
        }

        if(target_planet_index!=-1){
            Planet*p=&world.planets[target_planet_index];
            camera.x=p->world_x+p->size*3.0f;
            camera.y=p->world_y+p->size*2.0f;
            camera.z=p->world_z+p->size*3.0f;
        }

        for(int i=0;i<world.count;i++){
            Planet*p=&world.planets[i];
            glPushMatrix();
            glTranslatef(p->world_x,p->world_y,p->world_z);
            GLUquadric*quad=gluNewQuadric();
            gluQuadricTexture(quad,GL_TRUE);

            if(p->obj_type==OBJ_STAR){
                glDisable(GL_LIGHTING);
                glEnable(GL_TEXTURE_2D);
                glBindTexture(GL_TEXTURE_2D,p->texture_id);
                float ss=p->size;
                if(sun_intensity<0.8f)      ss*=1.0f+(0.8f-sun_intensity)*2.0f;
                else if(sun_intensity>1.5f){ss*=1.0f-(sun_intensity-1.5f)*1.5f;if(ss<0.2f)ss=0.2f;}
                glColor3f(r,g,b);
                GLUquadric*sq=gluNewQuadric();gluQuadricTexture(sq,GL_TRUE);
                gluSphere(sq,ss,32,32);gluDeleteQuadric(sq);
                draw_sun_glow(ss*1.5f,r,g,b);
                glEnable(GL_BLEND);glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
                glColor4f(1.0f,0.9f,0.3f,0.08f);
                GLUquadric*gq=gluNewQuadric();
                gluSphere(gq,ss*1.6f,32,32);gluDeleteQuadric(gq);
                glDisable(GL_BLEND);
                glEnable(GL_LIGHTING);glColor3f(1,1,1);glDisable(GL_TEXTURE_2D);
            }else{
                glEnable(GL_LIGHTING);
                glPushMatrix();
                glRotatef(-90.0f,1,0,0);
                glRotatef(p->axial_tilt,    1,0,0);
                glRotatef(p->rotation_angle,0,0,1);
                glEnable(GL_TEXTURE_2D);
                glBindTexture(GL_TEXTURE_2D,p->texture_id);
                gluSphere(quad,p->size,32,32);
                if(p->has_atmosphere)
                    draw_atmosphere(p->size,p->atmo_r,p->atmo_g,p->atmo_b,0.25f);
                glDisable(GL_TEXTURE_2D);
                glPopMatrix();

                if(p->has_rings){
                    if(p->ring_particles){
                        for(int j=0;j<p->particle_count;j++){
                            p->ring_particles[j].angle+=p->ring_particles[j].speed;
                            if(p->ring_particles[j].angle>=360.0f)
                                p->ring_particles[j].angle-=360.0f;
                        }
                    }
                    glPushMatrix();
                    // FIX: always use ring_tilt — independent per-planet field
                    // Saturn ring_tilt=26.7, Uranus ring_tilt=97.77 → look different
                    glRotatef(p->ring_tilt, 1, 0, 0);
                    draw_ring_particles(p);
                    glPopMatrix();
                }
            }
            gluDeleteQuadric(quad);
            glPopMatrix();
        }

        draw_moon_shadows(&world);

        halley.angle+=0.002f*time_scale;
        if(halley.angle>6.28f)halley.angle=0;
        draw_comet(&halley,delta_time,&comet_model);

        draw_hud(target_planet_index,sun_intensity,&world,win_w,win_h,show_help,
                 time_scale,cam_preset,font);

        if(show_help && font){
            // Draw help overlay with TTF — always up to date with controls
            glMatrixMode(GL_PROJECTION);glPushMatrix();glLoadIdentity();
            glOrtho(0,win_w,win_h,0,-1,1);
            glMatrixMode(GL_MODELVIEW);glPushMatrix();glLoadIdentity();
            glDisable(GL_LIGHTING);glDisable(GL_DEPTH_TEST);glDisable(GL_CULL_FACE);
            glEnable(GL_BLEND);glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

            float hx=100, hy=80, hw=(float)win_w-200, hh=(float)win_h-160;

            // Background
            glColor4f(0.02f,0.05f,0.15f,0.94f);
            glBegin(GL_QUADS);
                glVertex2f(hx,hy);glVertex2f(hx+hw,hy);
                glVertex2f(hx+hw,hy+hh);glVertex2f(hx,hy+hh);
            glEnd();
            // Border
            glColor4f(0.20f,0.55f,1.0f,1.0f);
            glBegin(GL_LINE_LOOP);
                glVertex2f(hx,hy);glVertex2f(hx+hw,hy);
                glVertex2f(hx+hw,hy+hh);glVertex2f(hx,hy+hh);
            glEnd();
            // Title underline
            glColor4f(0.20f,0.55f,1.0f,0.35f);
            glBegin(GL_LINES);
                glVertex2f(hx+12,hy+40);glVertex2f(hx+hw-12,hy+40);
            glEnd();

            // Content
            float tx=hx+20, ty=hy+12;
            float lh=26.0f; // line height
            float col2=hx+hw/2.0f; // second column x

            ui_draw_text(font,"Controls",       tx,ty, 0.40f,0.82f,1.0f,1.0f,win_w,win_h); ty+=44;

            // Column 1 — movement
            ui_draw_text(font,"MOVEMENT",        tx,ty, 0.40f,0.60f,0.90f,1.0f,win_w,win_h); ty+=lh;
            ui_draw_text(font,"W/A/S/D      Move forward/back/left/right",tx,ty,0.80f,0.85f,0.90f,1.0f,win_w,win_h); ty+=lh;
            ui_draw_text(font,"Q/E          Move down/up",                tx,ty,0.80f,0.85f,0.90f,1.0f,win_w,win_h); ty+=lh;
            ui_draw_text(font,"Arrow keys   Rotate camera",               tx,ty,0.80f,0.85f,0.90f,1.0f,win_w,win_h); ty+=lh;
            ui_draw_text(font,"Right mouse  Look around",                 tx,ty,0.80f,0.85f,0.90f,1.0f,win_w,win_h); ty+=lh;
            ui_draw_text(font,"Shift        Sprint (3x speed)",           tx,ty,0.80f,0.85f,0.90f,1.0f,win_w,win_h); ty+=lh+6;

            ui_draw_text(font,"CAMERA PRESETS",  tx,ty, 0.40f,0.60f,0.90f,1.0f,win_w,win_h); ty+=lh;
            ui_draw_text(font,"T            Top-down view",               tx,ty,0.80f,0.85f,0.90f,1.0f,win_w,win_h); ty+=lh;
            ui_draw_text(font,"Y            Side view",                   tx,ty,0.80f,0.85f,0.90f,1.0f,win_w,win_h); ty+=lh;
            ui_draw_text(font,"U            Free camera (reset)",         tx,ty,0.80f,0.85f,0.90f,1.0f,win_w,win_h); ty+=lh+6;

            ui_draw_text(font,"SELECTION",       tx,ty, 0.40f,0.60f,0.90f,1.0f,win_w,win_h); ty+=lh;
            ui_draw_text(font,"Left click   Select planet",               tx,ty,0.80f,0.85f,0.90f,1.0f,win_w,win_h); ty+=lh;
            ui_draw_text(font,"1-9          Jump to planet",              tx,ty,0.80f,0.85f,0.90f,1.0f,win_w,win_h); ty+=lh;
            ui_draw_text(font,"0            Release follow",              tx,ty,0.80f,0.85f,0.90f,1.0f,win_w,win_h); ty+=lh;

            // Column 2
            ty = hy+56;
            ui_draw_text(font,"TIME CONTROL",    col2,ty, 0.40f,0.60f,0.90f,1.0f,win_w,win_h); ty+=lh;
            ui_draw_text(font,"P            Pause / resume",              col2,ty,0.80f,0.85f,0.90f,1.0f,win_w,win_h); ty+=lh;
            ui_draw_text(font,"[            Slow down (0.25x)",           col2,ty,0.80f,0.85f,0.90f,1.0f,win_w,win_h); ty+=lh;
            ui_draw_text(font,"]            Speed up (0.25x, max 10x)",   col2,ty,0.80f,0.85f,0.90f,1.0f,win_w,win_h); ty+=lh;
            ui_draw_text(font,"Click time bar  Set speed directly",       col2,ty,0.80f,0.85f,0.90f,1.0f,win_w,win_h); ty+=lh+6;

            ui_draw_text(font,"DISPLAY",         col2,ty, 0.40f,0.60f,0.90f,1.0f,win_w,win_h); ty+=lh;
            ui_draw_text(font,"+/-          Sun intensity",               col2,ty,0.80f,0.85f,0.90f,1.0f,win_w,win_h); ty+=lh;
            ui_draw_text(font,"F            Toggle fog",                  col2,ty,0.80f,0.85f,0.90f,1.0f,win_w,win_h); ty+=lh;
            ui_draw_text(font,"O            Toggle orbit paths",          col2,ty,0.80f,0.85f,0.90f,1.0f,win_w,win_h); ty+=lh+6;

            ui_draw_text(font,"EDITOR",          col2,ty, 0.40f,0.60f,0.90f,1.0f,win_w,win_h); ty+=lh;
            ui_draw_text(font,"Main menu -> Planet Editor",               col2,ty,0.80f,0.85f,0.90f,1.0f,win_w,win_h); ty+=lh;
            ui_draw_text(font,"Add/Delete planets, set type, parent",     col2,ty,0.80f,0.85f,0.90f,1.0f,win_w,win_h); ty+=lh;
            ui_draw_text(font,"Set textures, rings, atmosphere",          col2,ty,0.80f,0.85f,0.90f,1.0f,win_w,win_h); ty+=lh;
            ui_draw_text(font,"Save custom solar system to CSV",          col2,ty,0.80f,0.85f,0.90f,1.0f,win_w,win_h); ty+=lh+6;

            ui_draw_text(font,"OTHER",           col2,ty, 0.40f,0.60f,0.90f,1.0f,win_w,win_h); ty+=lh;
            ui_draw_text(font,"F1/H         Toggle this help",            col2,ty,0.80f,0.85f,0.90f,1.0f,win_w,win_h); ty+=lh;
            ui_draw_text(font,"ESC          Quit",                        col2,ty,0.80f,0.85f,0.90f,1.0f,win_w,win_h);

            glDisable(GL_BLEND);
            glEnable(GL_DEPTH_TEST);glEnable(GL_CULL_FACE);
            glPopMatrix();glMatrixMode(GL_PROJECTION);glPopMatrix();
            glMatrixMode(GL_MODELVIEW);
        }

        if(app_state==STATE_EDITOR){
            glViewport(0,0,win_w,win_h);
            ui_draw_editor(font,&world,&editor,win_w,win_h);
        }

        SDL_GL_SwapWindow(window);
    }

    for(int i=0;i<world.count;i++)free_ring_particles(&world.planets[i]);
    if(comet_model.vertices)free(comet_model.vertices);
    if(comet_model.faces)   free(comet_model.faces);
    ui_quit(font);
    SDL_GL_DeleteContext(glContext);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}