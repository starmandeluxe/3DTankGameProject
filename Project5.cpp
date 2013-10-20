/*
 * Alex Kort
 * Project 5
 * CPE 474
 * Animated Game
 *
 */


#include <windows.h>
#include <stdio.h>
#include <math.h>
#include <gl/gl.h>
#include <gl/glu.h>
#include <gl/glut.h>
#include <iostream>
//#include "glfont.h"
#include <cstdlib>

// some function prototypes
void display(void);
void normalize(float[3]);
void normCrossProd(float[3], float[3], float[3]);
void processKeyboard();
void processKeyboard2();
void drawTank1();
void drawTank2();


void sumForces(void);
void EulerIntegrate(void);
void initializeParticleDataStructure(float power, bool player);
void firstParticleDataStructure(); 

void sumForces2(void);
void EulerIntegrate2(void);
void initializeParticleDataStructure2(float power, bool player);
void firstParticleDataStructure2(); 

// initial viewer position
static GLdouble viewer[] = {0.0, 0.0, 5.0};
// initial viewer angle
static GLfloat theta[] = {0.0, 0.0, 0.0};

//particles
const int NUM_PARTICLES = 1;
const float GRAVITY = 9.8;
const float DRAG = 0.01;
const float WIND = 1;
const float TIMESTEP = 0.05;

//player 1 bullet particle
float mass[NUM_PARTICLES];
float pos[NUM_PARTICLES][3];
float vel[NUM_PARTICLES][3];
float acc[NUM_PARTICLES][3];
float force[NUM_PARTICLES][3];

//player 2 bullet particle
float mass2[NUM_PARTICLES];
float pos2[NUM_PARTICLES][3];
float vel2[NUM_PARTICLES][3];
float acc2[NUM_PARTICLES][3];
float force2[NUM_PARTICLES][3];

// animation variables
static int frame = 0;
static int startFrame = 0;
static int endFrame = 100;
static int increment = 1;

static GLUquadric* quad;

static float initpow = 0.0;
static float endpow = 0.0;
static float initpow2 = 0.0;
static float endpow2 = 0.0;

static float power = 0.0;
static float power2 = 0.0;
//counts the time to calculate power, always incrementing
static float tcount = 0.0;
static float tcount2 = 0.0;

// animation transform variables
static GLdouble t1trans[3] = {-10.0, 0.0, 4.50};
static GLdouble t2trans[3] = {10.0, 0.0, 4.50};
static GLdouble sheet1trans[3] = {0.0, 0.0, 4};

//rotation and angle for player 1 left
static int t1rotate = 140;
static int ang1 = 0;

//rotation and angle for player 2 right
static int t2rotate = -20;
static int ang2 = 0;

static int health1 = 20;
static int health2 = 20;

static int powerct = 0;
static int powerct2 = 0;

static bool keys[256];
static bool bulletflag1 = 0;
//flag to check if the spacebar was held

//flags for player 1 on left
static bool holdflag = 0;
static bool pressed = 0;
static bool lockflag = 0;
static bool hitflag = 0;
static bool stopflag1 = 0;

//flags for player 2 on right
static bool holdflag2 = 0;
static bool pressed2 = 0;
static bool lockflag2 = 0;
static bool hitflag2 = 0;
static bool bulletflag2 = 0;
static bool stopflag2 = 0;

//flag that switches turns between players
static bool playerflag = 0;

//GLFONT font;

//---------------------------------------------------------
//   Set up the view

void setUpView() {
   // this code initializes the viewing transform
   glLoadIdentity();

   // moves viewer along coordinate axes
   gluLookAt(viewer[0], viewer[1], viewer[2], 
             0.0, 0.0, 0.0, 0.0, 1.0, 0.0);

   // move the view back some relative to viewer[] position
   glTranslatef(0.0f,-5.0f, -17.0f);
	//glTranslatef(0.0f,0.0f, -5.0f);

   // rotates view
   glRotatef(theta[0], 1.0, 0.0, 0.0);
   glRotatef(theta[1], 0.0, 1.0, 0.0);
   glRotatef(theta[2], 0.0, 0.0, 1.0);

	quad=gluNewQuadric();
	gluQuadricNormals(quad, GLU_SMOOTH);
	gluQuadricTexture(quad, GL_TRUE);

	//Create font
	//glFontCreate(&font, "arial.glf", 0);


   return;
}

