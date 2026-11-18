#include "controlador_mira.h"
#include <cmath>

// Leva um ângulo em graus ao intervalo [-180, 180)
static double normalizaAngulo(double a){
    a = fmod(a + 180.0, 360.0);
    if (a < 0) a += 360.0;
    return a - 180.0;
}

void RastreadorOrdemZero::atua(double dt){
    Vec3 alvoC = alvo->CentroCorpo();
    Vec3 p0 = ego->p0arma();
    GLdouble dx = alvoC.x - p0.x, dy = alvoC.y - p0.y, dz = alvoC.z - p0.z;

    double desejado = atan2(dy, dx) * 180.0 / M_PI;
    double erro = normalizaAngulo(desejado - ego->thetaArmaMundo());
    ego->GiraArma(erro, dt);

    double desejadoPhi = atan2(dz, sqrt(dx*dx + dy*dy)) * 180.0 / M_PI;
    ego->GiraArmaVertical(desejadoPhi - ego->phiArma(), dt);
}
