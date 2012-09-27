#include <iostream>
#include <vector>
#include "keys.h"
#include <assert.h>
#include "FileLoader.h"
#include "Plane.h"


using namespace std;

/* Prototypes */
void displayFunction();
void idle();
void motion(int x, int y);
void mouse(int button, int state, int x, int y);
void keyboard(unsigned char key, int x, int y);
void keyboardup(unsigned char key, int x, int y);
void resize(int w, int h);
void startup();   


int      last_time;

GLdouble zoom;
GLdouble rotation_x;
GLdouble rotation_y;
GLdouble rotation_z;
GLdouble position_x;
GLdouble position_y;
GLdouble position_z;

GLdouble keyboard_dx;
GLdouble keyboard_dy;
GLdouble keyboard_dz;

int      click_button;
GLdouble click_zoom;
GLdouble click_rotation_x;
GLdouble click_rotation_y;
GLdouble click_nx;
GLdouble click_ny;

GLdouble obj_scale = 1.0;                                                      \
GLdouble obj_x     = 0.0;                                                      \
GLdouble obj_y     = 0.0;                                                      \
GLdouble obj_z     = 0.0;   

float g_lightPos[4] = { 10, 10, -100, 1 };

/* Here are our Global Variables for the Object being Loaded */
vector<vert> verts;
vector<elem> elems;
model *ourobj;
LoadedObject *object;

/* Variables for our backdrop Plane */
Plane *p;
planeobject *plane;




int main(int argc, char *argv[]) {
	cout << argc << endl;
	cout << argv[1] << endl;
	
	/* Initialize all of our included classes */
	object = new LoadedObject;
	p = new Plane();
	new FileLoader(object, argv);

	/* Set our pointers to the verts and elems */
	// We could grab these everytime by object-> but for typing purposes this will do //
	verts = object->getVerts();
	elems = object->getElems();

	/* Initialize all glut functions for Pre Rendering */
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE);
   	glutInitWindowSize(1024, 780);
   	glutInit(&argc, argv);
	glutCreateWindow(argv[0]);

	startup();

	/* Glut functions relative to the main loop */
    glutKeyboardFunc(keyboard);
    glutReshapeFunc(resize);
    glutDisplayFunc(displayFunction);
    glutMotionFunc(motion);
    glutMouseFunc(mouse);

    
 	glutMainLoop();
    

	return 0;
}

/* glutReshapefunc */
void resize(int w, int h) {
	GLdouble x = 0.5 * (GLdouble) w / (GLdouble) h;
	GLdouble y = 0.5;

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glFrustum(-x, x, -y, y, 1.0, 10.0);

	glViewport(0, 0, w, h);
}

/* glutDisplayFunc */
void displayFunction() {
	glFlush();
	GLdouble tb = zoom;
    GLdouble lr = zoom * glutGet(GLUT_WINDOW_WIDTH)
                       / glutGet(GLUT_WINDOW_HEIGHT);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
	glFrustum(-lr, lr, -tb, tb, 1.0, 100.0);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(0, 0, -10); 
	
	glRotated(rotation_x, 1.0, 0.0, 0.0);                                      
    glRotated(rotation_y, 0.0, 1.0, 0.0);   
    glRotated(rotation_z, 0.0, 0.0, 1.0);                                   
    glTranslated(-position_x, -position_y, -position_z);  
	
	glLightfv(GL_LIGHT0, GL_POSITION, g_lightPos);
	
	glPushMatrix();
		glDisable(GL_LIGHTING);
		p->plane_render(plane);
		glTranslated(0.0, 1.0, 0.0); 

		glEnable(GL_LIGHTING);  
		object->renderObject(ourobj);
		
	glPopMatrix();

	glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);

	glutSwapBuffers();
	
	
}

