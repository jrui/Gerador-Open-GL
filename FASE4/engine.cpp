#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glew.h>
#include <GL/glut.h>
#endif
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <vector>
#include <string>
#include <map>
#include <iostream>
#include "transformacao.h"
#include "tinyxml2.h"
using namespace tinyxml2;
#include <IL/il.h>
#include <cstring>

struct cmp_str
{
    bool operator()(char const *a, char const *b)
    {
        return std::strcmp(a, b) < 0;
    }
};

/**
*		Global variables used to store program data, rotations, translations, view
*	mode, mouse coords, colors, some variables used throughout the code, and
* finnaly the triangles used to render every object. It also specifies where the
* camera will be initially, by the variables z_trans and y_trans, as well as the angle
* of the camera by the varuable vert_rot and hori_rot.
*		We will get into more details when we get to the functions that alter this
* values.
*/
float alfa = 0.0f, beta = 0.5f, racio_trans = 20.0f;
float camX, camY, camZ;
float rotation = 0.0f;
float vert_rot = 10.0f, hori_rot = 0.0f;
float x_trans = 0.0f, y_trans = 500.0f, z_trans = -1500.0f;
char view_mode = 'l';
int x_pos = 400, y_pos = 400;
bool click = false;
float x,y,z;
int N = 1, timebase = 0, frame = 0, lights = 0;
GLuint buffers[2];
GLuint vertexCount, vertices, normals, texCoord, indices, indCount;


/**
*		This time we decided to use a vector to store every information
*	about the XML transformations, whether is angle and coordinates,
* rotations, colors and names of the models.
*		This structure also has a pointer to the next transformation that it
* has to render.
*		We also decided to store every model in the structur files, saving the name
* associated to the object, loading the same object only one time and reusing it 
* when necessary. Same goes for the textures.
*/
std::vector<Transformacao*> transformacoes;
std::map< const char*, std::vector< std::vector<float> > , cmp_str> files;
std::map< const char*, std::vector< std::vector<float> > , cmp_str>::iterator it;
std::map< const char*, GLuint, cmp_str> textures;
std::map< const char*, GLuint, cmp_str>::iterator itt;




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
void light(XMLElement* element2);
void material(XMLElement* element2);
void popMatrix();
void pushMatrix();
int parserXML(XMLElement* pListElement);
int processXML(char* file);
void renderScene(void);
void renderFigures(void);
void changeSize(int w, int h);
void special_key_handler(int key, int x, int y);
void normal_key_handler(unsigned char c, int x, int y);
void movement_handler(int x, int y);
void mouse_handler(int button, int state, int x, int y);
void spherical2Cartesian();
void showFPS();
std::vector<float> processPointTranslate(XMLElement *element2);
int loadTexture(std::string imagem);
//void init(char *imagem);





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
	float white[4] = { 1.0f, 1.0f, 1.0f, 1.0f };

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

	#ifndef __APPLE___
		glewInit();
	#endif

	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glEnable(GL_RESCALE_NORMAL);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glEnable(GL_LIGHTING);
	glEnable(GL_TEXTURE_2D);

	spherical2Cartesian();


	
	int exitCode = processXML(argv[1]);
	if(exitCode < 0) return -1;
	
	//init("terra.jpg");

	glutMainLoop();
	printf("Saí\n");

	return 1;
}



/**
* 	This function is used everytime we want to open a 3d model file and save
* it to a global variable (t).
*		It will read three triplet of floats for each vertice of the document that it reads
* , being the first three floats the positions of the vertices, the second three floats are the 
* normal vectors of that vertice and the last three floats are the texture coordenates of the vertice.
*	We defined that as the composition of our .3d model files, so this function
* reads them procedurally and saves it to a vector.
*		We can assume that the .3d file as an ammount of lines %3 equal to 0.
*
*	@param tok - Character representing the name of the file to open.
* @return vector - Vector containing the coordinates os every object.
*/
std::vector< std::vector<float> > open3dModel(const char* tok){
 	FILE *f_3d;
 	std::vector<float> v, n, t;
 	std::vector< std::vector<float> > vc;
 	f_3d = fopen(tok, "r+");
 	if (f_3d < 0) return vc;

	char *s1, *s2, *s3, *s4, *s5, *s6, *s7, *s8;
 	s1 = (char*) malloc(sizeof(char) * 64);
 	s2 = (char*) malloc(sizeof(char) * 64);
 	s3 = (char*) malloc(sizeof(char) * 64);
 	s4 = (char*) malloc(sizeof(char) * 64);
 	s5 = (char*) malloc(sizeof(char) * 64);
 	s6 = (char*) malloc(sizeof(char) * 64);
 	s7 = (char*) malloc(sizeof(char) * 64);
 	s8 = (char*) malloc(sizeof(char) * 64);
 
 	while(!feof(f_3d)){ 
 		fscanf(f_3d, "%s %s %s", s1, s2, s3);
		v.push_back(atof(s1));
		v.push_back(atof(s2));
		v.push_back(atof(s3));
	
		fscanf(f_3d, "%s %s %s", s4, s5, s6);
		n.push_back(atof(s4));
		n.push_back(atof(s5));
		n.push_back(atof(s6));	

		fscanf(f_3d, "%s %s", s7, s8);
		t.push_back(atof(s7));
		t.push_back(atof(s8));	
	}
 	fclose(f_3d);
 	vc.push_back(v);
 	vc.push_back(n);
 	vc.push_back(t);
 	free(s1); free(s2); free(s3); free(s4); free(s5); free(s6); free(s7); free(s8);
 	return vc;
}


