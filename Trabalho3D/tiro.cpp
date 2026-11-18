#include "tiro.h"
#include "textura.h"
#include <math.h>

void Tiro::Desenha(){
    static GLUquadric* q = gluNewQuadric();
    gluQuadricNormals(q, GLU_SMOOTH);
    gluQuadricTexture(q, GL_TRUE);
    glPushAttrib(GL_ENABLE_BIT | GL_CURRENT_BIT);
    glDisable(GL_LIGHTING);
    AplicaCor(cfg.cor.tiro);
    LigaTextura(tex.tiro);
    glPushMatrix();
        glTranslatef(gPos.x, gPos.y, gPos.z);
        gluSphere(q, GetRaio(), 10, 8);
    glPopMatrix();
    glPopAttrib();
}

void Tiro::Move(GLdouble dt){
    gPos = gPos + gDir * (gVel * dt);
}
bool Tiro::Valido(){
    return Norma(gPos - gPosInit) < cfg.jogo.tiro.distanciaMax && gPos.z >= 0;
}
