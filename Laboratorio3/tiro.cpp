#include "tiro.h"
#include <math.h>
#define DISTANCIA_MAX 500

void Tiro::DesenhaCirc(GLint radius, GLfloat R, GLfloat G, GLfloat B)
{
    int NumPontos = 10;
    glColor3f(R, G, B);
    glBegin(GL_POLYGON);
        for(int pp=0; pp<NumPontos; pp++){
            glVertex3f(radius*cos(2*M_PI/NumPontos*pp),radius*sin(2*M_PI/NumPontos*pp),0);
        }
    glEnd();
}

void Tiro::DesenhaTiro(GLfloat x, GLfloat y)
{
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glTranslatef(x,y,0);
    DesenhaCirc(radiusTiro,1,1,1);
}

void Tiro::Move(GLdouble timeDiference)
{
    gX += cos(gDirectionAng)*gVel*timeDiference;
    gY += sin(gDirectionAng)*gVel*timeDiference;
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
