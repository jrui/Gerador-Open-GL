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


class Triangulo {
	public:
		float p1_x, p1_y, p1_z;
		float p2_x, p2_y, p2_z;
		float p3_x, p3_y, p3_z;
		int grupo;
};
std::vector<Triangulo> triangulos;
int group_figure = 0;

typedef struct transf {
	float angle;
	float x, y, z;
	char* model;
	int modo;
	int colorR, colorB, colorG;
	struct transf *next;
} *Transformacao;


float vert_rot, hori_rot, vert_trans, hori_trans;
float val_trans = 0.2f;
char view_mode;
int x_pos, y_pos;
bool click = false;
Transformacao tf, tf_prev, tf_temp;
int r = 0, g = 0, b = 0;
float temp;
int temp2;


int processXML(char* file);
void freeXML();
void initXML(Transformacao t);
int parserXML(XMLElement* pListElement);
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
	freeXML();
	printf("FreeXML\n");

	return 1;
}



void initXML(Transformacao t) {
	t->angle = -1;
	t->x = 0;
	t->y = 0;
	t->z = 0;
	t->model = NULL;
	// 1= push, 2= pop, 3= color, 4= translate, 5 = scale, 6= rotate, 7 = model;
	t->modo = -1;
	t->colorB = 0;
	t->colorG = 0;
	t->colorR = 0;
}



void freeXML(){
	Transformacao tf_prev = tf, tf_temp = tf;
	while(tf_temp != NULL) {
		tf_temp = tf_temp->next;
		free(tf_prev);
		tf_prev = tf_temp;
	}
	free(tf_prev);
}



void color(XMLElement* element2) {
	tf_temp->modo = 3;
	if((temp2 = element2->IntAttribute("R"))) tf_temp->colorR = temp2%256;
	if((temp2 = element2->IntAttribute("G"))) tf_temp->colorG = temp2%256;
	if((temp2 = element2->IntAttribute("B"))) tf_temp->colorB = temp2%256;

	tf_temp->next = (Transformacao) malloc(sizeof(struct transf));
	tf_temp = tf_temp->next;
	initXML(tf_temp);
}



void translate(XMLElement* element2) {
	tf_temp->modo = 4;
	if((temp = element2->FloatAttribute("X"))) tf_temp->x = temp;
	if((temp = element2->FloatAttribute("Y"))) tf_temp->y = temp;
	if((temp = element2->FloatAttribute("Z"))) tf_temp->z = temp;

	tf_temp->next = (Transformacao) malloc(sizeof(struct transf));
	tf_temp = tf_temp->next;
	initXML(tf_temp);
}



void rotate(XMLElement* element2) {
	tf_temp->modo = 6;
	if((temp = element2->FloatAttribute("X"))) tf_temp->x = temp;
	if((temp = element2->FloatAttribute("Y"))) tf_temp->y = temp;
	if((temp = element2->FloatAttribute("Z"))) tf_temp->z = temp;
	if((temp = element2->FloatAttribute("angle"))) tf_temp->angle = temp;

	tf_temp->next = (Transformacao) malloc(sizeof(struct transf));
	tf_temp = tf_temp->next;
	initXML(tf_temp);
}



void scale(XMLElement* element2) {
	tf_temp->modo = 5;
	if((temp = element2->FloatAttribute("X"))) tf_temp->x = temp;
	if((temp = element2->FloatAttribute("Y"))) tf_temp->y = temp;
	if((temp = element2->FloatAttribute("Z"))) tf_temp->z = temp;

	tf_temp->next = (Transformacao) malloc(sizeof(struct transf));
	tf_temp = tf_temp->next;
	initXML(tf_temp);
}



void model(XMLElement* element2) {
	XMLElement* tftemp = element2;
	while(tftemp != NULL) {
		tf_temp->modo = 7;
		tf_temp->model = (char*) tftemp->Attribute("file");
		tf_temp->next = (Transformacao) malloc(sizeof(struct transf));
		tf_temp = tf_temp->next;
		initXML(tf_temp);
		tftemp = tftemp->NextSiblingElement("model");
	}
}



void popMatrix() {
	tf_temp->modo = 2;
	tf_temp->next = (Transformacao) malloc(sizeof(struct transf));
	tf_prev = tf_temp;
	tf_temp = tf_temp->next;
	initXML(tf_temp);
}



void pushMatrix() {
	tf_temp->modo = 1;
	tf_temp->next = (Transformacao) malloc(sizeof(struct transf));
	tf_temp = tf_temp->next;
	initXML(tf_temp);
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



int processXML(char* file) {
	XMLDocument xmlDoc;
	XMLNode *root;
	XMLElement *pListElement;
	if(xmlDoc.LoadFile(file) != XML_SUCCESS) {
		printf("Error loading %s.\n", file);
		return -1;
	}

	int errorCode = open3dModel("objects/sphere.3d");
	if(errorCode < 0) {
		printf("Error opening %s!\n", "sphere.3d");
		return -1;
	}
	printf("Opened %s successfully.\n", "sphere.3d");

	tf = (Transformacao) malloc(sizeof(struct transf));
	tf_temp = tf;
	initXML(tf_temp);

	root = xmlDoc.FirstChildElement("scene");
	pListElement = root -> FirstChildElement("group");
		parserXML(pListElement);
	tf_prev -> next = NULL;
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
	gluLookAt(0.0f, 500.0f, 500.0f,
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
	int color;
	Transformacao tftemp = tf;
	glLineWidth(1.0f);
	while( tftemp != NULL){
		switch( tftemp-> modo){
			case 1:
				glPushMatrix();
				break;
			case 2:
				glPopMatrix();
				break;
			case 3:
				r = tftemp -> colorR;
				g = tftemp -> colorG;
				b = tftemp -> colorB;
				break;
			case 4:
				glTranslatef(tftemp -> x, tftemp -> y, tftemp -> z);
				break;
			case 5:
				glScalef(tftemp -> x, tftemp -> y, tftemp -> z);
				break;
			case 6:
				glRotatef(tftemp -> angle,tftemp -> x, tftemp -> y, tftemp -> z);
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
		tftemp = tftemp -> next;
	}
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
