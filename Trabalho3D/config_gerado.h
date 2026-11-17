// GERADO a partir de configuracoes.xml por gerador_config. Nao edite.
#ifndef CONFIG_GERADO_H
#define CONFIG_GERADO_H
#include <string>

struct Cor {
    double R{};
    double G{};
    double B{};
};

struct Config {
    struct jogo_t {
        struct inimigo_t {
            double tirosPorSegundo{};
            double atrasoPrimeiroTiro{};
        } inimigo;
        struct tiros_t {
            double tirosJogadorSimultaneosPermitidos{};
            double tirosInimigosSimultaneosPerimtidos{};
        } tiros;
        struct barril_t {
            double tempoEntreBarris_ms{};
            double tempoPausaDestroi_ms{};
            double maximoSimultaneo{};
            double resistenciaMax{};
            double resistenciaMin{};
        } barril;
        struct regras_t {
            double barrisParaDestruir{};
        } regras;
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
        Cor arena;
        Cor arma;
        Cor cabecaJogador;
        Cor cabecaInimigo;
        Cor pernas;
        Cor barril;
    } cores;
};

#endif
