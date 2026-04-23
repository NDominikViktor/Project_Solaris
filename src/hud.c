//
// Created by novakdominikviktor on 2026. 02. 28..
//
#include "hud.h"
#include "scene.h"
#include "ui.h"
#include <SDL2/SDL_ttf.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <math.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

void setup_projection(int width, int height) {
    if (height == 0) height = 1;
    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    float aspect = (float)width / (float)height;
    float fov = 45.0f, zNear = 0.1f, zFar = 1000.0f;
    float fH  = tanf(fov / 360.0f * 3.14159f) * zNear;
    float fW  = fH * aspect;
    glFrustum(-fW, fW, -fH, fH, zNear, zFar);
    glMatrixMode(GL_MODELVIEW);
}

void setup_projection_editor(int win_w, int win_h) {
    if (win_h == 0) win_h = 1;
    int vw = win_w - PANEL_W;
    if (vw < 1) vw = 1;
    glViewport(PANEL_W, 0, vw, win_h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    float aspect = (float)vw / (float)win_h;
    float fov = 45.0f, zNear = 0.1f, zFar = 1000.0f;
    float fH  = tanf(fov / 360.0f * 3.14159f) * zNear;
    float fW  = fH * aspect;
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
        if (c >= 'a' && c <= 'z') c -= 32;
        if(c=='A'){glVertex2f(ox,10);glVertex2f(ox+4,0);glVertex2f(ox+4,0);glVertex2f(ox+8,10);glVertex2f(ox+2,5);glVertex2f(ox+6,5);}
        else if(c=='B'){glVertex2f(ox,0);glVertex2f(ox,10);glVertex2f(ox,0);glVertex2f(ox+6,2);glVertex2f(ox+6,2);glVertex2f(ox,5);glVertex2f(ox,5);glVertex2f(ox+7,7);glVertex2f(ox+7,7);glVertex2f(ox,10);}
        else if(c=='C'){glVertex2f(ox+8,2);glVertex2f(ox+4,0);glVertex2f(ox+4,0);glVertex2f(ox,4);glVertex2f(ox,4);glVertex2f(ox,6);glVertex2f(ox,6);glVertex2f(ox+4,10);glVertex2f(ox+4,10);glVertex2f(ox+8,8);}
        else if(c=='D'){glVertex2f(ox,0);glVertex2f(ox,10);glVertex2f(ox,0);glVertex2f(ox+6,0);glVertex2f(ox+6,0);glVertex2f(ox+8,5);glVertex2f(ox+8,5);glVertex2f(ox+6,10);glVertex2f(ox+6,10);glVertex2f(ox,10);}
        else if(c=='E'){glVertex2f(ox,0);glVertex2f(ox,10);glVertex2f(ox,0);glVertex2f(ox+8,0);glVertex2f(ox,5);glVertex2f(ox+6,5);glVertex2f(ox,10);glVertex2f(ox+8,10);}
        else if(c=='F'){glVertex2f(ox,0);glVertex2f(ox,10);glVertex2f(ox,0);glVertex2f(ox+8,0);glVertex2f(ox,5);glVertex2f(ox+6,5);}
        else if(c=='G'){glVertex2f(ox+8,2);glVertex2f(ox+4,0);glVertex2f(ox+4,0);glVertex2f(ox,4);glVertex2f(ox,4);glVertex2f(ox,10);glVertex2f(ox,10);glVertex2f(ox+8,10);glVertex2f(ox+8,10);glVertex2f(ox+8,5);glVertex2f(ox+8,5);glVertex2f(ox+4,5);}
        else if(c=='H'){glVertex2f(ox,0);glVertex2f(ox,10);glVertex2f(ox+8,0);glVertex2f(ox+8,10);glVertex2f(ox,5);glVertex2f(ox+8,5);}
        else if(c=='I'){glVertex2f(ox+4,0);glVertex2f(ox+4,10);glVertex2f(ox+1,0);glVertex2f(ox+7,0);glVertex2f(ox+1,10);glVertex2f(ox+7,10);}
        else if(c=='J'){glVertex2f(ox+8,0);glVertex2f(ox+8,8);glVertex2f(ox+8,8);glVertex2f(ox+4,10);glVertex2f(ox+4,10);glVertex2f(ox,8);}
        else if(c=='K'){glVertex2f(ox,0);glVertex2f(ox,10);glVertex2f(ox,5);glVertex2f(ox+8,0);glVertex2f(ox,5);glVertex2f(ox+8,10);}
        else if(c=='L'){glVertex2f(ox,0);glVertex2f(ox,10);glVertex2f(ox,10);glVertex2f(ox+8,10);}
        else if(c=='M'){glVertex2f(ox,10);glVertex2f(ox,0);glVertex2f(ox,0);glVertex2f(ox+4,5);glVertex2f(ox+4,5);glVertex2f(ox+8,0);glVertex2f(ox+8,0);glVertex2f(ox+8,10);}
        else if(c=='N'){glVertex2f(ox,10);glVertex2f(ox,0);glVertex2f(ox,0);glVertex2f(ox+8,10);glVertex2f(ox+8,10);glVertex2f(ox+8,0);}
        else if(c=='O'){glVertex2f(ox,0);glVertex2f(ox+8,0);glVertex2f(ox+8,0);glVertex2f(ox+8,10);glVertex2f(ox+8,10);glVertex2f(ox,10);glVertex2f(ox,10);glVertex2f(ox,0);}
        else if(c=='P'){glVertex2f(ox,0);glVertex2f(ox,10);glVertex2f(ox,0);glVertex2f(ox+8,0);glVertex2f(ox+8,0);glVertex2f(ox+8,5);glVertex2f(ox+8,5);glVertex2f(ox,5);}
        else if(c=='Q'){glVertex2f(ox,0);glVertex2f(ox+8,0);glVertex2f(ox+8,0);glVertex2f(ox+8,10);glVertex2f(ox+8,10);glVertex2f(ox,10);glVertex2f(ox,10);glVertex2f(ox,0);glVertex2f(ox+4,6);glVertex2f(ox+8,10);}
        else if(c=='R'){glVertex2f(ox,0);glVertex2f(ox,10);glVertex2f(ox,0);glVertex2f(ox+8,0);glVertex2f(ox+8,0);glVertex2f(ox+8,5);glVertex2f(ox+8,5);glVertex2f(ox,5);glVertex2f(ox+4,5);glVertex2f(ox+8,10);}
        else if(c=='S'){glVertex2f(ox+8,0);glVertex2f(ox,0);glVertex2f(ox,0);glVertex2f(ox,5);glVertex2f(ox,5);glVertex2f(ox+8,5);glVertex2f(ox+8,5);glVertex2f(ox+8,10);glVertex2f(ox+8,10);glVertex2f(ox,10);}
        else if(c=='T'){glVertex2f(ox+4,0);glVertex2f(ox+4,10);glVertex2f(ox,0);glVertex2f(ox+8,0);}
        else if(c=='U'){glVertex2f(ox,0);glVertex2f(ox,10);glVertex2f(ox,10);glVertex2f(ox+8,10);glVertex2f(ox+8,10);glVertex2f(ox+8,0);}
        else if(c=='V'){glVertex2f(ox,0);glVertex2f(ox+4,10);glVertex2f(ox+4,10);glVertex2f(ox+8,0);}
        else if(c=='W'){glVertex2f(ox,0);glVertex2f(ox,10);glVertex2f(ox,10);glVertex2f(ox+4,5);glVertex2f(ox+4,5);glVertex2f(ox+8,10);glVertex2f(ox+8,10);glVertex2f(ox+8,0);}
        else if(c=='X'){glVertex2f(ox,0);glVertex2f(ox+8,10);glVertex2f(ox+8,0);glVertex2f(ox,10);}
        else if(c=='Y'){glVertex2f(ox,0);glVertex2f(ox+4,5);glVertex2f(ox+8,0);glVertex2f(ox+4,5);glVertex2f(ox+4,5);glVertex2f(ox+4,10);}
        else if(c=='Z'){glVertex2f(ox,0);glVertex2f(ox+8,0);glVertex2f(ox+8,0);glVertex2f(ox,10);glVertex2f(ox,10);glVertex2f(ox+8,10);}
        else if(c=='0'){glVertex2f(ox,0);glVertex2f(ox+8,0);glVertex2f(ox+8,0);glVertex2f(ox+8,10);glVertex2f(ox+8,10);glVertex2f(ox,10);glVertex2f(ox,10);glVertex2f(ox,0);glVertex2f(ox+8,0);glVertex2f(ox,10);}
        else if(c=='1'){glVertex2f(ox+4,0);glVertex2f(ox+4,10);glVertex2f(ox,2);glVertex2f(ox+4,0);}
        else if(c=='2'){glVertex2f(ox,0);glVertex2f(ox+8,0);glVertex2f(ox+8,0);glVertex2f(ox+8,5);glVertex2f(ox+8,5);glVertex2f(ox,5);glVertex2f(ox,5);glVertex2f(ox,10);glVertex2f(ox,10);glVertex2f(ox+8,10);}
        else if(c=='3'){glVertex2f(ox,0);glVertex2f(ox+8,0);glVertex2f(ox+8,0);glVertex2f(ox+8,10);glVertex2f(ox+8,10);glVertex2f(ox,10);glVertex2f(ox+4,5);glVertex2f(ox+8,5);}
        else if(c=='4'){glVertex2f(ox,0);glVertex2f(ox,5);glVertex2f(ox,5);glVertex2f(ox+8,5);glVertex2f(ox+6,0);glVertex2f(ox+6,10);}
        else if(c=='5'){glVertex2f(ox+8,0);glVertex2f(ox,0);glVertex2f(ox,0);glVertex2f(ox,5);glVertex2f(ox,5);glVertex2f(ox+8,5);glVertex2f(ox+8,5);glVertex2f(ox+8,10);glVertex2f(ox+8,10);glVertex2f(ox,10);}
        else if(c=='6'){glVertex2f(ox,0);glVertex2f(ox,10);glVertex2f(ox,0);glVertex2f(ox+8,0);glVertex2f(ox,5);glVertex2f(ox+8,5);glVertex2f(ox+8,5);glVertex2f(ox+8,10);glVertex2f(ox+8,10);glVertex2f(ox,10);}
        else if(c=='7'){glVertex2f(ox,0);glVertex2f(ox+8,0);glVertex2f(ox+8,0);glVertex2f(ox+4,10);}
        else if(c=='8'){glVertex2f(ox,0);glVertex2f(ox+8,0);glVertex2f(ox+8,0);glVertex2f(ox+8,10);glVertex2f(ox+8,10);glVertex2f(ox,10);glVertex2f(ox,10);glVertex2f(ox,0);glVertex2f(ox,5);glVertex2f(ox+8,5);}
        else if(c=='9'){glVertex2f(ox,0);glVertex2f(ox+8,0);glVertex2f(ox+8,0);glVertex2f(ox+8,10);glVertex2f(ox,5);glVertex2f(ox+8,5);glVertex2f(ox,0);glVertex2f(ox,5);}
        else if(c=='.'){glVertex2f(ox+3,9);glVertex2f(ox+5,9);glVertex2f(ox+3,10);glVertex2f(ox+5,10);}
        else if(c==':'){glVertex2f(ox+4,2);glVertex2f(ox+4,3);glVertex2f(ox+4,7);glVertex2f(ox+4,8);}
        else if(c=='-'){glVertex2f(ox+2,5);glVertex2f(ox+6,5);}
    }
    glEnd();
    glLineWidth(1.0f);
    glPopMatrix();
}

