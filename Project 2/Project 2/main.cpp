#include <iostream>
#include <math.h>
#include <vector>
#include <time.h>
#include "gl.h"
#include "shaderloader.h"
#include "obj.h"
#include "plane.h"
#include "image.h"


	
/* Method Prototypes */
void display();
void initialize();
void idle();
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
void processReset(int);
void processMain(int);
void createMenuItems();

/* Fragment and Vertex Prototypes */
GLuint createProgram();
bool checkCompilationStatus(GLuint);
bool checkLinkingStatus(GLuint);

void Wait(int);


/* Variables for model Rendering */
static int scenario = 1;

/* Camera Variables */
static int last_time;

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

/* Shader Variables */
std::vector <GLuint> fragments;
std::vector <GLuint> vertexs;

std::vector<char *> fragmentNames;
std::vector<char *> vertexNames;

GLuint brickprogram;
GLuint waveprogram;
GLuint toonprogram;
GLuint animationprogram;
GLuint flattenprogram;

float timeFactor;
float timeFrequency = 0.2;
float addedTime = 0.3;
float brickColorChanger = 0.2;
clock_t initial = clock();
bool wave = false;
bool colors_animation = false;
bool flattened = false;

bool enabledPlane = true;



/* Definitions for constants regarding to the menu */
#define FIRSTOPTION 1
#define SECONDOPTION 2
#define THIRDOPTION 3
#define FOURTHOPTION 4
#define FIFTHOPTION 5

#define FIRSTMODEL 1
#define SECONDMODEL 2
#define THIRDMODEL 3
#define FOURTHMODEL 4
#define FIFTHMODEL 5

#define JET 1
#define TROLL 2
#define HELICOPTER 3
#define NONE 4

/* Menu openGL objects */
int shaderMenu, modelMenu, resetMenu, mainMenu;

plane *P;
obj *helicopter;
obj	*jet;
obj *troll;

using std::vector;
using std::cout;
using std::endl;

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
	glutIdleFunc(idle);



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
		if(enabledPlane) {
			glDisable(GL_LIGHTING);
			plane_render(P);  
			glEnable(GL_LIGHTING);    
		}
    }  
	glPopMatrix(); 

	                                         

	glTranslated(0.0, 1.0, 0.0);

	switch(scenario) {
	case JET:
		obj_render(jet);
		break;
	case TROLL:
		obj_render(troll);
		break;
	case HELICOPTER:
		obj_render(helicopter);
		break;
	case NONE:
		break;
	default:
		cout << "Shouldn't be here bud...GET OUT!!! 1" << endl;
		break;
	}

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
	jet = obj_create("jet.obj");
	troll = obj_create("trolluvd1.obj");
	helicopter = obj_create("helicopter.obj");

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
			addedTime += .1;
			break;
		case 'k':
			addedTime -= .1;
		case 'j':
			timeFrequency += .1;
			break;
		case 'l': 
			timeFrequency -= .1;
			break;
		case 'h':
			brickColorChanger += .1;
			cout << "Brick Color Changer Variable: " << brickColorChanger << endl;
			glUniform3f(glGetUniformLocation(brickprogram, "brick_color"), 1.0, .3, brickColorChanger);
			break;
		case 'u':
			brickColorChanger -= .1;
			cout << "Brick Color Changer Variable: " << brickColorChanger << endl;
			glUniform3f(glGetUniformLocation(brickprogram, "brick_color"), 1.0, .3, brickColorChanger);
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
			//controlTheTroll[2] += 1.0;
			break;
		case 'k':
			//controlTheTroll[2] -= 1.0;
		case 'j':
			//controlTheTroll[0] += 1.0;
			break;
		case 'l': 
			//controlTheTroll[0] -= 1.0;
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

	if(colors_animation) {
		glUniform2f(glGetUniformLocation(animationprogram, "mouse"), x, y);
		glutPostRedisplay();
	}
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
	
    glRotatef(rotation[0], 1.0f, 0.0f, 0.0f);
    glRotatef(rotation[1], 0.0f, 1.0f, 0.0f);
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

	shaderMenu = glutCreateMenu(processItem1);
	{
		glutAddMenuEntry("Brick Shader", FIRSTOPTION);
		glutAddMenuEntry("Wave Shader", SECONDOPTION);
		glutAddMenuEntry("Toon Shader", THIRDOPTION);
		glutAddMenuEntry("Colors Animation", FOURTHOPTION);
		glutAddMenuEntry("Flatten Shader", FIFTHOPTION);
	}

	modelMenu = glutCreateMenu(processItem2);
	{
		glutAddMenuEntry("Jet", FIRSTMODEL);
		glutAddMenuEntry("Troll", SECONDMODEL);
		glutAddMenuEntry("Helicopter", THIRDMODEL);
		glutAddMenuEntry("No Models", FIFTHMODEL);
		glutAddMenuEntry("Plane Toggle", FOURTHMODEL);

	}

	mainMenu = glutCreateMenu(processMain);
	{
		glutAddSubMenu("Swap Models", modelMenu);
		glutAddSubMenu("Toggle Shaders", shaderMenu);
		glutAddMenuEntry("Reset" , 1);
	}

	glutAttachMenu(GLUT_RIGHT_BUTTON);
}

