#include "atirador.h"
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#include <cmath>
#include <math.h>
#include "funcoesaux.h"
#include "controlador_mira.h"
#include "textura.h"

#pragma region  // Funções de desenho
// SC do pivô da arma -> SC do mundo.
// Mesma cadeia do Desenha: T(pos)*R(gTheta_jogador)*T(dx,0).
Vec2 Atirador::pivoParaMundo(Vec2 p){
    GLdouble dy = -escala * (cfg.geometriaBase.jogadores.raioCabeca
                           + cfg.geometriaBase.jogadores.larguraArma/2.0);
    GLdouble a  = gTheta_jogador * M_PI / 180.0;
    GLdouble Tpiv[3][3] = {
        {1, 0, 0},
        {0, 1, dy},
        {0, 0, 1}
    };
    GLdouble Rj[3][3]   = {
        {cos(a), -sin(a), 0},
        {sin(a),  cos(a), 0},
        {        0,         0, 1}
    };
    GLdouble Tpos[3][3] = {
        {1, 0, gPos_x},
        {0, 1, gPos_y},
        {0, 0, 1}
    };
    p = MultiplicaMatrizEsq2d(Tpiv, p);
    p = MultiplicaMatrizEsq2d(Rj,   p);
    p = MultiplicaMatrizEsq2d(Tpos, p);
    return p;
}
Vec3 Atirador::p0arma(){
    Vec2 p = pivoParaMundo(Vec2{0, 0, 1});
    return Vec3{p.x, p.y, gPos_z + zCentroCorpo()};
}
Vec3 Atirador::p1arma(){
    // Ponta: extremo dianteiro do retângulo, no eixo da arma
    GLdouble L = escala * (cfg.geometriaBase.jogadores.posyArma
                         + cfg.geometriaBase.jogadores.comprimentoArma/2.0);
    GLdouble a = gTheta_arma * M_PI / 180.0;
    GLdouble f = gPhi_arma * M_PI / 180.0;
    GLdouble Ra[3][3] = {
        {cos(a), -sin(a), 0},
        {sin(a),  cos(a), 0},
        {        0,         0, 1}
    };
    Vec2 p = pivoParaMundo(MultiplicaMatrizEsq2d(Ra, Vec2{L*cos(f), 0, 1}));
    return Vec3{p.x, p.y, gPos_z + zCentroCorpo() + L*sin(f)};
}

void Atirador::DesenhaEsfera(GLdouble raio, const Cor& cor){
    static GLUquadric* q = gluNewQuadric();
    gluQuadricNormals(q, GLU_SMOOTH);
    gluQuadricTexture(q, GL_TRUE);
    glColor3f(cor.R, cor.G, cor.B);
    gluSphere(q, raio, 20, 14);
}
void Atirador::DesenhaCaixa(GLdouble sx, GLdouble sy, GLdouble sz, const Cor& cor){
    static const GLfloat N[6][3] = {{1,0,0},{-1,0,0},{0,1,0},{0,-1,0},{0,0,1},{0,0,-1}};
    static const GLfloat S[4][2] = {{-1,-1},{1,-1},{1,1},{-1,1}};
    glColor3f(cor.R, cor.G, cor.B);
    glPushMatrix();
    glScalef(sx, sy, sz);
    glBegin(GL_QUADS);
    for (int f = 0; f < 6; f++){
        const GLfloat* n = N[f];
        GLfloat u[3] = {n[1], n[2], n[0]};
        GLfloat v[3] = {n[1]*u[2] - n[2]*u[1],
                        n[2]*u[0] - n[0]*u[2],
                        n[0]*u[1] - n[1]*u[0]};
        glNormal3fv(n);
        for (int k = 0; k < 4; k++){
            glTexCoord2f((S[k][0] + 1)/2, (S[k][1] + 1)/2);
            glVertex3f(0.5f*(n[0] + S[k][0]*u[0] + S[k][1]*v[0]),
                       0.5f*(n[1] + S[k][0]*u[1] + S[k][1]*v[1]),
                       0.5f*(n[2] + S[k][0]*u[2] + S[k][1]*v[2]));
        }
    }
    glEnd();
    glPopMatrix();
}
void Atirador::DesenhaCilindro(GLdouble raio, GLdouble comprimento, const Cor& cor){
    static GLUquadric* q = gluNewQuadric();
    gluQuadricNormals(q, GLU_SMOOTH);
    gluQuadricTexture(q, GL_TRUE);
    int n = cfg.geometria.miraLaser.fatias;
    AplicaCor(cor);
    gluCylinder(q, raio, raio, comprimento, n, 1);
    glPushMatrix();
        glRotatef(180, 1, 0, 0);
        gluDisk(q, 0, raio, n, 1);
    glPopMatrix();
    glPushMatrix();
        glTranslatef(0, 0, comprimento);
        gluDisk(q, 0, raio, n, 1);
    glPopMatrix();
}

