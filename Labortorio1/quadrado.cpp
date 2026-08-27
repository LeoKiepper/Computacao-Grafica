#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#include <stdio.h>
#define TAMANHO_JANELA 500


float gX = 0;
float gY = 0;
int keyStatus[256];

void display(void)
{
   /* Limpar todos os pixels  */
   glClear (GL_COLOR_BUFFER_BIT);

   /* Define cor dos vértices com os valores R, G e B variando de 0.0 a 1.0 */
   glColor3f (1.0, 0.5, 0.5);
   /* Desenhar um polígono branco (retângulo) */
   glBegin(GL_POLYGON);
      glVertex3f (0.25+gX, 0.25+gY, 0.0);
      glVertex3f (0.75+gX, 0.25+gY, 0.0);
      glVertex3f (0.75+gX, 0.75+gY, 0.0);
      glVertex3f (0.25+gX, 0.75+gY, 0.0);
   glEnd();

   /* Desenhar no frame buffer! */
   glutSwapBuffers(); //Funcao apropriada para janela double buffer
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

void keyPress(unsigned char key, int x,int y){
   if (key=='a'){
      keyStatus[(int)('a')] = 1;
   } 
   if (key=='d'){
      keyStatus[(int)('d')] = 1;
   }
   if (key=='w'){
      keyStatus[(int)('w')] = 1;
   }
   if (key=='s'){
      keyStatus[(int)('s')] = 1;
   }  
   glutPostRedisplay();
}
void keyUp(unsigned char key, int x, int y){
   if (key=='a'){
      keyStatus[(int)('a')] = 0;
   } 
   if (key=='d'){
      keyStatus[(int)('d')] = 0;
   }
   if (key=='w'){
      keyStatus[(int)('w')] = 0;
   }
   if (key=='s'){
      keyStatus[(int)('s')] = 0;
   }  
   glutPostRedisplay();
}
void idle(void){
   float inc = 0.001;
   if(keyStatus[(int)('a')]) gX -= inc;
   if(keyStatus[(int)('d')]) gX += inc;
   if(keyStatus[(int)('w')]) gY += inc;
   if(keyStatus[(int)('s')]) gY -= inc;
   glutPostRedisplay();
}
void mouse(int button, int state, int x, int y){
   // printf("mX=%i, mY=%i\n", x, TAMANHO_JANELA-y);
   // fflush(stdout);
   gX = float(x)/float(TAMANHO_JANELA);
   gY = float(TAMANHO_JANELA-y)/float(TAMANHO_JANELA);
}  
int main(int argc, char** argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode (GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize (TAMANHO_JANELA, TAMANHO_JANELA); 
    glutInitWindowPosition (100, 100);
    glutCreateWindow ("hello world");
    init ();
    glutDisplayFunc(display); 
    glutKeyboardFunc(keyPress);
    glutKeyboardUpFunc(keyUp);
    glutIdleFunc(idle);
    glutMouseFunc(mouse);
    glutMainLoop();

    return 0;
}
