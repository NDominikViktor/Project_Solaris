//
// Created by novakdominikviktor on 2026. 02. 28..
//
#include "comet.h"
#include <GL/gl.h>
#include <GL/glu.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>


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