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
#include <cmath>
#include "textura.h"

// Coloca em variáveis globais os parâmetros de configuração usados nesse módulo ====
#pragma region
    const Config cfg = lerConfiguracoes("configuracoes.xml");
    const GLfloat arenaComprimento = cfg.geometria.arena.comprimento;
    const GLfloat arenaLargura = cfg.geometria.arena.largura;
    const double diagonalArena = sqrt(arenaLargura*arenaLargura + arenaComprimento*arenaComprimento);
    const GLdouble raioCabeca = cfg.geometria.jogadores.raioCabeca;
    const GLdouble escala = raioCabeca / cfg.geometriaBase.jogadores.raioCabeca;
    const GLdouble alturaBarril = escala * cfg.geometriaBase.barril.altura;
    const GLdouble diametroBarril = escala * cfg.geometriaBase.barril.diametro;
    const GLfloat alturaParede = cfg.geometria.arena.alturaParede;
    const GLdouble janelaLargura = cfg.jogo.tamJanela.largura;
    const GLdouble janelaAltura = cfg.jogo.tamJanela.altura;
    Atirador* jogador = NULL;
    FilaDeEventos filaBarris;
    FilaDeEventos filaTiros;

    std::vector<Barril*> barril((size_t)cfg.jogo.barril.maximoSimultaneo, NULL);    // O vetor de booleans atende à interação destrutores -> main
    std::vector<bool> barrilDeletado((size_t)cfg.jogo.barril.maximoSimultaneo, false);
    std::vector<Tiro*> tirosJogador((size_t)cfg.jogo.tiro.tirosJogadorSimultaneosPermitidos, NULL);
    std::vector<Tiro*> tirosInimigo((size_t)cfg.jogo.tiro.tirosInimigosSimultaneosPerimtidos, NULL);
    int keyStatus[256];

    float cursor_x = 0, cursor_y = 0;
    float mouse_dx = 0, mouse_dy = 0;
    float last_cursor_x=0, last_cursor_y=0;

    const Limites limitesJogador = {
        -arenaLargura/2.0 + raioCabeca,     // xMin
         arenaLargura/2.0 - raioCabeca,     // xMax
        -arenaComprimento/2.0  + raioCabeca,     // yMin
                          - raioCabeca      // yMax (linha do meio)
    };
    const Limites limitesBarril = {
        -arenaLargura/2.0 + alturaBarril/2.0,   // xMin
         arenaLargura/2.0 - alturaBarril/2.0,   // xMax
        -arenaComprimento/2.0  - diametroBarril/2.0, // yMin
         arenaComprimento/2.0  + diametroBarril/2.0  // yMax
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
    int pontuacao = 0;
    bool primeiroQuadro = true;
    bool pausado = false;
    bool laserLigado = false;




    enum ModoCamera {CAM_OLHO, CAM_ARMA, CAM_TERCEIRA_PESSOA };
    ModoCamera modoCamera = CAM_OLHO;

    // Câmera de terceira pessoa, em coordenadas esféricas em torno do jogador
    const double cam3RaioMin   = cfg.camera.cam3.raioMin;
    const double cam3RaioMax   = cfg.camera.cam3.raioMax;
    const double cam3PassoZoom = cfg.camera.cam3.passoZoom;
    double cam3Raio            = cfg.camera.cam3.raioInicial;
    const double cam3PitchMin  = cfg.camera.cam3.pitchMin_deg;
    const double cam3PitchMax  = cfg.camera.cam3.pitchMax_deg;
    const double cam3Sensib    = cfg.camera.cam3.sensibilidadeMouse;
    double cam3Yaw   = cfg.camera.cam3.yawInicial_deg;
    double cam3Pitch = cfg.camera.cam3.pitchInicial_deg;


    // Estado do arrasto com o botão direito
    bool cam3Arrastando = false;
    int  arrasto_x = 0, arrasto_y = 0;
    const double camFovy = cfg.camera.geral.FOVy_deg;

    const double camNear = cfg.camera.geral.near;
    const double camFar = cam3RaioMax + diagonalArena;

    bool laserNaTela = false;
    GLdouble laserTelaX = 0, laserTelaY = 0;
    bool segurandoReinicio = false;
    GLdouble inicioReinicio_ms = 0;
    bool modoNoturno = false;

#pragma endregion

GLint LarguraJogo(){ return glutGet(GLUT_WINDOW_WIDTH); }
GLint AlturaVisao(){ return cfg.jogo.visaoOponente.altura; }
GLint AlturaJogo(){ return glutGet(GLUT_WINDOW_HEIGHT) - AlturaVisao(); }
bool CursorPreso(){
    return (modoCamera == CAM_OLHO || modoCamera == CAM_ARMA)
        && !pausado && !jogadorMorreu && !venceu;
}
void AtualizaCursor(){
    static int anterior = -1;
    bool preso = CursorPreso();
    if (preso == anterior) return;
    anterior = preso;
    glutSetCursor(preso ? GLUT_CURSOR_NONE : GLUT_CURSOR_INHERIT);
    if (preso) {
        int cx = LarguraJogo() / 2;
        int cy = AlturaVisao() + AlturaJogo() / 2;
        glutWarpPointer(cx, cy);
        cursor_x = cx;
        cursor_y = cy;
        mouse_dx = 0;
        mouse_dy = 0;
    }
}
void DefineViewport(GLint x, GLint y, GLint w, GLint h){
    glViewport(x, y, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(camFovy, (GLdouble)w / h, camNear, camFar);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}
void DefineModoCamera(ModoCamera m){
    modoCamera = m;
}
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
        glEnable(GL_LINE_STIPPLE);
        glLineStipple(1, 0x00FF);
        glLineWidth(cfg.linha.meioArena.espessura);
        AplicaCor(cfg.cor.linhaMeioArena);
        glBegin(GL_LINES);
            glVertex3f(-arenaLargura/2.0, 0, 0);
            glVertex3f( arenaLargura/2.0, 0, 0);
        glEnd();
    glPopAttrib();
}
void DesenhaBordaArena(){
    // Contorno da arena no chão (z=0)
    GLfloat xMax = arenaLargura/2.0;
    GLfloat yMax = arenaComprimento/2.0;
    glPushAttrib(GL_LINE_BIT | GL_CURRENT_BIT | GL_ENABLE_BIT);
        glDisable(GL_LIGHTING);
        glLineWidth(cfg.linha.bordaArena.espessura);
        AplicaCor(cfg.cor.linhaBordaArena);
        glBegin(GL_LINE_LOOP);   // fecha o último vértice com o primeiro
            glVertex3f(-xMax, -yMax, 0);
            glVertex3f( xMax, -yMax, 0);
            glVertex3f( xMax,  yMax, 0);
            glVertex3f(-xMax,  yMax, 0);
        glEnd();
    glPopAttrib();
}
void DesenhaRetanguloSubdividido(const GLfloat o[3], const GLfloat u[3], const GLfloat v[3], GLfloat tamCelula, GLfloat tamLadrilho){
    GLfloat n[3] = { u[1]*v[2] - u[2]*v[1],
                     u[2]*v[0] - u[0]*v[2],
                     u[0]*v[1] - u[1]*v[0] };
    GLfloat lu = sqrt(u[0]*u[0] + u[1]*u[1] + u[2]*u[2]);
    GLfloat lv = sqrt(v[0]*v[0] + v[1]*v[1] + v[2]*v[2]);
    int nu = (int)ceil(lu / tamCelula);
    int nv = (int)ceil(lv / tamCelula);
    GLfloat su = lu / tamLadrilho, sv = lv / tamLadrilho;

    glNormal3fv(n);
    for (int j = 0; j < nv; j++){
        glBegin(GL_QUAD_STRIP);
        for (int i = 0; i <= nu; i++){
            GLfloat a = (GLfloat)i / nu;
            for (int k = 0; k < 2; k++){
                GLfloat b = (GLfloat)(j + 1 - k) / nv;
                glTexCoord2f(a * su, b * sv);
                glVertex3f(o[0] + a*u[0] + b*v[0],
                           o[1] + a*u[1] + b*v[1],
                           o[2] + a*u[2] + b*v[2]);
            }
        }
        glEnd();
    }
}
void DesenhaArena(){
    GLfloat x = arenaLargura/2.0, y = arenaComprimento/2.0, h = alturaParede;
    const GLfloat tamMalha = cfg.geometria.arena.tamMalha;
    const GLfloat tamLadrilho = cfg.textura.arena.ladrilho;

    glPushAttrib(GL_ENABLE_BIT | GL_POLYGON_BIT);
    glEnable(GL_POLYGON_OFFSET_FILL);
    glPolygonOffset(1.0, 1.0);
    AplicaCor(cfg.cor.chao);
    LigaTextura(tex.chao);
    {

        GLfloat o[] = {-x, -y, 0}, u[] = {2*x, 0, 0}, v[] = {0, 2*y, 0};
        DesenhaRetanguloSubdividido(o, u, v, tamMalha, tamLadrilho);
    }
    glPopAttrib();

    AplicaCor(cfg.cor.paredes);
    LigaTextura(tex.paredes);
    {

        GLfloat o[] = {-x,  y, 0}, u[] = { 2*x, 0, 0}, v[] = {0, 0, h};
        DesenhaRetanguloSubdividido(o, u, v, tamMalha, tamLadrilho);
    }
    {
        GLfloat o[] = { x, -y, 0}, u[] = {-2*x, 0, 0}, v[] = {0, 0, h};
        DesenhaRetanguloSubdividido(o, u, v, tamMalha, tamLadrilho);
    }
    {
        GLfloat o[] = {-x, -y, 0}, u[] = {0,  2*y, 0}, v[] = {0, 0, h};
        DesenhaRetanguloSubdividido(o, u, v, tamMalha, tamLadrilho);
    }
    {
        GLfloat o[] = { x,  y, 0}, u[] = {0, -2*y, 0}, v[] = {0, 0, h};
        DesenhaRetanguloSubdividido(o, u, v, tamMalha, tamLadrilho);
    }
    DesligaTextura();
}

void EscreveTextoCentralizado(const char* texto, GLdouble dy, void* fonte){
    int larguraPx = glutBitmapLength(fonte, (const unsigned char*)texto);
    GLdouble w = LarguraJogo();
    GLdouble h = AlturaJogo();
    glRasterPos2f(w/2.0 - larguraPx/2.0, h/2.0 + dy);
    for (const char* c = texto; *c; c++)
        glutBitmapCharacter(fonte, *c);
}
void DesenhaSetor(GLdouble x, GLdouble y, GLdouble r, GLdouble fracao){
    if (fracao <= 0) return;
    int n = (int)ceil(48 * fracao);
    glBegin(GL_TRIANGLE_FAN);
        glVertex2d(x, y);
        for (int i = 0; i <= n; i++){
            GLdouble a = M_PI/2 - 2*M_PI*fracao*i/n;
            glVertex2d(x + r*cos(a), y + r*sin(a));
        }
    glEnd();
}
void DesenhaDisco(GLdouble x, GLdouble y, GLdouble r){
    DesenhaSetor(x, y, r, 1.0);
}
GLdouble FracaoReinicio(){
    if (!segurandoReinicio) return 0;
    return fmin(1.0, (glutGet(GLUT_ELAPSED_TIME) - inicioReinicio_ms) / cfg.jogo.reinicio.tempoSegurar_ms);
}
void DesenhaTelaDeFim(){
    const char *mensagem = jogadorMorreu ? "Voce morreu!" : "Voce venceu!";
    GLdouble f = FracaoReinicio();
    glColor3f(1, 1, 1);
    EscreveTextoCentralizado(mensagem, 0, GLUT_BITMAP_HELVETICA_18);
    EscreveTextoCentralizado(f < 1 ? "Mantenha o botao esquerdo apertado para reiniciar!"
                                   : "O jogo vai iniciar quando soltar o botao do mouse!",
                             -20, GLUT_BITMAP_HELVETICA_18);

    GLdouble r = cfg.hud.reinicio.raio;
    GLdouble x = LarguraJogo() / 2.0;
    GLdouble y = AlturaJogo() / 2.0 - 40 - r;
    AplicaCor(cfg.cor.reinicioFundo);
    DesenhaDisco(x, y, r);
    AplicaCor(cfg.cor.reinicioProgresso);
    DesenhaSetor(x, y, r, f);
}

void IniciaHUD(){
    // Entra em modo HUD: coordenadas em pixels da janela,
    // origem no canto inferior esquerdo, sem influência da câmera.
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    glOrtho(0, LarguraJogo(),
            0, AlturaJogo(),
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
Vec3 PosOlho(const Atirador* a){
    GLdouble H = Atirador::Altura(cfg);
    GLdouble dz = cfg.camera.cam1.alturaOlho_p100AlturaCorpo / 100.0 * H - (H - raioCabeca);
    GLdouble rho = sqrt(fmax(0.0, raioCabeca*raioCabeca - dz*dz));
    GLdouble t = a->GetTheta() * M_PI / 180.0;
    return a->CentroCorpo() + Vec3{rho*cos(t), rho*sin(t), dz};
}
void PosicionaCamera(){
    switch (modoCamera) {
        case CAM_ARMA: {
            Vec3 p0 = jogador->p0arma();
            Vec3 dir = Normaliza(jogador->p1arma() - p0);
            Vec3 olho = p0 + Vec3{0, 0, escala * cfg.camera.cam2.alturaOlho};
            Vec3 alvo = olho + dir;
            gluLookAt(olho.x, olho.y, olho.z,
                    alvo.x, alvo.y, alvo.z,
                    0, 0, 1);
            break;
        }
        case CAM_TERCEIRA_PESSOA: {
            GLdouble x, y;
            jogador->GetPos(x, y);
            GLdouble t = (jogador->GetTheta() + cam3Yaw) * M_PI / 180.0;
            GLdouble f = cam3Pitch * M_PI / 180.0;
            gluLookAt(x - cam3Raio*cos(f)*cos(t),
                    y - cam3Raio*cos(f)*sin(t),
                    cam3Raio*sin(f),
                    x, y, 0,
                    0, 0, 1);
            break;
        }
        case CAM_OLHO: {
            GLdouble t = jogador->GetTheta() * M_PI / 180.0;
            Vec3 olho = PosOlho(jogador);
            Vec3 alvo = olho + Vec3{cos(t), sin(t), 0};
            gluLookAt(olho.x, olho.y, olho.z,
                      alvo.x, alvo.y, alvo.z,
                      0, 0, 1);
            break;
        }
    }
}
Atirador* InimigoDaVisao(){
    static size_t atual = 0;
    if (atual < barril.size() && barril[atual] && barril[atual]->getInimigo())
        return barril[atual]->getInimigo();
    for (size_t bb = 0; bb < barril.size(); bb++)
        if (barril[bb] && barril[bb]->getInimigo()) {
            atual = bb;
            return barril[bb]->getInimigo();
        }
    return NULL;
}
bool PosicionaCameraOponente(){
    Atirador* ini = InimigoDaVisao();
    if (!ini) return false;
    Vec3 olho = PosOlho(ini);
    Vec3 alvo = jogador->CentroCorpo();
    gluLookAt(olho.x, olho.y, olho.z,
              alvo.x, alvo.y, alvo.z,
              0, 0, 1);
    return true;
}
void PosicionaLuzes(){
    const auto& l = cfg.luz.arena;
    GLfloat a = modoNoturno ? cfg.luz.global.ambienteNoturno : cfg.luz.global.ambiente;
    GLfloat ambiente[] = {a, a, a, 1};
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambiente);

    GLfloat d = l.offset / sqrt(2.0);
    GLfloat x = arenaLargura/2.0 - d, y = arenaComprimento/2.0 - d, z = l.altura;
    GLfloat cantos[4][2] = {{-x, -y}, {x, -y}, {x, y}, {-x, y}};
    GLfloat baixo[] = {0, 0, -1};
    for (int i = 0; i < 4; i++){
        GLfloat pos[] = {cantos[i][0], cantos[i][1], z, 1};
        glLightfv(GL_LIGHT0 + i, GL_POSITION, pos);
        glLightfv(GL_LIGHT0 + i, GL_SPOT_DIRECTION, baixo);
        if (modoNoturno) glDisable(GL_LIGHT0 + i);
        else             glEnable(GL_LIGHT0 + i);
    }

    Vec3 p = jogador->OrigemLaser();
    Vec3 dir = jogador->DirArma();
    GLfloat posL[] = {(GLfloat)p.x, (GLfloat)p.y, (GLfloat)p.z, 1};
    GLfloat dirL[] = {(GLfloat)dir.x, (GLfloat)dir.y, (GLfloat)dir.z};
    glLightfv(GL_LIGHT4, GL_POSITION, posL);
    glLightfv(GL_LIGHT4, GL_SPOT_DIRECTION, dirL);
    if (modoNoturno) glEnable(GL_LIGHT4);
    else             glDisable(GL_LIGHT4);

    if (modoNoturno) return;
    glPushAttrib(GL_ENABLE_BIT | GL_CURRENT_BIT);
    glDisable(GL_LIGHTING);
    AplicaCor(cfg.cor.octaedroLuzArena);
    GLfloat t = l.tamanhoOctaedro;
    for (int i = 0; i < 4; i++){
        glPushMatrix();
            glTranslatef(cantos[i][0], cantos[i][1], z);
            glScalef(t, t, t);
            glutSolidOctahedron();
        glPopMatrix();
    }
    glPopAttrib();
}


void ConfiguraLuzes(){
    const auto& l = cfg.luz.arena;
    GLfloat difusa[] = {(GLfloat)l.difusa, (GLfloat)l.difusa, (GLfloat)l.difusa, 1};
    GLfloat preto[] = {0, 0, 0, 1};
    for (int i = 0; i < 4; i++){
        GLenum L = GL_LIGHT0 + i;
        glLightfv(L, GL_AMBIENT, preto);
        glLightfv(L, GL_DIFFUSE, difusa);
        glLightf(L, GL_SPOT_CUTOFF, 90);
        glLightf(L, GL_SPOT_EXPONENT, l.expoenteSpot);
        glLightf(L, GL_CONSTANT_ATTENUATION, l.atenuacaoConstante);
        glLightf(L, GL_LINEAR_ATTENUATION, l.atenuacaoLinear);
        glLightf(L, GL_QUADRATIC_ATTENUATION, l.atenuacaoQuadratica);
    }
    const auto& t = cfg.luz.lanterna;
    GLfloat dl[] = {(GLfloat)t.difusa, (GLfloat)t.difusa, (GLfloat)t.difusa, 1};
    glLightfv(GL_LIGHT4, GL_AMBIENT, preto);
    glLightfv(GL_LIGHT4, GL_DIFFUSE, dl);
    glLightf(GL_LIGHT4, GL_SPOT_CUTOFF, t.corte_deg);
    glLightf(GL_LIGHT4, GL_SPOT_EXPONENT, t.expoente);
    glLightf(GL_LIGHT4, GL_CONSTANT_ATTENUATION, t.atenuacaoConstante);
    glLightf(GL_LIGHT4, GL_LINEAR_ATTENUATION, t.atenuacaoLinear);
    glLightf(GL_LIGHT4, GL_QUADRATIC_ATTENUATION, t.atenuacaoQuadratica);

}

void ImprimePlacar(GLfloat x, GLfloat y){
    static char str[1000];
    glColor3f(1.0, 1.0, 1.0);
    sprintf(str, "Pontos: %d/%d", pontuacao, (int)cfg.jogo.pontos.paraGanhar);
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
GLdouble RaioArena(Vec3 o, Vec3 d){
    GLdouble X = arenaLargura/2.0, Y = arenaComprimento/2.0;
    GLdouble t = INFINITY;
    if (d.x > 0) t = fmin(t, ( X - o.x) / d.x);
    if (d.x < 0) t = fmin(t, (-X - o.x) / d.x);
    if (d.y > 0) t = fmin(t, ( Y - o.y) / d.y);
    if (d.y < 0) t = fmin(t, (-Y - o.y) / d.y);
    if (d.z < 0) t = fmin(t, -o.z / d.z);
    if (o.z + t * d.z > alturaParede) return INFINITY;
    return t;
}
void DesenhaLaser(){
    Vec3 o = jogador->OrigemLaser();
    Vec3 d = jogador->DirArma();
    GLdouble t = RaioArena(o, d);
    for (size_t bb = 0; bb < barril.size(); bb++)
        if (barril[bb]) t = fmin(t, barril[bb]->intersecaoRaio(o, d));
    bool acertou = std::isfinite(t);
    Vec3 fim = o + d * (acertou ? t : diagonalArena);

    glPushAttrib(GL_LINE_BIT | GL_ENABLE_BIT | GL_CURRENT_BIT);
        glDisable(GL_LIGHTING);
        glLineWidth(cfg.linha.laser.espessura);
        AplicaCor(cfg.cor.laser);
        glBegin(GL_LINES);
            glVertex3d(o.x, o.y, o.z);
            glVertex3d(fim.x, fim.y, fim.z);
        glEnd();
    glPopAttrib();

    GLdouble mv[16], pj[16], z;
    GLint vp[4];
    glGetDoublev(GL_MODELVIEW_MATRIX, mv);
    glGetDoublev(GL_PROJECTION_MATRIX, pj);
    glGetIntegerv(GL_VIEWPORT, vp);
    gluProject(fim.x, fim.y, fim.z, mv, pj, vp, &laserTelaX, &laserTelaY, &z);
    laserNaTela = acertou && z >= 0 && z <= 1;
}

void DesenhaPontoLaser(){
    if (!laserNaTela) return;
    glPushAttrib(GL_CURRENT_BIT);
    AplicaCor(cfg.cor.laser);
    DesenhaDisco(laserTelaX, laserTelaY, cfg.hud.laser.raioPonto);
    glPopAttrib();
}
void DesenhaMinimapa(){
    GLint w = LarguraJogo(), h = AlturaJogo();
    GLint vw = w * cfg.hud.minimapa.fracao, vh = h * cfg.hud.minimapa.fracao;
    GLdouble sx = arenaLargura/2.0, sy = arenaComprimento/2.0;
    if (sx / sy < (GLdouble)vw / vh) sx = sy * vw / vh;
    else                             sy = sx * vh / vw;

    glPushAttrib(GL_VIEWPORT_BIT | GL_ENABLE_BIT | GL_CURRENT_BIT);
    glViewport(w - vw, 0, vw, vh);
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    glOrtho(-sx, sx, -sy, sy, -1, 1);
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();
    glDisable(GL_LIGHTING);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_TEXTURE_2D);

    DesenhaBordaArena();
    DesenhaLinhaDaMetade();

    GLdouble mx = alturaBarril / 2.0, my = diametroBarril / 2.0;
    for (size_t bb = 0; bb < barril.size(); bb++){
        if (!barril[bb]) continue;
        GLdouble x = barril[bb]->posX(), y = barril[bb]->posY();
        AplicaCor(cfg.cor.minimapaBarril);
        glRectd(x - mx, y - my, x + mx, y + my);
        Atirador* ini = barril[bb]->getInimigo();
        if (ini) {
            ini->GetPos(x, y);
            AplicaCor(cfg.cor.minimapaInimigo);
            DesenhaDisco(x, y, raioCabeca);
        }
    }
    GLdouble xj, yj;
    jogador->GetPos(xj, yj);
    AplicaCor(cfg.cor.minimapaJogador);
    DesenhaDisco(xj, yj, raioCabeca);

    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
    glPopAttrib();
}


void ResetKeyStatus(){
    int i;
    //Initialize keyStatus
    for(i = 0; i < 256; i++)
       keyStatus[i] = 0;
}
void DesenhaCena(){
    PosicionaLuzes();
    if (laserLigado) DesenhaLaser();
    DesenhaArena();
    jogador->Desenha();
    for (size_t i = 0; i < tirosJogador.size(); i++)
        if (tirosJogador[i]) tirosJogador[i]->Desenha();
    for (size_t i = 0; i < tirosInimigo.size(); i++)
        if (tirosInimigo[i]) tirosInimigo[i]->Desenha();
    for (size_t bb = 0; bb < barril.size(); bb++)
        if (barril[bb]) barril[bb]->Desenha(modoNoturno);
    DesenhaBordaArena();
    DesenhaLinhaDaMetade();
}
void DesenhaDivisoria(GLint w, GLint h){
    GLfloat e = cfg.linha.divisoria.espessura;
    glPushAttrib(GL_LINE_BIT | GL_CURRENT_BIT);
    glLineWidth(e);
    AplicaCor(cfg.cor.divisoria);
    glBegin(GL_LINES);
        glVertex2f(0, h - e/2);
        glVertex2f(w, h - e/2);
    glEnd();
    glPopAttrib();
}
void display(void){
    AtualizaCursor();

    // Clear the screen.
    const Cor& fundo = modoNoturno ? cfg.cor.fundoNoturno : cfg.cor.arena;
    glClearColor(fundo.R, fundo.G, fundo.B, 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    GLint w = LarguraJogo(), h = AlturaJogo(), hv = AlturaVisao();

    if (hv > 0) {
        DefineViewport(0, h, w, hv);
        if (PosicionaCameraOponente()) DesenhaCena();
    }

    DefineViewport(0, 0, w, h);
    PosicionaCamera();
    DesenhaCena();
    DesenhaMinimapa();

    IniciaHUD();
        ImprimePlacar(w - 200, h - 20);
        if (laserLigado) DesenhaPontoLaser();
        if (jogadorMorreu || venceu) {
            DesenhaTelaDeFim();
        } else if (pausado) {
            glColor3f(1, 1, 1);
            EscreveTextoCentralizado("PAUSADO", 0, GLUT_BITMAP_HELVETICA_18);
            EscreveTextoCentralizado("Aperte P para continuar", -20, GLUT_BITMAP_HELVETICA_18);
        }
        if (hv > 0) DesenhaDivisoria(w, h);
        ImprimePlacar(w - 200, h - 20);

    TerminaHUD();
    glutSwapBuffers(); // Desenha the new frame of the game.
}

void passiveMotion(int x, int y){
    mouse_dx += x - cursor_x;
    mouse_dy += cursor_y - y;
    cursor_x = x;
    cursor_y = y;

    if (CursorPreso()) {
        int cx = LarguraJogo() / 2;
        int cy = AlturaVisao() + AlturaJogo() / 2;
        if (x != cx || y != cy) {
            glutWarpPointer(cx, cy);
            cursor_x = cx;
            cursor_y = cy;
        }
    }
    glutPostRedisplay();
}
void motion(int x, int y){
    if (!cam3Arrastando) {
        passiveMotion(x, y);
        return;
    }
    cam3Yaw   -= (x - arrasto_x) * cam3Sensib;
    cam3Pitch -= (arrasto_y - y) * cam3Sensib;
    cam3Yaw    = fmod(cam3Yaw, 360.0);
    if (cam3Pitch > cam3PitchMax) cam3Pitch = cam3PitchMax;
    if (cam3Pitch < cam3PitchMin) cam3Pitch = cam3PitchMin;
    arrasto_x = x;
    arrasto_y = y;

    cursor_x = float(x) - arenaLargura/2.0;
    cursor_y = arenaComprimento/2.0 - float(y);
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
            laserLigado = !laserLigado;
            break;
        case '1':
            DefineModoCamera(CAM_OLHO);
            break;
        case '2':
            DefineModoCamera(CAM_ARMA);
            break;
        case '3':
            DefineModoCamera(CAM_TERCEIRA_PESSOA);
            break;
        case '+':
        case '=':
            cam3Raio = fmax(cam3Raio / cam3PassoZoom, cam3RaioMin);
            break;
        case '-':
        case '_':
            cam3Raio = fmin(cam3Raio * cam3PassoZoom, cam3RaioMax);
            break;
        case 'n':
        case 'N':
            modoNoturno = !modoNoturno;
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
bool SomaPontos(int p){
    pontuacao += p;
    if (pontuacao < cfg.jogo.pontos.paraGanhar) return false;
    venceu = true;
    glutIdleFunc(NULL);
    glutPostRedisplay();
    return true;
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
    GLdouble radPorPixel = cfg.jogo.controles.sensibilidadeMouseArma
                         * camFovy * M_PI / 180.0 / AlturaJogo();
    if (modoCamera == CAM_TERCEIRA_PESSOA) radPorPixel *= cfg.jogo.controles.fatorSensCam3;
    jogador->MiraJogador(mouse_dx * radPorPixel, mouse_dy * radPorPixel);
    mouse_dx = 0;
    mouse_dy = 0;

    for(size_t bb=0; bb<barril.size(); bb++){
        if(barril[bb]){
            if (!pausado) barril[bb]->Rola(dt_ms);
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

        Vec3 s = tirosJogador[tt]->GetPos();
        Vec3 d = tirosJogador[tt]->GetDir();

        for (size_t bb = 0; bb < barril.size(); bb++){
            if (!barril[bb]) continue;
            Atirador* ini = barril[bb]->getInimigo();
            if (ini && ini->colisaoComTiro(s, d, tirosJogador[tt]->GetRaio())) {
                barril[bb]->mataInimigo();
                deletaTiro(tirosJogador, tt);
                if (SomaPontos(cfg.jogo.pontos.atirador)) return;
                break;
            }
            if (!barril[bb]->colisaoComEsfera(s, tirosJogador[tt]->GetRaio())) continue;

            if (barril[bb]->danifica(1)) {
                deletaBarril(bb);
                filaBarris.agendaDaqui(0, pausaFilaPorDestruicao);
                if (SomaPontos(cfg.jogo.pontos.barril)) return;
            }
            deletaTiro(tirosJogador,tt);
            break;

        }
    }
    GLdouble xj, yj;
    jogador->GetPos(xj, yj);
    for (size_t tt = 0; tt < tirosInimigo.size(); tt++){
        if (!tirosInimigo[tt]) continue;
        if (!pausado) tirosInimigo[tt]->Move(dt_ms);
        if (!tirosInimigo[tt]->Valido()) { deletaTiro(tirosInimigo, tt); continue; }

        if (jogador->colisaoComTiro(tirosInimigo[tt]->GetPos(), tirosInimigo[tt]->GetDir(), tirosInimigo[tt]->GetRaio())) {
            jogadorMorreu = true;
            glutIdleFunc(NULL);
            glutPostRedisplay();
            return;
        }
    }

    if (!pausado) {
        filaBarris.avanca(dt_ms);
        filaTiros.avanca(dt_ms);
    }
    glutPostRedisplay();
}
void idleTelaDeFim(){
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
    pontuacao = 0;
    ResetKeyStatus();
    jogador = new Atirador(cfg, colisaoArena_jogador,
        x0_jogador(), y0_jogador(), cfg.cinematica.jogador.velocidadeMovimento,
        90, 1, cfg.cor.cabecaJogador);
    filaBarris.agenda(0, novoBarril);
    filaTiros.agendaDaqui(cfg.jogo.inimigo.atrasoPrimeiroTiro, tiroInimigo);
    primeiroQuadro = true;
    laserLigado = false;

    glutIdleFunc(idle);
    glutPostRedisplay();
}
void alternaPausa(){
    if (jogadorMorreu || venceu) return;
    pausado = !pausado;
    glutPostRedisplay();
}
void mouse(int button, int state, int x, int y){
    static bool apertado=false;
    if (button==GLUT_LEFT_BUTTON) {
        bool fim = jogadorMorreu || venceu;
        if (state==GLUT_DOWN){
            if(!apertado){
                apertado = true;
                if (fim) {
                    segurandoReinicio = true;
                    inicioReinicio_ms = glutGet(GLUT_ELAPSED_TIME);
                    glutIdleFunc(idleTelaDeFim);
                } else {
                    jogador->Atira(tirosJogador);
                }
            }
        } else {
            if(apertado){
                apertado = false;
                if (fim && segurandoReinicio) {
                    bool completo = FracaoReinicio() >= 1;
                    segurandoReinicio = false;
                    glutIdleFunc(NULL);
                    if (completo) reiniciaJogo();
                }
            }
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
    glutInitWindowSize(janelaLargura, janelaAltura + cfg.jogo.visaoOponente.altura);
    glutCreateWindow("Barris e atiradores 3d");
    CarregaTexturas(cfg);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
    ConfiguraLuzes();
    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
    glShadeModel(GL_SMOOTH);
    glEnable(GL_NORMALIZE);

    glClearColor (cfg.cor.arena.R, cfg.cor.arena.G, cfg.cor.arena.B, 1.0);



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
        90, 1, cfg.cor.cabecaJogador);
    filaBarris.agenda(0,
        novoBarril);
    filaTiros.agendaDaqui(cfg.jogo.inimigo.atrasoPrimeiroTiro,
        tiroInimigo);
    ResetKeyStatus();

    // Loop principal ==========================================================
    glutMainLoop();

    return 0;
}
