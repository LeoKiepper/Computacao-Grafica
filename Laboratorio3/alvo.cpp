#include "alvo.h"
#include <math.h>

void Alvo::DesenhaCirc(GLint radius, GLfloat R, GLfloat G, GLfloat B)
{
    int NumPontos = 20;
    glColor3f(R, G, B);
    glBegin(GL_POLYGON);
        for(int pp=0; pp<NumPontos; pp++){
            glVertex3f(radius*cos(2*M_PI/NumPontos*pp),radius*sin(2*M_PI/NumPontos*pp),0);
        }
    glEnd();
}

void Alvo::DesenhaAlvo(GLfloat x, GLfloat y)
{
    glLoadIdentity();
    glTranslatef(x,y,0);
    float R = kCoresAlvo[gColor][0];
    float G = kCoresAlvo[gColor][1];
    float B = kCoresAlvo[gColor][2];
    Alvo::DesenhaCirc(radiusAlvo,R,G,B);
}

void Alvo::Recria(GLfloat x, GLfloat y)
{
    gX = x;
    gY = y;
    gColor = (gColor + 1);
    if(gColor > kNumCoresAlvo){
        gColor = 0;
    }
}

bool Alvo::Atingido(Tiro *tiro)
{
    GLfloat x, y;
    tiro->GetPos(x, y);
    GLfloat dx = x - gX;
    GLfloat dy = y - gY;
    return (dx*dx + dy*dy) <= (radiusAlvo*radiusAlvo);
}
