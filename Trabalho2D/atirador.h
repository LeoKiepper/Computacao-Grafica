#ifndef ATIRADOR_H
#define ATIRADOR_H
#include <GL/gl.h>
#include "config.h"
#include <math.h>
#include "funcoesaux.h"
#include <vector>
#include "tiro.h"

class ControladorMira;
class Atirador{
    public:
        using ColisaoArena = void (*)(GLdouble& x, GLdouble& y);
    private:
        const Config& cfg;
        ColisaoArena limiteArena = NULL;
        GLdouble gPos_x;
        GLdouble gPos_y;
        GLdouble gVel;
        GLdouble gTheta_jogador;
        GLdouble gTheta_arma;
        int perna_empurrando = 0;
        GLdouble delta_perna_esq = 0;
        GLdouble delta_perna_dir = 0;
        GLdouble distMarcha = 0;
        GLdouble escala = 1;
        GLdouble fatorVelAnimPernas;
        Cor corCabeca;
        ControladorMira* controlador = NULL;
        Vec2 pivoParaMundo(Vec2 p);
    void DesenhaCirc(GLdouble radius,
        const Cor& cor);
    void DesenhaRect(GLdouble height, GLdouble width,
        const Cor& cor);


    public:
        Atirador(const Config& cfg, ColisaoArena limiteArena, GLdouble x0, GLdouble y0, GLdouble vel, GLdouble Direcao0, GLdouble fatorVelAnimPernas, Cor corCabeca)
            : cfg(cfg), limiteArena(limiteArena),
            gPos_x(x0), gPos_y(y0), gVel(vel),
            gTheta_jogador(Direcao0), gTheta_arma(0),
            fatorVelAnimPernas(fatorVelAnimPernas), corCabeca(corCabeca) {
                this->escala = cfg.geometria.jogadores.raioCabeca / cfg.geometriaBase.jogadores.raioCabeca;
            }
        void Desenha();
        void Anda(GLdouble dt);
        void Gira(GLdouble dt);
        void GiraArma(GLdouble delta, GLdouble dt);
        void MiraJogador(GLdouble dx,GLdouble dt);
        void MiraInimigo(GLdouble dt);
        void Atira(std::vector<Tiro*> &tiros);
        void GetPos(GLdouble& x, GLdouble& y) const {x = gPos_x;y = gPos_y;};
        Vec2 p0arma();
        Vec2 p1arma();
        ~Atirador();
        void setControlador(ControladorMira* c){ controlador = c; }
        GLdouble thetaArmaMundo() const { return gTheta_jogador + gTheta_arma; }
};

#endif /* JOGADOR_H */
