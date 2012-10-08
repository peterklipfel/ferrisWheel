/*
 *  Projections
 *  'm' to switch modes (projections)
 *  'a' to toggle axes
 *  '0' snaps angles to 0,0
 *  arrows to rotate the world
 *  PgUp/PgDn zooms in/out
 *  +/- changes field of view of rperspective
 */
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <math.h>
//  OpenGL with prototypes for glext
#define GL_GLEXT_PROTOTYPES
#include <GL/glut.h>

int axes=0;       //  Display axes
int th=0;         //  Azimuth of view angle
int ph=0;         //  Elevation of view angle
int fov=55;       //  Field of view (for perspective)
int num_lights=1;
int spokes = 6;
int mode=0;       //  Depth mode
double asp=1;     //  Aspect ratio
double dim=7.0;   //  Size of world
double rotation = 0;
double vel_division = 900000.0;
int earthquake = 0;


//  Macro for sin & cos in degrees
#define Cos(th) cos(3.1415927/180*(th))
#define Sin(th) sin(3.1415927/180*(th))

/*
 *  Convenience routine to output raster text
 *  Use VARARGS to make this more flexible
 */
#define LEN 8192  //  Maximum length of text string
void Print(const char* format , ...)
{
   char    buf[LEN];
   char*   ch=buf;
   va_list args;
   //  Turn the parameters into a character string
   va_start(args,format);
   vsnprintf(buf,LEN,format,args);
   va_end(args);
   //  Display the characters one at a time at the current raster position
   while (*ch)
      glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18,*ch++);
}

/*
 *  Set projection
 */
static void Project()
{
   //  Tell OpenGL we want to manipulate the projection matrix
   glMatrixMode(GL_PROJECTION);
   //  Undo previous transformations
   glLoadIdentity();
   //  Perspective transformation
   if (mode)
      gluPerspective(fov,asp,dim/4,4*dim);
   //  Orthogonal projection
   else
      glOrtho(-asp*dim,+asp*dim, -dim,+dim, -dim,+dim);
   //  Switch to manipulating the model matrix
   glMatrixMode(GL_MODELVIEW);
   //  Undo previous transformations
   glLoadIdentity();
}

/*
 *  Draw a passenger_box
 *     at (x,y,z)
 *     dimentions (dx,dy,dz)
 *     rotated th about the y axis
 */

static void outer_frame(double center_x, double center_y, 
                        double center_z, double r, 
                        double segments, double step)
{
   int i;
   glBegin(GL_LINE_STRIP);
      for (i=0;i<segments;i++)
      {
         double x = r * cos(i*step+rotation);
         double y = r * sin(i*step+rotation);
         double x2 = r * cos((i+1)*step+rotation);
         double y2 = r * sin((i+1)*step+rotation);
         glColor3f(1.0, 1.0, 0.0);
         glVertex3f(x, y, center_z);
         glVertex3f(x2, y2, center_z); //close the circle
      }
   glEnd(); 
}


static void beam(double x,double y,double z,
                 double dx,double dy,double dz,
                 double th1, double th2, double ph)
{
   //  Save transformation
   glPushMatrix();
   //  Offset
   glTranslated(x,y,z);
   glRotated(th1,ph,1,th2);
   glScaled(dx,dy,dz);
   //  passenger_box
   glBegin(GL_QUADS);
   //  Front
   glVertex3f(-1,-1, 1);
   glVertex3f(+1,-1, 1);
   glVertex3f(+1,+1, 1);
   glVertex3f(-1,+1, 1);
   //  Back
   glVertex3f(+1,-1,-1);
   glVertex3f(-1,-1,-1);
   glVertex3f(-1,+1,-1);
   glVertex3f(+1,+1,-1);
   //  Right
   glVertex3f(+1,-1,+1);
   glVertex3f(+1,-1,-1);
   glVertex3f(+1,+1,-1);
   glVertex3f(+1,+1,+1);
   //  Left
   glVertex3f(-1,-1,-1);
   glVertex3f(-1,-1,+1);
   glVertex3f(-1,+1,+1);
   glVertex3f(-1,+1,-1);
   //  Top
   glVertex3f(-1,+1,+1);
   glVertex3f(+1,+1,+1);
   glVertex3f(+1,+1,-1);
   glVertex3f(-1,+1,-1);
   //  Bottom
   glVertex3f(-1,-1,-1);
   glVertex3f(+1,-1,-1);
   glVertex3f(+1,-1,+1);
   glVertex3f(-1,-1,+1);
   //  End
   glEnd();
   //  Undo transofrmations
   glPopMatrix();
}

