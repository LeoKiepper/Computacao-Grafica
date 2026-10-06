#include "funcoesaux.h"
#include <GL/gl.h>
#include <math.h>

Vec2 MultiplicaMatrizEsq2d(const GLdouble M[3][3], Vec2 p){
    Vec2 ret = {
        M[0][0]*p.x + M[0][1]*p.y + M[0][2]*p.w, 
        M[1][0]*p.x + M[1][1]*p.y + M[1][2]*p.w,
        M[2][0]*p.x + M[2][1]*p.y + M[2][2]*p.w,
    };
    return ret;
}
GLdouble Norma(Vec2 v){
    return sqrt(v.x*v.x + v.y*v.y);
}
Vec2 Normaliza(Vec2 v){
    GLdouble norm = Norma(v);
    Vec2 ret = {
        v.x/norm,
        v.y/norm
    };
    return ret;
}
Vec2 operator+(const Vec2& a, const Vec2& b) { return Vec2{a.x + b.x, a.y + b.y, a.w + b.w}; }
Vec2 operator-(const Vec2& a, const Vec2& b) { return Vec2{a.x - b.x, a.y - b.y, a.w - b.w}; }
Vec2 operator-(const Vec2& a)                { return Vec2{-a.x, -a.y, -a.w}; }


