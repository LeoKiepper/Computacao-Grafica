// GERADO a partir de configuracoes.xml por gerador_config. Nao edite.
// Usado apenas por config.cpp para preencher Config.
#ifndef CONFIG_CAMPOS_H
#define CONFIG_CAMPOS_H
#include "config_gerado.h"

template <class V> void campos(Cor& s, V& v) { v("R", s.R); v("G", s.G); v("B", s.B); }
template <class V> void campos(Config& s, V& v) { v("jogo", s.jogo); v("geometriaBase", s.geometriaBase); v("geometria", s.geometria); v("cinematica", s.cinematica); v("cor", s.cor); v("linha", s.linha); v("luz", s.luz); v("camera", s.camera); v("hud", s.hud); v("textura", s.textura); }
template <class V> void campos(Config::jogo_t& s, V& v) { v("inimigo", s.inimigo); v("tiro", s.tiro); v("barril", s.barril); v("controles", s.controles); v("tamJanela", s.tamJanela); v("reinicio", s.reinicio); v("pontos", s.pontos); v("visaoOponente", s.visaoOponente); }
template <class V> void campos(Config::jogo_t::inimigo_t& s, V& v) { v("tirosPorSegundo", s.tirosPorSegundo); v("atrasoPrimeiroTiro", s.atrasoPrimeiroTiro); }
template <class V> void campos(Config::jogo_t::tiro_t& s, V& v) { v("tirosJogadorSimultaneosPermitidos", s.tirosJogadorSimultaneosPermitidos); v("tirosInimigosSimultaneosPerimtidos", s.tirosInimigosSimultaneosPerimtidos); v("distanciaMax", s.distanciaMax); }
template <class V> void campos(Config::jogo_t::barril_t& s, V& v) { v("tempoEntreBarris_ms", s.tempoEntreBarris_ms); v("tempoPausaDestroi_ms", s.tempoPausaDestroi_ms); v("maximoSimultaneo", s.maximoSimultaneo); v("resistenciaMax", s.resistenciaMax); v("resistenciaMin", s.resistenciaMin); }
template <class V> void campos(Config::jogo_t::controles_t& s, V& v) { v("sensibilidadeMouseArma", s.sensibilidadeMouseArma); v("fatorSensCam3", s.fatorSensCam3); }
template <class V> void campos(Config::jogo_t::tamJanela_t& s, V& v) { v("altura", s.altura); v("largura", s.largura); }
template <class V> void campos(Config::jogo_t::reinicio_t& s, V& v) { v("tempoSegurar_ms", s.tempoSegurar_ms); }
template <class V> void campos(Config::jogo_t::pontos_t& s, V& v) { v("barril", s.barril); v("atirador", s.atirador); v("paraGanhar", s.paraGanhar); }
template <class V> void campos(Config::jogo_t::visaoOponente_t& s, V& v) { v("altura", s.altura); }
template <class V> void campos(Config::geometriaBase_t& s, V& v) { v("jogadores", s.jogadores); v("barril", s.barril); v("miraLaser", s.miraLaser); }
template <class V> void campos(Config::geometriaBase_t::jogadores_t& s, V& v) { v("raioCabeca", s.raioCabeca); v("larguraArma", s.larguraArma); v("alturaArma", s.alturaArma); v("comprimentoArma", s.comprimentoArma); v("posyArma", s.posyArma); v("comprimentoPernas", s.comprimentoPernas); v("larguraPernas", s.larguraPernas); }
template <class V> void campos(Config::geometriaBase_t::barril_t& s, V& v) { v("altura", s.altura); v("diametro", s.diametro); }
template <class V> void campos(Config::geometriaBase_t::miraLaser_t& s, V& v) { v("diametro", s.diametro); v("comprimento", s.comprimento); v("sobressalto", s.sobressalto); }
template <class V> void campos(Config::geometria_t& s, V& v) { v("jogadores", s.jogadores); v("tiro", s.tiro); v("arena", s.arena); v("barril", s.barril); v("miraLaser", s.miraLaser); }
template <class V> void campos(Config::geometria_t::jogadores_t& s, V& v) { v("raioCabeca", s.raioCabeca); }
template <class V> void campos(Config::geometria_t::tiro_t& s, V& v) { v("raio", s.raio); }
template <class V> void campos(Config::geometria_t::arena_t& s, V& v) { v("comprimento", s.comprimento); v("largura", s.largura); v("alturaParede", s.alturaParede); v("tamMalha", s.tamMalha); }
template <class V> void campos(Config::geometria_t::barril_t& s, V& v) { v("fatias", s.fatias); v("stacks", s.stacks); }
template <class V> void campos(Config::geometria_t::miraLaser_t& s, V& v) { v("fatias", s.fatias); }
template <class V> void campos(Config::cinematica_t& s, V& v) { v("jogador", s.jogador); v("tiros", s.tiros); v("barril", s.barril); }
template <class V> void campos(Config::cinematica_t::jogador_t& s, V& v) { v("velocidadeMovimento", s.velocidadeMovimento); v("velocidadeGiro", s.velocidadeGiro); v("velocidadeMira", s.velocidadeMira); v("aberturaAngular", s.aberturaAngular); }
template <class V> void campos(Config::cinematica_t::tiros_t& s, V& v) { v("velocidadeTiro", s.velocidadeTiro); }
template <class V> void campos(Config::cinematica_t::barril_t& s, V& v) { v("velocidade", s.velocidade); }
template <class V> void campos(Config::cor_t& s, V& v) { v("arena", s.arena); v("arma", s.arma); v("miraLaser", s.miraLaser); v("cabecaJogador", s.cabecaJogador); v("cabecaInimigo", s.cabecaInimigo); v("pernas", s.pernas); v("barril", s.barril); v("tampaBarril", s.tampaBarril); v("textoBarril", s.textoBarril); v("textoBarrilNoturno", s.textoBarrilNoturno); v("chao", s.chao); v("paredes", s.paredes); v("tiro", s.tiro); v("linhaMeioArena", s.linhaMeioArena); v("linhaBordaArena", s.linhaBordaArena); v("octaedroLuzArena", s.octaedroLuzArena); v("laser", s.laser); v("minimapaJogador", s.minimapaJogador); v("minimapaInimigo", s.minimapaInimigo); v("minimapaBarril", s.minimapaBarril); v("reinicioFundo", s.reinicioFundo); v("reinicioProgresso", s.reinicioProgresso); v("fundoNoturno", s.fundoNoturno); v("divisoria", s.divisoria); }
template <class V> void campos(Config::linha_t& s, V& v) { v("meioArena", s.meioArena); v("bordaArena", s.bordaArena); v("laser", s.laser); v("divisoria", s.divisoria); }
template <class V> void campos(Config::linha_t::meioArena_t& s, V& v) { v("espessura", s.espessura); }
template <class V> void campos(Config::linha_t::bordaArena_t& s, V& v) { v("espessura", s.espessura); }
template <class V> void campos(Config::linha_t::laser_t& s, V& v) { v("espessura", s.espessura); }
template <class V> void campos(Config::linha_t::divisoria_t& s, V& v) { v("espessura", s.espessura); }
template <class V> void campos(Config::luz_t& s, V& v) { v("arena", s.arena); v("global", s.global); v("lanterna", s.lanterna); }
template <class V> void campos(Config::luz_t::arena_t& s, V& v) { v("tamanhoOctaedro", s.tamanhoOctaedro); v("altura", s.altura); v("offset", s.offset); v("difusa", s.difusa); v("expoenteSpot", s.expoenteSpot); v("atenuacaoConstante", s.atenuacaoConstante); v("atenuacaoLinear", s.atenuacaoLinear); v("atenuacaoQuadratica", s.atenuacaoQuadratica); }
template <class V> void campos(Config::luz_t::global_t& s, V& v) { v("ambiente", s.ambiente); v("ambienteNoturno", s.ambienteNoturno); }
template <class V> void campos(Config::luz_t::lanterna_t& s, V& v) { v("difusa", s.difusa); v("corte_deg", s.corte_deg); v("expoente", s.expoente); v("atenuacaoConstante", s.atenuacaoConstante); v("atenuacaoLinear", s.atenuacaoLinear); v("atenuacaoQuadratica", s.atenuacaoQuadratica); }
template <class V> void campos(Config::camera_t& s, V& v) { v("geral", s.geral); v("cam1", s.cam1); v("cam2", s.cam2); v("cam3", s.cam3); }
template <class V> void campos(Config::camera_t::geral_t& s, V& v) { v("FOVy_deg", s.FOVy_deg); v("near", s.near); }
template <class V> void campos(Config::camera_t::cam1_t& s, V& v) { v("alturaOlho_p100AlturaCorpo", s.alturaOlho_p100AlturaCorpo); }
template <class V> void campos(Config::camera_t::cam2_t& s, V& v) { v("alturaOlho", s.alturaOlho); }
template <class V> void campos(Config::camera_t::cam3_t& s, V& v) { v("raioInicial", s.raioInicial); v("raioMin", s.raioMin); v("raioMax", s.raioMax); v("passoZoom", s.passoZoom); v("sensibilidadeMouse", s.sensibilidadeMouse); v("yawInicial_deg", s.yawInicial_deg); v("pitchInicial_deg", s.pitchInicial_deg); v("pitchMin_deg", s.pitchMin_deg); v("pitchMax_deg", s.pitchMax_deg); }
template <class V> void campos(Config::hud_t& s, V& v) { v("laser", s.laser); v("minimapa", s.minimapa); v("reinicio", s.reinicio); }
template <class V> void campos(Config::hud_t::laser_t& s, V& v) { v("raioPonto", s.raioPonto); }
template <class V> void campos(Config::hud_t::minimapa_t& s, V& v) { v("fracao", s.fracao); }
template <class V> void campos(Config::hud_t::reinicio_t& s, V& v) { v("raio", s.raio); }
template <class V> void campos(Config::textura_t& s, V& v) { v("arquivo", s.arquivo); v("arena", s.arena); }
template <class V> void campos(Config::textura_t::arquivo_t& s, V& v) { v("chao", s.chao); v("paredes", s.paredes); v("barril", s.barril); v("tampaBarril", s.tampaBarril); v("corpo", s.corpo); v("pernas", s.pernas); v("arma", s.arma); v("tiro", s.tiro); }
template <class V> void campos(Config::textura_t::arena_t& s, V& v) { v("ladrilho", s.ladrilho); }

#endif
