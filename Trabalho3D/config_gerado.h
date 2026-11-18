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
        struct tiro_t {
            double tirosJogadorSimultaneosPermitidos{};
            double tirosInimigosSimultaneosPerimtidos{};
            double distanciaMax{};
        } tiro;
        struct barril_t {
            double tempoEntreBarris_ms{};
            double tempoPausaDestroi_ms{};
            double maximoSimultaneo{};
            double resistenciaMax{};
            double resistenciaMin{};
        } barril;
        struct controles_t {
            double sensibilidadeMouseArma{};
            double fatorSensCam3{};
        } controles;
        struct tamJanela_t {
            double altura{};
            double largura{};
        } tamJanela;
        struct reinicio_t {
            double tempoSegurar_ms{};
        } reinicio;
        struct pontos_t {
            double barril{};
            double atirador{};
            double paraGanhar{};
        } pontos;
        struct visaoOponente_t {
            double altura{};
        } visaoOponente;
    } jogo;
    struct geometriaBase_t {
        struct jogadores_t {
            double raioCabeca{};
            double larguraArma{};
            double alturaArma{};
            double comprimentoArma{};
            double posyArma{};
            double comprimentoPernas{};
            double larguraPernas{};
        } jogadores;
        struct barril_t {
            double altura{};
            double diametro{};
        } barril;
        struct miraLaser_t {
            double diametro{};
            double comprimento{};
            double sobressalto{};
        } miraLaser;
    } geometriaBase;
    struct geometria_t {
        struct jogadores_t {
            double raioCabeca{};
        } jogadores;
        struct tiro_t {
            double raio{};
        } tiro;
        struct arena_t {
            double comprimento{};
            double largura{};
            double alturaParede{};
            double tamMalha{};
        } arena;
        struct barril_t {
            double fatias{};
            double stacks{};
        } barril;
        struct miraLaser_t {
            double fatias{};
        } miraLaser;
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
    struct cor_t {
        Cor arena;
        Cor arma;
        Cor miraLaser;
        Cor cabecaJogador;
        Cor cabecaInimigo;
        Cor pernas;
        Cor barril;
        Cor tampaBarril;
        Cor textoBarril;
        Cor textoBarrilNoturno;
        Cor chao;
        Cor paredes;
        Cor tiro;
        Cor linhaMeioArena;
        Cor linhaBordaArena;
        Cor octaedroLuzArena;
        Cor laser;
        Cor minimapaJogador;
        Cor minimapaInimigo;
        Cor minimapaBarril;
        Cor reinicioFundo;
        Cor reinicioProgresso;
        Cor fundoNoturno;
        Cor divisoria;
    } cor;
    struct linha_t {
        struct meioArena_t {
            double espessura{};
        } meioArena;
        struct bordaArena_t {
            double espessura{};
        } bordaArena;
        struct laser_t {
            double espessura{};
        } laser;
        struct divisoria_t {
            double espessura{};
        } divisoria;
    } linha;
    struct luz_t {
        struct arena_t {
            double tamanhoOctaedro{};
            double altura{};
            double offset{};
            double difusa{};
            double expoenteSpot{};
            double atenuacaoConstante{};
            double atenuacaoLinear{};
            double atenuacaoQuadratica{};
        } arena;
        struct global_t {
            double ambiente{};
            double ambienteNoturno{};
        } global;
        struct lanterna_t {
            double difusa{};
            double corte_deg{};
            double expoente{};
            double atenuacaoConstante{};
            double atenuacaoLinear{};
            double atenuacaoQuadratica{};
        } lanterna;
    } luz;
    struct camera_t {
        struct geral_t {
            double FOVy_deg{};
            double near{};
        } geral;
        struct cam1_t {
            double alturaOlho_p100AlturaCorpo{};
        } cam1;
        struct cam2_t {
            double alturaOlho{};
        } cam2;
        struct cam3_t {
            double raioInicial{};
            double raioMin{};
            double raioMax{};
            double passoZoom{};
            double sensibilidadeMouse{};
            double yawInicial_deg{};
            double pitchInicial_deg{};
            double pitchMin_deg{};
            double pitchMax_deg{};
        } cam3;
    } camera;
    struct hud_t {
        struct laser_t {
            double raioPonto{};
        } laser;
        struct minimapa_t {
            double fracao{};
        } minimapa;
        struct reinicio_t {
            double raio{};
        } reinicio;
    } hud;
    struct textura_t {
        struct arquivo_t {
            std::string chao{};
            std::string paredes{};
            std::string barril{};
            std::string tampaBarril{};
            std::string corpo{};
            std::string pernas{};
            std::string arma{};
            std::string tiro{};
        } arquivo;
        struct arena_t {
            double ladrilho{};
        } arena;
    } textura;
};

#endif
