#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include <stdio.h>
#include <string.h>
#include <math.h>
#include <vector>
#include <map>
#include "tinyxml2.h"
using namespace tinyxml2
#include "transformacao.h"



/**
*		Global variables used to store program data, rotations, translations, view
*	mode, mouse coords, colors, some variables used throughout the code, and finnaly the triangles used to render every object.
*		We will get into more details when we get to the functions that alter this
* values.
*/
float alfa = 0.0f, beta = 0.5f, radius = 500.0f;
float camX, camY, camZ;
float rotation = 0.0f;
float vert_rot, hori_rot;
char view_mode;
int x_pos, y_pos;
bool click = false;
float x,y,z;
int r,g,b;



/**
*		This is the structure that we used to store every point in a triangles
*	we have variables to represent the 3 cartesian coordinates for all the
* three points that make a triangle.
*		This structure also has a pointer to the next triangle that it has to
*	render.
*/


/**
*		This is the structure that we used to store every information
*	about the XML, whether is angle and coordenates if it is rotates, colors
* if it is a Color or the name of the models.
*		This structure also has a pointer to the next transformation that it has to
*	render.
*/
std::vector<Transformacao*> transformacoes;
float temp;



/**
* 	Function's declatarion allowing them to be put after main, or without
*	any specific order.
*		The specification for each of the functions will be written just before
*	their full implementation.
*/
void color(XMLElement* element2);
void translate(XMLElement* element2);
void rotate(XMLElement* element2);
void scale(XMLElement* element2);
void model(XMLElement* element2);
void popMatrix();
void pushMatrix();
int parserXML(XMLElement* pListElement);
int processXML(char* file);
std::vector<float> open3dModel(const char* tok);
void renderScene(void);
void renderFigures(void);
void renderAxis(void);
void changeSize(int w, int h);
void movement_handler(int x, int y);
void mouse_handler(int button, int state, int x, int y);
void special_key_handler(int key, int x, int y);
void normal_key_handler(unsigned char c, int x, int y);
void spherical2Cartesian();



/**
* 	This function is the main function of the program, every other function is
*	fully deppendent of this onde, both directly or indirectly. This is
*	responsible for initializing variables and setting up OpenGL. It also
* serves as a first step to user input validation.
*		The main function of the program is responsible for setting up not only
* the OpenGL elements but making sure that every variable is correctly set
* up for the rest of the program to continue, it's code is in an unchangeble
* order, and by altering it we might not obtain the desired results.
*
*	@param argc - Integer representing the number of arguments on which
*								the program was invoked
* @param argv - Array of Strings with every word which the program was
*								invoked
* @return int - Integer that serves as last resource to indicate if an error
*								as occured
*/
int main(int argc, char **argv) {
	vert_rot = hori_rot = 0.0f;
	view_mode = 'l';
	x_pos = y_pos = 400;

	if(argc < 2) {
		printf("Invalid Input!\n");
		return -1;
	}

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH|GLUT_DOUBLE|GLUT_RGBA);
	glutInitWindowPosition(100,100);
	glutInitWindowSize(800,800);
	glutCreateWindow("Rendering scene");

	glutDisplayFunc(renderScene);
	glutReshapeFunc(changeSize);

	glutSpecialFunc(special_key_handler);
	glutKeyboardFunc(normal_key_handler);
	glutMouseFunc(mouse_handler);
	glutMotionFunc(movement_handler);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

	spherical2Cartesian();

	int exitCode = processXML(argv[1]);
	if(exitCode < 0) return -1;
	glutMainLoop();

	return 1;
}




void color(XMLElement* element2) {
	if(!(r = element2->IntAttribute("R"))) r = r%256;
	if(!(g = element2->IntAttribute("G"))) g = g%256;
	if(!(b = element2->IntAttribute("B"))) b = b%256;
	Transformacao* tf = new Color(r,g,b);
	transformacoes.push_back(tf);
}



void translate(XMLElement* element2) {
	if(!(x = element2->FloatAttribute("X"))) x=0;
	if(!(y = element2->FloatAttribute("Y"))) y=0;
	if(!(z = element2->FloatAttribute("Z"))) z=0;
	Transformacao* tf = new Translate(x,y,z);
	transformacoes.push_back(tf);
}



void rotate(XMLElement* element2) {
	float ang;
	if(!(x = element2->FloatAttribute("X"))) x=0;
	if(!(y = element2->FloatAttribute("Y"))) y=0;
	if(!(z = element2->FloatAttribute("Z"))) z=0;
	if(!(ang = element2->FloatAttribute("angle"))) ang = 0;
	Transformacao* tf = new Rotate(ang,x,y,z);
	transformacoes.push_back(tf);
}



void scale(XMLElement* element2) {
	if(!(x = element2->FloatAttribute("X"))) x=0;
	if(!(y = element2->FloatAttribute("Y"))) y=0;
	if(!(z = element2->FloatAttribute("Z"))) z=0;
	Transformacao* tf = new Scale(x,y,z);
	transformacoes.push_back(tf);
}



