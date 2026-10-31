#ifndef EVENTOS_H
#define	EVENTOS_H

#include <functional>

class Evento{
    private:
        double instante_ms;
        std::function<void()> acao;
        Evento* proximo = NULL;

        Evento* desanexa();
        void encadeia(Evento* p);

        friend class FilaDeEventos;

    public:
        Evento(double instante, std::function<void()> acao)
            : instante_ms(instante), acao(acao) {}

        double quando() const { return instante_ms; }
        Evento* seguinte() const { return proximo; }
        void executa(){ acao(); }

};

class FilaDeEventos{
    private:
        Evento* cabeca = NULL;
        double relogio_ms = 0;
        double pausaRestante_ms = 0;

    public:
        ~FilaDeEventos();

        void agenda(double instante_ms, std::function<void()> acao);
        void agendaDaqui(double atraso_ms, std::function<void()> acao){
            agenda(relogio_ms + atraso_ms, acao);
        }
        void pausa(double duracao_ms){ pausaRestante_ms += duracao_ms; }
        void avanca(double dt_ms);
        double agora() const { return relogio_ms; }
        bool pausada() const { return pausaRestante_ms > 0; }
        bool vazia() const { return cabeca == NULL; }
        void limpa();
};

#endif /* EVENTOS_H */
