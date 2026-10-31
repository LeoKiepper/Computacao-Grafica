#include "controlador_mira.h"
#include <cmath>

// Leva um ângulo em graus ao intervalo [-180, 180)
static double normalizaAngulo(double a){
    a = fmod(a + 180.0, 360.0);
    if (a < 0) a += 360.0;
    return a - 180.0;
}

void RastreadorOrdemZero::atua(double dt){
    GLdouble ax, ay;
    alvo->GetPos(ax, ay);
    Vec2 p0 = ego->p0arma();
    double desejado = atan2(ay - p0.y, ax - p0.x) * 180.0 / M_PI;
    double erro = normalizaAngulo(desejado - ego->thetaArmaMundo());
    ego->GiraArma(erro, dt);
}