/**
* 	This function is used everytime we want to process a tag "Material".
* It collects every aspect that might be used to define the material of the
* objects, creating a class "Transformacao" with those aspects, storing
* them to be used in the render scene.
*
*	@param element2 - Pointer to the tag "Material"
*/
void material(XMLElement* element2) {
	float dr=0, dg=0, db=0, ar=0, ag=0, ab=0, sr=0, sg=0, sb=0, er=0, eg=0, eb=0;
	GLfloat shin = 0;
	if(!(dr = element2->FloatAttribute("diffR"))) dr = 0.8;
	if(!(dg = element2->FloatAttribute("diffG"))) dg = 0.8;
	if(!(db = element2->FloatAttribute("diffB"))) db = 0.8;
	if(!(ar = element2->FloatAttribute("ambR"))) ar = 0.2;
	if(!(ag = element2->FloatAttribute("ambG"))) ar = 0.2;
	if(!(ab = element2->FloatAttribute("ambB"))) ar = 0.2;
	if(!(sr = element2->FloatAttribute("specR"))) sr = 0;
	if(!(sg = element2->FloatAttribute("specG"))) sg = 0;
	if(!(sb = element2->FloatAttribute("specB"))) sb = 0;
	if(!(er = element2->FloatAttribute("emiR"))) er = 0;
	if(!(eg = element2->FloatAttribute("emiG"))) eg = 0;
	if(!(eb = element2->FloatAttribute("emiB"))) eb = 0;
	if(!(shin = (float)element2->FloatAttribute("shininess"))) shin = 0;
	Transformacao* tf = new Material(dr,dg,db,ar,ag,ab,sr,sg,sb,er,eg,eb, shin);
	transformacoes.push_back(tf);
}



/**
* 	This function is used everytime we want to process a tag "Color".
* It collects every aspect that might be used to define the colors of the
* objects, creating a class "Transformacao" with those aspects, storing
* them to be used in the render scene.
*
*	@param element2 - Pointer to the tag "Color"
*/
void color(XMLElement* element2) {
	int r=0, g=0, b=0;
	if(!(r = element2->IntAttribute("R"))) r = r%256;
	if(!(g = element2->IntAttribute("G"))) g = g%256;
	if(!(b = element2->IntAttribute("B"))) b = b%256;
	Transformacao* tf = new Color(r,g,b);
	transformacoes.push_back(tf);
}




/**
* É utilizada  para guardar os pontos da tag "Translate". Estes pontos serão
* usados para definir a trajetória do objeto, utilizando Catmull.
*/
std::vector<float> processPointTranslate(XMLElement *element2) {
	std::vector<float> v;
	element2 = element2->FirstChildElement("point");
	v.push_back(element2->FloatAttribute("X"));
	v.push_back(element2->FloatAttribute("Y"));
	v.push_back(element2->FloatAttribute("Z"));
	element2 = element2->NextSiblingElement("point");
	while(element2) {
		v.push_back(element2->FloatAttribute("X"));
		v.push_back(element2->FloatAttribute("Y"));
		v.push_back(element2->FloatAttribute("Z"));
		element2 = element2->NextSiblingElement("point");
	}
	return v;
}




