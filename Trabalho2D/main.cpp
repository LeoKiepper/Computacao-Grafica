#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#include <cstddef>
#include <stdio.h>
#include "atirador.h"
#include "barril.h"
#include "config.h"
#include "tiro.h"
#include "funcoesaux.h"
#include <vector>
#include <ctime>
#include <cstdlib>

// Coloca em variáveis globais os parâmetros de configuração usados nesse módulo ====
#pragma region
    const Config cfg = lerConfiguracoes("configuracoes.xml");
    const GLfloat arenaAltura = cfg.geometriaBase.arena.altura; 
    const GLfloat arenaLargura = cfg.geometriaBase.arena.largura;
    const GLdouble raioJogador = cfg.geometria.jogadores.raioCabeca;
    const GLdouble alturaBarril = cfg.geometriaBase.barril.altura;
    const GLdouble diametroBarril = cfg.geometriaBase.barril.diametro;
    Atirador* jogador = NULL;
    std::vector<Barril*> barris((int)cfg.jogo.barril.maximoSimultaneo, NULL);
    std::vector<bool> barrisForaDaArena((int)cfg.jogo.barril.maximoSimultaneo, false);
    
    // Barril* barril = NULL;
    // bool barrilForaDaArena=false;
    std::vector<Tiro*> tirosJogador((int)cfg.jogo.tiros.tirosJogadorSimultaneosPermitidos, NULL);
    int keyStatus[256];

    float cursor_x=0, cursor_y=0;
    float last_cursor_x=0, last_cursor_y=0;

    const Limites limitesJogador = {
        -arenaLargura/2.0 + raioJogador,     // xMin
         arenaLargura/2.0 - raioJogador,     // xMax
        -arenaAltura/2.0  + raioJogador,     // yMin
                          - raioJogador      // yMax (linha do meio)
    };
    const Limites limitesBarril = {
        -arenaLargura/2.0 + alturaBarril/2.0,   // xMin
         arenaLargura/2.0 - alturaBarril/2.0,   // xMax
        -arenaAltura/2.0  - diametroBarril/2.0, // yMin
         arenaAltura/2.0  + diametroBarril/2.0  // yMax
    };
    GLdouble x0_jogador(){return limitesJogador.centroX();};
    GLdouble y0_jogador(){return limitesJogador.yMin;};
    GLdouble x0_barril(){
        GLdouble t = (rand() / (double)RAND_MAX);
        static const GLdouble xMin = limitesBarril.xMin;
        static const GLdouble xMax = limitesBarril.xMax;
        return xMin + t * (xMax - xMin);
    }
    GLdouble y0_barril(){return limitesBarril.yMax-100;};
#pragma endregion