void model(XMLElement* element2) {
	XMLElement* tftemp = element2;
	char* nome;
	std::vector<float> vc;
	while(tftemp != NULL) {
		nome = strdup((char*) tftemp->Attribute("file"));
		vc = open3dModel(nome);
		if(vc.size() == 0) {
			printf("Error opening %s!\n", nome);
			return;
		}
		printf("Opened %s successfully.\n", nome);
		Transformacao* tf = new Model(vc);
		transformacoes.push_back(tf);
		tftemp = tftemp->NextSiblingElement("model");
	}
}



void popMatrix() {
	Transformacao* tf = new PopMatrix();
	transformacoes.push_back(tf);
}



void pushMatrix() {
	Transformacao* tf = new PushMatrix();
	transformacoes.push_back(tf);
}



int parserXML(XMLElement* pListElement) {
	XMLElement* element2;
	XMLElement* tempEl;

	if(pListElement != NULL) {
		pushMatrix();

		element2 = pListElement->FirstChildElement("color");
		if(element2!=NULL) color(element2);

		element2 = pListElement->FirstChildElement("translate");
		if(element2!=NULL) translate(element2);

		element2 = pListElement->FirstChildElement("rotate");
		if(element2!=NULL) rotate(element2);

		element2 = pListElement->FirstChildElement("scale");
		if(element2!=NULL) scale(element2);

		element2 = pListElement->FirstChildElement("model");
		if(element2!=NULL) model(element2);

		tempEl = pListElement->FirstChildElement("group");
		if(tempEl != NULL) parserXML(tempEl);

		popMatrix();

		tempEl = pListElement->NextSiblingElement("group");
		if(tempEl != NULL) parserXML(tempEl);
	}
	return 1;
}



/**
* 	This function implements the tinyxml2 API, and it's used at program
*	startup when we are begining to process the XML file. This function is
* responsible for invoking the open3dModel function, since this will extract
*	the filenames where to find the 3d files.
*		It checks for the rootElement, in our case the scene, then it iterates
*	for the first group, running the parserXML which will parse the XML file
* indicated by "file", checking for error codes while doing so.
*
*	@param file - Character array with the path to the XML file
* @return int - Integer associated to an errorCode
*/
int processXML(char* file) {
	XMLDocument xmlDoc;
	XMLNode *root;
	XMLElement *pListElement;
	if(xmlDoc.LoadFile(file) != XML_SUCCESS) {
		printf("Error loading %s.\n", file);
		return -1;
	}

	root = xmlDoc.FirstChildElement("scene");
	pListElement = root -> FirstChildElement("group");
	parserXML(pListElement);
	return XML_SUCCESS;
}



/**
* 	This function is used everytime we want to open a 3d model file and save
* it to a global variable (t).
*		It will read a triplet of floats for each line of document that it reads.
*	We defined that as the composition of our .3d model files, so this function
* reads them procedurally and saves it to the global triangle variable.
*		We can assume that the .3d file as an ammount of lines %3 equal to 0.
*
*	@param tok - Character representing the name of the file to open.
* @return int - Integer containing exitStatus (0 = Sucess | <0 = Error Occured)
*/
std::vector<float> open3dModel(const char* tok) {
  	FILE *f_3d;
  	std::vector<float> vc;
	f_3d = fopen(tok, "r+");
	if (f_3d < 0) return vc;

	char *v1, *v2, *v3;
	v1 = (char*) malloc(sizeof(char) * 64);
	v2 = (char*) malloc(sizeof(char) * 64);
	v3 = (char*) malloc(sizeof(char) * 64);
	while(fscanf(f_3d, "%s %s %s", v1, v2, v3) != EOF) {
		vc.push_back( atof(v1) );
		vc.push_back( atof(v2) );
		vc.push_back( atof(v3) );
	}
	fclose(f_3d);
	return vc;
}



void renderScene(void) {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glLoadIdentity();
	gluLookAt(camX, camY, camZ,
		      	0.0f, 0.0f, 0.0f,
			  		0.0f, 1.0f, 0.0f);

	glRotatef(vert_rot, 1, 0, 0);
	glRotatef(hori_rot, 0, 1, 0);

	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glFrontFace(GL_CCW);
	glPolygonMode(GL_FRONT, view_mode == 'f' ? GL_FILL :
						              	view_mode == 'l' ? GL_LINE :
															GL_POINT);
	renderFigures();
	//renderAxis();

	glutSwapBuffers();
}



/**
* 	This function is only invoked by render scene, and it's used every frame
*	this is responsible for rendering the three axis (XYZ).
*		It is the only function in this project that as interely hard-coded values.
*
*	@param void
* @return void
*/
void renderAxis(void) {
	glLineWidth(2.5f);
	glBegin(GL_LINES);
		glColor3f(0.0, 0.0, 1.0);
		glVertex3f(0, 0, 0);
		glVertex3f(2, 0, 0);
		glColor3f(0.0, 1.0, 0.0);
		glVertex3f(0, 0, 0);
		glVertex3f(0, 2, 0);
		glColor3f(1.0, 0.0, 0.0);
		glVertex3f(0, 0, 0);
		glVertex3f(0, 0, 2);
	glEnd();
}