void draw_orbit_paths(World* world) {
    glDisable(GL_LIGHTING);
    glDisable(GL_TEXTURE_2D);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glLineWidth(1.0f);
    int segments = 128;
    for (int i = 0; i < world->count; i++) {
        Planet* p = &world->planets[i];
        if (p->obj_type == OBJ_STAR) continue;
        float cx = 0.0f, cz = 0.0f;
        if (p->parent_index != -1) {
            cx = world->planets[p->parent_index].world_x;
            cz = world->planets[p->parent_index].world_z;
        }
        if (p->parent_index != -1)
            glColor4f(0.6f, 0.6f, 0.8f, 0.25f);
        else
            glColor4f(0.4f, 0.6f, 1.0f, 0.18f);
        glBegin(GL_LINE_LOOP);
        for (int j = 0; j < segments; j++) {
            float angle = (float)j / (float)segments * 2.0f * 3.14159f;
            glVertex3f(cx + cosf(angle)*p->distance, 0.0f, cz + sinf(angle)*p->distance);
        }
        glEnd();
    }
    glEnable(GL_LIGHTING);
    glDisable(GL_BLEND);
}

// Forward declaration for TTF text in HUD
void ui_draw_text(TTF_Font* font, const char* text, float x, float y,
                  float r, float g, float b, float a, int win_w, int win_h);