/**
* 	This function is used everytime we want to process a tag "Translate".
* It collects every aspect that might be used to define movement of the objects
* depending if the object will move only one time, or will have a continuous movement.
* If the object has a continuous movement, it is specified the time it takes to make
* a fill loop of the trajectory specified by the points of the XML file,
* creating a class "Transformacao" with those aspects, storing
* them to be used in the render scene.
*
*	@param element2 - Pointer to the tag "Translate"
*/
void translate(XMLElement* element2) {
	float time;
	Transformacao* tf;
	if((time = element2->FloatAttribute("time"))) {
		std::vector<float> pontos;
		pontos = processPointTranslate(element2);
		tf = new Translate(time, pontos);
		pontos = std::vector<float>();
	}
	else {
		if(!(x = element2->FloatAttribute("X"))) x = 0;
		if(!(y = element2->FloatAttribute("Y"))) y = 0;
		if(!(z = element2->FloatAttribute("Z"))) z = 0;
		tf = new Translate(x,y,z);
	}
 	transformacoes.push_back(tf);
}




/**
* 	This function is used everytime we want to process a tag "Rotate".
* It collects every aspect that might be used to define the rotation of the
* objects, specifying the time it takes to make a full rotation over himself
* , and the angle of the rotation, creating a class "Transformacao" with 
* those aspects, storing them to be used in the render scene.
*
*	@param element2 - Pointer to the tag "Rotate"
*/
void rotate(XMLElement* element2) {
	float ang, time;
	Transformacao* tf;
	if(!(x = element2->FloatAttribute("X"))) x=0;
	if(!(y = element2->FloatAttribute("Y"))) y=0;
	if(!(z = element2->FloatAttribute("Z"))) z=0;
	if(!(ang = element2->FloatAttribute("angle"))) ang = 0;
	if(!(time = element2->FloatAttribute("time"))) time = 0;
	if(ang != 0) tf = new Rotate(ang, x, y, z);
	else tf = new Rotate(time, x, y, z, true);
	transformacoes.push_back(tf);
}




/**
* 	This function is used everytime we want to process a tag "Scale".
* It collects every aspect that might be used to define the dimensions of the
* scale that will be aplied to theobjects, creating a class "Transformacao" 
* with those aspects, storing them to be used in the render scene.
*
*	@param element2 - Pointer to the tag "Scale"
*/
void scale(XMLElement* element2) {
	if(!(x = element2->FloatAttribute("X"))) x=1;
	if(!(y = element2->FloatAttribute("Y"))) y=1;
	if(!(z = element2->FloatAttribute("Z"))) z=1;
	Transformacao* tf = new Scale(x,y,z);
	transformacoes.push_back(tf);
}




/**
* 	This function is used everytime we want to process a tag "Model".
* It collects every aspect that might be used to define the Model to be
* drawn. It can also specify the number of objects of the file model to
* be drawn, also specifying the variations of the dimensions of the objects
* and the área which will be drawn, indicating the innerRadius and OuterRadius,
* , creating a class "Transformacao" with those aspects, storing them to 
* be used in the render scene.
*
*	@param element2 - Pointer to the tag "Model"
*/
void model(XMLElement* element2) {
	XMLElement* tftemp = element2;
	const char* nome, *texture;
	int numero=1;
	float rMin=0, rMax=0, xMax=1, xMin=1, yMax=1, yMin=1,zMax=1, zMin=1;
	std::vector< std::vector<float> > vec;
	GLuint textID = -1;
	while(tftemp != NULL) {
		nome = strdup((const char*) tftemp->Attribute("file"));
		it = files.find(nome);
		if(it != files.end()) vec = files.at(nome);
		else{
			printf("Novo\n");
		 	vec = open3dModel(nome);
			if(vec.size() == 0) {
				printf("Error opening %s!\n", nome);
				return;
			}
			files[nome] = vec;
		}
		printf("Opened %s successfully.\n", nome);
		if(tftemp->Attribute("texture")){
			texture = strdup((const char*) tftemp->Attribute("texture"));
			itt = textures.find(texture);
			if(itt != textures.end()) textID = textures.at(texture);
			else{ 
				printf("Nova textura\n");
				textID = loadTexture(texture);
				textures[texture] = textID;
			}
		}
		numero = element2->IntAttribute("num");
		rMin = element2->FloatAttribute("rMin");
		if(!(rMax = element2->FloatAttribute("rMax"))) rMax = 1000;
		if(!(xMin = element2->FloatAttribute("xMin"))) xMin = 1;
		if(!(xMax = element2->FloatAttribute("xMax"))) xMax = 1;
		if(!(yMin = element2->FloatAttribute("yMin"))) yMin = 1;
		if(!(yMax = element2->FloatAttribute("yMax"))) yMax = 1;
		if(!(zMin = element2->FloatAttribute("zMin"))) zMin = 1;
		if(!(zMax = element2->FloatAttribute("zMax"))) zMax = 1;
		Transformacao* tf = new Model(vec, textID, numero, rMin, rMax, xMin, xMax, yMin, yMax, zMin, zMax);
		transformacoes.push_back(tf);
		tftemp = tftemp->NextSiblingElement("model");
		vec.at(0) = std::vector<float>();
		vec.at(1) = std::vector<float>();
		vec.at(2) = std::vector<float>();
		vec = std::vector< std::vector<float> > ();
	}
}