static void passenger_box(double x,double y,double z,
                 double dx,double dy,double dz,
                 double th)
{
   //  Save transformation
   glPushMatrix();
   //  Offset
   glTranslated(x,y,z);
   glRotated(th,0,1,0);
   glScaled(dx,dy,dz);
   //  passenger_box
   glBegin(GL_QUADS);
   //  Front
   glColor3f(0.6,0.3,0.3);
   glVertex3f(-1,-1, 1);
   glVertex3f(+1,-1, 1);
   glVertex3f(+1,+1, 1);
   glVertex3f(-1,+1, 1);
   //  Back
   glColor3f(0.6,0.3,0.3);
   glVertex3f(+1,-1,-1);
   glVertex3f(-1,-1,-1);
   glVertex3f(-1,+1,-1);
   glVertex3f(+1,+1,-1);
   //  Right
   glColor3f(0.6,0.3,0.3);
   glVertex3f(+1,-1,+1);
   glVertex3f(+1,-1,-1);
   glVertex3f(+1,+1,-1);
   glVertex3f(+1,+1,+1);
   //  Left
   glColor3f(0.6,0.3,0.3);
   glVertex3f(-1,-1,-1);
   glVertex3f(-1,-1,+1);
   glVertex3f(-1,+1,+1);
   glVertex3f(-1,+1,-1);
   //  Bottom
   glColor3f(1,1,1);
   glVertex3f(-1,-1,-1);
   glVertex3f(+1,-1,-1);
   glVertex3f(+1,-1,+1);
   glVertex3f(-1,-1,+1);
   //  End
   glEnd();
   //  Undo transofrmations
   glPopMatrix();
}

static void Vertex(double th,double ph)
{
   glVertex3d(Sin(th)*Cos(ph) , Sin(ph) , Cos(th)*Cos(ph));
}

static void light(double x,double y,double z,double r, double start_angle, double end_angle)
{
   const int d=5;
   int th,ph;

   //  Save transformation
   glPushMatrix();
   //  Offset and scale
   glTranslated(x,y,z);
   glScaled(r,r,r);
   glColor3f(sin(10*x), sin(7*x+3*y), cos(y*y+x*4));
   //  Latitude bands
   for (ph=start_angle;ph<end_angle;ph+=d)
   {
      glBegin(GL_QUAD_STRIP);
      for (th=90;th<=270;th+=d)
      {
         Vertex(th,ph);
         Vertex(th,ph+d);
      }
      glEnd();
   }

   //  Undo transformations
   glPopMatrix();
}

