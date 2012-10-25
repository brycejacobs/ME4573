#include <iostream>
#include <math.h>
#include "gl.h"
#include "shaderloader.h"
#include "obj.h"
#include "plane.h"
#include "image.h"


/* Method Prototypes */
void display();
void initialize();
void keyboard(unsigned char, int, int);
void keyboardup(unsigned char, int, int);
void motion(int, int);
void mouse(int, int, int, int);
void reshape(int, int);

/*Protypes for Camera */
static void camera_fly();
static void pan_camera(GLfloat, GLfloat);
static void zoom_camera(GLfloat);

/* Prototypes for light */
static void pan_light(GLfloat, GLfloat);
static void lights();

/* Prototypes for the menu */
void processItem1(int);
void processItem2(int);
void processMain(int);
void createMenuItems();

/* Fragment and Vertex Prototypes */
void CreatePhong();



/* All of our Global Variables */

GLfloat controlTheTroll[3] = {0.0, 0.0, 0.0}; //Variables for troll position


/* Camera Variables */
static int      last_time;

/* Camera state.                                                              */

static GLfloat  position[4] = {   0.0,  0.0,  5.0,  1.0 };
static GLfloat  rotation[2] = {   11.3,  10.0             };
static GLfloat  light[2]    = { -60.0, 30.0             };
static GLfloat  point[4]    = {   0.0,  0.0,  0.0,  0.0 };
static GLfloat  zoom        = 0.5;

/* Camera state at the beginning of a click.                                  */

static int      click_modifiers;
static int      click_button = -1;
static int      click_x;
static int      click_y;
static GLfloat  click_rotation[2];
static GLfloat  click_light[2];
static GLfloat  click_zoom;

/* Definitions for constants regarding to the menu */
#define FIRSTFRAGMENT 1
#define SECONDFRAGMENT 2
#define THIRDFRAGMENT 3

#define FIRSTVERTEX 1
#define SECONDVERTEX 2
#define THIRDVERTEX 3

/* Menu openGL objects */
int vertexMenu, fragmentMenu, mainMenu;

