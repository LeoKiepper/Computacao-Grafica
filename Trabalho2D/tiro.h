#ifndef TIRO_H
#define	TIRO_H
#include <GL/gl.h>
#include <GL/glu.h>
#include "funcoesaux.h"
#include <math.h>

#define radiusTiro 5

class Tiro {
    GLdouble gXInit; 
    GLdouble gYInit; 
    GLdouble gX; 
    GLdouble gY; 
    GLdouble gDirectionAng;
    GLdouble gVel;
private:
    void DesenhaCirc(GLdouble radius, GLfloat R, GLfloat G, GLfloat B);
    void DesenhaTiro(GLdouble x, GLdouble y);
public:
    Tiro(Vec2 p0, Vec2 dir, GLdouble vel){
        gXInit = p0.x; 
        gYInit = p0.y; 
        gX = gXInit; 
        gY = gYInit; 
        gDirectionAng = atan2(dir.y, dir.x); 
        gVel = vel;
    };
    void Desenha(){ 
        DesenhaTiro(gX, gY);
    };
    void Move(GLdouble dt);
    bool Valido();
    void GetPos(GLdouble &xOut, GLdouble &yOut){
        xOut = gX;
        yOut = gY;
    };
};


#endif	/* TIRO_H */