static void ferris_wheel(double x, double y , double z, double size)
{
   double pi = 3.14159265358979323846;
   double step = 2*pi/10;
   double r = 3;
   int i;
   //  Draw passenger_boxes
   glPushMatrix();
   glTranslated(x,y,z);
   glScaled(size,size,size);
   for (i=0;i<10;i++)
   {
      double passenger_box_x = r * cos(i*step+rotation);
      double passenger_box_y = r * sin(i*step+rotation);
      passenger_box(passenger_box_x, passenger_box_y,0 , 0.3,0.3,0.3 , 0);
   }

   // Draw Circle
   outer_frame(0, 0, -0.31, r, 10, step);
   outer_frame(0, 0, 0.31, r, 10, step);


   // Draw the spokes
   glColor3f(0.5,0.5,0.5);
   double conversion = 180/pi;
   int current_spoke;
   int num_spokes = spokes;

   double offset = 0;
   for(current_spoke = 0; current_spoke < num_spokes; current_spoke++)
   {
      offset = current_spoke*(180/num_spokes);
      beam(0,0,-0.3, r-0.1, 0.1, 0.1,offset + conversion*rotation, offset+conversion*rotation, 0);
      beam(0,0,0.3, r-0.1, 0.1, 0.1,offset + conversion*rotation, offset+conversion*rotation, 0);
   }

   // center axis
   glColor3f(0.2,0.5,0.2);
   beam(0,0,0, 1, 0.1, 0.1, 90, 0, 0);

   // braces
   glColor3f(0.2,0.2,0.5);
   beam(1.7,-1.7,-1, 2.5, 0.1, 0.1, 135, 135, 0);
   beam(-1.7,-1.7,-1, 2.5, 0.1, 0.1, 45, 45, 0);
   beam(1.7,-1.7,1, 2.5, 0.1, 0.1, 135, 135, 0);
   beam(-1.7,-1.7,1, 2.5, 0.1, 0.1, 45, 45, 0);


   if(num_lights){
      int spacing;
      r = r-0.1;
      num_spokes = num_spokes*2;
      double x_angle, y_angle, radius;
      for(current_spoke = 0; current_spoke < num_spokes; current_spoke++)
      {
         offset = current_spoke*2*pi/num_spokes;
         for(spacing = 1; spacing < num_lights+1; spacing++)
         {
            // light(spacing*r*cos(offset + rotation)/num_lights, 
            //       spacing*r*sin(offset + rotation)/num_lights, 0.35, 0.1, 90, 270);
            radius = spacing*r/num_lights;
            x_angle = cos(offset + rotation);
            y_angle = sin(offset + rotation);

            light(radius*x_angle, radius*y_angle, 0.35, 0.1, 90, 270);
            light(radius*x_angle, radius*y_angle, -0.35, 0.1, -90, 90);
         }
      }
   }
   glPopMatrix();
}

/*
 *  OpenGL (GLUT) calls this routine to display the scene
 */
void display()
{
   const double len=1.5;  //  Length of axes
   //  Erase the window and the depth buffer
   glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
   //  Enable Z-buffering in OpenGL
   glEnable(GL_DEPTH_TEST);
   //  Undo previous transformations
   glLoadIdentity();
   //  Perspective - set eye position

   if (mode)
   {
      double Ex = -2*dim*Sin(th)*Cos(ph);
      double Ey = +2*dim        *Sin(ph);
      double Ez = +2*dim*Cos(th)*Cos(ph);
      gluLookAt(Ex,Ey,Ez , 0,0,0 , 0,Cos(ph),0);
   }
   //  Orthogonal - set world orientation
   else
   {
      glRotatef(ph,1,0,0);
      glRotatef(th,0,1,0);
   }

   double ferris1x = 0;
   double ferris2x = 0;
   if (earthquake)
   {
      ferris1x = sin(500*rotation);
      ferris2x = cos(630*rotation);
   }

   ferris_wheel(ferris1x/10, 0, -5, 1);
   ferris_wheel(ferris2x/10, 0, 0, 1);

   //  Draw axes
   glColor3f(1,1,1);
   if (axes)
   {
      glBegin(GL_LINES);
      glVertex3d(0.0,0.0,0.0);
      glVertex3d(len,0.0,0.0);
      glVertex3d(0.0,0.0,0.0);
      glVertex3d(0.0,len,0.0);
      glVertex3d(0.0,0.0,0.0);
      glVertex3d(0.0,0.0,len);
      glEnd();
      //  Label axes
      glRasterPos3d(len,0.0,0.0);
      Print("X");
      glRasterPos3d(0.0,len,0.0);
      Print("Y");
      glRasterPos3d(0.0,0.0,len);
      Print("Z");
   }
   //  Display parameters
   glWindowPos2i(5,5);
   Print("Angle=%d,%d  Dim=%.1f FOV=%d vel=%.5f Projection=%s",th,ph,dim,fov,10000000/vel_division, mode?"Perpective":"Orthogonal");
   //  Render the scene and make it visible
   glFlush();
   glutSwapBuffers();
}