//----------------------------------------------------------
//  Set up the light

void setUpLight() {
   // set up the light sources for the scene
   // a directional light source from over the right shoulder
	glEnable(GL_LIGHTING);

	GLfloat light0_diffuse[] = {1.0, 1.0, 1.0, 1.0};
   GLfloat light0_position[] = {-1.0, 6.0, 4.0, 0.0};
   GLfloat light1_diffuse[] = {.4, .4, .4, 1.0};
   GLfloat light1_position[] = {1.0, 6.0, 0.0, 0.0};
   GLfloat light2_diffuse[] = {.4, .4, .4, 1.0};
   GLfloat light2_position[] = {0.0, 6.0, -4.0, 0.0};
   GLfloat ambient[] = {1.0f, 1.0f, 1.0f, 1.0f};
   GLfloat light4_diffuse[] = {1, 1, 1, 1.0};
   GLfloat light4_position[] = {0.0, 0, 9.0, 0.0};
	

   glEnable(GL_LIGHT0);
   glEnable(GL_LIGHT1);
   glEnable(GL_LIGHT2);
   glEnable(GL_LIGHT3);
   glEnable(GL_LIGHT4);
	
   glLightfv(GL_LIGHT0, GL_DIFFUSE, light0_diffuse);
   glLightfv(GL_LIGHT0, GL_POSITION, light0_position);
   glLightfv(GL_LIGHT1, GL_DIFFUSE, light1_diffuse);
   glLightfv(GL_LIGHT1, GL_POSITION, light1_position);
   glLightfv(GL_LIGHT2, GL_DIFFUSE, light2_diffuse);
   glLightfv(GL_LIGHT2, GL_POSITION, light2_position);
   glLightfv(GL_LIGHT3, GL_AMBIENT, ambient);
	glLightfv(GL_LIGHT4, GL_DIFFUSE, light4_diffuse);
   glLightfv(GL_LIGHT4, GL_POSITION, light4_position);
	

   return;
}

//--------------------------------------------------------
//  Set up the objects

void drawTank1() 
{
   // draw an object

   // save the transformation state
   glPushMatrix();

   // set the material

	GLfloat material_ambient[] = {0.0f, 0.0f, 0.0f};
	GLfloat material_diffuse[] = {.2, 0.5, .8, 1.0};
	GLfloat material_specular[] = {0.20f, 0.140f, 1.00f};
	GLfloat material_shininess = 30.0f;

	glMaterialfv(GL_FRONT, GL_AMBIENT, material_ambient);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, material_diffuse);
	glMaterialfv(GL_FRONT, GL_SPECULAR, material_specular);
	glMaterialf(GL_FRONT, GL_SHININESS, material_shininess);


   // locate it in the scene
   glMatrixMode(GL_MODELVIEW);
   // this translation will be used to animate the sphere
   glTranslatef(t1trans[0], t1trans[1], t1trans[2]);	 
   glRotatef(90, 1.0, 0.0, 0.0);  

	
   // draw the sphere - the parameters are radius, number of
   //   radial slices (longitude lines) and number of vertical
   //   slices (latitude lines)
   glutSolidSphere(1.0, 30.0, 30.0);

   // recover the transform state
   glPopMatrix();

	glPushMatrix();
   // set the material

	glMaterialfv(GL_FRONT, GL_AMBIENT, material_ambient);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, material_diffuse);
	glMaterialfv(GL_FRONT, GL_SPECULAR, material_specular);
	glMaterialf(GL_FRONT, GL_SHININESS, material_shininess);


   // locate it in the scene
   glMatrixMode(GL_MODELVIEW);
   // this translation will be used to animate the sphere
   glTranslatef(t1trans[0], t1trans[1], t1trans[2]);	 
   glRotatef(t1rotate, 1.0, 1.0, 1.0);  
	//glRotatef(45, 1.0, 0.0, 1.0);

	//2nd param is radius at bottom, 3rd is radius at top
	//4th is height (length), 5th is subdivisions around z,
	//6th is subdivisions along z
   gluCylinder(quad,.3f,.3f,2.0f,32,32);

   // recover the transform state
   glPopMatrix();

   return;
}

