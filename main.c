#include <stdio.h>
#include <stdbool.h>
#include <SDL2/SDL.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include "camera.h"
#include <math.h>
#include "scene.h"
#include <string.h>
#include <stdlib.h>
#include "ui.h"

const int SCREEN_WIDTH  = 1280;
const int SCREEN_HEIGHT = 720;

#define PANEL_W 340

GLuint load_texture(const char* filename);
void draw_skybox(GLuint texture_id);
void draw_atmosphere(float size, float r, float g, float b, float alpha);

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

void draw_hud(int target_index, float intensity, World* w,
              int scr_w, int scr_h, bool help_visible) {
    GLboolean lighting_was_on  = glIsEnabled(GL_LIGHTING);
    GLboolean cull_face_was_on = glIsEnabled(GL_CULL_FACE);
    glMatrixMode(GL_PROJECTION); glPushMatrix(); glLoadIdentity();
    glOrtho(0, scr_w, scr_h, 0, -1, 1);
    glMatrixMode(GL_MODELVIEW); glPushMatrix(); glLoadIdentity();
    glDisable(GL_LIGHTING); glDisable(GL_DEPTH_TEST); glDisable(GL_CULL_FACE);
    glEnable(GL_BLEND); glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glColor4f(1,1,1,0.2f);
    glBegin(GL_QUADS);
        glVertex2f(20,scr_h-50); glVertex2f(220,scr_h-50);
        glVertex2f(220,scr_h-20); glVertex2f(20,scr_h-20);
    glEnd();
    float fill = ((intensity-0.1f)/1.9f)*200.0f;
    glColor4f(1,0.7f,0,0.8f);
    glBegin(GL_QUADS);
        glVertex2f(20,scr_h-50); glVertex2f(20+fill,scr_h-50);
        glVertex2f(20+fill,scr_h-20); glVertex2f(20,scr_h-20);
    glEnd();

    if (target_index != -1 && target_index < w->count) {
        Planet* p = &w->planets[target_index];
        glColor4f(0,0.1f,0.2f,0.7f);
        glBegin(GL_QUADS);
            glVertex2f(scr_w-280,20); glVertex2f(scr_w-20,20);
            glVertex2f(scr_w-20,160); glVertex2f(scr_w-280,160);
        glEnd();
        glColor4f(0,0.8f,1,1);
        glBegin(GL_LINE_LOOP);
            glVertex2f(scr_w-280,20); glVertex2f(scr_w-20,20);
            glVertex2f(scr_w-20,160); glVertex2f(scr_w-280,160);
        glEnd();
        char buf[64];
        glColor3f(1,1,1);
        draw_text_simple(scr_w-260, 40,  p->name);
        sprintf(buf,"DIST: %.1f",  p->distance);  draw_text_simple(scr_w-260, 75,  buf);
        sprintf(buf,"SIZE: %.2f",  p->size);       draw_text_simple(scr_w-260, 110, buf);
        sprintf(buf,"SPD: %.3f",   p->orbit_speed);draw_text_simple(scr_w-260, 140, buf);
    }

    if (!help_visible) {
        glColor4f(0,0.8f,1,0.5f);
        glBegin(GL_LINE_LOOP);
            glVertex2f(20,20); glVertex2f(100,20); glVertex2f(100,50); glVertex2f(20,50);
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

typedef struct { float x,y,z; } Vertex;
typedef struct { int v[3]; } Face;
typedef struct { Vertex* vertices; Face* faces; int vertex_count,face_count,initialized; } OBJModel;
typedef struct { float x,y,z,angle,speed; } Comet;

void load_asteroid_obj(const char* filename, OBJModel* model) {
    FILE* file=fopen(filename,"r");
    if(!file){printf("Error: file not found: %s\n",filename);model->initialized=0;return;}
    char line[256];
    model->vertex_count=model->face_count=0;
    while(fgets(line,sizeof(line),file)){
        if(line[0]=='v'&&line[1]==' ')model->vertex_count++;
        if(line[0]=='f'&&line[1]==' ')model->face_count++;
    }
    model->vertices=(Vertex*)malloc(sizeof(Vertex)*model->vertex_count);
    model->faces   =(Face*)  malloc(sizeof(Face)  *model->face_count);
    if(!model->vertices||!model->faces){printf("OBJ OOM\n");fclose(file);return;}
    rewind(file);
    int vi=0,fi=0;
    while(fgets(line,sizeof(line),file)){
        if(line[0]=='v'&&line[1]==' '){
            sscanf(line,"v %f %f %f",&model->vertices[vi].x,&model->vertices[vi].y,&model->vertices[vi].z);vi++;
        }else if(line[0]=='f'&&line[1]==' '){
            char*p=line+2;
            for(int i=0;i<3;i++){model->faces[fi].v[i]=atoi(p)-1;while(*p&&*p!=' ')p++;while(*p&&*p==' ')p++;}
            fi++;
        }
    }
    fclose(file);model->initialized=1;
    printf("Loaded: %s (%d vertices).\n",filename,model->vertex_count);
}

void draw_obj_model(OBJModel* model, float scale) {
    if(!model->initialized)return;
    glPushMatrix();glScalef(scale,scale,scale);glBegin(GL_TRIANGLES);
    for(int i=0;i<model->face_count;i++)
        for(int j=0;j<3;j++){
            int idx=model->faces[i].v[j];
            glNormal3f(model->vertices[idx].x,model->vertices[idx].y,model->vertices[idx].z);
            glVertex3f(model->vertices[idx].x,model->vertices[idx].y,model->vertices[idx].z);
        }
    glEnd();glPopMatrix();
}

void draw_comet(Comet* c, float delta_time, OBJModel* model) {
    c->angle+=c->speed*delta_time;
    c->x=cosf(c->angle)*40.0f;c->z=sinf(c->angle)*15.0f;c->y=sinf(c->angle*0.5f)*10.0f;
    glPushMatrix();glTranslatef(c->x,c->y,c->z);
    glEnable(GL_LIGHTING);glColor3f(0.8f,0.8f,0.9f);draw_obj_model(model,0.6f);
    glDisable(GL_LIGHTING);glEnable(GL_BLEND);glBlendFunc(GL_SRC_ALPHA,GL_ONE);
    glColor4f(0.5f,0.8f,1.0f,0.4f);glDisable(GL_BLEND);glPopMatrix();
}


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
    GLuint  help_texture_id=0, skybox_texture_id=0;
    OBJModel comet_model={NULL,NULL,0,0,0};
    Comet    halley={0,0,0,0,0.0009f};
    Asteroid asteroid_belt[MAX_ASTEROID];

    Camera camera;
    init_camera(&camera);

    int win_w=SCREEN_WIDTH, win_h=SCREEN_HEIGHT;

    AppState    app_state=STATE_MENU;
    Button      menu_btns[3]={0};
    EditorState editor={-1,"NewPlanet",3,false,EDITOR_TAB_BASIC,0.0f};

    TTF_Font* font=ui_init("assets/font.ttf",15);
    setup_projection(win_w,win_h);

    FILE* ftest=fopen("assets/custom_planets.csv","r");
    if(ftest){fclose(ftest);load_planets(&world,"assets/custom_planets.csv");}
    else       load_planets(&world,"assets/planets.csv");

    init_asteroid_belt(asteroid_belt);
    help_texture_id  =load_texture("assets/help.png");
    skybox_texture_id=load_texture("assets/stars.jpg");
    load_asteroid_obj("assets/asteroid.obj",&comet_model);

    glEnable(GL_LIGHTING);glEnable(GL_LIGHT0);glEnable(GL_COLOR_MATERIAL);
    glEnable(GL_FOG);
    float fogColor[]={0.05f,0.02f,0.15f,1.0f};
    glFogfv(GL_FOG_COLOR,fogColor);glFogf(GL_FOG_DENSITY,0.06f);
    glHint(GL_FOG_HINT,GL_NICEST);glFogi(GL_FOG_MODE,GL_EXP2);
    float ambient[]={0.05f,0.05f,0.05f,1.0f}, specular[]={0.5f,0.5f,0.5f,1.0f};
    glLightfv(GL_LIGHT0,GL_AMBIENT,ambient);glLightfv(GL_LIGHT0,GL_SPECULAR,specular);

    int target_planet_index=-1;

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
                    if(event.key.keysym.sym==SDLK_ESCAPE)app_state=STATE_MENU;
                    if(editor.editing_name&&editor.selected>=0){
                        if(event.key.keysym.sym==SDLK_RETURN||event.key.keysym.sym==SDLK_ESCAPE){
                            editor.editing_name=false;SDL_StopTextInput();
                        }else if(event.key.keysym.sym==SDLK_BACKSPACE){
                            int len=strlen(world.planets[editor.selected].name);
                            if(len>0)world.planets[editor.selected].name[len-1]='\0';
                        }
                    }
                }
                if(event.type==SDL_TEXTINPUT&&editor.editing_name&&editor.selected>=0){
                    strncat(world.planets[editor.selected].name,event.text.text,
                            sizeof(world.planets[editor.selected].name)
                            -strlen(world.planets[editor.selected].name)-1);
                }
                continue;
            }

            // Simulation events
            if(event.type==SDL_WINDOWEVENT&&event.window.event==SDL_WINDOWEVENT_RESIZED){
                win_w=event.window.data1;win_h=event.window.data2;
                setup_projection(win_w,win_h);
            }
            if(event.type==SDL_MOUSEBUTTONDOWN&&event.button.button==SDL_BUTTON_LEFT){
                int hit=pick_planet(event.button.x,event.button.y,&camera,&world);
                if(hit!=-1)target_planet_index=hit;
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
                if(event.key.keysym.sym==SDLK_0)target_planet_index=-1;
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
            world.planets[i].current_angle +=world.planets[i].orbit_speed *delta_time*10.0f;
            world.planets[i].rotation_angle+=world.planets[i].rotation_speed*delta_time*50.0f;
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

        halley.angle+=0.002f;
        if(halley.angle>6.28f)halley.angle=0;
        draw_comet(&halley,delta_time,&comet_model);

        draw_hud(target_planet_index,sun_intensity,&world,win_w,win_h,show_help);

        if(show_help){
            glMatrixMode(GL_PROJECTION);glPushMatrix();glLoadIdentity();
            glOrtho(0,win_w,win_h,0,-1,1);
            glMatrixMode(GL_MODELVIEW);glPushMatrix();glLoadIdentity();
            glDisable(GL_LIGHTING);glDisable(GL_DEPTH_TEST);glDisable(GL_CULL_FACE);
            glEnable(GL_BLEND);glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
            glEnable(GL_TEXTURE_2D);glBindTexture(GL_TEXTURE_2D,help_texture_id);
            glColor3f(1,1,1);
            glBegin(GL_QUADS);
            glTexCoord2f(0,0);glVertex2f(100,      100);
            glTexCoord2f(1,0);glVertex2f(win_w-100,100);
            glTexCoord2f(1,1);glVertex2f(win_w-100,win_h-100);
            glTexCoord2f(0,1);glVertex2f(100,      win_h-100);
            glEnd();
            glDisable(GL_BLEND);glDisable(GL_TEXTURE_2D);
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