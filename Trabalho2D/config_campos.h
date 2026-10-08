// GERADO a partir de configuracoes.xml por gerador_config. Nao edite.
// Usado apenas por config.cpp para preencher Config.
#ifndef CONFIG_CAMPOS_H
#define CONFIG_CAMPOS_H
#include "config_gerado.h"

template <class V> void campos(Config& s, V& v) { v("jogo", s.jogo); v("geometriaBase", s.geometriaBase); v("geometria", s.geometria); v("cinematica", s.cinematica); v("cores", s.cores); }
template <class V> void campos(Config::jogo_t& s, V& v) { v("inimigo", s.inimigo); v("tiros", s.tiros); v("barril", s.barril); v("outros", s.outros); }
template <class V> void campos(Config::jogo_t::inimigo_t& s, V& v) { v("tirosPorSegundo", s.tirosPorSegundo); }
template <class V> void campos(Config::jogo_t::tiros_t& s, V& v) { v("tirosJogadorSimultaneosPermitidos", s.tirosJogadorSimultaneosPermitidos); }
template <class V> void campos(Config::jogo_t::barril_t& s, V& v) { v("resistencia", s.resistencia); v("nParaGanhar", s.nParaGanhar); v("maximoSimultaneo", s.maximoSimultaneo); }
template <class V> void campos(Config::jogo_t::outros_t& s, V& v) { v("pensar", s.pensar); v("emAlgo", s.emAlgo); }
template <class V> void campos(Config::geometriaBase_t& s, V& v) { v("arena", s.arena); v("jogadores", s.jogadores); v("barril", s.barril); }
template <class V> void campos(Config::geometriaBase_t::arena_t& s, V& v) { v("altura", s.altura); v("largura", s.largura); }
template <class V> void campos(Config::geometriaBase_t::jogadores_t& s, V& v) { v("raioCabeca", s.raioCabeca); v("larguraArma", s.larguraArma); v("comprimentoArma", s.comprimentoArma); v("posyArma", s.posyArma); v("comprimentoPernas", s.comprimentoPernas); v("larguraPernas", s.larguraPernas); }
template <class V> void campos(Config::geometriaBase_t::barril_t& s, V& v) { v("altura", s.altura); v("diametro", s.diametro); }
template <class V> void campos(Config::geometria_t& s, V& v) { v("jogadores", s.jogadores); }
template <class V> void campos(Config::geometria_t::jogadores_t& s, V& v) { v("raioCabeca", s.raioCabeca); }
template <class V> void campos(Config::cinematica_t& s, V& v) { v("jogador", s.jogador); v("tiros", s.tiros); v("barril", s.barril); }
template <class V> void campos(Config::cinematica_t::jogador_t& s, V& v) { v("velocidadeMovimento", s.velocidadeMovimento); v("velocidadeGiro", s.velocidadeGiro); v("velocidadeMira", s.velocidadeMira); v("aberturaAngular", s.aberturaAngular); }
template <class V> void campos(Config::cinematica_t::tiros_t& s, V& v) { v("velocidadeTiro", s.velocidadeTiro); }
template <class V> void campos(Config::cinematica_t::barril_t& s, V& v) { v("velocidade", s.velocidade); }
template <class V> void campos(Config::cores_t& s, V& v) { v("arena", s.arena); v("arma", s.arma); v("cabecaJogador", s.cabecaJogador); v("cabecaInimigo", s.cabecaInimigo); v("pernas", s.pernas); v("barril", s.barril); }
template <class V> void campos(Config::cores_t::arena_t& s, V& v) { v("R", s.R); v("G", s.G); v("B", s.B); }
template <class V> void campos(Config::cores_t::arma_t& s, V& v) { v("R", s.R); v("G", s.G); v("B", s.B); }
template <class V> void campos(Config::cores_t::cabecaJogador_t& s, V& v) { v("R", s.R); v("G", s.G); v("B", s.B); }
template <class V> void campos(Config::cores_t::cabecaInimigo_t& s, V& v) { v("R", s.R); v("G", s.G); v("B", s.B); }
template <class V> void campos(Config::cores_t::pernas_t& s, V& v) { v("R", s.R); v("G", s.G); v("B", s.B); }
template <class V> void campos(Config::cores_t::barril_t& s, V& v) { v("R", s.R); v("G", s.G); v("B", s.B); }

#endif
