// GERADO a partir de configuracoes.xml por gerador_config. Nao edite.
#ifndef CONFIG_GERADO_H
#define CONFIG_GERADO_H
#include <string>

struct Config {
    struct jogo_t {
        struct inimigo_t {
            std::string tirosPorSegundo{};
        } inimigo;
        struct tiros_t {
            double tirosJogadorSimultaneosPermitidos{};
        } tiros;
        struct barril_t {
            double tempoMinimoEntreBarris_s{};
            double maximoSimultaneo{};
        } barril;
        struct outros_t {
            std::string pensar{};
            std::string emAlgo{};
        } outros;
    } jogo;
    struct geometriaBase_t {
        struct arena_t {
            double altura{};
            double largura{};
        } arena;
        struct jogadores_t {
            double raioCabeca{};
            double larguraArma{};
            double comprimentoArma{};
            double posyArma{};
            double comprimentoPernas{};
            double larguraPernas{};
        } jogadores;
        struct barril_t {
            double altura{};
            double diametro{};
        } barril;
    } geometriaBase;
    struct geometria_t {
        struct jogadores_t {
            double raioCabeca{};
        } jogadores;
    } geometria;
    struct cinematica_t {
        struct jogador_t {
            double velocidadeMovimento{};
            double velocidadeGiro{};
            double velocidadeMira{};
            double aberturaAngular{};
        } jogador;
        struct tiros_t {
            double velocidadeTiro{};
        } tiros;
        struct barril_t {
            double velocidade{};
        } barril;
    } cinematica;
    struct cores_t {
        struct arena_t {
            double R{};
            double G{};
            double B{};
        } arena;
        struct arma_t {
            double R{};
            double G{};
            double B{};
        } arma;
        struct cabecaJogador_t {
            double R{};
            double G{};
            double B{};
        } cabecaJogador;
        struct cabecaInimigo_t {
            double R{};
            double G{};
            double B{};
        } cabecaInimigo;
        struct pernas_t {
            double R{};
            double G{};
            double B{};
        } pernas;
        struct barril_t {
            double R{};
            double G{};
            double B{};
        } barril;
    } cores;
};

#endif