void drawTank2() 
{
   // draw an object
   // save the transformation state
   glPushMatrix();
   // set the material
   //GLfloat diffuseColor[] = {1.0, .4.0, .4.0, 1.0};
   //glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuseColor);

	GLfloat material_ambient[] = {0.0f, 0.0f, 0.0f};
	GLfloat material_diffuse[] = {.9, 0.3, .2, 1.0};
	GLfloat material_specular[] = {0.20f, 0.140f, 1.00f};
	GLfloat material_shininess = 25.0f;

	glMaterialfv(GL_FRONT, GL_AMBIENT, material_ambient);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, material_diffuse);
	glMaterialfv(GL_FRONT, GL_SPECULAR, material_specular);
	glMaterialf(GL_FRONT, GL_SHININESS, material_shininess);


   // locate it in the scene
   glMatrixMode(GL_MODELVIEW);
   // this translation will be used to animate the sphere
   glTranslatef(t2trans[0], t2trans[1], t2trans[2]);	 
   glRotatef(90, 1.0, 0.0, 0.0);  

   // draw the sphere - the parameters are radius, number of
   //   radial slices (longitude lines) and number of vertical
   //   slices (latitude lines)
   glutSolidSphere(1.0, 30, 30);

   // recover the transform state
   glPopMatrix();


	glPushMatrix();
   // set the material
   //GLfloat diffuseColor[] = {1.0, .4.0, .4.0, 1.0};
   //glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuseColor);

	glMaterialfv(GL_FRONT, GL_AMBIENT, material_ambient);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, material_diffuse);
	glMaterialfv(GL_FRONT, GL_SPECULAR, material_specular);
	glMaterialf(GL_FRONT, GL_SHININESS, material_shininess);


   // locate it in the scene
   glMatrixMode(GL_MODELVIEW);
   // this translation will be used to animate the sphere
   glTranslatef(t2trans[0], t2trans[1], t2trans[2]);	 
   glRotatef(-90, 0.0, 1.0, 0.0);
	glRotated(t2rotate, 1.0, 1.0, 1.0);

	//2nd param is radius at bottom, 3rd is radius at top
	//4th is height (length), 5th is subdivisions around z,
	//6th is subdivisions along z
   gluCylinder(quad,.3f,.3f,2.0f,32,32);

   // recover the transform state
   glPopMatrix();

   return;
}

void drawGround()
{
// draw an object

   // save the transformation state
   glPushMatrix();
   // set the material
   GLfloat diffuseColor[] = {0.9, 0.6, 0.8, 1.0};
   glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuseColor);

   // locate it in the scene
   glMatrixMode(GL_MODELVIEW);
   // this translation will be used to animate the cube
   glTranslatef(0, -1, 0);	 
   glRotatef(90, .5, 0.0, 0.0);  
   glScalef(20.0,8,1);

   
   glutSolidCube(2.0);

   // recover the transform state
   glPopMatrix();

   return;
}

void drawSheet1() 
{

   // save the transformation state
   glPushMatrix();

   // set the material
   GLfloat diffuseColor[] = {0, .5, 1, 1.0};
   glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuseColor);

   // locate it in the scene
   glMatrixMode(GL_MODELVIEW);
   // this translation will be used to animate the sphere
	
   glTranslatef(sheet1trans[0], sheet1trans[1], sheet1trans[2]);	 
   //glRotatef(-90, 1.0, 0.0, 0.0);  
	glRotatef(90, 1, 0, 0.0); 
	glRotatef(5, 0, 0, 1.0); 
   glScalef(.4,6,10);
   // draw the cone - the parameters are radius, height, number of
   //   radial slices and number of vertical slices
   glutSolidCube(1.0);

   // recover the transform state
   glPopMatrix();

   return;
}