void Atirador::Desenha(){
    GLfloat dx, dy, w, h, r, L;
    L = escala * cfg.geometriaBase.jogadores.comprimentoPernas;
    r = escala * cfg.geometriaBase.jogadores.raioCabeca;
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
        glTranslatef(gPos_x, gPos_y, gPos_z);
        glPushMatrix();
            glRotatef(gTheta_jogador, 0, 0, 1);
            w = escala * cfg.geometriaBase.jogadores.larguraPernas;
            LigaTextura(tex.pernas);
            glPushMatrix();
                dy = escala * (-cfg.geometriaBase.jogadores.larguraPernas/2.0+cfg.geometriaBase.jogadores.raioCabeca);
                glTranslatef(0, dy, L);
                glRotatef(-asin(delta_perna_esq / cfg.geometriaBase.jogadores.comprimentoPernas) * 180.0 / M_PI, 0, 1, 0);
                glTranslatef(0, 0, -L/2.0);
                DesenhaCaixa(w, w, L, cfg.cor.pernas);
            glPopMatrix();
            glPushMatrix();
                dy = escala * (cfg.geometriaBase.jogadores.larguraPernas/2.0-cfg.geometriaBase.jogadores.raioCabeca);
                glTranslatef(0, dy, L);
                glRotatef(-asin(delta_perna_dir / cfg.geometriaBase.jogadores.comprimentoPernas) * 180.0 / M_PI, 0, 1, 0);
                glTranslatef(0, 0, -L/2.0);
                DesenhaCaixa(w, w, L, cfg.cor.pernas);
            glPopMatrix();
        glPopMatrix();
        glPushMatrix();
            glTranslatef(0, 0, L + r);
            glRotatef(gTheta_jogador, 0, 0, 1);
            LigaTextura(tex.corpo);
            DesenhaEsfera(r, corCabeca);
        glPopMatrix();
        glPushMatrix();
            // SC do atirador: frente em +x
            glRotatef(gTheta_jogador, 0, 0, 1);
            dy = -escala * (cfg.geometriaBase.jogadores.raioCabeca
                          + cfg.geometriaBase.jogadores.larguraArma/2.0);
            // pivô: ao lado da cabeça, em -y
            glTranslatef(0, dy, L + r);
            // mira, em torno do pivô
            glRotatef(gTheta_arma, 0, 0, 1);
            glRotatef(-gPhi_arma, 0, 1, 0);
            dx = escala * cfg.geometriaBase.jogadores.posyArma;
            // centro do retângulo, ao longo da arma
            glTranslatef(dx, 0, 0);
            L = escala * cfg.geometriaBase.jogadores.comprimentoArma;
            w = escala * cfg.geometriaBase.jogadores.larguraArma;
            h = escala * cfg.geometriaBase.jogadores.alturaArma;
            LigaTextura(tex.arma);
            DesenhaCaixa(L, w,h, cfg.cor.arma);
            DesligaTextura();
            const auto& m = cfg.geometriaBase.miraLaser;
            glTranslatef(L/2.0 + escala * (m.sobressalto - m.comprimento), w/2.0, h/2.0);
            glRotatef(90, 0, 1, 0);
            DesenhaCilindro(escala * m.diametro / 2.0, escala * m.comprimento, cfg.cor.miraLaser);
        glPopMatrix();
    glPopMatrix();
};

#pragma endregion // Funções de desenho