/**
* 	This function is only invoked by render scene, and it's sole purpose
*	is to render every triangle that has been previously saved to memory.
*	It's important to denote that this render is made using tri-vertex
* aggregation to triangles.
*		This uses a color swap method, wich means that every odd iteration the
* color remains the same, but changing from one to another iteration. It
* acesses the global pointer to the structure that we defined to store every
* triangle, making sure to draw the correct coordinates.
*
*	@param void
* @return void
*/
void renderFigures(void) {
	int color;
	std::vector<float> vc;
	Transformacao* tftemp;
	glLineWidth(1.0f);
	for(int i=0;i<transformacoes.size();i++){
		tftemp = transformacoes.at(i);
		tftemp -> transformar();
	}
}



/**
* 	This function is invoked everytime the viewing window is reshaped.
*		It checks if the window is too short, preventing the user to make
*	a small window. It then sets the correct parameters for the renderization
* to take place.
*
* @param w - Integer representing the new window width
* @param h - Integer representing the new window height
* @return void
*/
void changeSize(int w, int h) {
	// Prevent a divide by zero, when window is too short
	// (you cant make a window with zero width).
	if(h == 0) h = 1;
	// compute window's aspect ratio
	float ratio = w * 1.0 / h;
	// Set the projection matrix as current
	glMatrixMode(GL_PROJECTION);
	// Load Identity Matrix
	glLoadIdentity();
	// Set the viewport to be the entire window
    glViewport(0, 0, w, h);
	// Set perspective
	gluPerspective(45.0f ,ratio, 1.0f ,10000.0f);
	// return to the model view matrix mode
	glMatrixMode(GL_MODELVIEW);
}



void spherical2Cartesian() {
	camX = radius * cos(beta) * sin(alfa);
	camY = radius * sin(beta);
	camZ = radius * cos(beta) * cos(alfa);
}



/**
* 	This function is invoked only when mouse moves on screen. It receives
*	two valus representing the current mouse position.
*		The movement_handler function makes sure that we apply the correct
*	rotation to the scene when we drag the mouse arrow, it updates the final
* mouse position once it's done ajusting the rotation for the next render
* invocation.
*
* @param x - Integer representing the mouse x position
* @param y - Integer representing the mouse y position
* @return void
*/
void movement_handler(int x, int y) {
	if (click) {
		hori_rot -= (x_pos - x) / 2;
		vert_rot -= (y_pos - y) / 2;
		x_pos = x;
		y_pos = y;

		//Re-render
		glutPostRedisplay();
	}
}



/**
* 	This function is invoked only when mouse buttons are pressed. It receives
*	an indicator of which key, and the state of it (Down meaning it's being pressed
*	and Down meaning it's not).
*		This function arranges with three variables, it sets the (click) variable
*	either to true or false meaning that the mouse is or is not pressed. It also
*	updates the coordenates of the mouse, so that we can track it's movement.
*
*	@param button - Integer representing the mouse button that is being pressed
* @param state - Integer representation of Press value (GLUT_UP | GLUT_DOWN)
* @param x - Integer representing the mouse x position
* @param y - Integer representing the mouse y position
* @return void
*/
void mouse_handler(int button, int state, int x, int y) {
	if (button == GLUT_LEFT_BUTTON) {
		if (state == GLUT_DOWN) {
			click = true;
			x_pos = x;
			y_pos = y;
		}
		if (state == GLUT_UP) click = false;
	}
}




void special_key_handler(int key, int x, int y) {
	switch(key) {
		case GLUT_KEY_PAGE_DOWN:
			radius -= 20.0f;
			if (radius < 250.0f) radius = 250.0f;
			break;
		case GLUT_KEY_PAGE_UP:
			radius += 20.0f;
			break;
	}
	spherical2Cartesian();
	glutPostRedisplay();
}




/**
* 	This function handles a normal key press, changing the right variables
*	to obtain the desired results.
*		We use a global variable (view_mode) to indicate the key that the user
*	pressed, and then we can render the desired scenes accordingly. This
* variable stores an 'f' if the user wants to see the object filled, an 'l'
* if we want to see only lines and finnaly a 'p' if we only want to render
* points.
*
*	@param c - Character representing the key pressed (non special keys)
* @param x - Integer representing the mouse x position
* @param y - Integer representing the mouse y position
* @return void
*/
void normal_key_handler(unsigned char c, int x, int y) {
	switch (c) {
		case 'f':
		case 'F':
			view_mode = 'f';
			break;
		case 'l':
		case 'L':
			view_mode = 'l';
			break;
		case 'p':
		case 'P':
			view_mode = 'p';
			break;
		default: break;
	}

	//Re-render
	glutPostRedisplay();
}
