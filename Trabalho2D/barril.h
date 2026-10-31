#ifndef BARRIL_H
#define BARRIL_H

#include "config.h"
#include "atirador.h"
#include <GL/gl.h>
#include <random>
#include "controlador_mira.h"

class Barril{
    public:
        using ColisaoArena = void (*)(GLdouble x, GLdouble y, size_t indice);
    private:
        const Config& cfg;
        ColisaoArena limiteArena = NULL;
        GLdouble gPos_x;
        GLdouble gPos_y;
        GLdouble escala = 1;
        Atirador* inimigo = NULL;
        size_t indice;
        int resistencia=0;
    public:
        Barril(const Config& cfg, ColisaoArena limiteArena, GLdouble x0, GLdouble y0, int bb, const Atirador* alvo)
            : cfg(cfg), limiteArena(limiteArena), gPos_x(x0), gPos_y(y0), indice(bb){
                this->escala = cfg.geometria.jogadores.raioCabeca / cfg.geometriaBase.jogadores.raioCabeca;

                Atirador::ColisaoArena semColisao = [](auto&...){};

                if(rand() % 2){ // Sorteia se esse barril vem com um inimigo, 50% de chance
                    // Cria um inimigo que mexe as pernas na mesma velocidade
                    // do barril, no sentido contrário ao movimento.
                    // A superfície de cima do barril se move com velocidade
                    // igual a 2 * velBarril com respeito ao SC_mundo.
                    // Para o inimigo ter uma velocidade resultante igual a
                    // + velBarril, também com respeito a SC_mundo, ele tem
                    // que se deslocar com respeito à superfície de cima do
                    // barril com velocidade igual a - velBarril
                    inimigo = new Atirador(cfg,semColisao,x0,y0,
                        cfg.cinematica.barril.velocidade,-90,
                        -1,
                        cfg.cores.cabecaInimigo);
                    inimigo->setControlador(new RastreadorOrdemZero(inimigo, alvo));
                }
                GLdouble t = (rand() / (double)RAND_MAX);
                static const int rMin = cfg.jogo.barril.resistenciaMin;
                static const int rMax = cfg.jogo.barril.resistenciaMax;
                resistencia = rMin + t * (rMax - rMin);
        };
        ~Barril(){ delete inimigo;};
        void DesenhaRect(GLdouble height, GLdouble width, const Cor& cor);
        void DesenhaResistencia();
        void Desenha();
        void Rola(GLdouble dt);
        bool colisaoComCirculo(GLdouble x, GLdouble y, GLdouble r);
        bool danifica(int d);
        Atirador* getInimigo(){return inimigo;}
        GLdouble posX(){return gPos_x;};
        GLdouble posY(){return gPos_y;};
};

#endif /* BARRIL_H */
