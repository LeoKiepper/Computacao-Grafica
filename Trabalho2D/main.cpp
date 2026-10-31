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
#include "eventos.h"
#include <vector>
#include <ctime>
#include <cstdlib>

// Coloca em variáveis globais os parâmetros de configuração usados nesse módulo ====
#pragma region
    const Config cfg = lerConfiguracoes("configuracoes.xml");
    const GLfloat arenaAltura = cfg.geometriaBase.arena.altura; 
    const GLfloat arenaLargura = cfg.geometriaBase.arena.largura;
    const GLdouble raioCabeca = cfg.geometria.jogadores.raioCabeca;
    const GLdouble alturaBarril = cfg.geometriaBase.barril.altura;
    const GLdouble diametroBarril = cfg.geometriaBase.barril.diametro;
    Atirador* jogador = NULL;
    FilaDeEventos filaBarris;
    FilaDeEventos filaTiros;

    std::vector<Barril*> barril((size_t)cfg.jogo.barril.maximoSimultaneo, NULL);    // O vetor de booleans atende à interação destrutores -> main
    std::vector<bool> barrilDeletado((size_t)cfg.jogo.barril.maximoSimultaneo, false);    
    std::vector<Tiro*> tirosJogador((size_t)cfg.jogo.tiros.tirosJogadorSimultaneosPermitidos, NULL);
    std::vector<Tiro*> tirosInimigo((size_t)cfg.jogo.tiros.tirosInimigosSimultaneosPerimtidos, NULL);
    int keyStatus[256];

    float cursor_x = 0, cursor_y = 0;
    float mouse_dx = 0;
    float last_cursor_x=0, last_cursor_y=0;

    const Limites limitesJogador = {
        -arenaLargura/2.0 + raioCabeca,     // xMin
         arenaLargura/2.0 - raioCabeca,     // xMax
        -arenaAltura/2.0  + raioCabeca,     // yMin
                          - raioCabeca      // yMax (linha do meio)
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
    GLdouble y0_barril(){return limitesBarril.yMax;};
    bool jogadorMorreu = false;
    bool venceu = false;
    int barris_destruidos = 0;
    bool primeiroQuadro = true;
    bool pausado = false;
    bool mostraLinhaDeTiro = true;

#pragma endregion


void colisaoArena_jogador(GLdouble& x, GLdouble& y) {
    limitesJogador.limita(x, y);
}
void colisaoArena_barril(GLdouble x, GLdouble y, size_t bb){
    if (y > limitesBarril.yMin) return;  // O barril ainda não chegou no seu yMin
    barrilDeletado[bb]=true;
}
void deletaTiro(std::vector<Tiro*>& tiros, size_t tt){
    delete tiros[tt];
    tiros[tt] = NULL;
}
void deletaBarril(size_t bb){
    delete barril[bb];
    barril[bb]=NULL;
    barrilDeletado[bb]=false;
}
void novoBarril(){
    for (size_t bb = 0; bb < barril.size(); bb++){
        if (barril[bb]) continue;
        barril[bb] = new Barril(cfg,
            colisaoArena_barril,
            x0_barril(), y0_barril(), bb,
            jogador);
        break;
    }
    filaBarris.agendaDaqui(cfg.jogo.barril.tempoEntreBarris_ms, novoBarril);
}
void pausaFilaPorDestruicao(){
    filaBarris.pausa(cfg.jogo.barril.tempoPausaDestroi_ms);
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
void DesenhaLinhaDeTiro(Atirador* jogador){
    Vec2 p0 = jogador->p0arma();
    Vec2 p1 = jogador->p1arma();

    Vec2 dir = Normaliza(p1 - p0);
    GLdouble len = sqrt(
        cfg.geometriaBase.arena.altura * cfg.geometriaBase.arena.altura +
        cfg.geometriaBase.arena.largura * cfg.geometriaBase.arena.largura
    );
    Vec2 p2 = p1 + dir * len;
    glPushAttrib(GL_LINE_BIT);
        glLineWidth(3.0);
        glEnable(GL_LINE_STIPPLE);
        glLineStipple(1, 0x00FF);
        glColor3f(0, 0, 0);
        glBegin(GL_LINES);
            glVertex3f(p1.x, p1.y, 0);
            glVertex3f(p2.x, p2.y, 0);
        glEnd();
    glPopAttrib(); 
}
void EscreveTextoCentralizado(const char* texto, GLdouble y, void* fonte){
    int larguraPx = glutBitmapLength(fonte, (const unsigned char*)texto);
    glRasterPos2f(-larguraPx/2.0f, y);
    for (const char* c = texto; *c; c++)
        glutBitmapCharacter(fonte, *c);
}
void DesenhaTelaDeFim(){
    const char *mensagem;
    if(jogadorMorreu){
        mensagem = "Voce morreu!";
    } else {
        mensagem = "Voce venceu!";
    }
    // Cobre a arena inteira na cor do plano de fundo
    glColor3f(cfg.cores.arena.R, cfg.cores.arena.G, cfg.cores.arena.B);
    glBegin(GL_POLYGON);
        glVertex3f(-arenaLargura/2.0, -arenaAltura/2.0, 0);
        glVertex3f( arenaLargura/2.0, -arenaAltura/2.0, 0);
        glVertex3f( arenaLargura/2.0,  arenaAltura/2.0, 0);
        glVertex3f(-arenaLargura/2.0,  arenaAltura/2.0, 0);
    glEnd();

    glColor3f(1, 1, 1);
    EscreveTextoCentralizado(mensagem, 0, GLUT_BITMAP_HELVETICA_18);
    EscreveTextoCentralizado("Aperte ENTER para reiniciar", -20, GLUT_BITMAP_HELVETICA_18);

}
void ImprimePlacar(GLfloat x, GLfloat y){
    static char str[1000];
    glPushMatrix();
        glLoadIdentity();
        glColor3f(1.0, 1.0, 1.0);
        //Cria a string a ser impressa
        char *tmpStr;
        sprintf(str, "Barris destruidos: %d", barris_destruidos );
        //Define a posicao onde vai comecar a imprimir
        glRasterPos2f(x, y);
        //Imprime um caractere por vez
        tmpStr = str;
        while( *tmpStr ){
            glutBitmapCharacter(GLUT_BITMAP_9_BY_15, *tmpStr);
            tmpStr++;
        }
    glPopMatrix();
}
void tiroInimigo(){
    // Lista efêmera: vale só dentro deste evento
    std::vector<Atirador*> vivos;
    for (size_t bb = 0; bb < barril.size(); bb++)
        if (barril[bb] && barril[bb]->getInimigo())
            vivos.push_back(barril[bb]->getInimigo());

    if (!vivos.empty())
        vivos[rand() % vivos.size()]->Atira(tirosInimigo);

    filaTiros.agendaDaqui(1000.0 / cfg.jogo.inimigo.tirosPorSegundo, tiroInimigo);
}
void ResetKeyStatus(){
    int i;
    //Initialize keyStatus
    for(i = 0; i < 256; i++)
       keyStatus[i] = 0; 
}
void display(void){
    // Clear the screen.
    glClear(GL_COLOR_BUFFER_BIT);
    
    if (jogadorMorreu||venceu) {
        DesenhaTelaDeFim();
        glutSwapBuffers();
        return;
    }

    jogador->Desenha();


    for (size_t i = 0; i < tirosJogador.size(); i++){
        if (tirosJogador[i]) tirosJogador[i]->Desenha();
    }
    for (size_t i = 0; i < tirosInimigo.size(); i++)
        if (tirosInimigo[i]) tirosInimigo[i]->Desenha();

    for(size_t bb=0; bb<barril.size(); bb++)
        if(barril[bb]) barril[bb]->Desenha();

    ImprimePlacar(-200 + cfg.geometriaBase.arena.largura/2.0,
                  -20 + cfg.geometriaBase.arena.altura/2.0);

    DesenhaLinhaDaMetade();
    if (mostraLinhaDeTiro) DesenhaLinhaDeTiro(jogador);

    if (pausado) {
        glColor3f(1, 1, 1);
        EscreveTextoCentralizado("PAUSADO", 0, GLUT_BITMAP_HELVETICA_18);
        EscreveTextoCentralizado("Aperte P para continuar", -20, GLUT_BITMAP_HELVETICA_18);
    }
    glutSwapBuffers(); // Desenha the new frame of the game.
}
void passiveMotion(int x, int y){
    float novo_x = float(x) - arenaLargura/2.0;
    mouse_dx += novo_x - cursor_x;       // soma: vários eventos por quadro não se perdem
    cursor_x = novo_x;
    cursor_y = arenaAltura/2.0 - float(y);
    glutPostRedisplay();
}

// Cabeçalhos colocados aqui para o keyPress 
// saber da existência dessas funções, antes
// da declaração
void reiniciaJogo();
void alternaPausa();
void keyPress(unsigned char key, int x, int y){
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
        case 'p':
        case 'P':
            alternaPausa();
            break;
        case 'l':
        case 'L':
            mostraLinhaDeTiro = !mostraLinhaDeTiro;
            break;
        case 13: // Enter
            reiniciaJogo();
            break;
        case 27 :
            exit(0);

        }
    glutPostRedisplay();
}
void keyUp(unsigned char key, int x, int y){
    keyStatus[(int)(key)] = 0;
    glutPostRedisplay();
}
void idle(void){
    // Declarar 'antes' como static faz com que ele seja 
    // 'global' (portanto sobrevive ao término da função),
    // mas escopada dentro da função idle, e sua inicialização
    // acontece só na primeira passada.
    static GLdouble antes_ms = 0;
    GLdouble agora_ms = glutGet(GLUT_ELAPSED_TIME);
    if (primeiroQuadro){
        antes_ms = agora_ms;
        primeiroQuadro = false;
        return;
    }
    GLdouble dt_ms = agora_ms - antes_ms;
    if (dt_ms==0) return;
    antes_ms = agora_ms;


    // Trata teclas
    if(keyStatus[(int)('w')]) jogador->Anda(dt_ms);
    if(keyStatus[(int)('s')]) jogador->Anda(-dt_ms);
    if(keyStatus[(int)('a')]) jogador->Gira(dt_ms);
    if(keyStatus[(int)('d')]) jogador->Gira(-dt_ms);
    

    // Trata mouse
    jogador->MiraManual(mouse_dx, dt_ms);
    mouse_dx = 0;
    
    for(size_t bb=0; bb<barril.size(); bb++){
        if(barril[bb]){
            barril[bb]->Rola(dt_ms);
            if(barrilDeletado[bb]){ deletaBarril(bb); continue; }
            GLdouble x, y;
            jogador->GetPos(x, y);
            if (barril[bb]->colisaoComCirculo(x,y, raioCabeca)) {
                jogadorMorreu = true;
                glutIdleFunc(NULL);     // congela: idle não é mais chamado
                glutPostRedisplay();    // um último quadro, agora com a tela de morte
                return;
            }
        }
    }

    //Trata os tiros
    for (size_t tt = 0; tt < tirosJogador.size(); tt++){
        if (!tirosJogador[tt]) continue;
        tirosJogador[tt]->Move(dt_ms);
        if (!tirosJogador[tt]->Valido()) { deletaTiro(tirosJogador,tt); continue; }

        GLdouble x, y;
        tirosJogador[tt]->GetPos(x, y);      // não muda dentro do laço de barris

        for (size_t bb = 0; bb < barril.size(); bb++){
            if (!barril[bb]) continue;       // posição vazia do vetor
            if (!barril[bb]->colisaoComCirculo(x, y, radiusTiro)) continue;

            if (barril[bb]->danifica(1)) {
                deletaBarril(bb);
                filaBarris.agendaDaqui(0, pausaFilaPorDestruicao);
                barris_destruidos++;
                if(barris_destruidos>=cfg.jogo.regras.barrisParaDestruir){
                    venceu = true;
                    glutIdleFunc(NULL);     // congela: idle não é mais chamado
                    glutPostRedisplay();    // um último quadro, agora com a tela de morte
                    return;
                }
            }
            deletaTiro(tirosJogador,tt);
            break;                           // o tiro deixou de existir
        }
    }
    GLdouble xj, yj;
    jogador->GetPos(xj, yj);
    for (size_t tt = 0; tt < tirosInimigo.size(); tt++){
        if (!tirosInimigo[tt]) continue;
        tirosInimigo[tt]->Move(dt_ms);
        if (!tirosInimigo[tt]->Valido()) { deletaTiro(tirosInimigo, tt); continue; }

        GLdouble x, y;
        tirosInimigo[tt]->GetPos(x, y);
        GLdouble dx = x - xj;
        GLdouble dy = y - yj;
        GLdouble alcance = raioCabeca + radiusTiro;
        if (dx*dx + dy*dy <= alcance*alcance) {
            jogadorMorreu = true;
            glutIdleFunc(NULL);
            glutPostRedisplay();
            return;
        }
    }

    filaBarris.avanca(dt_ms);
    filaTiros.avanca(dt_ms);
    glutPostRedisplay();
}
void reiniciaJogo(){
    // Destrói o mundo atual. Seguro na primeira chamada: está tudo NULL.
    for (size_t bb = 0; bb < barril.size(); bb++)       if (barril[bb])       deletaBarril(bb);
    for (size_t tt = 0; tt < tirosJogador.size(); tt++) if (tirosJogador[tt]) deletaTiro(tirosJogador, tt);
    for (size_t tt = 0; tt < tirosInimigo.size(); tt++) if (tirosInimigo[tt]) deletaTiro(tirosInimigo, tt);
    delete jogador;
    filaBarris.limpa();
    filaTiros.limpa();

    // Estado
    jogadorMorreu = false;
    venceu = false;
    pausado = false;
    barris_destruidos = 0;
    ResetKeyStatus();

    // Mundo novo
    jogador = new Atirador(cfg, colisaoArena_jogador,
        x0_jogador(), y0_jogador(), cfg.cinematica.jogador.velocidadeMovimento,
        90, 1, cfg.cores.cabecaJogador);
    filaBarris.agenda(0, novoBarril);
    filaTiros.agendaDaqui(cfg.jogo.inimigo.atrasoPrimeiroTiro, tiroInimigo);

    primeiroQuadro = true;
    glutIdleFunc(idle);
    glutPostRedisplay();
}
void alternaPausa(){
    if (jogadorMorreu || venceu) return;
    if (pausado) {
        glutIdleFunc(NULL);
    } else {
        primeiroQuadro = true;
        glutIdleFunc(idle);
    }
    glutPostRedisplay();
}
void mouse(int button, int state, int x, int y){
    static bool apertado=false;
    if (button==0) {    // Botão esquerdo
        if (state==0){  // Botão apertado
            if(!apertado){
                apertado = true;
                if (!pausado) jogador->Atira(tirosJogador);
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
    glutMouseFunc(mouse);   // Trata os cliques do mouse
    glutIdleFunc(idle);     // Atualiza o estado do mundo



    // Configurações iniciais do mundo =========================================
    srand((unsigned)time(NULL));    //Pega um seed "aleatório"

    jogador = new Atirador(cfg, colisaoArena_jogador,
        x0_jogador(), y0_jogador(), cfg.cinematica.jogador.velocidadeMovimento,
        90, 1, cfg.cores.cabecaJogador);
    filaBarris.agenda(0, 
        novoBarril);
    filaTiros.agendaDaqui(cfg.jogo.inimigo.atrasoPrimeiroTiro, 
        tiroInimigo);
    ResetKeyStatus();

    // Loop principal ==========================================================
    glutMainLoop();

    return 0;
}