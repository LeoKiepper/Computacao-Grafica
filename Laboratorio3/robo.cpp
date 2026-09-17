#include "robo.h"
#include <math.h>

void Robo::DesenhaRect(GLint height, GLint width, GLfloat R, GLfloat G, GLfloat B)
{
    glColor3f (R, G, B);
    glBegin(GL_POLYGON);
        glVertex3f (-width/2, 0,   0);
        glVertex3f ( width/2, 0,   0);
        glVertex3f ( width/2, height, 0);
        glVertex3f (-width/2, height, 0);
    glEnd();
}

void Robo::DesenhaCirc(GLint radius, GLfloat R, GLfloat G, GLfloat B)
{
    int NumPontos = 20;
    glColor3f(R, G, B);
    glPointSize(2);
    glBegin(GL_POINTS);
        for(int pp=0; pp<NumPontos; pp++){
            glVertex3f(radius*cos(2*M_PI/NumPontos*pp),radius*sin(2*M_PI/NumPontos*pp),0);
        }
    glEnd();
}

void Robo::DesenhaRoda(GLfloat x, GLfloat y, GLfloat thetaWheel, GLfloat R, GLfloat G, GLfloat B)
{
    glPushMatrix();
        glTranslatef(x,y,0);
        glRotatef(thetaWheel,0,0,1);
        DesenhaCirc(radiusWheel,R,G,B);
    glPopMatrix();
}

void Robo::DesenhaBraco(GLfloat x, GLfloat y, GLfloat theta1, GLfloat theta2, GLfloat theta3)
{
    glPushMatrix();
        glRotatef(theta1, 0, 0, 1);
        DesenhaRect(paddleHeight,paddleWidth,0,0,1);
        glTranslatef(0,paddleHeight,0);
        glRotatef(theta2, 0, 0, 1);
        DesenhaRect(paddleHeight,paddleWidth,1,1,0);
        glTranslatef(0,paddleHeight,0);
        glRotatef(theta3, 0, 0, 1);
        DesenhaRect(paddleHeight,paddleWidth,0,1,0);
    glPopMatrix();
}

void Robo::DesenhaRobo(GLfloat x, GLfloat y, GLfloat thetaWheel, GLfloat theta1, GLfloat theta2, GLfloat theta3)
{
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glPushMatrix();
        glTranslatef(x, y, 0);
        DesenhaRect(baseHeight, baseWidth, 1, 0, 0);
        glTranslatef(0, baseHeight, 0);
        DesenhaBraco(x, y+baseHeight, theta1, theta2, theta3);
    glPopMatrix();
    glPushMatrix();
        glTranslatef(-baseWidth/2,0,0);
        DesenhaRoda(x, y, thetaWheel, 1,1,1);
    glPopMatrix();
    glPushMatrix();
        glTranslatef(baseWidth/2,0,0);
        DesenhaRoda(x, y, thetaWheel, 1,1,1);
    glPopMatrix();
}

void Robo::RodaBraco1(GLfloat inc)
{
    gTheta1 += inc;
}

void Robo::RodaBraco2(GLfloat inc)
{
    gTheta2 += inc;
}

void Robo::RodaBraco3(GLfloat inc)
{
    gTheta3 += inc;
}

void Robo::MoveEmX(GLfloat dx)
{
    gX += dx;
    gThetaWheel -= dx * 180.0 / (M_PI * radiusWheel);
}

//Funcao auxiliar de rotacao
void RotatePoint(GLfloat x, GLfloat y, GLfloat angle, GLfloat &xOut, GLfloat &yOut){
    GLfloat rad = angle * M_PI / 180.0;
    GLfloat c = cos(rad);
    GLfloat s = sin(rad);
    xOut = x*c - y*s;
    yOut = x*s + y*c;
}
void Robo::PontoDaHaste3(GLfloat xLocal, GLfloat yLocal, GLfloat &xOut, GLfloat &yOut)
{
    GLfloat x = xLocal, y = yLocal;

    RotatePoint(x, y, gTheta3, x, y);
    y += paddleHeight;
    RotatePoint(x, y, gTheta2, x, y);
    y += paddleHeight;
    RotatePoint(x, y, gTheta1, x, y);
    y += baseHeight;

    xOut = x + gX;
    yOut = y + gY;
}
Tiro* Robo::Atira()
{
    GLfloat xBase, yBase, xPonta, yPonta;

    PontoDaHaste3(0.0, 0.0,          xBase,  yBase);
    PontoDaHaste3(0.0, paddleHeight, xPonta, yPonta);

    GLfloat ang = atan2(yPonta - yBase, xPonta - xBase);

    return new Tiro(xPonta, yPonta, ang);
}