void drawBullet1()
{
	for (int i=0; i<NUM_PARTICLES; i++) {
      // for each particle

      // save the transformation state
      
	glPushMatrix();
      // this translation will be used to animate the particle
      glTranslatef(pos[i][0], pos[i][1], pos[i][2]);	 

		// locate it in the scene
		glMatrixMode(GL_MODELVIEW);
		// this translation will be used to animate the sphere
		glRotatef(90, 1.0, 0.0, 0.0); 

		// draw the sphere - the parameters are radius, number of
		//   radial slices (longitude lines) and number of vertical
		//   slices (latitude lines)
		GLfloat material_ambient[] = {0.0f, 0.0f, 0.0f};
	   GLfloat material_diffuse[] = {.2, 0.6, 1.0, 1.0};
	   GLfloat material_specular[] = {0.20f, 0.140f, 1.00f};
	   GLfloat material_shininess = 25.0f;

	   glMaterialfv(GL_FRONT, GL_AMBIENT, material_ambient);
	   glMaterialfv(GL_FRONT, GL_DIFFUSE, material_diffuse);
	   glMaterialfv(GL_FRONT, GL_SPECULAR, material_specular);
	   glMaterialf(GL_FRONT, GL_SHININESS, material_shininess);

		glutSolidSphere(.2, 20, 20); 

     // recover the transform state
      glPopMatrix();
   }
   return;
}

void drawBullet2()
{
	for (int i=0; i<NUM_PARTICLES; i++) {
      // for each particle

      // save the transformation state
      
	glPushMatrix();
      // this translation will be used to animate the particle
      glTranslatef(pos2[i][0], pos2[i][1], pos2[i][2]);	 

		// locate it in the scene
		glMatrixMode(GL_MODELVIEW);
		// this translation will be used to animate the sphere
		glRotatef(90, 1.0, 0.0, 0.0); 

		// draw the sphere - the parameters are radius, number of
		//   radial slices (longitude lines) and number of vertical
		//   slices (latitude lines)
		GLfloat material_ambient[] = {0.0f, 0.0f, 0.0f};
	   GLfloat material_diffuse[] = {1, 0.4, .3, 1.0};
	   GLfloat material_specular[] = {0.20f, 0.140f, 1.00f};
	   GLfloat material_shininess = 25.0f;

	   glMaterialfv(GL_FRONT, GL_AMBIENT, material_ambient);
	   glMaterialfv(GL_FRONT, GL_DIFFUSE, material_diffuse);
	   glMaterialfv(GL_FRONT, GL_SPECULAR, material_specular);
	   glMaterialf(GL_FRONT, GL_SHININESS, material_shininess);

		glutSolidSphere(.2, 20, 20); 

     // recover the transform state
      glPopMatrix();
   }
   return;
}


void drawt1Text()
{
	/*glFontBegin(&font);
	glScalef(8.0, 8.0, 8.0);
	glTranslatef(3, 3, 0);
	glFontTextOut("Test", 5, 5, 0);
	glFontEnd();
	glFlush();
*/
}




void timeStep(int step) {
   // animation code goes here
   
   // This function is called for each frame of animation
   double t = (double) (frame - startFrame) / (endFrame - startFrame);

	sheet1trans[1] = 3 + t * (3 - (8));
	int i;
	for (i=0; i<NUM_PARTICLES; i++) 
	{
		//printf("pos[i][1]: %f\n", pos[i][1]);
		//printf("sheet[1]: %f\n", sheet1trans[1]);
		if (pos[i][1] < -1 || 
			((abs(pos[i][1] - sheet1trans[1]) < 5) && (abs(pos[i][0] - sheet1trans[0]) < .02)))
		{
			//DO A BURST HERE?

			mass[i] = 1.0;


			acc[i][0] = 0.0;
			acc[i][1] = 0.0;
			acc[i][2] = 0.0;
      
			force[i][0] = 0.0;
			force[i][1] = 0.0;
			force[i][2] = 0.0;

			bulletflag1 = 0;
			lockflag = 0;
			hitflag = 0;
			stopflag1 = 1;

		}
	}

	for (i=0; i<NUM_PARTICLES; i++) 
	{
		if (pos2[i][1] < -1|| 
			((abs(pos2[i][1] - sheet1trans[1]) < 5) && (abs(pos2[i][0] - sheet1trans[0]) < .02)))
		{
			//DO A BURST HERE?
			mass2[i] = 1.0;

			acc2[i][0] = 0.0;
			acc2[i][1] = 0.0;
			acc2[i][2] = 0.0;
      
			force2[i][0] = 0.0;
			force2[i][1] = 0.0;
			force2[i][2] = 0.0;

			bulletflag2 = 0;
			lockflag2 = 0;
			hitflag2 = 0;
			stopflag2 = 1;
		}
	}
	sumForces();
   EulerIntegrate();

	sumForces2();
   EulerIntegrate2();

   if (frame == endFrame) increment = -1;
   else if (frame == startFrame) increment = 1;
    
   frame = frame + increment;


   tcount += .01;
	tcount2 += .01;
	//printf("tcount = %f\n", tcount);

   display();

   glutTimerFunc(50,timeStep, 0);

}