// Funções de controle
void Atirador::Anda(GLdouble dt){
    GLdouble vel = gVel;
    GLdouble desl = dt * vel;

    gPos_x += desl * cos(gTheta_jogador*M_PI/180);
    gPos_y += desl * sin(gTheta_jogador*M_PI/180);

    #pragma region // Animação das pernas
        // A origem do SC de cada retângulo da perna é posicionada no seu centro
        // Cada retângulo é posicionado pela sua âncora num ponto sobre o diâmetro
        // do círculo da cabeça. Para ser posicionada de modo que o "pé" fique
        // parado, cada perna tem que ser desenhada deslocada ortogonalmente
        // a esse diâmetro de uma distância específica, que muda com o passar do tempo.

        // A forma de onda dessa distância para uma perna tem que ser uma onda triangular,
        // De modo que a inclinação de cada fase seja, em módulo, igual à velocidade do
        // atirador

        //   0 ---------- 1A ---------- 2A ---------- 3A ---------- 4A
        //   |   fase 1    |    fase 2   |   fase 3    |    fase 4   |
        //   |  esq apoia  |  dir apoia  |  dir apoia  |  esq apoia  |
        //   | delta_esq + | delta_esq + | delta_esq - | delta_esq - |

        // Busca-se uma função em domínio modular que leva a distância percorrida pelo
        // atirador, no SC_mundo, ao delta de uma perna. A da outra perna é derivável
        // a partir daí.

        // No SC do atirador, a distância percorrida em um ciclo completo (portanto o
        // período, já que a função é parametrizada em distância percorrida) é o dobro do
        // comprimento de uma perna. Então, um quarto de ciclo é:
        const GLdouble amplitude = cfg.geometriaBase.jogadores.comprimentoPernas/2.0;
        // Como não se deseja escorregamento, então a velocidade do pé, em módulo, tem
        // que ser idêntica ao do atirador, e a inclinação da onda tem que ser 1.
        // Portanto, a amplitude da onda é numericamente igual à distância percorrida em
        // um quarto de ciclo.

        // distMarcha é a distância acumulada no SC do atirador, crescendo sem limite
        // Precisa ser corrigido pela escala, já que envolve movimento de componentes
        // afetados por ela
        distMarcha += desl * fatorVelAnimPernas / escala;

        // fmod é o resto da divisão x/y, em float
        // p mapea distMarcha para álgebra modular de um ciclo = 4*amplitude
        GLdouble p = fmod(distMarcha, 4*amplitude);
        if (p < 0) p += 4*amplitude; // Remapeia o domínio negativo. Sobra (0, 4*amplitude]

        // Atribua a primeira metade do ciclo à perna esquerda empurrando
        bool pernaEsqEmpurrando = (p < 2*amplitude);
        GLdouble distNaFaseAtual = pernaEsqEmpurrando ? p : p - 2*amplitude;

        // Calcule o delta do qual cada perna tem que ser que ser desenhada. Como as
        // ondas de delta de cada perna são defasadas de meio ciclo, então pode-se
        // apenas inverter o sinal
        GLdouble delta_pernaDeApoio   =  amplitude - distNaFaseAtual;
        GLdouble delta_pernaEmBalanco = -delta_pernaDeApoio;

        delta_perna_esq  = pernaEsqEmpurrando ? delta_pernaDeApoio   : delta_pernaEmBalanco;
        delta_perna_dir  = pernaEsqEmpurrando ? delta_pernaEmBalanco : delta_pernaDeApoio;
        perna_empurrando = pernaEsqEmpurrando ? 0 : 1;
    #pragma endregion

    if (limiteArena) limiteArena(gPos_x, gPos_y);
}
void Atirador::Gira(GLdouble dt){
    GLdouble vel = cfg.cinematica.jogador.velocidadeGiro;
    gTheta_jogador += vel * dt;
}

GLdouble Atirador::GiraLimitado(GLdouble atual, GLdouble delta, GLdouble dt){
    GLdouble passoMax = cfg.cinematica.jogador.velocidadeMira * dt;
    if (delta >  passoMax) delta =  passoMax;
    if (delta < -passoMax) delta = -passoMax;
    GLdouble angulo = atual + delta;
    GLdouble meia = cfg.cinematica.jogador.aberturaAngular / 2.0;
    if (angulo > + meia) angulo = meia;
    if (angulo < - meia) angulo = -meia;
    return angulo;
}
// Função compartilhada entre os Mira dos dois tipos de Atirador
void Atirador::GiraArma(GLdouble delta, GLdouble dt){
    gTheta_arma = GiraLimitado(gTheta_arma, delta, dt);
    LimitaAoCone();
}
void Atirador::GiraArmaVertical(GLdouble delta, GLdouble dt){
    gPhi_arma = GiraLimitado(gPhi_arma, delta, dt);
    LimitaAoCone();
}