void draw_hud(int target_index, float intensity, World* w,
              int scr_w, int scr_h, bool help_visible,
              float time_scale, int cam_preset, TTF_Font* font) {
    GLboolean lighting_was_on  = glIsEnabled(GL_LIGHTING);
    GLboolean cull_face_was_on = glIsEnabled(GL_CULL_FACE);
    glMatrixMode(GL_PROJECTION); glPushMatrix(); glLoadIdentity();
    glOrtho(0, scr_w, scr_h, 0, -1, 1);
    glMatrixMode(GL_MODELVIEW); glPushMatrix(); glLoadIdentity();
    glDisable(GL_LIGHTING); glDisable(GL_DEPTH_TEST); glDisable(GL_CULL_FACE);
    glEnable(GL_BLEND); glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // ── Sun intensity bar (bottom-left) ───────────────────────────────────────
    glColor4f(1,1,1,0.2f);
    glBegin(GL_QUADS);
        glVertex2f(20,scr_h-50); glVertex2f(220,scr_h-50);
        glVertex2f(220,scr_h-20); glVertex2f(20,scr_h-20);
    glEnd();
    float fill=((intensity-0.1f)/1.9f)*200.0f;
    glColor4f(1,0.7f,0,0.8f);
    glBegin(GL_QUADS);
        glVertex2f(20,scr_h-50); glVertex2f(20+fill,scr_h-50);
        glVertex2f(20+fill,scr_h-20); glVertex2f(20,scr_h-20);
    glEnd();

    // ── Time scale bar + label (bottom-left, above intensity bar) ────────────
    float ts_y = (float)scr_h - 85.0f;
    // Track
    glColor4f(0.2f,0.2f,0.3f,0.7f);
    glBegin(GL_QUADS);
        glVertex2f(20,ts_y); glVertex2f(220,ts_y);
        glVertex2f(220,ts_y+16); glVertex2f(20,ts_y+16);
    glEnd();
    // Fill — colour shifts warm→fast, cool→slow, grey→paused
    float ts_fill = (time_scale/10.0f)*200.0f;
    float ts_r = (time_scale==0.0f)?0.4f:(time_scale<=1.0f?0.3f:1.0f);
    float ts_g = (time_scale==0.0f)?0.4f:(time_scale<=1.0f?0.7f:0.5f);
    float ts_b = (time_scale==0.0f)?0.5f:(time_scale<=1.0f?1.0f:0.2f);
    glColor4f(ts_r,ts_g,ts_b,0.85f);
    glBegin(GL_QUADS);
        glVertex2f(20,ts_y); glVertex2f(20+ts_fill,ts_y);
        glVertex2f(20+ts_fill,ts_y+16); glVertex2f(20,ts_y+16);
    glEnd();
    // Border
    glColor4f(0.5f,0.6f,0.8f,0.6f);
    glBegin(GL_LINE_LOOP);
        glVertex2f(20,ts_y); glVertex2f(220,ts_y);
        glVertex2f(220,ts_y+16); glVertex2f(20,ts_y+16);
    glEnd();
    // Label using draw_text_simple
    char ts_buf[32];
    if (time_scale==0.0f) snprintf(ts_buf,sizeof(ts_buf),"TIME: PAUSED");
    else                  snprintf(ts_buf,sizeof(ts_buf),"TIME: %.2fx", time_scale);
    glColor3f(0.8f,0.9f,1.0f);
    draw_text_simple(24, ts_y+1, ts_buf);

    // ── Camera preset + pause buttons (bottom-left, above time bar) ─────────
    // 4 buttons: FREE | TOP | SIDE | PAUSE — each 46px wide, 24px gap
    float cb_y = ts_y - 30.0f;
    const char* cam_labels[4] = {"FREE","TOP","SIDE","PAUSE"};
    for (int i=0; i<4; i++) {
        float cx = 20.0f + i*70.0f;
        float cw = 46.0f;
        // Highlight active state
        int active = (i==0 && cam_preset==0) ||
                     (i==1 && cam_preset==1) ||
                     (i==2 && cam_preset==2) ||
                     (i==3 && time_scale==0.0f);
        // Background
        if (active)
            glColor4f(0.14f,0.35f,0.70f,0.92f);
        else
            glColor4f(0.06f,0.09f,0.20f,0.82f);
        glBegin(GL_QUADS);
            glVertex2f(cx,cb_y); glVertex2f(cx+cw,cb_y);
            glVertex2f(cx+cw,cb_y+24); glVertex2f(cx,cb_y+24);
        glEnd();
        // Border — brighter when active
        if (active) glColor4f(0.40f,0.75f,1.0f,1.0f);
        else        glColor4f(0.22f,0.44f,0.80f,0.75f);
        glBegin(GL_LINE_LOOP);
            glVertex2f(cx,cb_y); glVertex2f(cx+cw,cb_y);
            glVertex2f(cx+cw,cb_y+24); glVertex2f(cx,cb_y+24);
        glEnd();
        // Label
        if (active) glColor3f(0.55f,0.90f,1.0f);
        else        glColor3f(0.65f,0.78f,0.95f);
        draw_text_simple(cx+3, cb_y+5, cam_labels[i]);
    }

    // ── Planet info panel (top-right) — sim data ──────────
    if (target_index!=-1 && target_index<w->count && font) {
        Planet* p=&w->planets[target_index];

        // Panel height: base sim rows
        int sim_rows = 6;
        float line_h = 20.0f;
        float pw = 300.0f;
        float ph = 36.0f + sim_rows * line_h;
        float px = (float)scr_w - pw - 16.0f;
        float py = 16.0f;

        // Background
        glColor4f(0.02f,0.06f,0.16f,0.88f);
        glBegin(GL_QUADS);
            glVertex2f(px,py); glVertex2f(px+pw,py);
            glVertex2f(px+pw,py+ph); glVertex2f(px,py+ph);
        glEnd();
        // Border
        glColor4f(0.15f,0.55f,1.0f,0.9f);
        glBegin(GL_LINE_LOOP);
            glVertex2f(px,py); glVertex2f(px+pw,py);
            glVertex2f(px+pw,py+ph); glVertex2f(px,py+ph);
        glEnd();
        // Title accent line
        glColor4f(0.15f,0.55f,1.0f,0.35f);
        glBegin(GL_LINES);
            glVertex2f(px+8,py+30); glVertex2f(px+pw-8,py+30);
        glEnd();

        // Title (TTF)
        glEnable(GL_TEXTURE_2D);
        ui_draw_text(font, p->name, px+10, py+8, 0.4f,0.82f,1.0f,1.0f, scr_w,scr_h);
        glDisable(GL_TEXTURE_2D);

        // Sim rows
        char buf[80];
        float ty = py + 36.0f;
        glEnable(GL_TEXTURE_2D);

        const char* type_str = (p->obj_type==OBJ_STAR)?"Star":
                               (p->obj_type==OBJ_MOON)?"Moon":"Planet";
        snprintf(buf,sizeof(buf),"Type:         %s", type_str);
        ui_draw_text(font,buf, px+10,ty, 0.65f,0.75f,0.90f,1.0f,scr_w,scr_h); ty+=line_h;

        const char* par=( p->parent_index>=0&&p->parent_index<w->count)
            ? w->planets[p->parent_index].name : "none";
        snprintf(buf,sizeof(buf),"Parent:       %s", par);
        ui_draw_text(font,buf, px+10,ty, 0.65f,0.75f,0.90f,1.0f,scr_w,scr_h); ty+=line_h;

        snprintf(buf,sizeof(buf),"Distance:     %.2f (sim)", p->distance);
        ui_draw_text(font,buf, px+10,ty, 0.65f,0.75f,0.90f,1.0f,scr_w,scr_h); ty+=line_h;

        snprintf(buf,sizeof(buf),"Size:         %.2f (sim)", p->size);
        ui_draw_text(font,buf, px+10,ty, 0.65f,0.75f,0.90f,1.0f,scr_w,scr_h); ty+=line_h;

        snprintf(buf,sizeof(buf),"Orbit speed:  %.4f rad/s", p->orbit_speed);
        ui_draw_text(font,buf, px+10,ty, 0.65f,0.75f,0.90f,1.0f,scr_w,scr_h); ty+=line_h;

        snprintf(buf,sizeof(buf),"Axial tilt:   %.1f deg", p->axial_tilt);
        ui_draw_text(font,buf, px+10,ty, 0.65f,0.75f,0.90f,1.0f,scr_w,scr_h); ty+=line_h;

        glDisable(GL_TEXTURE_2D);
    }

    // ── Help icon (top-left) ─────────────────────────────────────────────────
    if (!help_visible) {
        glColor4f(0,0.8f,1,0.5f);
        glBegin(GL_LINE_LOOP);
            glVertex2f(20,20); glVertex2f(100,20);
            glVertex2f(100,50); glVertex2f(20,50);
        glEnd();
        glBegin(GL_LINES);
            glVertex2f(60,30); glVertex2f(60,32);
            glVertex2f(60,37); glVertex2f(60,45);
        glEnd();
    }

    glDisable(GL_BLEND); glEnable(GL_DEPTH_TEST);
    if (lighting_was_on)  glEnable(GL_LIGHTING);
    if (cull_face_was_on) glEnable(GL_CULL_FACE);
    glPopMatrix(); glMatrixMode(GL_PROJECTION); glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
}

