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
    // 1. Update Position
    c->angle += c->speed * delta_time * 10.0f;
    c->x = cosf(c->angle) * 40.0f;
    c->z = sinf(c->angle) * 15.0f;
    c->y = sinf(c->angle * 0.5f) * 10.0f;

    // 2. Update Trail Points
    // Add new point at current head
    c->trail[c->trail_head].x = c->x;
    c->trail[c->trail_head].y = c->y;
    c->trail[c->trail_head].z = c->z;
    c->trail[c->trail_head].life = 1.0f;

    // Increment head
    c->trail_head = (c->trail_head + 1) % MAX_TRAIL_POINTS;
    if (c->trail_length < MAX_TRAIL_POINTS) c->trail_length++;

    // Fade all points
    for (int i = 0; i < c->trail_length; i++) {
        c->trail[i].life -= delta_time * 0.4f;
        if (c->trail[i].life < 0) c->trail[i].life = 0;
    }

    // 3. Draw Trail (Quad strip approach for smooth tail)
    glDisable(GL_LIGHTING);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);

    glBegin(GL_QUAD_STRIP);
    for (int i = 0; i < c->trail_length; i++) {
        // Index from tail to head
        int idx = (c->trail_head - i - 1 + MAX_TRAIL_POINTS) % MAX_TRAIL_POINTS;
        if (c->trail[idx].life <= 0) continue;

        float alpha = c->trail[idx].life * 0.5f;
        float width = c->trail[idx].life * 0.3f;

        glColor4f(0.5f, 0.7f, 1.0f, alpha);
        glVertex3f(c->trail[idx].x, c->trail[idx].y - width, c->trail[idx].z);
        glVertex3f(c->trail[idx].x, c->trail[idx].y + width, c->trail[idx].z);
    }
    glEnd();

    // 4. Draw Core
    glPushMatrix();
    glTranslatef(c->x, c->y, c->z);
    glEnable(GL_LIGHTING);
    glColor3f(0.8f, 0.8f, 0.9f);
    draw_obj_model(model, 0.6f);
    glPopMatrix();

    glDisable(GL_BLEND);
}
