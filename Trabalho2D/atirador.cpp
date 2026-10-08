#include "atirador.h"
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#include <cmath>
#include <math.h>
#include "funcoesaux.h"

#pragma region  // Funções de desenho
Vec2 Atirador::p0arma(){
    // Tome como p0 a origem do SC da arma, que, no SC de coordenadas do jogador, é o centro de rotação da mira
    Vec2 p0 = {
        escala * (cfg.geometriaBase.jogadores.larguraArma/2.0 + cfg.geometriaBase.jogadores.raioCabeca),
        0,
        1 // ponto, não vetor
    };

    // Recriando o caminho das transformações escritas em Jogador::Desenha()

    // GLdouble ang = gTheta_jogador*M_PI/180.0;
    // GLdouble rot[3][3]={
    //     { cos(ang),-sin(ang),0},
    //     { sin(ang), cos(ang),0},
    //     {           0,          0,1}
    // };
    // Ponto2D aux = MultiplicaMatrizEsq2d(rot,p0);

    // No momento, o centro de rotação na mira não gira com o jogador 
    Vec2 aux = p0;
    GLdouble transl[3][3]={
        {1,0,gPos_x},
        {0,1,gPos_y},
        {0,0,1}
    };
    Vec2 ret = MultiplicaMatrizEsq2d(transl,aux);
    return ret;
}
Vec2 Atirador::p1arma(){
    // Tome como p1 a ponta da arma, no SC da arma
    Vec2 p1 = {
escala * (cfg.geometriaBase.jogadores.comprimentoArma + cfg.geometriaBase.jogadores.posyArma),        0,
        1 // ponto, não vetor
    };

    // Recriando o caminho das transformações escritas em Jogador::Desenha()
    GLdouble ang = gTheta_arma*M_PI/180.0;
    GLdouble rot[3][3]={
        { cos(ang),-sin(ang),0},
        { sin(ang), cos(ang),0},
        {           0,          0,1}
    };
    Vec2 aux1 = MultiplicaMatrizEsq2d(rot,p1);
    GLdouble dx = escala * (cfg.geometriaBase.jogadores.larguraArma/2.0 + cfg.geometriaBase.jogadores.raioCabeca);
    GLdouble transl1[3][3]={
        {1,0,dx},
        {0,1,0},
        {0,0,1}
    };
    Vec2 aux2 = MultiplicaMatrizEsq2d(transl1,aux1);
    GLdouble transl2[3][3]={
        {1,0,gPos_x},
        {0,1,gPos_y},
        {0,0,1}
    };
    Vec2 ret = MultiplicaMatrizEsq2d(transl2,aux2);
    return ret;
}
template <class Cor>
void Atirador::DesenhaCirc(GLdouble radius, const Cor& cor) {
    int NumPontos = 20;
    glColor3f(cor.R, cor.G, cor.B);
    glPointSize(2);
    glBegin(GL_POLYGON);
        for(int pp=0; pp<NumPontos; pp++){
            glVertex3f(radius*cos(2*M_PI/NumPontos*pp),radius*sin(2*M_PI/NumPontos*pp),0);
        }
    glEnd();
}
template <class Cor>
void Atirador::DesenhaRect(GLdouble height, GLdouble width, const Cor& cor)
    {
    glColor3f(cor.R, cor.G, cor.B);
    glBegin(GL_POLYGON);
        glVertex3f (-width/2.0, -height/2.0, 0);
        glVertex3f (-width/2.0,  height/2.0, 0);
        glVertex3f ( width/2.0,  height/2.0, 0);
        glVertex3f ( width/2.0, -height/2.0, 0);
    glEnd();
}
void Atirador::Desenha(){
    GLfloat dx, dy, w, h, r;
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
        glTranslatef(gPos_x, gPos_y, 0);
        glPushMatrix();
            glRotatef(gTheta_jogador, 0, 0, 1);
            glPushMatrix();
                dx = escala * delta_perna_esq;
                dy = escala * (-cfg.geometriaBase.jogadores.larguraPernas/2.0+cfg.geometriaBase.jogadores.raioCabeca);
                glTranslatef(dx, dy,0);
                h = escala * cfg.geometriaBase.jogadores.larguraPernas;
                w = escala * cfg.geometriaBase.jogadores.comprimentoPernas;
                DesenhaRect(h, w, cfg.cores.pernas);
            glPopMatrix();
            glPushMatrix();
                dx = escala * delta_perna_dir;
                dy = escala * (cfg.geometriaBase.jogadores.larguraPernas/2.0-cfg.geometriaBase.jogadores.raioCabeca);
                glTranslatef(dx, dy,0);
                h = escala * cfg.geometriaBase.jogadores.larguraPernas;
                w = escala * cfg.geometriaBase.jogadores.comprimentoPernas;
                DesenhaRect(h, w, cfg.cores.pernas);
            glPopMatrix();
        glPopMatrix();
        r = escala * cfg.geometriaBase.jogadores.raioCabeca;
        DesenhaCirc(r, cfg.cores.cabecaJogador);
        glPushMatrix();
            // Desloca de uma quantidade no eixo X do SC do jogador, que é o mesmo do mundo
            dx = escala * (cfg.geometriaBase.jogadores.larguraArma/2.0 + cfg.geometriaBase.jogadores.raioCabeca);
            glTranslatef(dx,0,0);
            // O centro de rotação da arma fica sendo dx no SC do jogador
            glRotatef(gTheta_arma, 0, 0, 1);
            // Depois do rotate, desloca no eixo X do SC da arma, (x alinhado com o comprimento)
            dx = escala * (cfg.geometriaBase.jogadores.comprimentoArma/2.0 + cfg.geometriaBase.jogadores.posyArma);
            glTranslatef(dx,0,0);
            h = escala * cfg.geometriaBase.jogadores.larguraArma;
            w = escala * cfg.geometriaBase.jogadores.comprimentoArma;
            DesenhaRect(h, w, cfg.cores.arma);
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

    dist_perna += desl*fatorVelAnimPernas*2;
    if(dist_perna > cfg.geometriaBase.jogadores.comprimentoPernas){
        perna_empurrando = !perna_empurrando;
        dist_perna = -cfg.geometriaBase.jogadores.comprimentoPernas;
    }
    if(dist_perna < -cfg.geometriaBase.jogadores.comprimentoPernas){
        perna_empurrando = !perna_empurrando;
        dist_perna = cfg.geometriaBase.jogadores.comprimentoPernas;
    }

    if(perna_empurrando==0){    // perna esquerda empurrando
        delta_perna_esq -= desl*fatorVelAnimPernas;
        delta_perna_dir += desl*fatorVelAnimPernas;
    } else {                    // perna direita empurrando
        delta_perna_esq += desl*fatorVelAnimPernas;
        delta_perna_dir -= desl*fatorVelAnimPernas;
    }
    GLdouble lim = cfg.geometriaBase.jogadores.comprimentoPernas / 2.0;
    if(delta_perna_dir > lim) delta_perna_dir = lim;
    if(delta_perna_dir < -lim) delta_perna_dir = -lim;
    if(delta_perna_esq > lim) delta_perna_esq = lim;
    if(delta_perna_esq < -lim) delta_perna_esq = -lim;

    if (limiteArena) limiteArena(gPos_x, gPos_y);


}
void Atirador::Gira(GLdouble dt){
    GLdouble vel = cfg.cinematica.jogador.velocidadeGiro;
    gTheta_jogador += vel * dt;
}
void Atirador::Mira(GLdouble dx, GLdouble dt){
    GLdouble passoMax = cfg.cinematica.jogador.velocidadeMira * dt;
    GLdouble delta = - dx;  // O sinal invertido compensa movimento em +x virando giro em -DeltaTheta
    if (delta >  passoMax) delta =  passoMax;
    if (delta < -passoMax) delta = -passoMax;

    GLdouble theta = gTheta_arma + delta;
    GLdouble limite;

    limite = 90 + cfg.cinematica.jogador.aberturaAngular/2.0;
    if(theta>limite) theta = limite;
    limite = 90 - cfg.cinematica.jogador.aberturaAngular/2.0;
    if(theta<limite) theta = limite;
    gTheta_arma = theta;
}
void Atirador::Atira(std::vector<Tiro*> &tiros){
    Vec2 p1 = this->p1arma();
    Vec2 dir = Normaliza(p1 - this->p0arma());
    GLdouble vel = cfg.cinematica.tiros.velocidadeTiro;
    for (size_t i = 0; i < tiros.size(); i++)
        if (!tiros[i]) { tiros[i] = new Tiro(p1,dir,vel); break; }
}