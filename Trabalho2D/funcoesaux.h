#ifndef FUNCOESAUX_H
#define	FUNCOESAUX_H

#include <GL/gl.h>

struct Limites {
    GLdouble xMin, xMax, yMin, yMax;

    GLdouble centroX() const { return (xMin + xMax) / 2.0; }
    GLdouble centroY() const { return (yMin + yMax) / 2.0; }

    void limita(GLdouble& x, GLdouble& y) const {
        if (x < xMin) x = xMin;
        if (x > xMax) x = xMax;
        if (y < yMin) y = yMin;
        if (y > yMax) y = yMax;
    }
};
struct Vec2{
    GLdouble x;
    GLdouble y;
    GLdouble w;
};
Vec2 MultiplicaMatrizEsq2d(const GLdouble M[3][3], Vec2 p);
GLdouble Norma(Vec2 v);
Vec2 Normaliza(Vec2 v);
Vec2 operator+(const Vec2& a, const Vec2& b);
Vec2 operator-(const Vec2& a, const Vec2& b);
Vec2 operator-(const Vec2& a);


#endif /* FUNCOESAUX_H */