Vec3 Atirador::DirArmaLocal() const {
    GLdouble t = gTheta_arma * M_PI / 180.0;
    GLdouble f = gPhi_arma * M_PI / 180.0;
    return Vec3{cos(f)*cos(t), cos(f)*sin(t), sin(f)};
}
void Atirador::DefineDirArmaLocal(Vec3 d){
    gTheta_arma = atan2(d.y, d.x) * 180.0 / M_PI;
    gPhi_arma   = asin(d.z) * 180.0 / M_PI;
}
void Atirador::LimitaAoCone(){
    GLdouble lim = cfg.cinematica.jogador.aberturaAngular / 2.0 * M_PI / 180.0;
    Vec3 d = DirArmaLocal();
    if (d.x >= cos(lim)) return;
    Vec3 lateral = Normaliza(Vec3{0, d.y, d.z});
    DefineDirArmaLocal(Vec3{cos(lim), 0, 0} + lateral * sin(lim));
}
void Atirador::MiraJogador(GLdouble dx, GLdouble dy){
    Vec3 d = DirArmaLocal();
    Vec3 r = Normaliza(Cross(d, Vec3{0, 0, 1}));
    Vec3 u = Cross(r, d);
    DefineDirArmaLocal(Normaliza(d + r*dx + u*dy));
    LimitaAoCone();
}
void Atirador::MiraInimigo(GLdouble dt){ if (controlador) controlador->atua(dt); }
Atirador::~Atirador(){ delete controlador; }
void Atirador::Atira(std::vector<Tiro*> &tiros){
    Vec3 p1 = this->p1arma();
    Vec3 dir = Normaliza(p1 - this->p0arma());
    GLdouble vel = cfg.cinematica.tiros.velocidadeTiro;
    for (size_t i = 0; i < tiros.size(); i++)
        if (!tiros[i]) { tiros[i] = new Tiro(cfg, p1, dir, vel); break; }
}
void Atirador::Prismas(Prisma p[3]) const {
    const auto& b = cfg.geometriaBase.jogadores;
    GLdouble L = escala * b.comprimentoPernas;
    GLdouble w = escala * b.larguraPernas;
    Vec3 base = {gPos_x, gPos_y, gPos_z};

    GLdouble yPerna = escala * (b.raioCabeca - b.larguraPernas/2.0);
    GLdouble deltas[2] = {delta_perna_esq, delta_perna_dir};
    GLdouble lados[2]  = {+yPerna, -yPerna};
    for (int i = 0; i < 2; i++){
        GLdouble alfa = asin(deltas[i] / b.comprimentoPernas) * 180.0 / M_PI;
        p[i].ex = RotZ(RotY(Vec3{1, 0, 0}, -alfa), gTheta_jogador);
        p[i].ey = RotZ(Vec3{0, 1, 0}, gTheta_jogador);
        p[i].ez = RotZ(RotY(Vec3{0, 0, 1}, -alfa), gTheta_jogador);
        p[i].O  = base + RotZ(Vec3{0, lados[i], L} + RotY(Vec3{0, 0, -L/2.0}, -alfa), gTheta_jogador);
        p[i].ax = w/2;
        p[i].ay = w/2;
        p[i].c  = L/2;
    }

    GLdouble thetaMundo = gTheta_jogador + gTheta_arma;
    p[2].ex = RotZ(Vec3{0, 1, 0}, thetaMundo);
    p[2].ey = RotZ(RotY(Vec3{0, 0, 1}, -gPhi_arma), thetaMundo);
    p[2].ez = RotZ(RotY(Vec3{1, 0, 0}, -gPhi_arma), thetaMundo);
    p[2].O  = armaParaMundo(Vec3{escala * b.posyArma, 0, 0});
    p[2].ax = escala * b.larguraArma / 2;
    p[2].ay = escala * b.alturaArma / 2;
    p[2].c  = escala * b.comprimentoArma / 2;
}
Vec3 Atirador::armaParaMundo(Vec3 q) const {
    const auto& b = cfg.geometriaBase.jogadores;
    Vec3 pivo = Vec3{gPos_x, gPos_y, gPos_z}
              + RotZ(Vec3{0, -escala * (b.raioCabeca + b.larguraArma/2.0), zCentroCorpo()}, gTheta_jogador);
    return pivo + RotZ(RotY(q, -gPhi_arma), gTheta_jogador + gTheta_arma);
}
bool Atirador::colisaoComTiro(Vec3 P, Vec3 t, GLdouble rc) const {
    GLdouble r = escala * cfg.geometriaBase.jogadores.raioCabeca;
    if (Norma(P - CentroCorpo()) <= r + rc) return true;
    Prisma p[3];
    Prismas(p);
    for (int i = 0; i < 3; i++)
        if (EsferaTocaPrisma(P, t, rc, p[i].O, p[i].ex, p[i].ey, p[i].ez, p[i].ax, p[i].ay, p[i].c)) return true;
    return false;
}
GLdouble Atirador::intersecaoRaio(Vec3 o, Vec3 d) const {
    GLdouble t = RaioEsfera(o, d, CentroCorpo(), escala * cfg.geometriaBase.jogadores.raioCabeca);
    Prisma p[3];
    Prismas(p);
    for (int i = 0; i < 3; i++)
        t = fmin(t, RaioPrisma(o, d, p[i].O, p[i].ex, p[i].ey, p[i].ez, p[i].ax, p[i].ay, p[i].c));
    return t;
}
Vec3 Atirador::DirArma() const {
    return RotZ(RotY(Vec3{1, 0, 0}, -gPhi_arma), gTheta_jogador + gTheta_arma);
}
Vec3 Atirador::OrigemLaser() const {
    const auto& b = cfg.geometriaBase.jogadores;
    return armaParaMundo(Vec3{escala * (b.posyArma + b.comprimentoArma/2.0 + cfg.geometriaBase.miraLaser.sobressalto),
                              escala * b.larguraArma / 2.0,
                              escala * b.alturaArma / 2.0});
}
