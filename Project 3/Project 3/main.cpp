#include <iostream>
#include <string>
#include <math.h>

#include "gl.h"
#include "image.h"
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

/* Frame Buffer Prototypes below */
bool CheckFrameBuffer();
void fbofail(std::string);


/*All Other Prototypes */
void Initialize();
void LoadTextures(const char *);
static void lights();
static void pan_light(GLfloat, GLfloat);
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

//For SpotLight
int spotLightChosen;

//For Light
//light position variables
static int      click_modifiers;
static GLfloat  click_light[2];
static int      click_button = -1;
static int      click_x;
static int      click_y;
static GLfloat  light[2]    = { -60.0, 30.0             };
static GLfloat  point[4]    = {   0.0,  0.0,  0.0,  0.0 };
static GLfloat  zoom        = 0.5;

//For Loading Shaders with objects
ShaderLoader *boxloader;
ShaderLoader *spotlight1loader;

/* All of our definitions should go here */
#define THIRTEENBOX 1
#define TROLL 2
#define NOMODELS 3
#define NOPLANE 4

#define SPOTLIGHT1 1
#define SPOTLIGHT2 2
#define NOSPOTLIGHT 3


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

	//glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	
	GLfloat S[16] = {
			0.5f, 0.0f, 0.0f, 0.0f,
			0.0f, 0.5f, 0.0f, 0.0f,
			0.0f, 0.0f, 0.5f, 0.0f,
			0.5f, 0.5f, 0.5f, 1.0f,
    };

	GLdouble size = 16.0;

	switch(spotLightChosen) {
	case SPOTLIGHT1: {
						GLint spotlight1location = glGetUniformLocation(spotlight1loader->getProgram(), "image");
						spotlight1loader->bind();
						glUniform1i(spotlight1location, 0);

						spotlight1loader->bind();
						glActiveTexture(GL_TEXTURE0);
						//std::cout << "Texture 1: " << boxloader->getTexture(0) << std::endl;
						glBindTexture(GL_TEXTURE_2D, spotlight1loader->getTexture(0));
						
						glMatrixMode(GL_TEXTURE);
						{
							glLoadMatrixf(S);
						    glOrtho(-size, +size, -size, +size, -size, +size);
						    glRotated(-light[0], 1.0, 0.0, 0.0);
							glRotated(-light[1], 0.0, 1.0, 0.0);
						}
					}
		break;
	case SPOTLIGHT2:
		break;
	case NOSPOTLIGHT:
		//
		break;
	default:
		std::cout << "OnDraw - spotLightChosen: Shouldn't be here" << std::endl;
		break;
	}

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	lights();

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
	case THIRTEENBOX:{	//glMatrixMode(GL_TEXTURE);
						//glEnable(GL_TEXTURE_2D);
						GLint diffuselocation = glGetUniformLocation(boxloader->getProgram(), "diffuse_texture");
						GLint normallocation = glGetUniformLocation(boxloader->getProgram(), "normal_texture");

						//std::cout << "diffuse location: " << diffuselocation << std::endl;
						//std::cout << "normal location: " << normallocation << std::endl;

						boxloader->bind();
						glUniform1i(diffuselocation, 0);
						glUniform1i(normallocation, 1);

						//glActiveTexture(GL_TEXTURE0 + boxloader->getTexture(0));
						glActiveTexture(GL_TEXTURE0);
						//std::cout << "Texture 1: " << boxloader->getTexture(0) << std::endl;
						glBindTexture(GL_TEXTURE_2D, boxloader->getTexture(0));
						glActiveTexture(GL_TEXTURE1);
						//std::cout << "Texture 2: " << boxloader->getTexture(1) << std::endl;
						glBindTexture(GL_TEXTURE_2D, boxloader->getTexture(1));
					
						obj_render(box);
						
						boxloader->unbind();
					}
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
	glMatrixMode(GL_MODELVIEW);

	glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);
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

	const int w = glutGet(GLUT_WINDOW_WIDTH);
    const int h = glutGet(GLUT_WINDOW_HEIGHT);

    GLfloat H = .1f * zoom * w / h;
    GLfloat V = .1f * zoom;
    GLfloat r;

    /* Compute the pointer motion as a fraction of window size. */

    GLfloat dx = (GLfloat) (x - click_x) / w;
    GLfloat dy = (GLfloat) (y - click_y) / h;

   	if (click_button == GLUT_LEFT_BUTTON)
    {
		if (click_modifiers == GLUT_ACTIVE_ALT){
			std::cout << "Panning Light" << std::endl;
			pan_light (dx, dy); 
		} 
    } else {
    	/* Camera */
		std::cout << "Moving Camera" << std::endl;
		viewglut_point(camera, x, y);
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

static void mouse(int b, int s, int x, int y) {
	/* Camera */
	if(b == GLUT_LEFT_BUTTON) {
		click_modifiers   = glutGetModifiers();
		if(s == GLUT_DOWN) {
			if(click_modifiers == GLUT_ACTIVE_ALT) {
				std::cout << "Mouse has ALT Modifier" << std::endl;
				click_light[0]    = light[0];
	        	click_light[1]    = light[1];
	        	click_button      = b;
	            click_x           = x;
	            click_y           = y;
			} else {
				std::cout << "ViewGlut caught click." << std::endl;
				viewglut_click(camera, x, y, 1);
			}
			
			

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
		std::cout << "User has chose to reset the program" << std::endl;
		//glActiveTexture(GL_TEXTURE15);
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
		std::cout << "User has selected the ThirteenBox Model" << std::endl;
		modelChosen = THIRTEENBOX;
		glutPostRedisplay();
		break;
	case TROLL:
		std::cout << "User has selected the Troll Model" << std::endl;
		modelChosen = TROLL;
		break;
	case NOMODELS:
		std::cout << "User has selected No Models" << std::endl;
		modelChosen = NOMODELS;
		break;
	case NOPLANE:
		std::cout << "User has toggled the Plane" << std::endl;
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
		std::cout << "Spotlight 1 selected" << std::endl;
		spotLightChosen = SPOTLIGHT1;
		glutPostRedisplay();
		break;
	case 2:
		std::cout << "Spotlight 2 selected" << std::endl;
		break;
	default:
		std::cout << "Shader Menu - Not Supposed to be here. Sent option: " << option << std::endl;
		break;
	}

}

/*All other Prototypes definitions start here */

void Initialize() {
	std::cout << "Starting Initialization" << std::endl;
	/* Variable Initialization */
	modelChosen = THIRTEENBOX;
    spotLightChosen = NOSPOTLIGHT;
	planeEnabled = true;

	/* Model Initialization */
    camera = viewglut_create(1.0, 100.0);
    p = plane_create(20);
	box = obj_create("thirteen-box.obj");
	troll = obj_create("trolluvd1.obj");
	
	//glEnable(GL_TEXTURE_2D);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LESS);
	createMenuItems();

	boxloader = new ShaderLoader("thirteenBoxVertexShader2.glsl", "thirteenBoxFragmentShader2.glsl");
	boxloader->initializeTextures("thirteen-diffuse.png");
	boxloader->initializeTextures("thirteen-normal.png");
	//boxloader->initializeTextures("thirteen-specular.png");

	spotlight1loader = new ShaderLoader("spotlightVertexShader.glsl", "spotlightFragmentShader.glsl");
	spotlight1loader->initializeTextures("spotlight2.png");

	std::cout << "Initialization Finished" << std::endl;
}

void LoadTextures(const char *textureName) {
	std::cout << "Loading Texture " << textureName << std::endl;
    int w, h, c, b;

    void *p = image_read(textureName, &w, &h, &c, &b);

    int i = image_internal_form(c, b);
	int e = image_external_form(c);
    int t = image_external_type(b);

    image_flip(w, h, c, b, p);

    glTexImage2D(GL_TEXTURE_2D, 0, i, w, h, 0, e, t, p);
    free(p);

	std::cout << "Done Loading Texture " << textureName << std::endl;
}

/* Frame buffer prototype definitions below */
bool CheckFrameBuffer() {
    switch(glCheckFramebufferStatus(GL_FRAMEBUFFER)) {
    case GL_FRAMEBUFFER_COMPLETE: return true;
    case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT: fbofail("Attachment"); return false;
    case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT: fbofail("Missing attachment"); return false;
    case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER: fbofail("Draw buffer"); return false;
    case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER: fbofail("Read buffer"); return false;
    case GL_FRAMEBUFFER_UNSUPPORTED: fbofail("Unsupported"); return false;
    default: fbofail("Unknown"); return false;
    }
}
void fbofail(std::string message) {
	std::cout << "Frame Buffer Encountered Problem " << message << std::endl;
}


/* For the lights */
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