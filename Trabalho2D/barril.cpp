#include "barril.h"
#include <GL/gl.h>
#include <GL/glut.h>
#include <stdlib.h>
#include <cstdio>

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
void Barril::DesenhaResistencia(){
    GLdouble xTexto = escala * (cfg.geometriaBase.jogadores.raioCabeca
                              + cfg.geometriaBase.jogadores.larguraArma) + 2;
    char texto[8];
    snprintf(texto, sizeof texto, "%d", resistencia);

    glColor3f(0, 0, 0);
    glRasterPos2f(gPos_x + xTexto, gPos_y - 4);
    for (const char* c = texto; *c; c++)
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, *c);
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
    DesenhaResistencia();
}
void Barril::Rola(GLdouble dt){
    GLdouble vel = cfg.cinematica.barril.velocidade;
    gPos_y -= vel * dt;
    if(inimigo){
        inimigo->Anda(dt);
        inimigo->MiraInimigo(dt);
    }

    // Repassa a posição atual para tratamento de colisão nos limites da arena
    limiteArena(gPos_x, gPos_y, indice);
}
bool Barril::colisaoComCirculo(GLdouble x, GLdouble y, GLdouble r)
{ // x e y são as coordenadas do centro do círculo, no SC_mundo
    static const GLdouble meiaLargura = escala * cfg.geometriaBase.barril.altura   / 2.0; // em x
    static const GLdouble meiaAltura  = escala * cfg.geometriaBase.barril.diametro / 2.0; // em y


    const GLdouble paraSCBarril[3][3] = {
        {1, 0, -gPos_x},
        {0, 1, -gPos_y},
        {0, 0,    1   }
    };
    Vec2 c = MultiplicaMatrizEsq2d(paraSCBarril, Vec2{x, y, 1});


    if (fabs(c.x) <= meiaLargura && fabs(c.y) <= meiaAltura  + r) return true;
    if (fabs(c.y) <= meiaAltura  && fabs(c.x) <= meiaLargura + r) return true;


    const GLdouble sx[4] = {+1, +1, -1, -1};
    const GLdouble sy[4] = {+1, -1, +1, -1};
    for (int i = 0; i < 4; i++) {
        const GLdouble paraSCVertice[3][3] = {
            {1, 0, -sx[i]*meiaLargura},
            {0, 1, -sy[i]*meiaAltura },
            {0, 0,          1        }
        };
        if (Norma(MultiplicaMatrizEsq2d(paraSCVertice, c)) <= r) return true;
    }
    return false;
}
bool Barril::danifica(int d){
    resistencia -=d;
    return resistencia <=0;
}
