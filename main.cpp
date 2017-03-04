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

typedef struct triang {
	float p1_x, p1_y, p1_z;
	float p2_x, p2_y, p2_z;
	float p3_x, p3_y, p3_z;
	struct triang *next;
} *Triangulo;



float vert_rot, hori_rot, vert_trans, hori_trans;
float val_trans = 0.2f;
char view_mode;
int x_pos, y_pos;
int color;
bool click = false;
Triangulo t, t_prev, t_temp;



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



void renderFigures(void) {
	Triangulo temp = t;
	glBegin(GL_TRIANGLES);
	glLineWidth(1.0f);
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
	key == GLUT_KEY_UP ? vert_trans -= val_trans :
		key == GLUT_KEY_DOWN ? vert_trans += val_trans :
			key == GLUT_KEY_RIGHT ? hori_trans += val_trans :
				key == GLUT_KEY_LEFT ? hori_trans -= val_trans : ' ';

	//Re-render
	glutPostRedisplay();
}



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