//---------------------------------------------------------
//  Main routines

void display (void) {
   // this code executes whenever the window is redrawn (when opened,
   //   moved, resized, etc.
   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

   // set the viewing transform
   setUpView();
	
   // set up light source
   setUpLight();
   // start drawing objects
	drawt1Text();
   drawTank1();
	drawTank2();
   //drawCone();
	drawGround();
	drawSheet1();

	if (bulletflag1 == 1)
	{
		drawBullet1();
	}
	if (bulletflag2 == 1)
	{
		drawBullet2();
	}

   glutSwapBuffers();
}

//-----------------------------------------------------------
//  Callback functions

void reshapeCallback(int w, int h) {
   // from Angel, p.562

   glViewport(0,0,w,h);

   glMatrixMode(GL_PROJECTION);
   glLoadIdentity();
   if (w < h) {
      glFrustum(-2.0, 2.0, -2.0*(GLfloat) h / (GLfloat) w,
                2.0*(GLfloat) h / (GLfloat) w, 2.0, 20.0);
   }
   else {
      glFrustum(-2.0, 2.0, -2.0*(GLfloat) w / (GLfloat) h,
                2.0*(GLfloat) w / (GLfloat) h, 2.0, 20.0);
   }

   glMatrixMode(GL_MODELVIEW);
}

//spacebar = ' ' 
void keyCallback(unsigned char key, int x, int y) 
{

	  if (key=='a')
          keys[4]=true;         // mark left as being pressed
     if (key=='d')
          keys[5]=true;         // mark right as being pressed
	  if (key=='w')
          keys[6]=true;         // mark up as being pressed
     if (key=='s')
          keys[7]=true;         // mark down as being pressed
	  if (key==' ')
	  {
		    pressed = 1;
			 printf("power: %d\n", powerct);
			 powerct += 2; 
			 if (powerct > 100)
			 {
				 powerct = 100;
			 }
		    if (holdflag == 0)
			 {
				initpow = tcount;
				holdflag = 1;
			 }
          keys[8]=true;         // mark spacebar as being pressed
	  }
	  if (key=='+')
	  {
		    pressed2 = 1;
			 printf("power2: %d\n", powerct2);
			 powerct2 += 2; 
			 if (powerct2 > 100)
			 {
				 powerct2 = 100;
			 }
		    if (holdflag2 == 0)
			 {
				initpow2 = tcount2;
				holdflag2 = 1;
			 }
          keys[9]=true;         // mark spacebar as being pressed
	  }

	  processKeyboard();
}

void processKeysUp(unsigned char key, int x, int y)
{

     if (key=='a')
          keys[4]=false;         // mark up as being released
     if (key=='d')
          keys[5]=false;         // mark down as being released
	  if (key=='w')
          keys[6]=false;         // mark up as being released
     if (key=='s')
          keys[7]=false;         // mark down as being released
	  if (key==' ')
	  {
			 if (powerct > 100)
			 {
			    powerct = 100;
			 }
			 printf("power up: %d\n", powerct);
		    powerct = 0;
		    holdflag = 0;
		    endpow = tcount;
          keys[8]=false;         // mark spacebar as being released
	  }
	  if (key=='+')
	  {
			 if (powerct2 > 100)
			 {
			    powerct2 = 100;
			 }
			 printf("power2 up: %d\n", powerct2);
		    powerct2 = 0;
		    holdflag2 = 0;
		    endpow2 = tcount2;
          keys[9]=false;         // mark spacebar as being released
	  }

	  processKeyboard();
}

void special(int key, int x, int y)
{
	  if (key==GLUT_KEY_LEFT)
          keys[0]=true;         // mark up as being pressed
     if (key==GLUT_KEY_RIGHT)
          keys[1]=true;         // mark down as being pressed
	  if (key==GLUT_KEY_UP)
          keys[2]=true;         // mark up as being pressed
     if (key==GLUT_KEY_DOWN)
          keys[3]=true;         // mark down as being pressed

	  processKeyboard();
}

