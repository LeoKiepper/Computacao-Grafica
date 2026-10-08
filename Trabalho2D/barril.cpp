#include "barril.h"
#include <GL/gl.h>
#include <stdlib.h>

template <class Cor>
void Barril::DesenhaRect(GLdouble height, GLdouble width, const Cor& cor)
    {
    glColor3f(cor.R, cor.G, cor.B);
    glBegin(GL_POLYGON);
        glVertex3f (-width/2.0, -height/2.0, 0);
        glVertex3f (-width/2.0,  height/2.0, 0);
        glVertex3f ( width/2.0,  height/2.0, 0);
        glVertex3f ( width/2.0, -height/2.0, 0);
    glEnd();
}
void Barril::Desenha(){
    GLfloat w, h;
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
        glTranslatef(gPos_x, gPos_y, 0);
        h = escala * cfg.geometriaBase.barril.diametro;
        w = escala * cfg.geometriaBase.barril.altura;
        DesenhaRect(h, w, cfg.cores.barril);
    glPopMatrix();
    if(inimigo) inimigo->Desenha();
}
void Barril::Rola(GLdouble dt){
    GLdouble vel = cfg.cinematica.barril.velocidade;
    gPos_y -= vel * dt;
    if(inimigo) inimigo->Anda(dt);

    // Repassa a posição atual para tratamento de colisão nos limites da arena
    limiteArena(gPos_x, gPos_y, indice);
}