void processItem1(int option) {
	ShaderLoader *loader;
	loader = new ShaderLoader();
	switch (option) {
		case FIRSTOPTION:

			fragmentNames.clear();
			vertexNames.clear();

			fragmentNames.push_back("brick_fragment.glsl");
			vertexNames.push_back("brick_vertex.glsl");
			
			brickprogram = createProgram();

			glUseProgram(brickprogram);

			glUniform3f(glGetUniformLocation(brickprogram, "brick_color"), 1.0, .3, brickColorChanger);
			glUniform3f(glGetUniformLocation(brickprogram, "mortar_color"), .85, .86, .84);
		    glUniform2f(glGetUniformLocation(brickprogram, "brick_size"), .3, .15);
			glUniform2f(glGetUniformLocation(brickprogram, "brick_frac"), .90, .85);

			glutPostRedisplay();

			break;
		case SECONDOPTION:

			fragmentNames.clear();
			vertexNames.clear();
			vertexNames.push_back("wave_vertex.glsl");
			
			waveprogram = createProgram();

			glUseProgram(waveprogram);
			
			wave = true;
			glUniform1f(glGetUniformLocation(waveprogram, "time"), timeFactor);

			glutPostRedisplay();
				break;
		case THIRDOPTION:
			fragmentNames.clear();
			vertexNames.clear();

			fragmentNames.push_back("toon_fragment.glsl");
			vertexNames.push_back("toon_vertex.glsl");

			toonprogram = createProgram();

			glUseProgram(toonprogram);

			glutPostRedisplay();

			break;
		case FOURTHOPTION:
			fragmentNames.clear();
			vertexNames.clear();

			fragmentNames.push_back("colors_animation_fragment.glsl");

			animationprogram = createProgram();

			glUseProgram(animationprogram);

			colors_animation = true;
			glUniform1f(glGetUniformLocation(animationprogram, "time"), 1.0);
			glUniform2f(glGetUniformLocation(animationprogram, "mouse"), 0.0, 0.0);
			glUniform2f(glGetUniformLocation(animationprogram, "resolution"), 640, 480);

			cout << "animation program succeeded" << endl;
			
			glutPostRedisplay();
			break;
		case FIFTHOPTION:

			fragmentNames.clear();
			vertexNames.clear();

			fragmentNames.push_back("flatten_fragment.glsl");
			vertexNames.push_back("flatten_vertex.glsl");

			flattenprogram = createProgram();

			glUseProgram(flattenprogram);

			flattened = true;
			glUniform1f(glGetUniformLocation(flattenprogram, "time"),  timeFactor);
			break;

		default:
			cout << "Shouldn't be here bud...GET OUT!!!" << endl;
		break;
	}
}

void processItem2(int option) {
	switch(option) {
	case FIRSTMODEL:
		scenario = JET;
		glutPostRedisplay();
		break;
	case SECONDMODEL:
		scenario = TROLL;
		glutPostRedisplay();
		break;
	case THIRDMODEL:
		scenario = HELICOPTER;
		glutPostRedisplay();
		break;
	case FOURTHMODEL:
		if(enabledPlane) {
			enabledPlane = false;
		} else {
			enabledPlane = true;
		}
		glutPostRedisplay();
		break;
	case FIFTHMODEL:
		scenario = NONE;
		glutPostRedisplay();
		break;
	default:
		cout << "Shouldn't be here bud...GET OUT!!! 2" << endl;
	break;


	}

}