plane *P;
obj *shuttle;
obj	*jet;
obj *troll;

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

	GLfloat V = 0.1f * zoom;
    GLfloat H = 0.1f * zoom * glutGet(GLUT_WINDOW_WIDTH)
                            / glutGet(GLUT_WINDOW_HEIGHT);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    glFrustum(-H, H, -V, V, 0.1, 100.0);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

	lights();
	camera_fly();

	glTranslatef(0, 0, -10); 

	
    glPushMatrix();                                                            
    {   
		glDisable(GL_LIGHTING);
        plane_render(P);  
		glEnable(GL_LIGHTING);                                                        
    }  
	glPopMatrix(); 

	                                         
	//This is so we can control the troll only.
	glPushMatrix(); 
	{
		glTranslated(controlTheTroll[0], 1.0, controlTheTroll[2]);  
		obj_render(troll);
	}
	glPopMatrix();

	glTranslated(0.0, 1.0, 0.0);
	obj_render(jet);
                                                                
    
	glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);
    glutSwapBuffers();

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/* For initializing all the global variables */
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void initialize() {
	last_time   = 0;
    zoom        = 0.5;

    P = plane_create(60);
	//shuttle = obj_create("trolluvd1.obj");
	//helicopter = obj_create("helicopter");
	jet = obj_create("jet.obj");
	troll = obj_create("trolluvd1.obj");

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    
    glClearColor(0.2f, 0.2f, 0.2f, 0.0f);

	createMenuItems();
	


}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/* The keyboard function glutKeyboardFunc() will use */
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void keyboard(unsigned char key, int x, int y)
{
    switch (key)
    {
        case 'a':      
			position[0] -= 1.0;
			break;
        case 'd': 
			position[0] += 1.0;
			break;
        case 'c': 
			position[1] -= 1.0; 
			break;
        case ' ':
			position[1] += 1.0;
			break;
        case 'w':
			position[2] -= 1.0;
			break;
        case 's': 
			position[2] += 1.0;
			break;
		case 'i':
			controlTheTroll[2] += 1.0;
			break;
		case 'k':
			controlTheTroll[2] -= 1.0;
		case 'j':
			controlTheTroll[0] += 1.0;
			break;
		case 'l': 
			controlTheTroll[0] -= 1.0;
			break;

    }
	glutPostRedisplay();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/* Keyboardup function glutKeyboardUpFunc() will use */
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void keyboardup(unsigned char key, int x, int y) {
    switch (key)
    {
        case 'a':      
			position[0] -= 1.0;
			break;
        case 'd': 
			position[0] += 1.0;
			break;
        case 'c': 
			position[1] -= 1.0; 
			break;
        case ' ':
			position[1] += 1.0;
			break;
        case 'w':
			position[2] -= 1.0;
			break;
        case 's': 
			position[2] += 1.0;
			break;
		case 'i':
			controlTheTroll[2] += 1.0;
			break;
		case 'k':
			controlTheTroll[2] -= 1.0;
		case 'j':
			controlTheTroll[0] += 1.0;
			break;
		case 'l': 
			controlTheTroll[0] -= 1.0;
			break;


    }
	glutPostRedisplay();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/* The motion function glutMotionFunc() will use */
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void motion(int x, int y)
{
    const int w = glutGet(GLUT_WINDOW_WIDTH);
    const int h = glutGet(GLUT_WINDOW_HEIGHT);

    GLfloat H = .1f * zoom * w / h;
    GLfloat V = .1f * zoom;
    GLfloat r;

    /* Compute the pointer motion as a fraction of window size. */

    GLfloat dx = (GLfloat) (x - click_x) / w;
    GLfloat dy = (GLfloat) (y - click_y) / h;

    /* Apply the pointer motion to the camera or light. */

	if (click_button == GLUT_LEFT_BUTTON)
    {
        if      (click_modifiers == 0)                  pan_camera(dx, dy);
		else if (click_modifiers == GLUT_ACTIVE_CTRL)   pan_light (dx, dy);
        else if (click_modifiers == GLUT_ACTIVE_SHIFT) zoom_camera(    dy);
    }

    /* Compute the eye-space pointer vector. */
    point[0] =  (2.0f * x / w - 1.0f) * H;
    point[1] = -(2.0f * y / h - 1.0f) * V;
    point[2] = -(0.1f);
    r = 1.0f / (GLfloat) sqrt(point[0] * point[0] +
                              point[1] * point[1] +
                              point[2] * point[2]);
    point[0] *= r;
    point[1] *= r;
    point[2] *= r;

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/* The mouse function glutMouseFunc() will use */
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void mouse(int button, int state, int x, int y)
{
    /* Note all camera state at the beginning of a click. */

    if (state == GLUT_DOWN)
    {
        click_modifiers   = glutGetModifiers();
        click_button      = button;
        click_x           = x;
        click_y           = y;
        click_zoom        = zoom;
        click_rotation[0] = rotation[0];
        click_rotation[1] = rotation[1];
        click_light[0]    = light[0];
        click_light[1]    = light[1];
    }
    else click_button = -1;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/* The reshape function glutReshapeFunc() will use */
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void reshape(int w, int h) {
	glViewport(0, 0, w, h);
}


static void zoom_camera(GLfloat dy)
{
    zoom = click_zoom + dy;

    if (zoom < 0.01) zoom = 0.01;

    glutPostRedisplay();
}

static void pan_camera(GLfloat dx, GLfloat dy)
{
    rotation[0] = click_rotation[0] +  90.0 * dy * zoom;
    rotation[1] = click_rotation[1] + 180.0 * dx * zoom;

    if (rotation[0] >   90.0) rotation[0]  =  90.0;
    if (rotation[0] <  -90.0) rotation[0]  = -90.0;
    if (rotation[1] >  180.0) rotation[1] -= 360.0;
    if (rotation[1] < -180.0) rotation[1] += 360.0;

    glutPostRedisplay();
}

static void camera_fly()
{
	std::cout  << "Camera_FLY CALLED with Rotation 1 and 2 as " << rotation[0] << " " << rotation[1] << std::endl;
    glRotatef(rotation[0], 1.0f, 0.0f, 0.0f);
    glRotatef(rotation[1], 0.0f, 1.0f, 0.0f);

	std::cout << "Will be translating pos 1 2 3: " << position[0] << " " << position[1] << " " << position[2] << std::endl;
    glTranslatef(-position[0], -position[1], -position[2]);
}

static void pan_light(GLfloat dx, GLfloat dy)
{
    light[0] = click_light[0] +  90.0 * dy;
    light[1] = click_light[1] + 180.0 * dx;

    if (light[0] >   90.0) light[0]  =  90.0;
    if (light[0] <  -90.0) light[0]  = -90.0;
    if (light[1] >  180.0) light[1] -= 360.0;
    if (light[1] < -180.0) light[1] += 360.0;

    glutPostRedisplay();
}

static void lights()
{
    /* Position the global light. */

    const GLfloat L[4] = { 0.0f, 0.0f, 1.0f, 0.0f };

    glPushMatrix();
    {
        glRotatef(light[1], 0.0f, 1.0f, 0.0f);
        glRotatef(light[0], 1.0f, 0.0f, 0.0f);
        glLightfv(GL_LIGHT0, GL_POSITION, L);
    }
    glPopMatrix();

    /* Position the flashlight. */

    glLightfv(GL_LIGHT1, GL_POSITION, point);
}

/////////////////////////////////////////////////////////////////////
//This will create all of our items for the menu.
////////////////////////////////////////////////////////////////////
void createMenuItems(){

	fragmentMenu = glutCreateMenu(processItem1);
	{
		glutAddMenuEntry("Brick Fragment Program", FIRSTFRAGMENT);
		glutAddMenuEntry("Second Fragment Program", SECONDFRAGMENT);
		glutAddMenuEntry("Third Fragment Program", THIRDFRAGMENT);
	}

	vertexMenu = glutCreateMenu(processItem2);
	{
		glutAddMenuEntry("Wave Vertex Program", FIRSTVERTEX);
		glutAddMenuEntry("Second Vertext Program", SECONDVERTEX);
		glutAddMenuEntry("Third Vertext Program", THIRDVERTEX);
	}

	mainMenu = glutCreateMenu(processMain);
	{
		glutAddSubMenu("Toggle Vertex Programs", vertexMenu);
		glutAddSubMenu("Toggle Fragment Programs", fragmentMenu);
	}

	glutAttachMenu(GLUT_RIGHT_BUTTON);
}

void processItem1(int option) {
	
	switch (option) {
		case FIRSTFRAGMENT:
			CreatePhong();

			break;
		case SECONDFRAGMENT:
			
			break;
		case THIRDFRAGMENT :
			break;
	}
}

void processItem2(int option) {
	switch(option) {
	case FIRSTVERTEX:

		break;
	case SECONDVERTEX:

		break;

	case THIRDVERTEX:

		break;


	}

}
 //Empt method
void processMain(int option) {}

void CreatePhong() {
	ShaderLoader *loader;
	loader = new ShaderLoader();
	GLuint phong_program = glCreateProgram();

	//Phong shader
	GLuint phong_vertex = glCreateShader(GL_VERTEX_SHADER);

	GLchar *phong_vertext_text = loader->load("phongvertex.glsl"); 
	glShaderSource(phong_vertex, 1, (const GLchar **) &phong_vertext_text, 0);
	glCompileShader(phong_vertex);
	free(phong_vertext_text);

	GLuint phong_fragment = glCreateShader(GL_FRAGMENT_SHADER);

	GLchar *phong_fragment_text = loader->load("phongfragment.glsl");
	glShaderSource(phong_fragment, 1, (const GLchar**) &phong_fragment_text, 0);
	glCompileShader(phong_fragment);
	free(phong_fragment_text);

			

	glAttachShader(phong_program, phong_vertex);
	glAttachShader(phong_program, phong_fragment);

	glLinkProgram(phong_program);

	glUseProgram(phong_program);
}