/*
 *  GLUT calls this routine when an arrow key is pressed
 */
void special(int key,int x,int y)
{
   //  Right arrow key - increase angle by 5 degrees
   if (key == GLUT_KEY_RIGHT)
      th += 5;
   //  Left arrow key - decrease angle by 5 degrees
   else if (key == GLUT_KEY_LEFT)
      th -= 5;
   //  Up arrow key - increase elevation by 5 degrees
   else if (key == GLUT_KEY_UP)
      ph += 5;
   //  Down arrow key - decrease elevation by 5 degrees
   else if (key == GLUT_KEY_DOWN)
      ph -= 5;
   //  PageUp key - increase dim
   else if (key == GLUT_KEY_PAGE_UP)
      dim += 0.1;
   //  PageDown key - decrease dim
   else if (key == GLUT_KEY_PAGE_DOWN && dim>1)
      dim -= 0.1;
   //  Keep angles to +/-360 degrees
   th %= 360;
   ph %= 360;
   //  Update projection
   Project();
   //  Tell GLUT it is necessary to redisplay the scene
   glutPostRedisplay();
}

/*
 *  GLUT calls this routine when a key is pressed
 */
void key(unsigned char ch,int x,int y)
{
   //  Exit on ESC
   if (ch == 27)
      exit(0);
   //  Reset view angle
   else if (ch == 'd' || ch == 'D')
   {
      th = ph = 0;
      fov=55;
      num_lights=1;
      asp=1;     //  Aspect ratio
      dim=5.0;
   }
   //  Toggle axes
   else if (ch == 'a' || ch == 'A')
      axes = 1-axes;
   //  Change field of view angle
   else if (ch == '-' && ch>1)
      fov--;
   else if (ch == '+' && ch<179)
      fov++;
   else if (ch == 'l')
   {
      if (num_lights > 0)
      {
         num_lights = num_lights - 1;
      }
      else{
         num_lights = 0;
      }
   }
   else if (ch == 'L')
      num_lights = num_lights + 1;
   else if (ch == 's')
      if (spokes > 1){
         spokes = spokes - 1;
      }
      else{
         spokes = 1;
      }
   else if (ch == 'S')
   {
      if (spokes < 6){
         spokes++;
      }
      else{
         spokes = 6;
      }
   }
   else if (ch == 'V')
      if(vel_division > 50000.0){
         vel_division -= 50000.0;
      }
      else {
         vel_division = 1;
      }
   else if (ch == 'v')
      vel_division += 50000.0;
   else if (ch == 'e' || ch == 'E')
      earthquake = 1 - earthquake;
   else if (ch == 'm' || ch == 'M')
      mode = 1-mode;
   //  Reproject
   Project();
   //  Tell GLUT it is necessary to redisplay the scene
   glutPostRedisplay();
}

/*
 *  GLUT calls this routine when the window is resized
 */
void reshape(int width,int height)
{
   //  Ratio of the width to the height of the window
   asp = (height>0) ? (double)width/height : 1;
   //  Set the viewport to the entire window
   glViewport(0,0, width,height);
   //  Set projection
   Project();
}

void idle()
{
   //  Get elapsed (wall) time in seconds
   double t = glutGet(GLUT_ELAPSED_TIME)/vel_division;
   //  Calculate spin angle 90 degrees/second
   rotation = fmod(90*t,360);
   //  Request display update
   glutPostRedisplay();
}


/*
 *  Start up GLUT and tell it what to do
 */
int main(int argc,char* argv[])
{
   //  Initialize GLUT
   glutInit(&argc,argv);
   //  Request double buffered, true color window with Z buffering at 600x600
   glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE);
   glutInitWindowSize(600,600);
   glutCreateWindow("Ferris Wheel");
   //  Set callbacks
   glutDisplayFunc(display);
   glutReshapeFunc(reshape);
   glutIdleFunc(idle);
   glutSpecialFunc(special);
   glutKeyboardFunc(key);
   //  Pass control to GLUT so it can interact with the user
   glutMainLoop();
   return 0;
}
