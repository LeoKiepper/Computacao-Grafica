#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#include <stdio.h>
#include "tinyxml.h"

// variáveis globais com a configuração
double arenaAltura, arenaLargura;
double jogadorRaio, jogadorVelocidade;
double inimigoRaio, inimigoTirosPorSegundo, inimigoVelocidadeTiro;
double barrilAltura, barrilLargura, barrilVelocidade;
int    barrilNumeroTiros, barrilNParaGanhar;

bool lerConfiguracoes(const char* caminho)
{
    TiXmlDocument doc(caminho);
    if (!doc.LoadFile()) {
        printf("Erro ao ler %s: %s\n", caminho, doc.ErrorDesc());
        return false;
    }

    TiXmlElement* jogo = doc.RootElement();            
    if (!jogo) return false;
    TiXmlElement* arena = jogo->FirstChildElement("arena");
    if (arena) {
        arena->QueryDoubleAttribute("altura",  &arenaAltura);
        arena->QueryDoubleAttribute("largura", &arenaLargura);
    }
    TiXmlElement* jogador = jogo->FirstChildElement("jogador");
    if (jogador) {
        jogador->QueryDoubleAttribute("raioCabeca", &jogadorRaio);
        jogador->QueryDoubleAttribute("velocidade", &jogadorVelocidade);
    }
    TiXmlElement* inimigo = jogo->FirstChildElement("inimigo");
    if (inimigo) {
        inimigo->QueryDoubleAttribute("raioCabeca",      &inimigoRaio);
        inimigo->QueryDoubleAttribute("tirosPorSegundo", &inimigoTirosPorSegundo);
        inimigo->QueryDoubleAttribute("velocidadeTiro",  &inimigoVelocidadeTiro);
    }
    TiXmlElement* barril = jogo->FirstChildElement("barril");
    if (barril) {
        barril->QueryDoubleAttribute("altura",      &barrilAltura);
        barril->QueryDoubleAttribute("largura",     &barrilLargura);
        barril->QueryIntAttribute   ("numeroTiros", &barrilNumeroTiros);
        barril->QueryIntAttribute   ("nParaGanhar", &barrilNParaGanhar);
        barril->QueryDoubleAttribute("velocidade",  &barrilVelocidade);
    }
    return true;
}
void init (void) 
{
  /* selecionar cor de fundo (preto) */
  glClearColor (0.0, 0.0, 0.0, 0.0);

  /* inicializar sistema de visualizacao */
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho(0.0, 1.0, 0.0, 1.0, -1.0, 1.0);
}

int main(int argc, char** argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode (GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize (arenaLargura, arenaAltura); 
    glutInitWindowPosition (100, 100);
    glutCreateWindow ("Barris");
    init ();
    // glutDisplayFunc(display); 
    // glutKeyboardFunc(keyPress);
    // glutKeyboardUpFunc(keyUp);
    // glutMotionFunc(passiveMotion);
    // glutIdleFunc(idle);
    // glutMouseFunc(mouse);
    // glutMainLoop();

    return 0;
}