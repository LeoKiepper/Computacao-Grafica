#include "eventos.h"

Evento* Evento::desanexa(){
    Evento* p = proximo;
    proximo = NULL;
    return p;
}
void Evento::encadeia(Evento* p){
    proximo = p;
}
void FilaDeEventos::agenda(double instante, std::function<void()> acao){
    Evento* novo = new Evento(instante, acao);

    if (!cabeca || instante < cabeca->quando()) {
        novo->encadeia(cabeca);
        cabeca = novo;
        return;
    }

    Evento* p = cabeca;
    while (p->seguinte() && p->seguinte()->quando() <= instante) p = p->seguinte();
    novo->encadeia(p->seguinte());
    p->encadeia(novo);
}
void FilaDeEventos::avanca(double dt){
    if (pausaRestante_ms > 0) {
        if (dt <= pausaRestante_ms) { pausaRestante_ms -= dt; return; }
        dt -= pausaRestante_ms;
        pausaRestante_ms = 0;
    }
    relogio_ms += dt;

    while (cabeca && !pausada() && cabeca->quando() <= relogio_ms) {
        Evento* e = cabeca;
        cabeca = e->desanexa();
        e->executa();
        delete e;
    }
}
void FilaDeEventos::limpa(){
    while (cabeca) {
        Evento* p = cabeca->desanexa();
        delete cabeca;
        cabeca = p;
    }
    relogio_ms = 0;
    pausaRestante_ms = 0;
}
FilaDeEventos::~FilaDeEventos(){ limpa();}
