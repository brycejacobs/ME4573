#include <iostream>
#include <string>

#include "gl.h"
#include "obj.h"
#include "plane.h"
#include "shaderloader.h"
#include "viewglut.h"

/* OpenGL callback function prototypes only */
static void display();
static void idle();
static void keyboard(unsigned char, int, int);
static void keyboardup(unsigned char, int, int);
static void motion(int, int);
static void mouse(int, int, int, int);

/* OpenGL Menu prototypes only */
void createMenuItems();
void MainMenu(int);
void ModelMenu(int);
void ShaderMenu(int);

/* OpenGL Shader Prototypes go here */
GLuint createProgram(std::string, std::string);
bool checkCompilationStatus(GLuint);
bool checkLinkingStatus(GLuint);

/*All Other Prototypes */
void Initialize();

/* Global Variables start here */

//For camera
static viewglut *camera;

//For Menu
int mainMenu, shaderMenu, modelMenu, resetMenu;

//For Models
static plane *p;
static obj *box;
static obj *troll;
int modelChosen;
bool planeEnabled;

/* All of our definitions should go here */
#define THIRTEENBOX 1
#define TROLL 2
#define NOMODELS 3
#define NOPLANE 4


////////////////////////////////////////////////////////
/* Main function  */
////////////////////////////////////////////////////////
int main(int argc, char **argv) {
	std::cout << "Beam me up Scotty!" << std::endl;
    glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE);
    glutInitWindowSize(640,480);
    glutInit(&argc, argv);

    glutCreateWindow(argv[0]);

    glutIdleFunc(idle);
    glutMouseFunc(mouse);
    glutMotionFunc(motion);
    glutDisplayFunc(display);
    glutKeyboardFunc(keyboard);
    glutKeyboardUpFunc(keyboardup);

    if(glewInit() == GLEW_OK) {
        Initialize();

        glutIgnoreKeyRepeat(1);
        glutMainLoop();
    }

    return 0;
}

////////////////////////////////////////////////////////
/* All OpenGL Callback Prototype Definitions go here */
////////////////////////////////////////////////////////
static void display() {
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	/*Camera */
	viewglut_apply(camera);
	viewglut_render(camera);

	/*Drawing*/
	glTranslatef(0, 0, -10);

	glPushMatrix(); 
	{
		if(planeEnabled) {
			glDisable(GL_LIGHTING);
			plane_render(p);
			glEnable(GL_LIGHTING);
		}
		
	}
	glPopMatrix();

    glTranslated(0.0, 1.0, 0.0);
	switch(modelChosen) {
	case THIRTEENBOX:
		obj_render(box);
		break;
	case TROLL:
		obj_render(troll);
		break;
	case NOMODELS:
		//Nothing gets rendered homeboy
		break;
	default:
		std::cout << "modelChosen Out of Range. Draw Method-modelChosen = " << modelChosen << std::endl;
		break;
	}

	glutSwapBuffers();
}

static void idle() {
	viewglut_idle(camera);
}

static void keyboard(unsigned char k, int x, int y) {
	/* Camera */
	viewglut_key(camera, k, 1);
}

static void keyboardup(unsigned char k, int x, int y) {
	/* Camera */
	viewglut_key(camera, k, 0);
}

static void motion(int x, int y) {
	/* Camera */
	viewglut_point(camera, x, y);

}

static void mouse(int b, int s, int x, int y) {
	/* Camera */
	if(b == GLUT_LEFT_BUTTON) {
		if(s == GLUT_DOWN) {
			viewglut_click(camera, x, y, 1);
		} else {
			viewglut_click(camera, x, y, 0);
		}
	}
}

/* All OpenGL Menu Prototype Definitions go here */