void processSpecialKeysUp(int key, int x, int y)
{
     if (key==GLUT_KEY_LEFT)
          keys[0]=false;         // mark up as being released
     if (key==GLUT_KEY_RIGHT)
          keys[1]=false;         // mark down as being released
	  if (key==GLUT_KEY_UP)
          keys[2]=false;         // mark up as being pressed
     if (key==GLUT_KEY_DOWN)
          keys[3]=false;         // mark down as being pressed

	  processKeyboard();
}


void processKeyboard()
{
	  //translate player 2
     if(keys[0]==true)
     {
          t2trans[0] -= 0.3;
          if(t2trans[0] < 4)
          {
               t2trans[0] += 0.3;
          }
     }
     if(keys[1] == true)
     {
           t2trans[0]+= 0.3;
          if(t2trans[0] > 18)
          {
               t2trans[0] -= 0.3;
          }
	  }
	  //rotate player 2
	  if(keys[2] == true)
     {
          t2rotate -= 2;
			 ang2 += 2;
			 printf("ang2: %d\n", ang2);
          if(t2rotate < -108)
          {
               t2rotate += 2;
					ang2 -= 2;
          }
     }
	  if(keys[3]==true)
     {
          t2rotate += 2;
			 ang2 -= 2;
			 printf("ang2: %d\n", ang2);
          if(t2rotate > -24)
          {
               t2rotate -= 2;
					ang2 += 2;
          }
     }
	  //translate player 1
	  if(keys[4]==true)
     {
          t1trans[0] -= 0.3;
          if(t1trans[0] < -18)
          {
               t1trans[0] +=0.3;
          }
     }
     if(keys[5] == true)
     {
           t1trans[0] += 0.3;
          if(t1trans[0] > -4)
          {
               t1trans[0] -=0.3;
          }
     }
	  //rotate player 1
	  if(keys[6] == true)
     {
          t1rotate += 2;
			 ang1 += 2;
			 printf("ang1: %d\n", ang1);
          if(t1rotate > 228)
          {
               t1rotate -= 2;
					ang1 -= 2;
          }
     }
	  if(keys[7]==true)
     {
          t1rotate -= 2;
			 ang1 -= 2;
			 printf("ang1: %d\n", ang1);
          if(t1rotate < 140)
          {
               t1rotate += 2;
					ang1 += 2;
          }
     }
	  //press spacebar handling
	  if(keys[8]==false && pressed == 1)
     {
		  pressed = 0;
		  powerct = 0;
		  power = endpow - initpow;
		  //printf("power: %f\n", power);
		  if (power > .5)
		  {
			  power = .5;
		  }

		  if (lockflag == 0)
		  {
			  lockflag = 1;
			  stopflag1 = 0;
			  initializeParticleDataStructure(power, 0);
			  bulletflag1 = 1;
		  }
     }
	  //press + handling
	  if(keys[9]==false && pressed2 == 1)
     {
		  pressed2 = 0;
		  powerct2 = 0;
		  power2 = endpow2 - initpow2;
		  //printf("power: %f\n", power);
		  if (power2 > .5)
		  {
			  power2 = .5;
		  }

		  if (lockflag2 == 0)
		  {
			  lockflag2 = 1;
			  stopflag2 = 0;
			  initializeParticleDataStructure(power2, 1);
			  bulletflag2 = 1;
		  }
     }
}

// create a double buffered 800x800 pixel color window
int main(int argc, char* argv[]) 
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowSize(800, 800);
	glutInitWindowPosition(100, 100);

	glutCreateWindow("Animation Project 5: Tank Game");

	glClearColor(.3, .4, .5, .2);

	glEnable(GL_DEPTH_TEST);
	
	glutDisplayFunc(display);

	firstParticleDataStructure(); 
	firstParticleDataStructure2();
   //glutSetKeyRepeat(GLUT_KEY_REPEAT_OFF ); 
	glutIgnoreKeyRepeat(0); 
   glutReshapeFunc(reshapeCallback);

	glutKeyboardFunc(keyCallback);
	glutKeyboardUpFunc (processKeysUp); 

	glutSpecialFunc(special);
	glutSpecialUpFunc (processSpecialKeysUp); 

	glutTimerFunc(50,timeStep, 0);  // 50 millisecond callback

	glutMainLoop();
	return 0;
}



//---------------------------------------------------------
//  Utility functions