/* glutMotionFunc */
void motion(int x, int y)
{
    GLdouble nx = (GLdouble) x / glutGet(GLUT_WINDOW_WIDTH);
    GLdouble ny = (GLdouble) y / glutGet(GLUT_WINDOW_HEIGHT);

    GLdouble dx = nx - click_nx;
    GLdouble dy = ny - click_ny;

    if (click_button == GLUT_LEFT_BUTTON)
    {
        rotation_x = click_rotation_x +  90.0 * dy * zoom;
        rotation_y = click_rotation_y + 180.0 * dx * zoom;

        if (rotation_x >   90.0) rotation_x  =  90.0;
        if (rotation_x <  -90.0) rotation_x  = -90.0;
        if (rotation_y >  180.0) rotation_y -= 360.0;
        if (rotation_y < -180.0) rotation_y += 360.0;
    }
    if (click_button == GLUT_RIGHT_BUTTON)
    {
        zoom = click_zoom + dy;

        if (zoom < 0.01) zoom = 0.01;
    }
    glutPostRedisplay();
}

/* glutMouseFunc */
void mouse(int button, int state, int x, int y)
{
	std::cout << "Mouse function firing" << std::endl;
    click_nx = (GLdouble) x / glutGet(GLUT_WINDOW_WIDTH);
    click_ny = (GLdouble) y / glutGet(GLUT_WINDOW_HEIGHT);

    click_button     = button;
    click_zoom       = zoom;
    click_rotation_x = rotation_x;
    click_rotation_y = rotation_y;
}

/* glutKeyboardFunc */
void keyboard(unsigned char key, int x, int y)
{
	std::cout << "Keyboard func firing" << std::endl;
	unsigned char ourkey = key;
    switch (ourkey) {
        case KEY_L: 
        	keyboard_dx -= 1.0;
        	rotation_y += 2;
            break;
        case KEY_R: 
        	keyboard_dx += 1.0; 
        	rotation_y -= 2;
        	break;
        case KEY_D: 
        	keyboard_dy -= 1.0; 
        	rotation_z -= 2;
        	break;
        case KEY_U: 
        	keyboard_dy += 1.0; 
        	rotation_z += 2;
        	break;
        case KEY_F: 
        	keyboard_dz -= 1.0; 
        	rotation_x += 2;
        	break;
        case KEY_B: 
        	keyboard_dz += 1.0; 
        	rotation_x -= 2;
        	break;
        default:
        	std::cout << "Key is not of significance" << endl;
        	break;
    }
    glutPostRedisplay();
}

void startup()                                                             
{             
	/*Initialize our Global Variables */  
    last_time   = 0;

    zoom        = 0.0;
    rotation_x  = 0.0;
    rotation_y  = 0.0;
    rotation_z  = 0.0;

    position_x  = 0.0;
    position_y  = 2.0;
    position_z  = 5.0;
    
    keyboard_dx = 0.0;
    keyboard_dy = 0.0;
    keyboard_dz = 0.0;

    /* Create all of our objects that we will render in the display function */
    ourobj = object->createModel();
    plane =  p->plane_create(20);                                                 
                                                             
    glEnable(GL_DEPTH_TEST);                                                   
    glEnable(GL_LIGHTING);                                                     
    glEnable(GL_LIGHT0);                                                     
                                                                               
    glClearColor(1.0f, 1.0f, 1.0f, 0.0f);                                      
}  

void idle()
{
    GLdouble M[16], speed = 3.0;

    int curr_time = glutGet(GLUT_ELAPSED_TIME);

    GLdouble dt = (curr_time - last_time) / 1000.0;
    GLdouble kx = keyboard_dx * dt * speed;
    GLdouble ky = keyboard_dy * dt * speed;
    GLdouble kz = keyboard_dz * dt * speed;

    last_time = curr_time;

    glGetDoublev(GL_MODELVIEW_MATRIX, M);

    position_x += kx * M[ 0] + ky * M[ 1] + kz * M[ 2];
    position_y += kx * M[ 4] + ky * M[ 5] + kz * M[ 6];
    position_z += kx * M[ 8] + ky * M[ 9] + kz * M[10];

    glutPostRedisplay();
}