void processMain(int option) {
	switch(option) {
	case 1: 
		glUseProgram(0); 
		wave = false;
		colors_animation = false;
		flattened = false;
		glutPostRedisplay();
		break;
	default:
		cout << "Shouldn't be here bud...GET OUT!!!" << endl;
	break;
	}
}

GLuint createProgram() {
	GLuint *_fragment;
	GLuint *_vertex;
	GLchar *vertexText;
	GLchar *fragmentText;
	ShaderLoader *loader;
	loader = new ShaderLoader();

 	GLuint _program = glCreateProgram();

	for(int i = 0; i < fragmentNames.size(); i++) {
		_fragment = new GLuint;

		*_fragment = glCreateShader(GL_FRAGMENT_SHADER);

		fragmentText = loader->load(fragmentNames[i]);

		glShaderSource(*_fragment, 1, (const GLchar **) &fragmentText, 0);

		glCompileShader(*_fragment);

		if(checkCompilationStatus(*_fragment)) {
			std::cout << "Fragment Shader " << i << " Compiled Properly" << std::endl;
		} else {
			std::cout << "Shaders had a problem compiling" << std::endl;
			return NULL;
		}

		glAttachShader(_program, *_fragment);

		free(fragmentText);
	}

	for(int j = 0; j < vertexNames.size(); j++) {
		_vertex = new GLuint;
		*_vertex = glCreateShader(GL_VERTEX_SHADER);
	
	
		vertexText = loader->load(vertexNames[j]);

	
		glShaderSource(*_vertex, 1, (const GLchar **) &vertexText, 0);

	
		glCompileShader(*_vertex);

		if(checkCompilationStatus(*_vertex)) {
			std::cout << "Shaders Compiled Properly" << std::endl;
		} else {
			std::cout << "Shaders had a problem compiling" << std::endl;
			return NULL;
		}

	
		free(vertexText);

	
		glAttachShader(_program, *_vertex);

	}

	glLinkProgram(_program);

	if(checkLinkingStatus(_program)) {
		std::cout  << "Program Linked Successfully" << std::endl;
	} else {
		std::cout << "Program had a problem Linking" << std::endl;
		return NULL;
	}

	return _program;

}

bool checkCompilationStatus(GLuint object) {
	GLchar *p;
	GLint s, n;

	glGetShaderiv(object, GL_COMPILE_STATUS, &s);
	glGetShaderiv(object, GL_INFO_LOG_LENGTH, &n);
	
	if ((s == 0) && (p = (GLchar *) calloc(n + 1, 1))) {
		glGetShaderInfoLog(object, n, NULL, p);
		fprintf(stderr, "OpenGL Shader Error:\n%s", p);
		free(p);
		return false;
	}

	return true;

}

bool checkLinkingStatus(GLuint object) {
	GLchar *p;
	GLint s, n;

	glGetProgramiv(object, GL_LINK_STATUS, &s);
	glGetProgramiv(object, GL_INFO_LOG_LENGTH, &n);
	
	if ((s == 0) && (p = (GLchar *) calloc(n + 1, 1))) {
		glGetProgramInfoLog(object, n, NULL, p);
		fprintf(stderr, "OpenGL Program Error:\n%s", p);
		free(p);
		return false;
	}

	return true;
}


void idle() {
	
	clock_t current = clock();
	float difference = (current - initial) / 1000.0f;

	if(difference >  timeFrequency) {
		
		timeFactor += addedTime;

		if(wave) {
			glUniform1f(glGetUniformLocation(waveprogram, "time"), timeFactor);
			glutPostRedisplay();
		} else if(colors_animation) {
			glUniform1f(glGetUniformLocation(animationprogram, "time"), timeFactor);
			glutPostRedisplay();
		} else if(flattened) {
			glUniform1f(glGetUniformLocation(flattenprogram, "time"), timeFactor);
			glutPostRedisplay();
		}
		initial = clock();
		
	}

	
}




