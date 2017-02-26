#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include <math.h>
#include <stdio.h>



float vert_rot, hori_rot, vert_trans, hori_trans;
float val_rot = 2.0f, val_trans = 0.2f;
char view_mode;
int x_pos, y_pos;
bool click = false;



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
	gluLookAt(0.0f, 10.0f, 10.0f,
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


	glBegin(GL_TRIANGLES);
		//Face base1
		glColor3f(0.5f, 1.0f, 0.5f);
		glVertex3f(-0.5f, 0.0f, 0.5f);
		glVertex3f(-0.5f, 0.0f, -0.5f);
		glVertex3f(0.5f, 0.0f, 0.5f);

		//Face base2
		glColor3f(0.5f, 1.0f, 0.5f);
		glVertex3f(-0.5f, 0.0f, -0.5f);
		glVertex3f(0.5f, 0.0f, -0.5f);
		glVertex3f(0.5f, 0.0f, 0.5f);

		//Face lateral frente
		glColor3f(0.0f, 0.0f, 1.0f);
		glVertex3f(0.5f, 0.0f, 0.5f);
		glVertex3f(0.0f, 2.0f, 0.0f);
		glVertex3f(-0.5f, 0.0f, 0.5f);


		//Face lateral esquerda
		glColor3f(1.0f, 1.0f, 0.0f);
		glVertex3f(0.0f, 2.0f, 0.0f);
		glVertex3f(-0.5f, 0.0f, -0.5f);
		glVertex3f(-0.5f, 0.0f, 0.5f);


		//Face lateral direita
		glColor3f(1.0f, 1.0f, 0.0f);
		glVertex3f(0.0f, 2.0f, 0.0f);
		glVertex3f(0.5f, 0.0f, 0.5f);
		glVertex3f(0.5f, 0.0f, -0.5f);


		//Face lateral tras
		glColor3f(0.0f, 0.0f, 1.0f);
		glVertex3f(0.0f, 2.0f, 0.0f);
		glVertex3f(0.5f, 0.0f, -0.5f);
		glVertex3f(-0.5f, 0.0f, -0.5f);


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

// init GLUT and the window
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH|GLUT_DOUBLE|GLUT_RGBA);
	glutInitWindowPosition(100,100);
	glutInitWindowSize(800,800);
	glutCreateWindow("Engine");


// Required callback registry
	glutDisplayFunc(renderScene);
	glutReshapeFunc(changeSize);
	glutIdleFunc(renderScene);


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


	return 1;
}
