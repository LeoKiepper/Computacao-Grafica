#ifndef TIRO_H
#define TIRO_H
#include <GL/gl.h>
#include <GL/glu.h>
#include "funcoesaux.h"
#include "config.h"
#include <math.h>

class Tiro {
    const Config& cfg;
    Vec3 gPosInit;
    Vec3 gPos;
    Vec3 gDir;
    GLdouble gVel;
public:
    Tiro(const Config& cfg, Vec3 p0, Vec3 dir, GLdouble vel)
        : cfg(cfg), gPosInit(p0), gPos(p0), gDir(dir), gVel(vel) {}
    void Desenha();
    void Move(GLdouble dt);
    bool Valido();
    Vec3 GetPos() const { return gPos; }
    Vec3 GetDir() const { return gDir; }
    GLdouble GetRaio() const { return cfg.geometria.tiro.raio; }
};

#endif /* TIRO_H */