void draw_sun_glow(float size, float r, float g, float b) {
    glPushMatrix();
    glDisable(GL_LIGHTING); glDisable(GL_TEXTURE_2D);
    glEnable(GL_BLEND); glBlendFunc(GL_SRC_ALPHA, GL_ONE);
    glDisable(GL_DEPTH_TEST); glDepthMask(GL_FALSE);
    for (int i = 1; i <= 8; i++) {
        float cs = size*(1.0f+(float)i*0.25f);
        float alpha = 0.12f*(1.0f-(float)i/8.0f);
        glColor4f(r,g,b,alpha);
        float mv[16]; glGetFloatv(GL_MODELVIEW_MATRIX,mv);
        for(int k=0;k<3;k++) for(int j=0;j<3;j++) mv[k*4+j]=(k==j)?1.0f:0.0f;
        glLoadMatrixf(mv);
        glBegin(GL_TRIANGLE_FAN); glVertex3f(0,0,0);
        for(int a=0;a<=360;a+=2){float rad=a*3.14159f/180.0f;glVertex3f(cosf(rad)*cs,sinf(rad)*cs,0);}
        glEnd();
    }
    glDepthMask(GL_TRUE); glEnable(GL_DEPTH_TEST); glEnable(GL_LIGHTING); glDisable(GL_BLEND);
    glPopMatrix();
}