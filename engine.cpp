#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif


#include <stdio.h>
#include <string.h>
#include <time.h>

typedef struct triang {
	float p1_x, p1_y, p1_z;
	float p2_x, p2_y, p2_z;
	float p3_x, p3_y, p3_z;
	struct triang *next;
} *Triangulo;


float vert_rot, hori_rot, vert_trans, hori_trans;
float val_rot = 2.0f, val_trans = 0.2f;
char view_mode;
int x_pos, y_pos;
bool click = false;
Triangulo t;



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



void renderScene(void) {
	// clear buffers
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


	// set the camera
	glLoadIdentity();
	gluLookAt(0.0f, 15.0f, 15.0f,
		      0.0f, 0.0f, 0.0f,
			  0.0f, 1.0f, 0.0f);


// put the geometric transformations here
	glTranslatef(hori_trans, 0, 0);
	glTranslatef(0, 0, vert_trans);
	glRotatef(vert_rot, 1, 0, 0);
	glRotatef(hori_rot, 0, 1, 0);


// put drawing instructions here
	//Eliminar as faces cujo lado da frente nao est� para a camara
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	//Metodo de render das faces (Clock-Wise ou Counter Clock-Wise)
	glFrontFace(GL_CCW);

	glPolygonMode(GL_FRONT, view_mode == 'f' ? GL_FILL :
						 view_mode == 'l' ? GL_LINE : GL_POINT);


	Triangulo temp = t;
	glBegin(GL_TRIANGLES);
		while(temp != NULL) {
			glColor3ub(rand() % 256, rand() % 256, rand() % 256);
			glVertex3f(temp->p1_x, temp->p1_y, temp->p1_z);
			glVertex3f(temp->p2_x, temp->p2_y, temp->p2_z);
			glVertex3f(temp->p3_x, temp->p3_y, temp->p3_z);
			temp = temp->next;
		}
	glEnd();

	// End of frame
	glutSwapBuffers();
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



// write function to process keyboard events
void special_key_handler(int key, int x, int y) {
	//vert_rot -> ++Baixo   | --Cima
	//hori_rot -> ++Direita | --Esquerda
	key == GLUT_KEY_UP ? vert_rot -= val_rot :
		key == GLUT_KEY_DOWN ? vert_rot += val_rot :
			key == GLUT_KEY_RIGHT ? hori_rot += val_rot :
				key == GLUT_KEY_LEFT ? hori_rot -= val_rot : ' ';

	//Re-render
	glutPostRedisplay();
}



void normal_key_handler(unsigned char c, int x, int y) {
	switch (c) {
		case 'w':
		case 'W':
			vert_trans -= val_trans;
			break;
		case 's':
		case 'S':
			vert_trans += val_trans;
			break;
		case 'd':
		case 'D':
			hori_trans += val_trans;
			break;
		case 'a':
		case 'A':
			hori_trans -= val_trans;
			break;
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



int main(int argc, char **argv) {
	vert_rot = hori_rot = vert_trans = hori_trans = 0.0f;
	view_mode = 'l';
	x_pos = y_pos = 400;
	srand(time(NULL));

	if(argc < 2) {
		printf("Invalid Input!\n");
		return -1;
	}

	t = (Triangulo) malloc(sizeof(struct triang));
	FILE *fp;
	int i;
	char *line;
	char equal[3] = "=\"";
	line = (char*) malloc(sizeof(char) * 1024);
	Triangulo t_temp = t;
	Triangulo t_prev;


	fp = fopen(argv[1], "r+");
	fscanf(fp, "%s\n", line);
	if(strcmp(line, "<scene>") != 0) {
		printf("Invalid XML Format!\n");
		return -1;
	}


	while(fscanf(fp, "%s", line) != EOF) {
		if(strcmp(line, "</scene>") == 0 ||
			 strcmp(line, "<model") == 0 ||
			 strcmp(line, "/>") == 0) continue;
		else {
			char *tok;
			tok = strtok(line, equal);
			tok = strtok(NULL, equal);
			//printf("%s\n", tok);

			FILE *f_3d;
			//printf("Abriu %s\n", tok);
			f_3d = fopen(tok, "r+");

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
					//printf("%f %f %f\n", t_temp->p1_x, t_temp->p1_y, t_temp->p1_z);
				}
				else {
					if(op == 2) {
						t_temp->p2_x = atof(v1);
						t_temp->p2_y = atof(v2);
						t_temp->p2_z = atof(v3);
						op = 3;
						//printf("%f %f %f\n", t_temp->p2_x, t_temp->p2_y, t_temp->p2_z);
					}
					else {
						t_temp->p3_x = atof(v1);
						t_temp->p3_y = atof(v2);
						t_temp->p3_z = atof(v3);
						op = 1;
						t_temp->next = (Triangulo) malloc(sizeof(struct triang));
						t_prev = t_temp;
						t_temp = t_temp->next;
						//printf("%f %f %f\n", t_temp->p3_x, t_temp->p3_y, t_temp->p3_z);
					}
				}
			}
			fclose(f_3d);
		}
	}
	t_prev->next = NULL;
	fclose(fp);


	// init GLUT and the window
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH|GLUT_DOUBLE|GLUT_RGBA);
	glutInitWindowPosition(100,100);
	glutInitWindowSize(800,800);
	glutCreateWindow("Engine");

	// Required callback registry
	glutDisplayFunc(renderScene);
	glutReshapeFunc(changeSize);

	// put here the registration of the keyboard callbacks
	glutSpecialFunc(special_key_handler);
	glutKeyboardFunc(normal_key_handler);
	glutMouseFunc(mouse_handler);
	glutMotionFunc(movement_handler);

	//  OpenGL settings
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

	// enter GLUT's main cycle
	glutMainLoop();

	//free(t) ????????
	return 1;
}
