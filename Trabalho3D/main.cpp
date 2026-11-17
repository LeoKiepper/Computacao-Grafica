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
#include <cmath>
#include <vector>
#include <ctime>
#include <cstdlib>

// Coloca em variáveis globais os parâmetros de configuração usados nesse módulo ====
#pragma region
    const Config cfg = lerConfiguracoes("configuracoes.xml");
    const GLfloat arenaAltura = cfg.geometriaBase.arena.altura;
    const GLfloat arenaLargura = cfg.geometriaBase.arena.largura;
    const double diagonalArena = sqrt(arenaLargura*arenaLargura + arenaAltura*arenaAltura);
    const GLdouble raioCabeca = cfg.geometria.jogadores.raioCabeca;
    const GLdouble alturaBarril = cfg.geometriaBase.barril.altura;
    const GLdouble diametroBarril = cfg.geometriaBase.barril.diametro;
    const GLfloat alturaParede = 50.0;
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




    enum ModoCamera { CAM_CIMA, CAM_TERCEIRA_PESSOA };
    ModoCamera modoCamera = CAM_CIMA;

    // Câmera de terceira pessoa, em coordenadas esféricas em torno do jogador
    const double cam3Raio        = 1100.0;  // distância do olho ao jogador
    const double cam3PitchMax    = 60.0;    // limite de elevação (graus, ±)
    const double cam3Sensib      = 0.3;     // graus por pixel de arrasto
    double cam3Yaw   = 0.0;    // desvio horizontal em relação à direção do jogador (graus)
    double cam3Pitch = 30.0;   // elevação acima do plano do chão (graus)

    // Estado do arrasto com o botão direito
    bool cam3Arrastando = false;
    int  arrasto_x = 0, arrasto_y = 0;

    const double camFovy = 60.0;   // graus
    const double distanciaCamera_Cima = (arenaAltura / 2.0) / tan(camFovy / 2.0 * M_PI / 180.0);

    const double camNear = 1.0;
    const double camFar        = fmax(2*distanciaCamera_Cima, cam3Raio + diagonalArena);

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
    glPushAttrib(GL_LINE_BIT | GL_ENABLE_BIT);
        glDisable(GL_LIGHTING);
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
    glPushAttrib(GL_LINE_BIT | GL_ENABLE_BIT);
        glDisable(GL_LIGHTING);
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
void DesenhaBordaArena(){
    // Contorno da arena no chão (z=0)
    GLfloat xMax = arenaLargura/2.0;
    GLfloat yMax = arenaAltura/2.0;
    glPushAttrib(GL_LINE_BIT | GL_CURRENT_BIT | GL_ENABLE_BIT);
        glDisable(GL_LIGHTING);
        glLineWidth(3.0);
        glColor3f(0, 0, 0);
        glBegin(GL_LINE_LOOP);   // fecha o último vértice com o primeiro
            glVertex3f(-xMax, -yMax, 0);
            glVertex3f( xMax, -yMax, 0);
            glVertex3f( xMax,  yMax, 0);
            glVertex3f(-xMax,  yMax, 0);
        glEnd();
    glPopAttrib();
}
void DesenhaRetanguloSubdividido(const GLfloat o[3], const GLfloat u[3], const GLfloat v[3], GLfloat tamCelula){
    GLfloat n[3] = { u[1]*v[2] - u[2]*v[1],
                     u[2]*v[0] - u[0]*v[2],
                     u[0]*v[1] - u[1]*v[0] };
    GLfloat lu = sqrt(u[0]*u[0] + u[1]*u[1] + u[2]*u[2]);
    GLfloat lv = sqrt(v[0]*v[0] + v[1]*v[1] + v[2]*v[2]);
    int nu = (int)ceil(lu / tamCelula);
    int nv = (int)ceil(lv / tamCelula);

    glNormal3fv(n);
    for (int j = 0; j < nv; j++){
        glBegin(GL_QUAD_STRIP);
        for (int i = 0; i <= nu; i++){
            GLfloat a = (GLfloat)i / nu;
            for (int k = 0; k < 2; k++){
                GLfloat b = (GLfloat)(j + 1 - k) / nv;
                glTexCoord2f(a, b);
                glVertex3f(o[0] + a*u[0] + b*v[0],
                           o[1] + a*u[1] + b*v[1],
                           o[2] + a*u[2] + b*v[2]);
            }
        }
        glEnd();
    }
}
void DesenhaArena(){
    GLfloat x = arenaLargura/2.0, y = arenaAltura/2.0, h = alturaParede;
    const GLfloat cel = 20.0;

    glPushAttrib(GL_ENABLE_BIT | GL_POLYGON_BIT);
    glEnable(GL_POLYGON_OFFSET_FILL);
    glPolygonOffset(1.0, 1.0);
    glColor3f(0.40, 0.35, 0.28);
    {
        GLfloat o[] = {-x, -y, 0}, u[] = {2*x, 0, 0}, v[] = {0, 2*y, 0};
        DesenhaRetanguloSubdividido(o, u, v, cel);
    }
    glPopAttrib();

    glColor3f(0.5, 0.5, 0.5);
    {
        GLfloat o[] = {-x,  y, 0}, u[] = { 2*x, 0, 0}, v[] = {0, 0, h};
        DesenhaRetanguloSubdividido(o, u, v, cel);
    }
    {
        GLfloat o[] = { x, -y, 0}, u[] = {-2*x, 0, 0}, v[] = {0, 0, h};
        DesenhaRetanguloSubdividido(o, u, v, cel);
    }
    {
        GLfloat o[] = {-x, -y, 0}, u[] = {0,  2*y, 0}, v[] = {0, 0, h};
        DesenhaRetanguloSubdividido(o, u, v, cel);
    }
    {
        GLfloat o[] = { x,  y, 0}, u[] = {0, -2*y, 0}, v[] = {0, 0, h};
        DesenhaRetanguloSubdividido(o, u, v, cel);
    }
}
void EscreveTextoCentralizado(const char* texto, GLdouble dy, void* fonte){
    int larguraPx = glutBitmapLength(fonte, (const unsigned char*)texto);
    GLdouble w = glutGet(GLUT_WINDOW_WIDTH);
    GLdouble h = glutGet(GLUT_WINDOW_HEIGHT);
    glRasterPos2f(w/2.0 - larguraPx/2.0, h/2.0 + dy);
    for (const char* c = texto; *c; c++)
        glutBitmapCharacter(fonte, *c);
}
void DesenhaTelaDeFim(){
    const char *mensagem = jogadorMorreu ? "Voce morreu!" : "Voce venceu!";
    glColor3f(1, 1, 1);
    EscreveTextoCentralizado(mensagem, 0, GLUT_BITMAP_HELVETICA_18);
    EscreveTextoCentralizado("Aperte ENTER para reiniciar", -20, GLUT_BITMAP_HELVETICA_18);
}
void IniciaHUD(){
    // Entra em modo HUD: coordenadas em pixels da janela,
    // origem no canto inferior esquerdo, sem influência da câmera.
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    glOrtho(0, glutGet(GLUT_WINDOW_WIDTH),
            0, glutGet(GLUT_WINDOW_HEIGHT),
            -1, 1);

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    // O texto bitmap passa pelo teste de profundidade e pela iluminação;
    // desliga os dois para o HUD não ser escondido pela cena nem tingido pela luz.
    glPushAttrib(GL_ENABLE_BIT);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_LIGHTING);
}
void TerminaHUD(){
    // Sai do modo HUD, restaurando projeção, câmera e estados.
    glPopAttrib();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
}
void PosicionaCamera(){
    // Carrega a câmera do modo atual na MODELVIEW
    if (modoCamera == CAM_TERCEIRA_PESSOA) {
        GLdouble x, y;
        jogador->GetPos(x, y);
        // Azimute: direção do jogador + desvio do arrasto
        GLdouble t = (jogador->GetTheta() + cam3Yaw) * M_PI / 180.0;
        GLdouble f = cam3Pitch * M_PI / 180.0;
        gluLookAt(
            // Eye x, y, z
            x - cam3Raio*cos(f)*cos(t),
            y - cam3Raio*cos(f)*sin(t),
            cam3Raio*sin(f),
            // Center x, y, z
            x, y, 0,
            // Up x, y, z
            0, 0, 1);
    } else { //modoCamera == CAM_CIMA
        gluLookAt(0, 0, distanciaCamera_Cima,
                  0, 0, 0,
                  0, 1, 0);
    }
}
void PosicionaLuzes(){
    GLfloat posLuz0[] = {0, 0, 5 * alturaParede, 1};
    glLightfv(GL_LIGHT0, GL_POSITION, posLuz0);

    glPushAttrib(GL_ENABLE_BIT | GL_CURRENT_BIT);
    glDisable(GL_LIGHTING);
    glColor3f(1, 1, 1);
    glPushMatrix();
        glTranslatef(posLuz0[0], posLuz0[1], posLuz0[2]);
        glScalef(10, 10, 10);
        glutSolidOctahedron();
    glPopMatrix();
    glPopAttrib();
}
void ImprimePlacar(GLfloat x, GLfloat y){
    static char str[1000];
    glColor3f(1.0, 1.0, 1.0);
    sprintf(str, "Barris destruidos: %d", barris_destruidos);
    glRasterPos2f(x, y);
    for (char *p = str; *p; p++)
        glutBitmapCharacter(GLUT_BITMAP_9_BY_15, *p);
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
    int w;
    int h;

    // Clear the screen.
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    PosicionaCamera();
    PosicionaLuzes();

    DesenhaArena();



    jogador->Desenha();


    for (size_t i = 0; i < tirosJogador.size(); i++){
        if (tirosJogador[i]) tirosJogador[i]->Desenha();
    }
    for (size_t i = 0; i < tirosInimigo.size(); i++)
        if (tirosInimigo[i]) tirosInimigo[i]->Desenha();

    for(size_t bb=0; bb<barril.size(); bb++)
        if(barril[bb]) barril[bb]->Desenha();

    DesenhaBordaArena();
    DesenhaLinhaDaMetade();
    if (mostraLinhaDeTiro) DesenhaLinhaDeTiro(jogador);

    IniciaHUD();
        w = glutGet(GLUT_WINDOW_WIDTH);
        h = glutGet(GLUT_WINDOW_HEIGHT);
        ImprimePlacar(w - 200, h - 20);
        if (jogadorMorreu || venceu) {
            DesenhaTelaDeFim();
        } else if (pausado) {
            glColor3f(1, 1, 1);
            EscreveTextoCentralizado("PAUSADO", 0, GLUT_BITMAP_HELVETICA_18);
            EscreveTextoCentralizado("Aperte P para continuar", -20, GLUT_BITMAP_HELVETICA_18);
        }
    TerminaHUD();
    glutSwapBuffers(); // Desenha the new frame of the game.
}
void passiveMotion(int x, int y){
    float novo_x = float(x) - arenaLargura/2.0;
    mouse_dx += novo_x - cursor_x;       // soma: vários eventos por quadro não se perdem
    cursor_x = novo_x;
    cursor_y = arenaAltura/2.0 - float(y);
    glutPostRedisplay();
}
void motion(int x, int y){
    if (!cam3Arrastando) {
        passiveMotion(x, y);
        return;
    }
    cam3Yaw   -= (x - arrasto_x) * cam3Sensib;
    cam3Pitch += (arrasto_y - y) * cam3Sensib;
    cam3Yaw    = fmod(cam3Yaw, 360.0);
    if (cam3Pitch >  cam3PitchMax) cam3Pitch =  cam3PitchMax;
    if (cam3Pitch < -cam3PitchMax) cam3Pitch = -cam3PitchMax;
    arrasto_x = x;
    arrasto_y = y;

    cursor_x = float(x) - arenaLargura/2.0;
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
        case '3': 
            modoCamera = CAM_TERCEIRA_PESSOA; 
            break;
        case '0': 
            modoCamera = CAM_CIMA;
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
    jogador->MiraJogador(mouse_dx, dt_ms);
    mouse_dx = 0;

    for(size_t bb=0; bb<barril.size(); bb++){
        if(barril[bb]){
            barril[bb]->Rola(dt_ms);
            if(barrilDeletado[bb]){ deletaBarril(bb); continue; }
            GLdouble x, y;
            jogador->GetPos(x, y);
            if (barril[bb]->colisaoComCirculo(x,y, raioCabeca)) {
                jogadorMorreu = true;
                glutIdleFunc(NULL);
                glutPostRedisplay();
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
    for (size_t bb = 0; bb < barril.size(); bb++)       if (barril[bb])       deletaBarril(bb);
    for (size_t tt = 0; tt < tirosJogador.size(); tt++) if (tirosJogador[tt]) deletaTiro(tirosJogador, tt);
    for (size_t tt = 0; tt < tirosInimigo.size(); tt++) if (tirosInimigo[tt]) deletaTiro(tirosInimigo, tt);
    delete jogador;
    filaBarris.limpa();
    filaTiros.limpa();
    jogadorMorreu = false;
    venceu = false;
    pausado = false;
    barris_destruidos = 0;
    ResetKeyStatus();
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
    if (button==GLUT_LEFT_BUTTON) {
        if (state==0){  // Botão apertado
            if(!apertado){
                apertado = true;
                if (!pausado) jogador->Atira(tirosJogador);
            }
        } else {
            if(apertado) apertado = false;
        }
    }
    if (button == GLUT_RIGHT_BUTTON) {
        cam3Arrastando = (state == GLUT_DOWN && modoCamera == CAM_TERCEIRA_PESSOA);
        arrasto_x = x;
        arrasto_y = y;
    }
    glutPostRedisplay();
}


int main(int argc, char** argv)
{
    // Configurações iniciais do glut ==========================================
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize (arenaLargura, arenaAltura);
    glutInitWindowPosition (100, 100);
    glutCreateWindow ("Barris e atiradores 2d");
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
    glShadeModel(GL_SMOOTH);
    glEnable(GL_NORMALIZE);

    glClearColor (cfg.cores.arena.R, cfg.cores.arena.G, cfg.cores.arena.B, 1.0);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    // fovy escolhido para que, em z=0, o volume cubra exatamente a arena
    gluPerspective(
        camFovy, 
        (double)arenaLargura / arenaAltura, 
        camNear, 
        camFar);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();




    // Registra callbacks ======================================================
    glutDisplayFunc(display);
    glutKeyboardFunc(keyPress);
    glutKeyboardUpFunc(keyUp);
    glutPassiveMotionFunc(passiveMotion);
    glutMotionFunc(motion);
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
