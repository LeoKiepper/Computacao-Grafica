#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#include <stdio.h>
#define TAMANHO_JANELA 500


float gX = 0.5;
float gY = 0.5;
float gW = 0.5;
float gH = 0.5;
int keyStatus[256];
float last_cursor_x = 0.5;
float last_cursor_y = 0.5;
float cursor_x = 0.5;
float cursor_y = 0.5;
float delta_x = 0;
float delta_y = 0;
int mbleft_is_down = 0;

void display(void)
{
   /* Limpar todos os pixels  */
   glClear (GL_COLOR_BUFFER_BIT);

   /* Define cor dos vértices com os valores R, G e B variando de 0.0 a 1.0 */
   glColor3f (1.0, 0.5, 0.5);
   /* Desenhar um polígono branco (retângulo) */
   glBegin(GL_POLYGON);
      glVertex3f (gX-gW/2, gY-gH/2, 0.0);
      glVertex3f (gX+gW/2, gY-gH/2, 0.0);
      glVertex3f (gX+gW/2, gY+gH/2, 0.0);
      glVertex3f (gX-gW/2, gY+gH/2, 0.0);
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
   if (mbleft_is_down) {
      delta_x = cursor_x - last_cursor_x;
      delta_y = cursor_y - last_cursor_y;
      if (dragging){
         gX += delta_x;
         gY += delta_y;
      }
      last_cursor_x = cursor_x;
      last_cursor_y = cursor_y;
   }
   glutPostRedisplay();
}
void mouse(int button, int state, int x, int y){
   // printf("button=%i, state=%i\n", button, state);
   // fflush(stdout);
   if (button==0) {    // Botão esquerdo
      if (state==0){   // Botão apertado
         mbleft_is_down = 1;
         cursor_x = float(x)/float(TAMANHO_JANELA);
         cursor_y = float(TAMANHO_JANELA - y)/float(TAMANHO_JANELA);
         if (((gX-gW/2)<=cursor_x && cursor_x<=(gX+gW/2)) && ((gY-gH/2)<=cursor_y && cursor_y<=(gY+gH/2))){
            dragging = 1;
         }
      } else {
         mbleft_is_down = 0;
         dragging = 0;
      }
      last_cursor_x = cursor_x;
      last_cursor_y = cursor_y;
   }
   glutPostRedisplay();
}
void passiveMotion(int x, int y)
{
   cursor_x = float(x)/float(TAMANHO_JANELA);
   cursor_y = float(TAMANHO_JANELA - y)/float(TAMANHO_JANELA);
   glutPostRedisplay();
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
    glutMotionFunc(passiveMotion);
    glutIdleFunc(idle);
    glutMouseFunc(mouse);
    glutMainLoop();

    return 0;
}
