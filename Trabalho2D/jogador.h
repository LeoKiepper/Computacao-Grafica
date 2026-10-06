#ifndef JOGADOR_H
#define	JOGADOR_H
#include <GL/gl.h>
#include "config.h"
#include <math.h>
#include "funcoesaux.h"
#include <vector>
#include "tiro.h"

typedef void (*ColisaoArena)(GLdouble& x, GLdouble& y);

class Jogador{
    private:
        const Config& cfg;
        ColisaoArena limiteArena = NULL;
        GLdouble gPos_x;
        GLdouble gPos_y;
        GLdouble gTheta_jogador;
        GLdouble gTheta_arma;
        int perna_empurrando = 0;
        GLdouble delta_perna_esq = 0;
        GLdouble delta_perna_dir = 0;
        GLdouble dist_perna = 0;
        GLdouble escala = 1;
        
    template <class Cor>
    void DesenhaCirc(GLdouble radius, 
        const Cor& cor);
    template <class Cor>
    void DesenhaRect(GLdouble height, GLdouble width, 
        const Cor& cor);


    public:
        Jogador(const Config& cfg, ColisaoArena limiteArena, GLdouble x0, GLdouble y0)
            : cfg(cfg), limiteArena(limiteArena), gPos_x(x0), gPos_y(y0),
            gTheta_jogador(90), gTheta_arma(90) {
                this->escala = cfg.geometria.jogadores.raioCabeca / cfg.geometriaBase.jogadores.raioCabeca;
            }
        void Desenha();
        void Anda(GLdouble dt);
        void Gira(GLdouble dt);
        void Mira(GLdouble dt);
        void Atira(std::vector<Tiro*> &tiros);
        GLdouble posX(){return gPos_x;};
        GLdouble posY(){return gPos_y;};
        Vec2 p0arma();
        Vec2 p1arma();
};

#endif /* JOGADOR_H */