void colisaoArena_jogador(GLdouble& x, GLdouble& y) {
    limitesJogador.limita(x, y);
}
void colisaoArena_barril(GLdouble x, GLdouble y, int bb){
    if (y > limitesBarril.yMin) return;  // O barril ainda não chegou no seu yMin
    barrisForaDaArena[bb]=true;
}
void DesenhaLinhaDaMetade(){
    glPushAttrib(GL_LINE_BIT);
        glLineWidth(3.0);
        glEnable(GL_LINE_STIPPLE);
        glLineStipple(1, 0x00FF);
        glColor3f(0, 0, 0);
        glBegin(GL_LINES);
            glVertex3f(-arenaLargura/2.0, 0, 0);
            glVertex3f( arenaLargura/2.0, 0, 0);
        glEnd();
    glPopAttrib(); 
}
void DesenhaLinhaDeTiro(Atirador jogador){
    // glPushAttrib(GL_LINE_BIT);
    //     glLineWidth(3.0);
    //     glEnable(GL_LINE_STIPPLE);
    //     glLineStipple(1, 0x00FF);
    //     glColor3f(0, 0, 0);
    //     glBegin(GL_LINES);
    //         glVertex3f(-arenaLargura/2.0, 0, 0);
    //         glVertex3f( arenaLargura/2.0, 0, 0);
    //     glEnd();
    // glPopAttrib(); 
}
void ResetKeyStatus()
{
    int i;
    //Initialize keyStatus
    for(i = 0; i < 256; i++)
       keyStatus[i] = 0; 
}
void display(void)
{
    // Clear the screen.
    glClear(GL_COLOR_BUFFER_BIT);
    

    jogador->Desenha();


    for (size_t i = 0; i < tirosJogador.size(); i++){
        if (tirosJogador[i]) tirosJogador[i]->Desenha();
    }

    for(int bb=0; bb<cfg.jogo.barril.maximoSimultaneo; bb++)
        if(barris[bb]) barris[bb]->Desenha();

    //  ImprimePlacar(5-ViewingWidth/2,5-ViewingHeight/2);

    DesenhaLinhaDaMetade();

    glutSwapBuffers(); // Desenha the new frame of the game.
}
void passiveMotion(int x, int y)
{
    last_cursor_x = cursor_x;
    last_cursor_y = cursor_y;

    // Mapeia a posição do cursor para (0,0) sendo o centro da tela
    cursor_x = float(x) + cfg.geometriaBase.arena.largura/2.0;
    cursor_y = cfg.geometriaBase.arena.altura/2.0 - float(y);

    // if (button==0) {        // Botão esquerdo
    //     if (state==0){      // Botão apertado

    //     } else {

    //     }
    // }
    glutPostRedisplay();
}
void keyPress(unsigned char key, int x, int y)
{
    switch (key)
    {
        case 'a':
        case 'A':
            keyStatus[(int)('a')] = 1;
            break;
        case 'd':
        case 'D':
            keyStatus[(int)('d')] = 1;
            break;        
        case 'w':
        case 'W':
            keyStatus[(int)('w')] = 1;
            break;
        case 's':
        case 'S':
            keyStatus[(int)('s')] = 1;
            break;
        case 27 :
            exit(0);
    }
    glutPostRedisplay();
}
void keyUp(unsigned char key, int x, int y)
{
    keyStatus[(int)(key)] = 0;
    glutPostRedisplay();
}
void idle(void)
{
    // Declarar 'antes' como static faz com que ele seja 
    // 'global' (portanto sobrevive ao término da função),
    // mas escopada dentro da função idle, e sua inicialização
    // acontece só na primeira passada.
    static GLdouble antes = glutGet(GLUT_ELAPSED_TIME);
    GLdouble agora, dt;
    agora = glutGet(GLUT_ELAPSED_TIME);
    dt = agora - antes;
    if (dt==0) return; // Isso pode acontecer quando o cálculo é muito rápido
    antes = agora;


    // Trata teclas
    if(keyStatus[(int)('w')]) jogador->Anda(dt);
    if(keyStatus[(int)('s')]) jogador->Anda(-dt);
    if(keyStatus[(int)('a')]) jogador->Gira(dt);
    if(keyStatus[(int)('d')]) jogador->Gira(-dt);
    


    // Trata mouse
    jogador->Mira(cursor_x - last_cursor_x, dt);
    // if(cursor_x-last_cursor_x > 0) jogador->Mira(cursor_x - last_cursor_x, dt);

    // O barril pode ter sido destruído. A função de 
    // rolar tem que estar dentro do tratamento adequado
    for(int bb=0; bb<cfg.jogo.barril.maximoSimultaneo; bb++){
        if(barris[bb]){
            barris[bb]->Rola(dt);
            if(barrisForaDaArena[bb]){
                delete barris[bb];
                barris[bb]=NULL;
                barrisForaDaArena[bb]=false;
            }
        }
    }
    //Trata os tiros
    for (size_t i = 0; i < tirosJogador.size(); i++){
        if (tirosJogador[i]) {
            tirosJogador[i]->Move(dt);
            if (!tirosJogador[i]->Valido()) { delete tirosJogador[i]; tirosJogador[i] = NULL; }
        }
    }
    
    glutPostRedisplay();
}
void mouse(int button, int state, int x, int y){
    static bool apertado=false;
    if (button==0) {    // Botão esquerdo
        if (state==0){  // Botão apertado
            if(!apertado){
                apertado = true;
                jogador->Atira(tirosJogador);
            }
        } else {
            if(apertado) apertado = false;
        }
    }
    glutPostRedisplay();
}


int main(int argc, char** argv)
{


    // Configurações iniciais do glut ==========================================
    glutInit(&argc, argv);
    glutInitDisplayMode (GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize (arenaLargura, arenaAltura); 
    glutInitWindowPosition (100, 100);
    glutCreateWindow ("Barris e atiradores 2d");
    glClearColor (cfg.cores.arena.R, cfg.cores.arena.G, cfg.cores.arena.B, 1.0);

    glMatrixMode(GL_PROJECTION);   
    glOrtho(-arenaLargura/2, arenaLargura/2,   -arenaAltura/2.0, arenaAltura/2.0,   -100, 100);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();



    // Registra callbacks ======================================================
    glutDisplayFunc(display); 
    glutKeyboardFunc(keyPress);
    glutKeyboardUpFunc(keyUp);
    // Registra a função passiveMotion como passiva e ativa, para a lógica 
    // enquanto o mouse estiver apertado; Isso corrige um bug que fazia a arma
    // Ser jogada para um lado quando o jogador clicava.
    glutPassiveMotionFunc(passiveMotion);
    glutMotionFunc(passiveMotion);
    glutMouseFunc(mouse);                   // Trata os cliques do mouse

    glutIdleFunc(idle);                     // Atualiza o estado do mundo



    // Configurações iniciais do mundo
    srand((unsigned)time(NULL));    //Pega um seed "aleatório"

    jogador = new Atirador(cfg, colisaoArena_jogador,
        x0_jogador(), y0_jogador(), cfg.cinematica.jogador.velocidadeMovimento,
        90, 1);
    barris[0] = new Barril(cfg, colisaoArena_barril,
        x0_barril(),y0_barril(), 0);
    ResetKeyStatus();

    // Loop principal ==========================================================
    glutMainLoop();

    return 0;
}