/**
* 	This function is used everytime we want to process a tag "/Group".
* It only creates a class "Transformacao" that will be executed in the render
* executing a glPopMatrix();
*/
void popMatrix() {
	Transformacao* tf = new PopMatrix();
	transformacoes.push_back(tf);
}




/**
* 	This function is used everytime we want to process a tag "Group".
* It only creates a class "Transformacao" that will be executed in the render
* executing a glPushMatrix();
*/
void pushMatrix() {
	Transformacao* tf = new PushMatrix();
	transformacoes.push_back(tf);
}



/**
* 	This function is used everytime we want to process a tag "Light".
* It collects every aspect that might be used to define the Light to be
* positioned in the scene. It can also specify type of Light, defining every aspect
* of the light, depending on the type. The type can be "Point, SPOTLIGHT, DIRECTIONAL".
* It will be stored in a classe "Transformacao" that will be executed in the render.
*
*	@param element2 - Pointer to the tag "Light"
*/
void light(XMLElement* element2){
	char* type;
	GLfloat pos[3], diff[3], amb[3], spec[3], dir[3];
	float exp, cut;
	int shin;
	if(!(diff[0] = element2->FloatAttribute("diffR"))) diff[0] = 0;
	if(!(diff[1] = element2->FloatAttribute("diffG"))) diff[1] = 0;
	if(!(diff[2] = element2->FloatAttribute("diffB"))) diff[2] = 0;
	if(!(amb[0] = element2->FloatAttribute("ambR"))) amb[0] = 0;
	if(!(amb[1] = element2->FloatAttribute("ambG"))) amb[1] = 0;
	if(!(amb[2] = element2->FloatAttribute("ambB"))) amb[2] = 0;
	if(!(spec[0] = element2->FloatAttribute("specR"))) spec[0] = 0;
	if(!(spec[1] = element2->FloatAttribute("specG"))) spec[1] = 0;
	if(!(spec[2] = element2->FloatAttribute("specB"))) spec[2] = 0;
	if(!(pos[0] = element2->FloatAttribute("posX"))) pos[0]=0;
 	if(!(pos[1] = element2->FloatAttribute("posY"))) pos[1]=0;
 	if(!(pos[2] = element2->FloatAttribute("posZ"))) pos[2]=0;
 	if(!(dir[0] = element2->FloatAttribute("dirX"))) dir[0] = 0;
	if(!(dir[1] = element2->FloatAttribute("dirY"))) dir[1] = 0;
	if(!(dir[2] = element2->FloatAttribute("dirZ"))) dir[2] = 0;
	if(!(exp = element2->FloatAttribute("exp"))) exp = 0;
	if(!(cut = element2->FloatAttribute("cut"))) cut = 180;
 	type = strdup((char*) element2->Attribute("type"));
 	Transformacao* tf = new Light(pos,diff,amb,spec, dir,exp, cut,lights,type);
 	lights++;
 	transformacoes.push_back(tf);
}


/**
* 		It will be called to treat all the tags "Light" inside of the
* tag "Lights". It will iterate over every tag "Light", calling the
* function "light" for every tag.
*
* @param element2 - Pointer to the tag "Lights"
*/
int parserXMLLight(XMLElement* pListElement) {
 XMLElement* element2;
 XMLElement* tempEl;

 if(pListElement != NULL) {
	element2 = pListElement->FirstChildElement("light");
	while(element2!=NULL){	
		pushMatrix();
	 	light(element2);
	 	popMatrix();
		element2 = element2 -> NextSiblingElement("light");
	}
 }
 return 1;
}


