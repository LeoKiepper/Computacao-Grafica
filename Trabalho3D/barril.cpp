#include "barril.h"
#include <GL/gl.h>
#include <GL/glut.h>
#include <stdlib.h>
#include <cstdio>
#include <GL/glu.h>
#include "textura.h"

void Barril::DesenhaResistencia(bool noturno){
    char texto[8];
    snprintf(texto, sizeof texto, "%d", resistencia);

    GLdouble topoBarril = escala * cfg.geometriaBase.barril.diametro;
    GLdouble z = topoBarril + 1.15 * Atirador::Altura(cfg);
    int largura = glutBitmapLength(GLUT_BITMAP_HELVETICA_12, (const unsigned char*)texto);

    glPushAttrib(GL_ENABLE_BIT);
    glDisable(GL_LIGHTING);
    AplicaCor(noturno ? cfg.cor.textoBarrilNoturno : cfg.cor.textoBarril);
    glRasterPos3f(gPos_x, gPos_y, z);
    glBitmap(0, 0, 0, 0, -largura/2.0, -4, NULL);
    for (const char* c = texto; *c; c++)
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, *c);
    glPopAttrib();
}

void Barril::Desenha(bool noturno){
    static GLUquadric* q = gluNewQuadric();
    gluQuadricNormals(q, GLU_SMOOTH);
    gluQuadricTexture(q, GL_TRUE);

    GLdouble r = escala * cfg.geometriaBase.barril.diametro / 2.0;
    GLdouble c = escala * cfg.geometriaBase.barril.altura;

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
        glTranslatef(gPos_x, gPos_y, r);
        glRotatef(anguloRolagem, 1, 0, 0);
        glRotatef(90, 0, 1, 0);
        glTranslatef(0, 0, -c/2.0);

        AplicaCor(cfg.cor.barril);
        LigaTextura(tex.barril);
        gluCylinder(q, r, r, c, cfg.geometria.barril.fatias,cfg.geometria.barril.stacks);

        AplicaCor(cfg.cor.tampaBarril);
        LigaTextura(tex.tampaBarril);
        glPushMatrix();
            glRotatef(180, 0, 1, 0);
            gluDisk(q, 0, r, cfg.geometria.barril.fatias, 1);
        glPopMatrix();
        glTranslatef(0, 0, c);
        gluDisk(q, 0, r, 24, 1);
    glPopMatrix();
    DesligaTextura();

    if(inimigo) inimigo->Desenha();
    DesenhaResistencia(noturno);
}

void Barril::Rola(GLdouble dt){
    GLdouble vel = cfg.cinematica.barril.velocidade;
    gPos_y -= vel * dt;
    GLdouble r = escala * cfg.geometriaBase.barril.diametro / 2.0;
    anguloRolagem += (vel * dt / r) * 180.0 / M_PI;
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
bool Barril::colisaoComEsfera(Vec3 c, GLdouble rc){
    GLdouble raio = escala * cfg.geometriaBase.barril.diametro / 2.0;
    GLdouble meio = escala * cfg.geometriaBase.barril.altura / 2.0;
    Vec3 p = c - Vec3{gPos_x, gPos_y, raio};
    GLdouble axial = fabs(p.x);
    GLdouble dyz   = sqrt(p.y*p.y + p.z*p.z);

    if (axial <= meio)
        return dyz <= raio + rc;
    if (dyz <= raio)
        return axial - meio <= rc;
    GLdouble da = axial - meio;
    GLdouble dr = dyz - raio;
    return da*da + dr*dr <= rc*rc;
}

bool Barril::danifica(int d){
    resistencia -=d;
    return resistencia <=0;
}

GLdouble Barril::intersecaoRaio(Vec3 o, Vec3 d) const {
    GLdouble r = escala * cfg.geometriaBase.barril.diametro / 2.0;
    GLdouble m = escala * cfg.geometriaBase.barril.altura / 2.0;
    GLdouble t = RaioCilindro(o, d, Vec3{gPos_x, gPos_y, r}, Vec3{1, 0, 0}, r, m);
    if (inimigo) t = fmin(t, inimigo->intersecaoRaio(o, d));
    return t;
}
