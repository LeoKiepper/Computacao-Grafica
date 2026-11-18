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
        GLdouble gPos_z = 0;
        GLdouble gVel;
        GLdouble gTheta_jogador;
        GLdouble gTheta_arma;
        GLdouble gPhi_arma = 0;
        GLdouble zCentroCorpo() const {
            return escala * (cfg.geometriaBase.jogadores.comprimentoPernas
                        + cfg.geometriaBase.jogadores.raioCabeca);
        }
        GLdouble GiraLimitado(GLdouble atual, GLdouble delta, GLdouble dt);

        int perna_empurrando = 0;
        GLdouble delta_perna_esq = 0;
        GLdouble delta_perna_dir = 0;
        GLdouble distMarcha = 0;
        GLdouble escala = 1;
        GLdouble fatorVelAnimPernas;
        Cor corCabeca;
        ControladorMira* controlador = NULL;
        Vec2 pivoParaMundo(Vec2 p);

        void DesenhaEsfera(GLdouble raio, const Cor& cor);
        void DesenhaCaixa(GLdouble sx, GLdouble sy, GLdouble sz, const Cor& cor);

        Vec3 DirArmaLocal() const;
        void DefineDirArmaLocal(Vec3 d);
        void LimitaAoCone();
        void Prismas(Prisma p[3]) const;
        Vec3 armaParaMundo(Vec3 q) const;
        void DesenhaCilindro(GLdouble raio, GLdouble comprimento, const Cor& cor);

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
        void MiraJogador(GLdouble dx, GLdouble dy);
        void MiraInimigo(GLdouble dt);
        void Atira(std::vector<Tiro*> &tiros);
        void GetPos(GLdouble& x, GLdouble& y) const {x = gPos_x;y = gPos_y;};
        void SetPosZ(GLdouble z) { gPos_z = z; }
        GLdouble GetTheta() const { return gTheta_jogador; }
        Vec3 p0arma();
        Vec3 p1arma();
        Vec3 CentroCorpo() const { return Vec3{gPos_x, gPos_y, gPos_z + zCentroCorpo()}; }
        GLdouble phiArma() const { return gPhi_arma; }
        void GiraArmaVertical(GLdouble delta, GLdouble dt);
        bool colisaoComTiro(Vec3 P, Vec3 t, GLdouble rc) const;

        ~Atirador();
        void setControlador(ControladorMira* c){ controlador = c; }
        GLdouble thetaArmaMundo() const { return gTheta_jogador + gTheta_arma; }
        static GLdouble Altura(const Config& cfg) {
            const auto& b = cfg.geometriaBase.jogadores;
            return cfg.geometria.jogadores.raioCabeca / b.raioCabeca
                * (b.comprimentoPernas + 2*b.raioCabeca);
        }
        Vec3 OrigemLaser() const;
        Vec3 DirArma() const;
        GLdouble intersecaoRaio(Vec3 o, Vec3 d) const;


};

#endif /* JOGADOR_H */