void createMenuItems() {
    shaderMenu = glutCreateMenu(ShaderMenu);
    {
        glutAddMenuEntry("Spotlight 1", 1);
        glutAddMenuEntry("Spotlight 2", 2);
    }

    modelMenu = glutCreateMenu(ModelMenu);
    {
		glutAddMenuEntry("Thirteen Box", THIRTEENBOX);
		glutAddMenuEntry("Troll", TROLL);
        glutAddMenuEntry("No Models", 3);
        glutAddMenuEntry("Plane Toggle", 4);
    }

    mainMenu = glutCreateMenu(MainMenu);
    {
        glutAddSubMenu("Swap Models", modelMenu);
        glutAddSubMenu("Toggle Shaders", shaderMenu);
        glutAddMenuEntry("Reset", 1);
    }

    glutAttachMenu(GLUT_RIGHT_BUTTON);
}
void MainMenu(int option) {
	switch(option) {
	case 1: 
		glUseProgram(0);
		glutPostRedisplay();
		break;
	default:
		std::cout << "Main Menu - Not Supposed to be here. Sent option: " << option << std::endl;
		break;
	}
}
void ModelMenu(int option) {
	switch(option) {
	case THIRTEENBOX:
		modelChosen = THIRTEENBOX;
		break;
	case TROLL:
		modelChosen = TROLL;
		break;
	case NOMODELS:
		modelChosen = NOMODELS;
		break;
	case NOPLANE:
		planeEnabled = planeEnabled == false ? true : false;
		break;
	default:
		std::cout << "Model Menu - Not Supposed to be here. Sent option: " << option << std::endl;
		break;
	}
    glutPostRedisplay();
}
void ShaderMenu(int option) {
	switch(option) {
	case 1:
		break;
	case 2:
		break;
	default:
		std::cout << "Shader Menu - Not Supposed to be here. Sent option: " << option << std::endl;
		break;
	}

}

/* Shader method prototypes go here */
GLuint createProgram(std::string vertexShader, std::string fragmentShader) {
	GLuint *_fragment;
	GLuint *_vertex;
	GLchar *vertexText;
	GLchar *fragmentText;
	ShaderLoader *loader;
	loader = new ShaderLoader();

 	GLuint _program = glCreateProgram();

	//Handle the Fragment Shader
	{
        _fragment = new GLuint;
        *_fragment = glCreateShader(GL_FRAGMENT_SHADER);

		//fragmentText = loader->load("TODO");

        glShaderSource(*_fragment, 1, (const GLchar **) &fragmentText, 0);
        glCompileShader(*_fragment);

		//Check to see if the shader compiled correctly.
        if(checkCompilationStatus(*_fragment)) {
            std::cout << "Fragment Shader Compiled Properly" << std::endl;
        } else {
            std::cout << "Fragment Shader had a problem compiling" << std::endl;
            return NULL;
        }

        glAttachShader(_program, *_fragment);

        free(fragmentText);
	}

	//Handle the Vertex Shader
    {
        _vertex = new GLuint;
        *_vertex = glCreateShader(GL_VERTEX_SHADER);

	    //vertexText = loader->load("TPDP");
        glShaderSource(*_vertex, 1, (const GLchar **) &vertexText, 0);
        glCompileShader(*_vertex);

		//Check to see if the shader compiled correctly.
        if(checkCompilationStatus(*_vertex)) {
			std::cout << "Vertex Shader Compiled Properly" << std::endl;
        } else {
			std::cout << "Vertex Shader had a problem compiling" << std::endl;
			return NULL;
        }

	    glAttachShader(_program, *_vertex);

	    free(vertexText);
    }

    glLinkProgram(_program);

	//Check to make sure our program was linked properly.
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


/*All other Prototypes definitions start here */

void Initialize() {
	/* Variable Initialization */
	modelChosen = THIRTEENBOX;
	planeEnabled = true;

	/* Model Initialization */
    camera = viewglut_create(1.0, 100.0);
    p = plane_create(20);
	box = obj_create("thirteen-box.obj");
	troll = obj_create("trolluvd1.obj");

	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	createMenuItems();
}