void normalize(float v[3]) {
   // normalize v[] and return the result in v[]
   // from OpenGL Programming Guide, p. 58
   GLfloat d = sqrt(v[0]*v[0] + v[1]*v[1] + v[2]*v[2]);
   if (d == 0.0) {
      printf("zero length vector");
      return;
   }
   v[0] = v[0]/d; v[1] = v[1]/d; v[2] = v[2]/d;
}

void normCrossProd(float v1[3], float v2[3], float out[3]) {
   // cross v1[] and v2[] and return the result in out[]
   // from OpenGL Programming Guide, p. 58
   out[0] = v1[1]*v2[2] - v1[2]*v2[1];
   out[1] = v1[2]*v2[0] - v1[0]*v2[2];
   out[2] = v1[0]*v2[1] - v1[1]*v2[0];
   normalize(out);
}

void initializeParticleDataStructure(float power, bool player) 
{
   // initial values for all the particles

   for (int i=0; i<NUM_PARTICLES; i++) 
	{
      mass[i] = 1.0;

		if (player == 0)
		{
			pos[i][0] = t1trans[0];
			pos[i][1] = t1trans[1]+.5;
			pos[i][2] = t1trans[2]-.5;
			//printf("power: %f\n", power);
			double radians = (3.14159 / 180) * ang1;
			vel[i][0] = power*70 * cos(radians);

			vel[i][1] = power*70 * sin(radians);
		}
		else if (player == 1)
		{
			pos2[i][0] = t2trans[0];
			pos2[i][1] = t2trans[1]+.5;
			pos2[i][2] = t2trans[2]-.5;
			//printf("power2: %f\n", power2);
			double radians = (3.14159 / 180) * ang2;
			vel2[i][0] = power2*70 * -cos(radians);

			vel2[i][1] = power2*70 * sin(radians);
		}

      acc[i][0] = 0.0;
      acc[i][1] = 0.0;
      acc[i][2] = 0.0;
      
      force[i][0] = 0.0;
      force[i][1] = 0.0;
      force[i][2] = 0.0;

		acc2[i][0] = 0.0;
      acc2[i][1] = 0.0;
      acc2[i][2] = 0.0;
      
      force2[i][0] = 0.0;
      force2[i][1] = 0.0;
      force2[i][2] = 0.0;
	}
}

void firstParticleDataStructure() 
{
   // initial values for all the particles

   for (int i=0; i<NUM_PARTICLES; i++) 
	{
      mass[i] = 1.0;

      pos[i][0] = 0;
      pos[i][1] = 0;
      pos[i][2] = 0;
		//double radians = (3.14159 / 180) * ang1;
		vel[i][0] = 0;

		vel[i][1] = 0;

      acc[i][0] = 0.0;
      acc[i][1] = 0.0;
      acc[i][2] = 0.0;
      
      force[i][0] = 0.0;
      force[i][1] = 0.0;
      force[i][2] = 0.0;
	}
}

void firstParticleDataStructure2() 
{
   // initial values for all the particles

   for (int i=0; i<NUM_PARTICLES; i++) 
	{
      mass2[i] = 1.0;

      pos2[i][0] = 0;
      pos2[i][1] = 0;
      pos2[i][2] = 0;
		//double radians = (3.14159 / 180) * ang2;
		vel2[i][0] = 0;

		vel2[i][1] = 0;

      acc2[i][0] = 0.0;
      acc2[i][1] = 0.0;
      acc2[i][2] = 0.0;
      
      force2[i][0] = 0.0;
      force2[i][1] = 0.0;
      force2[i][2] = 0.0;
	}
}

void sumForces() 
{
   // for this timestep, accumulate all the forces that
   //   act on each particle

   for (int i=0; i<NUM_PARTICLES; i++) 
	{

      // ZERO ALL FORCES
      force[i][0] = force[i][1] = force[i][2] = 0.0;

	   // GRAVITY AND WIND AND DRAG
      force[i][1] = -GRAVITY*mass[i] + WIND + (-DRAG*pow(vel[i][1], 2));

		
   }
}

void sumForces2() 
{
   // for this timestep, accumulate all the forces that
   //   act on each particle

   for (int i=0; i<NUM_PARTICLES; i++) 
	{

      // ZERO ALL FORCES
      force2[i][0] = force2[i][1] = force2[i][2] = 0.0;

	   // GRAVITY AND WIND AND DRAG
      force2[i][1] = -GRAVITY*mass2[i] + WIND + (-DRAG*pow(vel2[i][1], 2));

		
   }
}
  
