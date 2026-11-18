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
        v.y/norm,
        v.w
    };
    return ret;
}
Vec2 operator+(const Vec2& a, const Vec2& b) { return Vec2{a.x + b.x, a.y + b.y, a.w + b.w};}
Vec2 operator-(const Vec2& a, const Vec2& b) { return Vec2{a.x - b.x, a.y - b.y, a.w - b.w};}
Vec2 operator-(const Vec2& a)                { return Vec2{-a.x, -a.y, -a.w};               }
Vec2 operator*(const Vec2& a, GLdouble c)    { return Vec2{a.x * c, a.y * c, a.w * c};      }


Vec3 operator+(const Vec3& a, const Vec3& b) { return Vec3{a.x + b.x, a.y + b.y, a.z + b.z}; }
Vec3 operator-(const Vec3& a, const Vec3& b) { return Vec3{a.x - b.x, a.y - b.y, a.z - b.z}; }
Vec3 operator*(const Vec3& a, GLdouble c)    { return Vec3{a.x * c, a.y * c, a.z * c}; }
GLdouble Norma(Vec3 v){ return sqrt(v.x*v.x + v.y*v.y + v.z*v.z); }
Vec3 Normaliza(Vec3 v){ return v * (1.0 / Norma(v)); }
Vec3 RotZ(Vec3 v, GLdouble graus){
    GLdouble a = graus * M_PI / 180.0, c = cos(a), s = sin(a);
    return Vec3{v.x*c - v.y*s, v.x*s + v.y*c, v.z};
}
Vec3 RotY(Vec3 v, GLdouble graus){
    GLdouble a = graus * M_PI / 180.0, c = cos(a), s = sin(a);
    return Vec3{v.x*c + v.z*s, v.y, -v.x*s + v.z*c};
}
GLdouble Dot(Vec3 a, Vec3 b){ return a.x*b.x + a.y*b.y + a.z*b.z; }
Vec3 Cross(Vec3 a, Vec3 b){
    return Vec3{a.y*b.z - a.z*b.y, a.z*b.x - a.x*b.z, a.x*b.y - a.y*b.x};
}

int RecortaPoligono(const Vec3 in[], int n, Vec3 normal, GLdouble limite, Vec3 out[]){
    int k = 0;
    for (int i = 0; i < n; i++){
        Vec3 A = in[i], B = in[(i + 1) % n];
        GLdouble fa = Dot(A, normal) - limite;
        GLdouble fb = Dot(B, normal) - limite;
        if (fa <= 0) out[k++] = A;
        if ((fa < 0 && fb > 0) || (fa > 0 && fb < 0))
            out[k++] = A + (B - A) * (fa / (fa - fb));
    }
    return k;
}

bool CirculoTocaPoligonoConvexo(const Vec2 V[], int n, GLdouble r){
    bool temPositivo = false, temNegativo = false;
    for (int i = 0; i < n; i++){
        Vec2 a = V[i];
        Vec2 e = V[(i + 1) % n] - a;
        Vec2 ac = -a;
        GLdouble e2 = e.x*e.x + e.y*e.y;
        if (a.x*a.x + a.y*a.y <= r*r) return true;
        GLdouble s  = (ac.x*e.x + ac.y*e.y) / e2;
        GLdouble cr = e.x*ac.y - e.y*ac.x;
        if (s >= 0 && s <= 1 && cr*cr <= r*r*e2) return true;
        if (cr > 0) temPositivo = true;
        if (cr < 0) temNegativo = true;
    }
    return !(temPositivo && temNegativo);
}

bool EsferaTocaPrisma(Vec3 P, Vec3 t, GLdouble r,
                      Vec3 O, Vec3 ex, Vec3 ey, Vec3 ez,
                      GLdouble ax, GLdouble ay, GLdouble c){
    Vec3 b = Cross(ez, t);
    if (Norma(b) < 1e-9) return false;
    b = Normaliza(b);
    Vec3 n = Cross(t, b);
    GLdouble nz = Dot(n, ez);

    static const GLdouble sx[4] = {+1, -1, -1, +1};
    static const GLdouble sy[4] = {+1, +1, -1, -1};
    Vec3 quad[4];
    for (int i = 0; i < 4; i++){
        Vec3 E = O + ex*(sx[i]*ax) + ey*(sy[i]*ay) - P;
        quad[i] = E + ez*(-Dot(n, E) / nz);
    }

    GLdouble hOP = Dot(O - P, ez);
    Vec3 meio[6], secao[8];
    int k = RecortaPoligono(quad, 4, ez, hOP + c, meio);
    k = RecortaPoligono(meio, k, ez * -1.0, c - hOP, secao);
    if (k < 3) return false;

    Vec2 V[8];
    for (int i = 0; i < k; i++)
        V[i] = Vec2{Dot(secao[i], t), Dot(secao[i], b), 0};
    return CirculoTocaPoligonoConvexo(V, k, r);
}
static bool RecortaFaixa(GLdouble p, GLdouble d, GLdouble h, GLdouble& tn, GLdouble& tf){
    if (fabs(d) < 1e-12) return fabs(p) <= h;
    GLdouble t1 = (-h - p) / d, t2 = (h - p) / d;
    if (t1 > t2) std::swap(t1, t2);
    tn = fmax(tn, t1);
    tf = fmin(tf, t2);
    return tn <= tf;
}
static GLdouble Entrada(GLdouble tn, GLdouble tf){
    if (tf < 0) return INFINITY;
    return fmax(tn, 0.0);
}
GLdouble RaioEsfera(Vec3 o, Vec3 d, Vec3 C, GLdouble r){
    Vec3 p = o - C;
    GLdouble b = Dot(p, d), c = Dot(p, p) - r*r;
    GLdouble disc = b*b - c;
    if (disc < 0) return INFINITY;
    return Entrada(-b - sqrt(disc), -b + sqrt(disc));
}
GLdouble RaioPrisma(Vec3 o, Vec3 d, Vec3 O, Vec3 ex, Vec3 ey, Vec3 ez, GLdouble ax, GLdouble ay, GLdouble c){
    Vec3 p = o - O;
    GLdouble tn = -INFINITY, tf = INFINITY;
    if (!RecortaFaixa(Dot(p, ex), Dot(d, ex), ax, tn, tf)) return INFINITY;
    if (!RecortaFaixa(Dot(p, ey), Dot(d, ey), ay, tn, tf)) return INFINITY;
    if (!RecortaFaixa(Dot(p, ez), Dot(d, ez), c,  tn, tf)) return INFINITY;
    return Entrada(tn, tf);
}
GLdouble RaioCilindro(Vec3 o, Vec3 d, Vec3 C, Vec3 e, GLdouble r, GLdouble m){
    Vec3 p = o - C;
    GLdouble tn = -INFINITY, tf = INFINITY;
    if (!RecortaFaixa(Dot(p, e), Dot(d, e), m, tn, tf)) return INFINITY;
    Vec3 pr = p - e * Dot(p, e);
    Vec3 dr = d - e * Dot(d, e);
    GLdouble a = Dot(dr, dr), b = Dot(pr, dr), c = Dot(pr, pr) - r*r;
    if (a < 1e-12) return c <= 0 ? Entrada(tn, tf) : INFINITY;
    GLdouble disc = b*b - a*c;
    if (disc < 0) return INFINITY;
    tn = fmax(tn, (-b - sqrt(disc)) / a);
    tf = fmin(tf, (-b + sqrt(disc)) / a);
    if (tn > tf) return INFINITY;
    return Entrada(tn, tf);
}
