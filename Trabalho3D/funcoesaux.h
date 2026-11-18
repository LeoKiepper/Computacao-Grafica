#ifndef FUNCOESAUX_H
#define FUNCOESAUX_H

#include <GL/gl.h>
#include "config_gerado.h"

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
Vec2 operator*(const Vec2& a, GLdouble c);

struct Vec3{
    GLdouble x, y, z;
};
Vec3 operator+(const Vec3& a, const Vec3& b);
Vec3 operator-(const Vec3& a, const Vec3& b);
Vec3 operator*(const Vec3& a, GLdouble c);
GLdouble Norma(Vec3 v);
Vec3 Normaliza(Vec3 v);
Vec3 RotZ(Vec3 v, GLdouble graus);
Vec3 RotY(Vec3 v, GLdouble graus);
GLdouble Dot(Vec3 a, Vec3 b);
Vec3 Cross(Vec3 a, Vec3 b);
int RecortaPoligono(const Vec3 in[], int n, Vec3 normal, GLdouble limite, Vec3 out[]);
bool CirculoTocaPoligonoConvexo(const Vec2 V[], int n, GLdouble r);
bool EsferaTocaPrisma(Vec3 P, Vec3 t, GLdouble r,
                      Vec3 O, Vec3 ex, Vec3 ey, Vec3 ez,
                      GLdouble ax, GLdouble ay, GLdouble c);


inline void AplicaCor(const Cor& c){ glColor3f(c.R, c.G, c.B); }

struct Prisma { Vec3 O, ex, ey, ez; GLdouble ax, ay, c; };

GLdouble RaioEsfera(Vec3 o, Vec3 d, Vec3 C, GLdouble r);
GLdouble RaioPrisma(Vec3 o, Vec3 d, Vec3 O, Vec3 ex, Vec3 ey, Vec3 ez, GLdouble ax, GLdouble ay, GLdouble c);
GLdouble RaioCilindro(Vec3 o, Vec3 d, Vec3 C, Vec3 e, GLdouble r, GLdouble m);

#endif /* FUNCOESAUX_H */