void EulerIntegrate() {
   // for each particle, compute the new velocity
   //   and position

   for (int i = 0; i < NUM_PARTICLES; i++) 
	{
  
      // CALCULATE NEW ACCEL
      acc[i][0] = force[i][0] / mass[i];
      acc[i][1] = force[i][1] / mass[i];
      acc[i][2] = force[i][2] / mass[i];


      // CALCULATE NEW POS
      pos[i][0] += vel[i][0] * TIMESTEP +
                        0.5 * acc[i][0] * TIMESTEP * TIMESTEP;
      pos[i][1] += vel[i][1] * TIMESTEP +
                        0.5 * acc[i][1] * TIMESTEP * TIMESTEP;
      pos[i][2] += vel[i][2] * TIMESTEP +
                        0.5 * acc[i][2] * TIMESTEP * TIMESTEP;

		//printf("pos[i][1]: %f\n", pos[i][1]);
		//printf("t2trans[0]: %f\n", t2trans[0]);
		if (abs(pos[i][0] - t2trans[0]) <= 1.0 && stopflag1 == 0)
		{
			if ((abs(pos[i][1] - t2trans[1]) <= .2) && pos[i][1] >= -.2)
			{
				printf("HIT RED!\n");
				
				if (ang1 >= 80)
				{
					printf("Ultra High Angle Bonus\n");
					health2 -= 4;
				}
				else if (ang1 >= 72)
				{
					printf("High Angle Bonus\n");
					health2 -= 2;
				}
				else
				{
					health2 -= 1;
				}
				printf("PLAYER2 HEALTH: %d/20\n", health2);
				if (health2 <= 0)
				{
					printf("PLAYER 1 WINS!!\n");
					exit(0);
				}

				hitflag = 1;
			}
		}
		
   
      // CALCULATE NEW VEL
      vel[i][0] += acc[i][0] * TIMESTEP;
      vel[i][1] += acc[i][1] * TIMESTEP;
      vel[i][2] += acc[i][2] * TIMESTEP;
      
   }
}

void EulerIntegrate2() {
   // for each particle, compute the new velocity
   //   and position

   for (int i = 0; i < NUM_PARTICLES; i++) 
	{
  
      // CALCULATE NEW ACCEL
      acc2[i][0] = force2[i][0] / mass2[i];
      acc2[i][1] = force2[i][1] / mass2[i];
      acc2[i][2] = force2[i][2] / mass2[i];


      // CALCULATE NEW POS
      pos2[i][0] += vel2[i][0] * TIMESTEP +
                        0.5 * acc2[i][0] * TIMESTEP * TIMESTEP;
      pos2[i][1] += vel2[i][1] * TIMESTEP +
                        0.5 * acc2[i][1] * TIMESTEP * TIMESTEP;
      pos2[i][2] += vel2[i][2] * TIMESTEP +
                        0.5 * acc2[i][2] * TIMESTEP * TIMESTEP;

		//printf("pos[i][1]: %f\n", pos[i][1]);
		//printf("t2trans[0]: %f\n", t2trans[0]);
		if (abs(pos2[i][0] - t1trans[0]) <= .9 && stopflag2 == 0)
		{
			if ((abs(pos2[i][1] - t1trans[1]) <= .18) && pos2[i][1] >= -.2)
			{
				printf("HIT BLUE!\n");

				if (ang2 >= 80)
				{
					printf("Ultra High Angle Bonus\n");
					health1 -= 4;
				}
				else if (ang2 >= 72)
				{
					printf("High Angle Bonus\n");
					health1 -= 2;
				}
				else
				{
					health1 -= 1;
				}
				printf("PLAYER1 HEALTH: %d/20\n", health1);
				if (health1 <= 0)
				{
					printf("PLAYER 2 WINS!!\n");
					exit(0);
				}

				hitflag2 = 1;
			}
		}
		
   
      // CALCULATE NEW VEL
      vel2[i][0] += acc2[i][0] * TIMESTEP;
      vel2[i][1] += acc2[i][1] * TIMESTEP;
      vel2[i][2] += acc2[i][2] * TIMESTEP;
      
   }
}

