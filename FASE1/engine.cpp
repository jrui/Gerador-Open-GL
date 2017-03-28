#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include <stdio.h>
#include <string.h>
#include <math.h>

#include "tinyxml2.h"
using namespace tinyxml2;



/**
*		This is the structure that we used to store every point in a triangles
*	we have variables to represent the 3 cartesian coordinates for all the
* three points that make a triangle.
*		This structure also has a pointer to the next triangle that it has to
*	render.
*/
typedef struct triang {
	float p1_x, p1_y, p1_z;
	float p2_x, p2_y, p2_z;
	float p3_x, p3_y, p3_z;
	struct triang *next;
} *Triangulo;



/**
*		Global variables used to store program data, rotations, translations, view
*	mode, mouse coords, and finnaly the triangles used to render every object.
*		We will get into more details when we get to the functions that alter this
* values.
*/
float vert_rot, hori_rot, vert_trans, hori_trans;
float val_trans = 0.2f;
char view_mode;
int x_pos, y_pos;
int color;
bool click = false;
Triangulo t, t_prev, t_temp;



/**
* 	Function's declatarion allowing them to be put after main, or without
*	any specific order.
*		The specification for each of the functions will be written just before
*	their full implementation.
*/
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
	vert_rot = hori_rot = vert_trans = hori_trans = 0.0f;
	view_mode = 'l';
	color = 0;
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

	glutMainLoop();

	return 1;
}



/**
* 	This function implements the tinyxml2 API, and it's used at program
*	startup when we are begining to process the XML file. This function is
* responsible for invoking the open3dModel function, since this will extract
*	the filenames where to find the 3d files.
*		It checks for the rootElement, in our case the scene, then it iterates
*	for every model element, then it retrieves the filename under the attribute
* indicated by "file", checking for error codes while doing so.
*
*	@param file - Character array with the path to the XML file
* @return int - Integer associated to an errorCode
*/
int processXML(char* file) {
	XMLDocument xmlDoc;
	if(xmlDoc.LoadFile(file) != XML_SUCCESS) {
		printf("Error loading %s.\n", file);
		return -1;
	}

	XMLNode *pRoot = xmlDoc.FirstChild();
	if (pRoot == NULL) {
		printf("Invalid XML Format!\n");
		return XML_ERROR_FILE_READ_ERROR;
	}

	XMLElement *pListElement = pRoot->FirstChildElement("model");
	if (pListElement == NULL) {
		printf("Invalid XML Tag!\n");
		return XML_ERROR_PARSING_ELEMENT;
	}

	t = (Triangulo) malloc(sizeof(struct triang));
	t_temp = t;
	const char *file3d = NULL;
	while(pListElement != NULL) {
		file3d = pListElement->Attribute("file");
		if (file3d == NULL) {
			printf("Error getting .3d file!\n");
			return XML_ERROR_PARSING_ATTRIBUTE;
		}

		int errorCode = open3dModel(file3d);
		if(errorCode < 0) {
			printf("Error opening %s!\n", file3d);
			return -1;
		}
		printf("Opened %s successfully.\n", file3d);


		pListElement = pListElement->NextSiblingElement("model");
	}
	t_prev->next = NULL;

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
	while(fscanf(f_3d, "%s %s %s", v1, v2, v3) != EOF) {
		if(op == 1) {
			t_temp->p1_x = atof(v1);
			t_temp->p1_y = atof(v2);
			t_temp->p1_z = atof(v3);
			op = 2;
		}
		else {
			if(op == 2) {
				t_temp->p2_x = atof(v1);
				t_temp->p2_y = atof(v2);
				t_temp->p2_z = atof(v3);
				op = 3;
			}
			else {
				t_temp->p3_x = atof(v1);
				t_temp->p3_y = atof(v2);
				t_temp->p3_z = atof(v3);
				op = 1;
				t_temp->next = (Triangulo) malloc(sizeof(struct triang));
				t_prev = t_temp;
				t_temp = t_temp->next;
			}
		}
	}
	fclose(f_3d);
	return 0;
}



/**
* 	This function is only invoked either by the glutMainLoop or by a user
*	key press / mouse interaction. What this function does is render the scene
*	specified in the program input XML file.
*		This function sets up the camera position, view point. It also renders
* rotations and translations that the user might have done to the scenery.
*	It then checks how the user wants to render the images, either by filling
* them, seeing only the lines or the points. Finally it calls the renderFigures
* function.
*
*	@param void
* @return void
*/
void renderScene(void) {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glLoadIdentity();
	gluLookAt(0.0f, 15.0f, 15.0f,
		      	0.0f, 0.0f, 0.0f,
			  		0.0f, 1.0f, 0.0f);

	glTranslatef(hori_trans, 0, 0);
	glTranslatef(0, 0, vert_trans);
	glRotatef(vert_rot, 1, 0, 0);
	glRotatef(hori_rot, 0, 1, 0);

	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glFrontFace(GL_CCW);
	glPolygonMode(GL_FRONT, view_mode == 'f' ? GL_FILL :
						              	view_mode == 'l' ? GL_LINE :
															GL_POINT);

	renderFigures();
	renderAxis();

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
	Triangulo temp = t;
	glLineWidth(1.0f);
	glBegin(GL_TRIANGLES);
		while(temp != NULL) {
			color = 100 - color;
			glColor3ub(0, 150 - color, 150 + color);
			glVertex3f(temp->p1_x, temp->p1_y, temp->p1_z);
			glVertex3f(temp->p2_x, temp->p2_y, temp->p2_z);
			glVertex3f(temp->p3_x, temp->p3_y, temp->p3_z);
			temp = temp->next;
		}
	glEnd();
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
	gluPerspective(45.0f ,ratio, 1.0f ,1000.0f);
	// return to the model view matrix mode
	glMatrixMode(GL_MODELVIEW);
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



/**
* 	This function is invoked only when special key are pressed, changing
*	the right variables to obtain the desired results.
* 	This function manipulates two variables, and uses a third one. It
*	updates the value of the rotation of the scene, allowing it to rotate
* either over the X axis (UP and DOWN arrows) or the Y axis (LEFT and RIGHT
* arrows). The third variable (val_trans) indicates the value of the increment
* or decrement, so we can consider it a sort of rotational speed variable.
*
*	@param key - Integer representing the key pressed (special keys only)
* @param x - Integer representing the mouse x position
* @param y - Integer representing the mouse y position
* @return void
*/
void special_key_handler(int key, int x, int y) {
	key == GLUT_KEY_UP ? vert_trans -= val_trans :
		key == GLUT_KEY_DOWN ? vert_trans += val_trans :
			key == GLUT_KEY_RIGHT ? hori_trans += val_trans :
				key == GLUT_KEY_LEFT ? hori_trans -= val_trans : ' ';

	//Re-render
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
