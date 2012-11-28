#include <iostream>
#include <string>

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

/* OpenGL Shader Prototypes go here */
GLuint createProgram(const char *, const char *);
bool checkCompilationStatus(GLuint);
bool checkLinkingStatus(GLuint);

/* Frame Buffer Prototypes below */
bool CheckFrameBuffer();
void fbofail(std::string);


/*All Other Prototypes */
void Initialize();
void InitializeTextures();
void LoadTextures(const char *);


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
	//glLoadIdentity();

	GLfloat S[16] = {
			0.5f, 0.0f, 0.0f, 0.0f,
			0.0f, 0.5f, 0.0f, 0.0f,
			0.0f, 0.0f, 0.5f, 0.0f,
			0.5f, 0.5f, 0.5f, 1.0f,
    };

	GLdouble size = 16.0;

	switch(spotLightChosen) {
	case SPOTLIGHT1:
		glMatrixMode(GL_TEXTURE);
		{
			glLoadMatrixf(S);
		   // glOrtho(-size, +size, -size, +size, -size, +size);

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
		glActiveTexture(GL_TEXTURE0 + 0);
		glActiveTexture(GL_TEXTURE0 + 2);
		glActiveTexture(GL_TEXTURE0 + 4);
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
		std::cout << "User has chose to reset the program" << std::endl;
		glActiveTexture(GL_TEXTURE15);
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
		GLuint diffuse;
		GLuint normal;
		GLuint specular;

		GLint baseDiffuseLocation;
		GLint baseNormalLocation;
		GLint baseSpecularLocation;

		glGenTextures(1, &diffuse);
		LoadTextures("thirteen-diffuse.png");

		glGenTextures(1, &normal);
		LoadTextures("thirteen-normal.png");
		
		glGenTextures(1, &specular);
		LoadTextures("thirteen-specular.png");

		GLuint thirteenProgram;

		thirteenProgram = createProgram("spotlightVertexShader.glsl", "spotlightFragmentShader.glsl");

		baseDiffuseLocation = glGetUniformLocation(thirteenProgram, "diffuse");
		baseNormalLocation = glGetUniformLocation(thirteenProgram, "normalvec");
		baseSpecularLocation = glGetUniformLocation(thirteenProgram, "specular");
		
		glUseProgram(thirteenProgram);

		glUniform1i(baseDiffuseLocation, 0);
		glUniform1i(baseNormalLocation, 2);
		glUniform1i(baseSpecularLocation, 4);

		glActiveTexture(GL_TEXTURE0 + 0);
		glBindTexture(GL_TEXTURE_2D, diffuse);

		glActiveTexture(GL_TEXTURE0 + 2);
		glBindTexture(GL_TEXTURE_2D, normal);

		glActiveTexture(GL_TEXTURE0 + 4);
		glBindTexture(GL_TEXTURE_2D, specular);

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
	GLuint spotLight1Program;
	GLuint spotLight2Program;
	GLuint spotlight;
	GLint baseImageLoc;
	switch(option) {
	case 1:
		std::cout << "Spotlight 1 selected" << std::endl;
		
		glGenTextures(1, &spotlight);
		LoadTextures("spotlight2.png");

		spotLight1Program = createProgram("spotlightVertexShader.glsl", "spotlightFragmentShader.glsl");

		baseImageLoc = glGetUniformLocation(spotLight1Program, "image");
		
		glUseProgram(spotLight1Program);
		glUniform1i(baseImageLoc, 0);

		glActiveTexture(GL_TEXTURE0 + 0);
		glBindTexture(GL_TEXTURE_2D, spotlight);
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

/* Shader method prototypes go here */
GLuint createProgram(const char *vertexShaderName, const char *fragmentShaderName) {
	std::cout << "Creating new program for shaders " << vertexShaderName << " and " << fragmentShaderName << std::endl;
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

		fragmentText = loader->load(fragmentShaderName);

        glShaderSource(*_fragment, 1, (const GLchar **) &fragmentText, 0);

		std::cout << "Compiling Fragment Shader" << std::endl;
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

	    vertexText = loader->load(vertexShaderName);
        glShaderSource(*_vertex, 1, (const GLchar **) &vertexText, 0);

		std::cout << "Compiling Vertex Shader" << std::endl;
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

	std::cout << "Linking Program" << std::endl;
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

	std::cout << "Checking Compilation Status of Program" << std::endl;

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
	std::cout << "Checking Linking Status of Program" << std::endl;
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
	
	glEnable(GL_TEXTURE_2D);
	//glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	createMenuItems();

	InitializeTextures();
	std::cout << "Initialization Finished" << std::endl;
}

void InitializeTextures() {
	std::cout << "Initializing Textures" << std::endl;
    /*GLuint spotLight1;
	GLuint spotLight2;

	glGenTextures(1, &spotLight1);
	glGenTextures(1, &spotLight2);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, spotLight1);
	LoadTextures("spotlight2.png");
*/
	std::cout << "Initializing Textures Done" << std::endl;

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