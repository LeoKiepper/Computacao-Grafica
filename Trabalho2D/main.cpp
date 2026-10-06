#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#include <stdio.h>
#include "jogador.h"
#include "barril.h"
#include "config.h"
#include "tiro.h"
#include "funcoesaux.h"
#include <vector>

// Coloca em variáveis globais os parâmetros de configuração usados nesse módulo ====
#pragma region
    const Config cfg = lerConfiguracoes("configuracoes.xml");
    const GLfloat arenaAltura = cfg.geometriaBase.arena.altura; 
    const GLfloat arenaLargura = cfg.geometriaBase.arena.largura;
    const GLdouble raioJogador = cfg.geometria.jogadores.raioCabeca;
    Jogador* jogador = NULL;
    std::vector<Tiro*> tiros((int)cfg.jogo.tiros.tirosSimultaneosPermitidos, NULL);
    int keyStatus[256];

    float cursor_x=0, cursor_y=0;
    float last_cursor_x=0, last_cursor_y=0;

    const Limites limitesJogador = {
        -arenaLargura/2.0 + raioJogador,     // xMin
         arenaLargura/2.0 - raioJogador,     // xMax
        -arenaAltura/2.0  + raioJogador,     // yMin
                          - raioJogador      // yMax (linha do meio)
    };
    GLdouble x0_jogador(){return limitesJogador.centroX();};
    GLdouble y0_jogador(){return limitesJogador.yMin;};
#pragma endregion


void colisaoArena_jogador(GLdouble& x, GLdouble& y) {
    limitesJogador.limita(x, y);
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
void DesenhaLinhaDeTiro(Jogador jogador){
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


    for (size_t i = 0; i < tiros.size(); i++){
        if (tiros[i]) tiros[i]->Desenha();
    }

    //  alvo.Desenha();

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
    antes = agora;


    // Trata teclas
    if(keyStatus[(int)('w')]) jogador->Anda(dt);
    if(keyStatus[(int)('s')]) jogador->Anda(-dt);
    if(keyStatus[(int)('a')]) jogador->Gira(dt);
    if(keyStatus[(int)('d')]) jogador->Gira(-dt);
    


    // Trata mouse
    if(cursor_x-last_cursor_x < 0) jogador->Mira(dt);
    if(cursor_x-last_cursor_x > 0) jogador->Mira(-dt);


    //Trata os tiro
    for (size_t i = 0; i < tiros.size(); i++){
        if (tiros[i]) {
            tiros[i]->Move(dt);
            if (!tiros[i]->Valido()) { delete tiros[i]; tiros[i] = NULL; }
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
                jogador->Atira(tiros);
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
    glutPassiveMotionFunc(passiveMotion);   // Trata o cursor do mouse
    glutIdleFunc(idle);                     // Atualiza o estado do mundo
    glutMouseFunc(mouse);



    // Configurações iniciais do mundo
    jogador = new Jogador(cfg, colisaoArena_jogador,
        x0_jogador(), y0_jogador());
    ResetKeyStatus();

    // Loop principal ==========================================================
    glutMainLoop();

    return 0;
}