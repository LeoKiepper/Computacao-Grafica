#ifndef ALVO_H
#define	ALVO_H
#include <GL/gl.h>
#include <GL/glu.h>
#include "tiro.h"

#define radiusAlvo 20

static const GLfloat kCoresAlvo[][3] = {
    {1.0f, 0.0f, 0.0f},   // red
    {0.0f, 1.0f, 0.0f},   // green
    {0.0f, 0.0f, 1.0f},   // blue
};
static const int kNumCoresAlvo = sizeof(kCoresAlvo) / sizeof(kCoresAlvo[0]);

class Alvo {
    GLfloat gX; 
    GLfloat gY;
    int gColor;
private:
    void DesenhaCirc(GLint radius, GLfloat R, GLfloat G, GLfloat B);
    void DesenhaAlvo(GLfloat x, GLfloat y);
public:
    Alvo(GLfloat x, GLfloat y){
        gX = x; 
        gY = y;
        gColor = 0;
    };
    void Desenha(){ 
        DesenhaAlvo(gX, gY);
    };
    void Recria(GLfloat x, GLfloat y);
    bool Atingido(Tiro *tiro);


};

#endif	/* ALVO_H */

