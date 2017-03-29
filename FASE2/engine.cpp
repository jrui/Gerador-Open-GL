#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include <stdio.h>
#include <string.h>
#include <math.h>
#include <vector>
#include "tinyxml2.h"
using namespace tinyxml2;



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



/**
*		This is the structure that we used to store every point in a triangles
*	we have variables to represent the 3 cartesian coordinates for all the
* three points that make a triangle.
*		This structure also has a pointer to the next triangle that it has to
*	render.
*/
class Triangulo {
	public:
		float p1_x, p1_y, p1_z;
		float p2_x, p2_y, p2_z;
		float p3_x, p3_y, p3_z;
		int grupo;
};
std::vector<Triangulo> triangulos;
int group_figure = 0;



/**
*		This is the structure that we used to store every information
*	about the XML, whether is angle and coordenates if it is rotates, colors
* if it is a Color or the name of the models.
*		This structure also has a pointer to the next transformation that it has to
*	render.
*/
class Transformacao {
	public:
		float angle, x, y, z;
		char* model;
		int colorR, colorB, colorG, modo;
};
std::vector<Transformacao> transformacoes;
int r,g,b;
float temp;



/**
* 	Function's declatarion allowing them to be put after main, or without
*	any specific order.
*		The specification for each of the functions will be written just before
*	their full implementation.
*/
void initXML(Transformacao t);
void color(XMLElement* element2);
void translate(XMLElement* element2);
void rotate(XMLElement* element2);
void scale(XMLElement* element2);
void model(XMLElement* element2);
void popMatrix();
void pushMatrix();
int parserXML(XMLElement* pListElement);
int processXML(char* file);
int open3dModel(const char* tok);
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

	int exitCode = processXML(argv[1]);
	if(exitCode < 0) return -1;

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

	glutMainLoop();

	return 1;
}



Transformacao initXML() {
	Transformacao t;
	t.angle = -1;
	t.x = 0;
	t.y = 0;
	t.z = 0;
	t.model = (char*) malloc(sizeof(char)*64);
	t.model = NULL;
	t.modo = -1;
	t.colorB = 0;
	t.colorG = 0;
	t.colorR = 0;
	return t;
}



void color(XMLElement* element2) {
	int temp2;
	Transformacao tf = initXML();
	tf.modo = 3;
	if((temp2 = element2->IntAttribute("R"))) tf.colorR = temp2%256;
	if((temp2 = element2->IntAttribute("G"))) tf.colorG = temp2%256;
	if((temp2 = element2->IntAttribute("B"))) tf.colorB = temp2%256;
	transformacoes.push_back(tf);
}



void translate(XMLElement* element2) {
	Transformacao tf = initXML();
	tf.modo = 4;
	if((temp = element2->FloatAttribute("X"))) tf.x = temp;
	if((temp = element2->FloatAttribute("Y"))) tf.y = temp;
	if((temp = element2->FloatAttribute("Z"))) tf.z = temp;
	transformacoes.push_back(tf);
}



void rotate(XMLElement* element2) {
	Transformacao tf = initXML();
	tf.modo = 6;
	if((temp = element2->FloatAttribute("X"))) tf.x = temp;
	if((temp = element2->FloatAttribute("Y"))) tf.y = temp;
	if((temp = element2->FloatAttribute("Z"))) tf.z = temp;
	if((temp = element2->FloatAttribute("angle"))) tf.angle = temp;
	transformacoes.push_back(tf);
}



void scale(XMLElement* element2) {
	Transformacao tf = initXML();
	tf.modo = 5;
	if((temp = element2->FloatAttribute("X"))) tf.x = temp;
	if((temp = element2->FloatAttribute("Y"))) tf.y = temp;
	if((temp = element2->FloatAttribute("Z"))) tf.z = temp;
	transformacoes.push_back(tf);
}



void model(XMLElement* element2) {
	XMLElement* tftemp = element2;
	while(tftemp != NULL) {
		Transformacao tf = initXML();
		tf.modo = 7;
		tf.model = (char*) tftemp->Attribute("file");
		int errorCode = open3dModel(tf.model);
		if(errorCode < 0) {
			printf("Error opening %s!\n", "sphere.3d");
		}
		printf("Opened %s successfully.\n", "sphere.3d");
		transformacoes.push_back(tf);
		tftemp = tftemp->NextSiblingElement("model");
	}
}



void popMatrix() {
	Transformacao tf = initXML();
	tf.modo = 2;
	transformacoes.push_back(tf);
}



void pushMatrix() {
	Transformacao tf = initXML();
	tf.modo = 1;
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
int open3dModel(const char* tok) {
  FILE *f_3d;
	f_3d = fopen(tok, "r+");
	if (f_3d < 0) return -1;

	char *v1, *v2, *v3;
	v1 = (char*) malloc(sizeof(char) * 64);
	v2 = (char*) malloc(sizeof(char) * 64);
	v3 = (char*) malloc(sizeof(char) * 64);
	int op = 1;
	Triangulo t;
	while(fscanf(f_3d, "%s %s %s", v1, v2, v3) != EOF) {
		if(op == 1) {
			t.p1_x = atof(v1);
			t.p1_y = atof(v2);
			t.p1_z = atof(v3);
			op = 2;
		}
		else {
			if(op == 2) {
				t.p2_x = atof(v1);
				t.p2_y = atof(v2);
				t.p2_z = atof(v3);
				op = 3;
			}
			else {
				t.p3_x = atof(v1);
				t.p3_y = atof(v2);
				t.p3_z = atof(v3);
				op = 1;
				t.grupo = group_figure;
				triangulos.push_back(t);
			}
		}
	}
	group_figure++;
	fclose(f_3d);
	return 0;
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
	Transformacao tftemp;
	glLineWidth(1.0f);
	for(int i=0;i<transformacoes.size();i++){
		tftemp = transformacoes.at(i);
		switch(tftemp.modo){
			case 1:
				glPushMatrix();
				break;
			case 2:
				glPopMatrix();
				break;
			case 3:
				r = tftemp.colorR;
				g = tftemp.colorG;
				b = tftemp.colorB;
				break;
			case 4:
				glTranslatef(tftemp.x, tftemp.y, tftemp.z);
				break;
			case 5:
				glScalef(tftemp.x, tftemp.y, tftemp.z);
				break;
			case 6:
				glRotatef(tftemp.angle,tftemp.x, tftemp.y, tftemp.z);
				break;
			case 7:
				Triangulo t;
				int i;
				glBegin(GL_TRIANGLES);
				for(i = 0; i < triangulos.size(); i++) {
					t = triangulos.at(i);
					glColor3ub(r, g, b);
					glVertex3f(t.p1_x, t.p1_y, t.p1_z);
					glVertex3f(t.p2_x, t.p2_y, t.p2_z);
					glVertex3f(t.p3_x, t.p3_y, t.p3_z);
				}
				glEnd();
				break;
			default:
				break;
		}
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
			radius -= 10.0f;
			if (radius < 250.0f) radius = 250.0f;
			break;
		case GLUT_KEY_PAGE_UP:
			radius += 10.0f;
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