#include "tiro.h"
#include <math.h>
#define DISTANCIA_MAX 500

void Tiro::DesenhaCirc(GLdouble radius, GLfloat R, GLfloat G, GLfloat B)
{
    int NumPontos = 10;
    glColor3f(R, G, B);
    glBegin(GL_POLYGON);
        for(int pp=0; pp<NumPontos; pp++){
            glVertex3f(radius*cos(2*M_PI/NumPontos*pp),radius*sin(2*M_PI/NumPontos*pp),0);
        }
    glEnd();
}

void Tiro::DesenhaTiro(GLdouble x, GLdouble y)
{
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
        glTranslatef(x, y, 0);
        DesenhaCirc(radiusTiro, 1, 1, 1);
    glPopMatrix();
}

void Tiro::Move(GLdouble dt)
{
    gX += cos(gDirectionAng)*gVel*dt;
    gY += sin(gDirectionAng)*gVel*dt;
}

bool Tiro::Valido()
{
    GLfloat DeltaX = gX - gXInit;
    GLfloat DeltaY = gY - gYInit;
    if((DeltaX*DeltaX + DeltaY*DeltaY) >= (DISTANCIA_MAX*DISTANCIA_MAX)){
        return 0;
    }
    return 1;
}
