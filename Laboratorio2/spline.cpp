#include <GL/gl.h>
#include <GL/glu.h>
#include <stdlib.h>
#include <GL/glut.h>
#define TAMANHO_JANELA 500 // px
#define GRAB_R 30          // px
#define MAX_POINTS 200
#define MIN_POINTS 2
#define STRIDE 3

float size = 5.0;
float last_cursor_x = 0.5;
float last_cursor_y = 0.5;
float cursor_x = 0.5;
float cursor_y = 0.5;
float delta_x = 0;
float delta_y = 0;

//Pontos de controle da Spline
GLfloat ctrlpoints[MAX_POINTS][STRIDE] = {
   {0.2, 0.2, 0.0},
   {0.5, 0.9, 0.0},
   {0.7, 0.1, 0.0},
   {0.9, 0.9, 0.0}};
int dragging[MAX_POINTS] = {0, 0, 0, 0};
int NumPoints = 4;
GLfloat axis_lims[STRIDE][2] = {
        {0, 1}, {0, 1}, {0, 1}  };
void init_ctrlpoint(int pp){
   ctrlpoints[pp][0]=0.5;
   ctrlpoints[pp][1]=0.5;
   ctrlpoints[pp][2]=0;
}

void init(void)
{
   glClearColor(0.0, 0.0, 0.0, 0.0);
   glShadeModel(GL_FLAT);
   glEnable(GL_MAP1_VERTEX_3);

   //Definicao do polinomio com os pontos de controle
   glMap1f(GL_MAP1_VERTEX_3, 0.0, 1.0, STRIDE, NumPoints, &ctrlpoints[0][0]); 
   
   //Muda para a matriz de projecao (aulas futuras)

   glMatrixMode(GL_PROJECTION);
   glLoadIdentity();
   glOrtho(axis_lims[0][0],axis_lims[0][1],
   axis_lims[1][0],axis_lims[1][1],
   axis_lims[2][0],axis_lims[2][1]);
   for(int pp=NumPoints; pp<MAX_POINTS; pp++){
      init_ctrlpoint(pp)
   }
}


void display(void)
{
   int i;

   glClear(GL_COLOR_BUFFER_BIT);

   /* Desenha a curva aproximada por n+1 pontos. */
   int n = 30;
   glColor3f(1.0, 1.0, 1.0);
   glBegin(GL_LINE_STRIP);
      for (i = 0; i <= n; i++){
        //Avaliacao do polinomio, retorna um vertice (equivalente a um glVertex3fv) 
        glEvalCoord1f((GLfloat) i/(GLfloat)n);
      }
   glEnd();
   
   /* Desenha os pontos de controle. */
   glPointSize(5.0);
   glColor3f(1.0, 1.0, 0.0);
   glBegin(GL_POINTS);
      for (i = 0; i < NumPoints; i++) 
         glVertex3fv(&ctrlpoints[i][0]);
   glEnd();
   
   glutSwapBuffers();
}
void mouse(int button, int state, int x, int y){
   // printf("button=%i, state=%i\n", button, state);
   // fflush(stdout);
   if (button==0) {    // Botão esquerdo
      if (state==0){   // Botão apertado
         cursor_x = float(x)/float(TAMANHO_JANELA);
         cursor_y = float(TAMANHO_JANELA - y)/float(TAMANHO_JANELA);

         // Encontra o ponto mais próximo
         float dist_to_cursor_sq;         // Não precisa incializar, será calculado imediatamente
         float dist_to_cursor_sq_min=10;  // Um valor maior que o máximo esperado
         int closest_point;               // Não assuma um ponto mais próximo. Será determinado imediatamente
         for(int pp=0; pp<NumPoints; pp++){
            float dx = cursor_x - ctrlpoints[pp][0];
            float dy = cursor_y - ctrlpoints[pp][1];
            dist_to_cursor_sq = dx*dx + dy*dy;
            // Não precisa tratar o caso de empate. Essa lógica resolve o empate como 'preferir sempre o ponto de menor índice'.
            if (dist_to_cursor_sq < dist_to_cursor_sq_min){
               dist_to_cursor_sq_min = dist_to_cursor_sq;
               closest_point = pp;
            }
         }

         // Verifica o raio de grab. Não precisa fazer para todos os pontos. Só um pode ser o mais próximo.
         if ((dist_to_cursor_sq_min * TAMANHO_JANELA) <= GRAB_R){
            dragging[closest_point] = 1;
         }
      } else {
         // A solução preguiçosa é suficiente. Reset todos os pontos.
         for (int pp; pp<NumPoints; pp++) dragging[pp] = 0;
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
void keyPress(unsigned char key, int x,int y){
   if (key=='+'){
      if (NumPoints>=MAX_POINTS) {
         NumPoints=MAX_POINTS;
         return;
      }
      NumPoints++;
      init_ctrlpoint(NumPoints)
   } 
   if (key=='-'){
      if (NumPoints<=MIN_POINTS) {
         NumPoints=MIN_POINTS;
         return;
      }
      init_ctrlpoint(NumPoints)
      NumPoints--;
   }
   glMap1f(GL_MAP1_VERTEX_3, 0.0, 1.0, STRIDE, NumPoints, &ctrlpoints[0][0]); 
   glutPostRedisplay();
}
void idle(void){
   // Interatividade com os pontos
   for(int pp=0; pp<NumPoints; pp++){
      if (dragging[pp]) {
         delta_x = cursor_x - last_cursor_x;
         delta_y = cursor_y - last_cursor_y;
         ctrlpoints[pp][0] += delta_x;
         ctrlpoints[pp][1] += delta_y;
         last_cursor_x = cursor_x;
         last_cursor_y = cursor_y;
      }
   }

   // Reatividade da curva aos pontos atuais
   glMap1f(GL_MAP1_VERTEX_3, 0.0, 1.0, STRIDE, NumPoints, &ctrlpoints[0][0]);
   glutPostRedisplay();
}
int main(int argc, char** argv)
{
   glutInit(&argc, argv);
   glutInitDisplayMode (GLUT_DOUBLE | GLUT_RGB);
   glutInitWindowSize (TAMANHO_JANELA, TAMANHO_JANELA);
   glutInitWindowPosition (100, 100);
   glutCreateWindow (argv[0]);
   init();
   glutDisplayFunc(display);
   glutMotionFunc(passiveMotion);
   glutIdleFunc(idle);
   glutMouseFunc(mouse);
   glutKeyboardFunc(keyPress);

   glutMainLoop();
   return 0;
}
