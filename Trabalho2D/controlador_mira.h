#ifndef CONTROLADOR_MIRA_H
#define CONTROLADOR_MIRA_H

#include "atirador.h"
#include <functional>

class Atirador;
class ControladorMira{
    protected:
        Atirador* ego;
        const Atirador* alvo;
    public:
        ControladorMira(Atirador* ego, const Atirador* alvo)
            : ego(ego), alvo(alvo) {}
        virtual ~ControladorMira() {}
        // o núcleo é o método
        virtual void atua(double dt) = 0;
};

class RastreadorOrdemZero : public ControladorMira{
    public:
        using ControladorMira::ControladorMira;
        void atua(double dt) override;
};


#endif /* CONTROLADOR_MIRA_H */