/**
* 		It will be called to treat all the tags inside the tag "Group"
* It will iterate over every tag, calling the function that will collect
* the information necessary to treat that tag. It will also iterate over
* tags "Group" that are inside the tag "Group".
*
* @param element2 - Pointer to the tag "Group"
*/
int parserXMLGroup(XMLElement* pListElement) {
	XMLElement* element2;

 	pushMatrix();
 	for(element2 = pListElement -> FirstChildElement(); element2 != NULL ; element2 = element2 -> NextSiblingElement()){
		if(!strcmp(element2 -> Value(), "color")) color(element2);
		else{ if(!strcmp(element2 -> Value(), "material")) material(element2);
		else{ if(!strcmp(element2 -> Value(), "translate")) translate(element2);
		else{ if(!strcmp(element2 -> Value(), "rotate")) rotate(element2);
		else{ if(!strcmp(element2 -> Value(), "scale")) scale(element2);
		else{ if(!strcmp(element2 -> Value(), "model")) model(element2);
		else{ if(!strcmp(element2 -> Value(), "group"))	parserXMLGroup(element2);
		}}}}}}
	}
	popMatrix();
 	return 1;
}




/**
* 	This function implements the tinyxml2 API, and it's used at program
*	startup when we are begining to process the XML file. This function is
* responsible for invoking the open3dModel function, since this will extract
*	the filenames where to find the 3d files.
*		It checks for the rootElement, in our case the scene, then it iterates
*	for child elements, running the parserXML corresponding to that element
* which will parse the XML file indicated by "file", checking for error codes 
* while doing so.
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
	for(pListElement = root -> FirstChildElement(); pListElement != NULL ; pListElement = pListElement -> NextSiblingElement()){
		if(!strcmp(pListElement -> Value(), "lights")) parserXMLLight(pListElement);

		if(!strcmp(pListElement -> Value(), "group")) parserXMLGroup(pListElement);
	}

	xmlDoc.Clear();
	return XML_SUCCESS;
}




/**
* 	This function is only invoked either by the glutMainLoop or by a user
*	key press / mouse interaction. What this function does is render the scene
*	specified in the program input XML file.
*		This function sets up the camera position, view point. The camera gets set
* in the position specified by the x_trans (x position in the space), y_trans
* and z_trans. These positions can be modified by the user. The point where the camera
* is pointing is defined not only by the position of the câmera but the angles which
* can also be modified by the user.
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
 gluLookAt(-camX + x_trans, camY + y_trans, -camZ + z_trans,
		   x_trans, y_trans, z_trans,
		   0.0f, 1.0f, 0.0f);

 glCullFace(GL_BACK);
 glFrontFace(GL_CCW);
 glPolygonMode(GL_FRONT, view_mode == 'f' ? GL_FILL :
													 view_mode == 'l' ? GL_LINE :
														 GL_POINT);
 renderFigures();
 showFPS();
 glutSwapBuffers();
}




/**
* 	This function is only invoked by render scene, and it's sole purpose
*	is to render every triangle that has been previously saved to memory.
*	It's important to denote that this render is made using tri-vertex
* aggregation to triangles.
*		It acesses the global vectors that we defined to store every triangle
* and transformation, making sure to draw the correct coordinates.
*		It doesn't draw directly on screen but calls a function that does so.
*
* @return void
*/
void renderFigures() {
 int color;
 std::vector<float> vc;
 Transformacao* tftemp;
 for(int i = 0; i < transformacoes.size(); i++) {
	 tftemp = transformacoes.at(i);
	 tftemp->transformar();
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
 gluPerspective(45.0f ,ratio, 1.0f ,50000.0f);
 // return to the model view matrix mode
 glMatrixMode(GL_MODELVIEW);
}




/**
* 	This function is invoked only when we need to convert the camera
* angles to a cartesian format. It does so by setting new values
* to the global variables representing the camera position, converting from
* degrees to radians.
*
* @return void
*/
void spherical2Cartesian() {
	camX = 20 * sin(hori_rot*3.14/180);
	camZ = 20 * cos(hori_rot*3.14/180);
	camY = 20 * sin(vert_rot*3.14/180);
}


/**
* 	This function is invoked only when mouse moves on screen. It receives
*	two valus representing the current mouse position.
*		The movement handler determines the angle of the câmera, determining
* the direction the câmera will move.
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
	 if(vert_rot>90)
	 	vert_rot = 89;
	 if(vert_rot<-90)
	 	vert_rot = -89;
	 spherical2Cartesian();
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
* 	This function handles a special key press, changing the right variables
*	to obtain the desired results.
*		We use a global variable (racio_trans) to assure thar the user can determine
* the speed in which he desires to move the camera, doubling the speed or decresing it
* by half.
*
*	@param key - Character representing the special key pressed
* @param x - Integer representing the mouse x position
* @param y - Integer representing the mouse y position
* @return void
*/
void special_key_handler(int key, int x, int y) {
	switch(key) {
		case GLUT_KEY_PAGE_DOWN:
			racio_trans = (float)racio_trans/2;
			break;
		case GLUT_KEY_PAGE_UP:
			racio_trans = (float)racio_trans*2;;
			break;
	}
	glutPostRedisplay();
}




/**
* 	This function handles a normal key press, changing the right variables
*	to obtain the desired results.
*		We use a global variable (view_mode) to indicate the key that the user
*	pressed, and then we can render the desired scenes accordingly. This
* variable stores an 'f' if the user wants to see the object filled, an 'l'
* if we want to see only lines and finnaly a 'p' if we only want to render
* points. We also enabled the user to determine the speed in which he
* desires to move the camera with the keys + and -, incresing and decreasing
* it respectivly. The user cane move the câmera forward by pressing 'w' and
* backwards by pressing 's', move the câmera sideways by pressing 'a' or 'd'
* , depending in wich direction he wants to move.
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
	case '+':
		racio_trans += 20.0f;
		break;
	case '-':
		racio_trans -= 20.0f;
		if(racio_trans < 0)
				racio_trans = 0.0f;
		break;
	case 'w':
	case 'W':
		x_trans += racio_trans * sin(hori_rot*3.14/180)*cos(vert_rot*3.14/180);
		z_trans += racio_trans * cos(hori_rot*3.14/180)*cos(vert_rot*3.14/180);
		y_trans -= racio_trans * sin(vert_rot*3.14/180);
		break;
	case 's':
	case 'S':
		x_trans -= racio_trans * sin(hori_rot*3.14/180)*cos(vert_rot*3.14/180);
		z_trans -= racio_trans * cos(hori_rot*3.14/180)*cos(vert_rot*3.14/180);
		y_trans += racio_trans * sin(vert_rot*3.14/180);
		break;
	case 'd':
	case 'D':
		x_trans -= racio_trans * cos(hori_rot*3.14/180);
		z_trans += racio_trans * sin(hori_rot*3.14/180);
		break;
	case 'a':
	case 'A':
		x_trans += racio_trans * cos(hori_rot*3.14/180);
		z_trans -= racio_trans * sin(hori_rot*3.14/180);
		break;
	}
 	//Re-render
	spherical2Cartesian();
 	glutPostRedisplay();
}


/**
* This func enable the user to show the frames per second the image is
* being rendered, showing it in the title of the window.
*/
void showFPS() {
  float fps;
	int time;
	char s[64];
  frame++;
	time = glutGet(GLUT_ELAPSED_TIME);
	if (time - timebase > 1000) {
		fps = frame*1000.0/(time-timebase);
		timebase = time;
		frame = 0;
		sprintf(s, "Rendering scene at %.2f FPS", fps);
		glutSetWindowTitle(s);
	}
}

/**
* This function creats a texture from a file 

* @param s - string that represents de name of file with an image

* @return int - returns the id of the texture
*/
int loadTexture(std::string s) {

			unsigned int t,tw,th;
			unsigned char *texData;
			unsigned int texID;

			ilInit();
			ilEnable(IL_ORIGIN_SET);
			ilOriginFunc(IL_ORIGIN_LOWER_LEFT);
			ilGenImages(1,&t);
			ilBindImage(t);
			ilLoadImage((ILstring)s.c_str());
			tw = ilGetInteger(IL_IMAGE_WIDTH);
			th = ilGetInteger(IL_IMAGE_HEIGHT);
			ilConvertImage(IL_RGBA, IL_UNSIGNED_BYTE);
			texData = ilGetData();

			glGenTextures(1,&texID);
			
			glBindTexture(GL_TEXTURE_2D,texID);
			glTexParameteri(GL_TEXTURE_2D,	GL_TEXTURE_WRAP_S,		GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D,	GL_TEXTURE_WRAP_T,		GL_REPEAT);

			glTexParameteri(GL_TEXTURE_2D,	GL_TEXTURE_MAG_FILTER,   	GL_LINEAR_MIPMAP_LINEAR);
			glTexParameteri(GL_TEXTURE_2D,	GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
				
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, tw, th, 0, GL_RGBA, GL_UNSIGNED_BYTE, texData);
			glGenerateMipmap(GL_TEXTURE_2D);

			glBindTexture(GL_TEXTURE_2D, 0);

			return texID;

		}