#include <iostream>
#include <math.h>
#include "gl.h"
#include "key.h"
#include "obj.h"
#include "plane.h"

/* Method Prototypes */
void display();
void initialize();
void keyboard(unsigned char key, int x, int y);
void keyboardup(unsigned char key, int x, int y);
void motion(int x, int y);
void mouse(int button, int state, int x, int y);
void point(int x, int y);
void reshape(int w, int h);
void spotlight();

/* All of our Global Variables */
int last_time;

GLdouble distance_near;
GLdouble distance_far;

GLdouble zoom;
GLdouble rotation_x;
GLdouble rotation_y;
GLdouble position_x;
GLdouble position_y;
GLdouble position_z;

GLdouble keyboard_dx;
GLdouble keyboard_dy;
GLdouble keyboard_dz;

GLdouble pointer_vx;
GLdouble pointer_vy;
GLdouble pointer_vz;

int click_button;
GLdouble click_zoom;
GLdouble click_rotation_x;
GLdouble click_rotation_y;
GLdouble click_nx;
GLdouble click_ny;

GLboolean animating;                                                           
GLdouble  rotation;  

float g_lightPos[4] = { 10, 10, -100, 1 };


plane *P;
obj *shuttle;
obj	*jet;
obj *helicopter;

int main(int argc, char **argv) {

    glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE);
    glutInitWindowSize(640, 480);
    glutInit(&argc, argv);

    glutCreateWindow(argv[0]);
	std::cout << argv[1] << std::endl;
	
	
	glutKeyboardUpFunc(keyboardup);
    glutKeyboardFunc(keyboard);
	glutReshapeFunc(reshape);
    glutDisplayFunc(display);
	glutMotionFunc(motion);
	glutMouseFunc(mouse);

	if(glewInit() == GLEW_OK) {
		std::cout << "GLEW_OK" << std::endl;
		initialize();
		glutMainLoop();

	}



	return 0;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/* The display Function glutDisplayFunc() will use*/
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void display() {
	GLdouble tb = zoom;
    GLdouble lr = zoom * glutGet(GLUT_WINDOW_WIDTH)
                       / glutGet(GLUT_WINDOW_HEIGHT);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glFrustum(-lr, lr, -tb, tb, distance_near, distance_far);
    
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
	glTranslatef(0, 0, -10); 
    spotlight();

    glRotated(rotation_x, 1.0, 0.0, 0.0);
    glRotated(rotation_y, 0.0, 1.0, 0.0);
    glTranslated(-position_x, -position_y, -position_z);
	glLightfv(GL_LIGHT0, GL_POSITION, g_lightPos);
    glPushMatrix();                                                            
    {   
		glDisable(GL_LIGHTING);
        plane_render(P);  
        glTranslated(0.0, 1.0, 0.0);                                           
        //glRotated(rotation, 0.0, 1.0, 0.0);  

		
		obj_render(shuttle);
		//obj_mini(shuttle);
		obj_proc(shuttle);
		//obj_norm(shuttle);
        glEnable(GL_LIGHTING);                                                        
    }                                                                          
    glPopMatrix();                                                             
    
	glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);
    glutSwapBuffers();

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/* For initializing all the global variables */
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void initialize() {
	last_time   = 0;

    distance_near =   1.0;
    distance_far  = 100.0;

    zoom        = 0.5;
    rotation_x  = 0.0;
    rotation_y  = 0.0;

    position_x  = 0.0;
    position_y  = 2.0;
    position_z  = 5.0;
    
    keyboard_dx = 0.0;
    keyboard_dy = 0.0;
    keyboard_dz = 0.0;
    
    animating = GL_FALSE;                                                      
    rotation  = 0.0;                                                           
    
    P = plane_create(20);
	shuttle = obj_create("NASA_SHUTTLE.obj");
	//helicopter = obj_create("helicopter");
	//jet = obj_create("jet");

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    
    glClearColor(0.2f, 0.2f, 0.2f, 0.0f);


}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/* The keyboard function glutKeyboardFunc() will use */
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void keyboard(unsigned char key, int x, int y)
{
    switch (key)
    {
        case KEY_L: keyboard_dx -= 1.0; break;
        case KEY_R: keyboard_dx += 1.0; break;
        case KEY_D: keyboard_dy -= 1.0; break;
        case KEY_U: keyboard_dy += 1.0; break;
        case KEY_F: keyboard_dz -= 1.0; break;
        case KEY_B: keyboard_dz += 1.0; break;
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/* Keyboardup function glutKeyboardUpFunc() will use */
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void keyboardup(unsigned char key, int x, int y) {
    switch (key)
    {
        case KEY_L: keyboard_dx += 1.0; break;
        case KEY_R: keyboard_dx -= 1.0; break;
        case KEY_D: keyboard_dy += 1.0; break;
        case KEY_U: keyboard_dy -= 1.0; break;
        case KEY_F: keyboard_dz += 1.0; break;
        case KEY_B: keyboard_dz -= 1.0; break;
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/* The motion function glutMotionFunc() will use */
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void motion(int x, int y)
{
	GLdouble nx = (GLdouble) x / glutGet(GLUT_WINDOW_WIDTH);
    GLdouble ny = (GLdouble) y / glutGet(GLUT_WINDOW_HEIGHT);

    GLdouble dx = nx - click_nx;
    GLdouble dy = ny - click_ny;

    if (click_button == GLUT_LEFT_BUTTON)                                      \
    {                                                                          \
        rotation_x = click_rotation_x +  90.0 * dy * zoom;                     \
        rotation_y = click_rotation_y + 180.0 * dx * zoom;                     \
                                                                               \
        if (rotation_x >   90.0) rotation_x  =  90.0;                          \
        if (rotation_x <  -90.0) rotation_x  = -90.0;                          \
        if (rotation_y >  180.0) rotation_y -= 360.0;                          \
        if (rotation_y < -180.0) rotation_y += 360.0;                          \
    }                                                                          \
    if (click_button == GLUT_RIGHT_BUTTON)                                     \
    {                                                                          \
        zoom = click_zoom + dy;                                                \
                                                                               \
        if (zoom < 0.01) zoom = 0.01;                                          \
    }                                                                          \
    glutPostRedisplay();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/* The mouse function glutMouseFunc() will use */
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void mouse(int button, int state, int x, int y)
{
    click_nx = (GLdouble) x / glutGet(GLUT_WINDOW_WIDTH);
    click_ny = (GLdouble) y / glutGet(GLUT_WINDOW_HEIGHT);

    click_button     = button;                                                 \
    click_zoom       = zoom;                                                   \
    click_rotation_x = rotation_x;
    click_rotation_y = rotation_y;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/* The point function used in the motion function */
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void point(int x, int y)
{
    GLdouble tb = zoom;
    GLdouble lr = zoom * glutGet(GLUT_WINDOW_WIDTH)
                       / glutGet(GLUT_WINDOW_HEIGHT);
    GLdouble k;

    pointer_vx =  (2.0 * x / glutGet(GLUT_WINDOW_WIDTH)  - 1.0) * lr;
    pointer_vy = -(2.0 * y / glutGet(GLUT_WINDOW_HEIGHT) - 1.0) * tb;
    pointer_vz = -distance_near;

    k = sqrt(pointer_vx * pointer_vx +
             pointer_vy * pointer_vy +
             pointer_vz * pointer_vz);

    pointer_vx /= k;
    pointer_vy /= k;
    pointer_vz /= k;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/* The reshape function glutReshapeFunc() will use */
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void reshape(int w, int h) {
	glViewport(0, 0, w, h);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/* Function to make a spotlight, used in main() */
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void spotlight() {
    GLfloat v[4], p[4] = { 0.0f, 0.0f, 0.0f, 1.0f };

    v[0] = (GLfloat) pointer_vx;
    v[1] = (GLfloat) pointer_vy;
    v[2] = (GLfloat) pointer_vz;
    v[3] = (GLfloat) 1.0;

    glLightfv(GL_LIGHT0, GL_POSITION,       p);
    glLightfv(GL_LIGHT0, GL_SPOT_DIRECTION, v);
    glLightf (GL_LIGHT0, GL_SPOT_CUTOFF,   40);
    glLightf (GL_LIGHT0, GL_SPOT_EXPONENT, 64);
}



