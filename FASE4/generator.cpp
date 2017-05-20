#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>



void plane(float x, float z, char *ficheiro);
void box(float x, float y, float z, int slices, char *ficheiro);
void cone (float r, float h, int sl, int st, char *ficheiro);
void sphere (float r,int sl,int st , char *ficheiro);
void cylinder(float radius, float height, int slices, int stacks, char *ficheiro);
void ring(float innerRadius, float outerRadius, int sl, char *ficheiro);
void bezier(int tesselation, char* file, char* newFile);
void makeFigure_bezier(int tess, char *newFile, int patches[][16], int n, float points[][3], int np);
float getB_Point(float u, float v, float m[4][4] , float p[4][4]);




int main(int argc, char **argv) {
	int err = 0;
	if(argc < 2){
		printf("Input inválido.\n");
		exit(-1);
	}

	if(strcmp(argv[1], "plane") == 0)
			plane(atof(argv[2]), atof(argv[3]), argv[4]);
	else if(strcmp(argv[1], "box") == 0)
			box(atof(argv[2]), atof(argv[3]), atof(argv[4]), atoi(argv[5]), argv[6]);
	else if(strcmp(argv[1], "cone") == 0)
			cone(atof(argv[2]), atof(argv[3]), atoi(argv[4]), atoi(argv[5]), argv[6]);
	else if(strcmp(argv[1],"sphere") == 0)
			sphere(atof(argv[2]), atoi(argv[3]), atoi(argv[4]), argv[5]);
	else if(strcmp(argv[1], "cylinder") == 0)
		 	cylinder(atof(argv[2]), atof(argv[3]), atoi(argv[4]), atoi(argv[5]), argv[6]);
	else if(strcmp(argv[1], "ring") == 0)
		 	ring(atof(argv[2]), atof(argv[3]), atoi(argv[4]),argv[5]);
	else if(strcmp(argv[1], "bezier") == 0)
			bezier(atoi(argv[2]), argv[3], argv[4]);
	else printf("Not suported yet!");

	return 1;
}



void plane(float x, float z, char* ficheiro){
	int r;
	FILE *op;
	op = fopen(ficheiro, "w+");
	fprintf(op, "%f 0 %f\n", x/2, z/2);
	fprintf(op, "0 1 0\n");
	fprintf(op, "0 0\n");
	
	fprintf(op, "%f 0 %f\n", x/2, -z/2);
	fprintf(op, "0 1 0\n");
	fprintf(op, "1 0\n");
	
	fprintf(op, "%f 0 %f\n", -x/2, -z/2);
	fprintf(op, "0 1 0\n");
	fprintf(op, "1 1\n");
	
	fprintf(op, "%f 0 %f\n", -x/2, -z/2);
	fprintf(op, "0 1 0\n");
	fprintf(op, "1 1\n");
	
	fprintf(op, "%f 0 %f\n", -x/2, z/2);
	fprintf(op, "0 1 0\n");
	fprintf(op, "0 1\n");
	
	fprintf(op, "%f 0 %f\n", x/2, z/2);
	fprintf(op, "0 1 0\n");
	fprintf(op, "0 0\n");
	fclose(op);
}



void box(float x, float y, float z, int slices, char* ficheiro){
	int st,sl;
	float yaux0,yaux1,zaux0,zaux1,xaux0,xaux1;
	//altura das slices
	float yaux = y / slices;
	//comprimento das slices
	float zaux = z / slices;
	//largura das slices
	float xaux = x/slices;
	//coordenadas negativas do z
	float zn = z/2 - z;
	//coordenadas negativas do y
	float yn = y/2 - y;
	//coordenadas negativas do x
	float xn = x/2 - x;
	x=x/2;
	y=y/2;
	z=z/2;
	FILE *op = fopen(ficheiro, "w+");
	for(sl = 1; sl <= slices; sl++) {
		xaux0=xn+(sl-1)*xaux;
		xaux1=xn+sl*xaux;
		for(st=1;st<=slices;st++){
			yaux0=yn+(st-1)*yaux;
			yaux1=yn+st*yaux;
			zaux0=zn+(st-1)*zaux;
			zaux1=zn+st*zaux;
			//face de cima
			//triângulo "menos negativo" em x e z
			fprintf(op,"%f %f %f\n", xaux0, y, zaux0);
			fprintf(op, "0 1 0\n");
			fprintf(op,"%f %f\n",(float)sl/(float)slices,(float)st/(float)slices);
			
			fprintf(op,"%f %f %f\n", xaux0, y, zaux1);
			fprintf(op, "0 1 0\n");
			fprintf(op,"%f %f\n",(float)sl/(float)slices,(float)(st+1)/(float)slices);

			fprintf(op,"%f %f %f\n", xaux1, y, zaux1);
			fprintf(op, "0 1 0\n");
			fprintf(op,"%f %f\n",(float)(sl+1)/(float)slices,(float)(st+1)/(float)slices);
			//triângulo "mais negativo" em x e z
			fprintf(op,"%f %f %f\n", xaux0, y, zaux0);
			fprintf(op, "0 1 0\n");
			fprintf(op,"%f %f\n",(float)sl/(float)slices,(float)st/(float)slices);

			fprintf(op,"%f %f %f\n", xaux1, y, zaux1);
			fprintf(op, "0 1 0\n");
			fprintf(op,"%f %f\n",(float)(sl+1)/(float)slices,(float)(st+1)/(float)slices);

			fprintf(op,"%f %f %f\n", xaux1, y, zaux0);
			fprintf(op, "0 1 0\n");
			fprintf(op,"%f %f\n",(float)(sl+1)/(float)slices,(float)st/(float)slices);
			//face de baixo
			//triângulo "menos negativo" em x e z
			fprintf(op,"%f %f %f\n", xaux0, yn, zaux0);
			fprintf(op, "0 -1 0\n");
			fprintf(op,"%f %f\n",(float)sl/(float)slices,(float)st/(float)slices);

			fprintf(op,"%f %f %f\n", xaux1, yn, zaux1);
			fprintf(op, "0 -1 0\n");
			fprintf(op,"%f %f\n",(float)(sl+1)/(float)slices,(float)(st+1)/(float)slices);

			fprintf(op,"%f %f %f\n", xaux0, yn, zaux1);
			fprintf(op, "0 -1 0\n");
			fprintf(op,"%f %f\n",(float)sl/(float)slices,(float)(st+1)/(float)slices);

			//triângulo "mais negativo" em x e z
			fprintf(op,"%f %f %f\n", xaux0, yn, zaux0);
			fprintf(op, "0 -1 0\n");
			fprintf(op,"%f %f\n",(float)sl/(float)slices,(float)st/(float)slices);

			fprintf(op,"%f %f %f\n", xaux1, yn, zaux0);
			fprintf(op, "0 -1 0\n");
			fprintf(op,"%f %f\n",(float)(sl+1)/(float)slices,(float)st/(float)slices);

			fprintf(op,"%f %f %f\n", xaux1, yn, zaux1);
			fprintf(op, "0 -1 0\n");
			fprintf(op,"%f %f\n",(float)(sl+1)/(float)slices,(float)(st+1)/(float)slices);
			//face da frente
			//triangulo "menos negativo" em x e "mais negativo" em y
			fprintf(op,"%f %f %f\n", xaux0, yaux0, z);
			fprintf(op, "0 0 1\n");
			fprintf(op,"%f %f\n",(float)sl/(float)slices,(float)st/(float)slices);

			fprintf(op,"%f %f %f\n", xaux1, yaux0, z);
			fprintf(op, "0 0 1\n");
			fprintf(op,"%f %f\n",(float)(sl+1)/(float)slices,(float)st/(float)slices);

			fprintf(op,"%f %f %f\n", xaux1, yaux1, z);
			fprintf(op, "0 0 1\n");
			fprintf(op,"%f %f\n",(float)(sl+1)/(float)slices,(float)(st+1)/(float)slices);
			//triangulo "mais negativo" em x e "menos negativo" em y
			fprintf(op,"%f %f %f\n", xaux0, yaux0, z);
			fprintf(op, "0 0 1\n");
			fprintf(op,"%f %f\n",(float)sl/(float)slices,(float)st/(float)slices);
			
			fprintf(op,"%f %f %f\n", xaux1, yaux1, z);
			fprintf(op, "0 0 1\n");
			fprintf(op,"%f %f\n",(float)(sl+1)/(float)slices,(float)(st+1)/(float)slices);
			
			fprintf(op,"%f %f %f\n", xaux0, yaux1, z);
			fprintf(op, "0 0 1\n");
			fprintf(op,"%f %f\n",(float)sl/(float)slices,(float)(st+1)/(float)slices);
			//face de traz
			//triangulo "menos negativo" em x e "mais negativo" em y
			fprintf(op,"%f %f %f\n", xaux0, yaux0, zn);
			fprintf(op, "0 0 -1\n");
			fprintf(op,"%f %f\n",(float)sl/(float)slices,(float)st/(float)slices);

			fprintf(op,"%f %f %f\n", xaux1, yaux1, zn);
			fprintf(op, "0 0 -1\n");
			fprintf(op,"%f %f\n",(float)(sl+1)/(float)slices,(float)(st+1)/(float)slices);

			fprintf(op,"%f %f %f\n", xaux1, yaux0, zn);
			fprintf(op, "0 0 -1\n");
			fprintf(op,"%f %f\n",(float)(sl+1)/(float)slices,(float)st/(float)slices);

			//triangulo "mais negativo" em x e "menos negativo" em y
			fprintf(op,"%f %f %f\n", xaux0, yaux0, zn);
			fprintf(op, "0 0 -1\n");
			fprintf(op,"%f %f\n",(float)sl/(float)slices,(float)st/(float)slices);

			fprintf(op,"%f %f %f\n", xaux0, yaux1, zn);
			fprintf(op, "0 0 -1\n");
			fprintf(op,"%f %f\n",(float)sl/(float)slices,(float)(st+1)/(float)slices);

			fprintf(op,"%f %f %f\n", xaux1, yaux1, zn);
			fprintf(op, "0 0 -1\n");
			fprintf(op,"%f %f\n",(float)(sl+1)/(float)slices,(float)(st+1)/(float)slices);
		}
	}
	for(sl = 1; sl <= slices; sl++) {
		yaux0=yn+(sl-1)*yaux;
		yaux1=yn+sl*yaux;
		for(st=1;st<=slices;st++){
			zaux0=zn+(st-1)*zaux;
			zaux1=zn+st*zaux;
			//face da esquerda
			//triangulo "mais negativo" em z e y
			fprintf(op,"%f %f %f\n", xn, yaux0, zaux0);
			fprintf(op, "1 0 0\n");
			fprintf(op,"%f %f\n",(float)sl/(float)slices,(float)st/(float)slices);

			fprintf(op,"%f %f %f\n", xn, yaux0, zaux1);
			fprintf(op, "1 0 0\n");
			fprintf(op,"%f %f\n",(float)sl/(float)slices,(float)(st+1)/(float)slices);

			fprintf(op,"%f %f %f\n", xn, yaux1, zaux0);
			fprintf(op, "1 0 0\n");
			fprintf(op,"%f %f\n",(float)(sl+1)/(float)slices,(float)st/(float)slices);
			//triangulo "menos negativo" em z e y
			fprintf(op,"%f %f %f\n", xn, yaux0, zaux1);
			fprintf(op, "1 0 0\n");
			fprintf(op,"%f %f\n",(float)sl/(float)slices,(float)(st+1)/(float)slices);

			fprintf(op,"%f %f %f\n", xn, yaux1, zaux1);
			fprintf(op, "1 0 0\n");
			fprintf(op,"%f %f\n",(float)(sl+1)/(float)slices,(float)(st+1)/(float)slices);

			fprintf(op,"%f %f %f\n", xn, yaux1, zaux0);
			fprintf(op, "1 0 0\n");
			fprintf(op,"%f %f\n",(float)(sl+1)/(float)slices,(float)st/(float)slices);
			//face da direita
			//triangulo "mais negativo" em z e y
			fprintf(op,"%f %f %f\n", x, yaux0, zaux0);
			fprintf(op, "-1 0 0\n");
			fprintf(op,"%f %f\n",(float)sl/(float)slices,(float)st/(float)slices);

			fprintf(op,"%f %f %f\n", x, yaux1, zaux0);
			fprintf(op, "-1 0 0\n");
			fprintf(op,"%f %f\n",(float)(sl+1)/(float)slices,(float)st/(float)slices);

			fprintf(op,"%f %f %f\n", x, yaux0, zaux1);
			fprintf(op, "-1 0 0\n");
			fprintf(op,"%f %f\n",(float)sl/(float)slices,(float)(st+1)/(float)slices);

			//triangulo "menos negativo" em z e y
			fprintf(op,"%f %f %f\n", x, yaux0, zaux1);
			fprintf(op, "-1 0 0\n");
			fprintf(op,"%f %f\n",(float)sl/(float)slices,(float)(st+1)/(float)slices);

			fprintf(op,"%f %f %f\n", x, yaux1, zaux0);
			fprintf(op, "-1 0 0\n");
			fprintf(op,"%f %f\n",(float)(sl+1)/(float)slices,(float)st/(float)slices);

			fprintf(op,"%f %f %f\n", x, yaux1, zaux1);
			fprintf(op, "-1 0 0\n");
			fprintf(op,"%f %f\n",(float)(sl+1)/(float)slices,(float)(st+1)/(float)slices);
		}
	}
	fclose(op);
}


void cone (float r, float h, int slices, int stacks, char *ficheiro) {
	int sl, st,x;
	//raio da stack mais baixa em y
	float raux0=r;
	//raio da stack mais alta em y
	float raux1=r;
	//coordenada negativa de y
	float yaux = h/2-h;
	//angulo das slices
	float ang = 2*M_PI/slices;
	//angulo das stacks
	float angs = 2*M_PI/stacks;
	//altura das stacks
	float haux = h/stacks;
	//tamanho que decresce o raio em cada stack
	float xaux=r/stacks;
	float y;
	FILE *op = fopen(ficheiro, "w+");
	//base
	for(sl=0;sl<slices;sl++){
		fprintf(op, "%f %f %f\n",r*cos(sl*ang), yaux, r*sin(sl*ang));
		fprintf(op, "0 -1 0\n");
		fprintf(op,"0 0\n");
		fprintf(op, "%f %f %f\n",r*cos((sl+1)*ang), yaux, r*sin((sl+1)*ang));
		fprintf(op, "0 -1 0\n");
		fprintf(op,"0 0\n");
		fprintf(op, "0 %f 0\n", yaux);
		fprintf(op, "0 -1 0\n");
		fprintf(op,"0 0\n");
	}
	for(st=1;st<=stacks;st++){
		raux0=raux1;
		raux1=raux1-xaux;
		y=yaux+(st-1)*haux;
		for(sl=0;sl<slices;sl++){
			fprintf(op,"%f %f %f\n",raux0*cos(sl*ang),y,raux0*sin(sl*ang));
			fprintf(op,"%f %f %f\n",cos(sl*ang),sin((st-1)*angs),sin(sl*ang));
			fprintf(op,"%f %f\n",(float)sl/(float)slices,(float)st/(float)stacks);
			
			fprintf(op,"%f %f %f\n",raux1*cos(sl*ang),y+haux,raux1*sin(sl*ang));
			fprintf(op,"%f %f %f\n",cos(sl*ang),sin(st*angs),sin(sl*ang));
			fprintf(op,"%f %f\n",(float)sl/(float)slices,(float)(st+1)/(float)stacks);

			fprintf(op,"%f %f %f\n",raux0*cos((sl+1)*ang),y,raux0*sin((sl+1)*ang));
			fprintf(op,"%f %f %f\n",cos((sl+1)*ang),sin((st-1)*angs),sin((sl+1)*ang));
			fprintf(op,"%f %f\n",(float)(sl+1)/(float)slices,(float)st/(float)stacks);

			fprintf(op,"%f %f %f\n",raux1*cos(sl*ang),y+haux,raux1*sin(sl*ang));
			fprintf(op,"%f %f %f\n",cos(sl*ang),sin(st*angs),sin(sl*ang));
			fprintf(op,"%f %f\n",(float)sl/(float)slices,(float)(st+1)/(float)stacks);

			fprintf(op,"%f %f %f\n",raux1*cos((sl+1)*ang),y+haux,raux1*sin((sl+1)*ang));
			fprintf(op,"%f %f %f\n",cos((sl+1)*ang),sin(st*angs),sin((sl+1)*ang));
			fprintf(op,"%f %f\n",(float)(sl+1)/(float)slices,(float)(st+1)/(float)stacks);

			fprintf(op,"%f %f %f\n",raux0*cos((sl+1)*ang),y,raux0*sin((sl+1)*ang));
			fprintf(op,"%f %f %f\n",cos((sl+1)*ang),sin((st-1)*angs),sin((sl+1)*ang));
			fprintf(op,"%f %f\n",(float)(sl+1)/(float)slices,(float)st/(float)stacks);
		}
	}
	fclose(op);
}




void sphere (float r, int slices, int stacks , char *ficheiro){
	FILE *op = fopen(ficheiro, "w+");
	int sl,st;
	float stf;
	int odd = (stacks % 2 != 0);
	//ângulo das stacks
	float angst=M_PI/stacks;
	//ângulo das slices
	float angsl=2*M_PI/slices;
if(!odd) {
	for (st=0;st<stacks/2;st++) {
		for(sl = 0; sl<slices; sl++) {

				//face superior
				fprintf(op,"%f %f %f\n", r*cos(st*angst)*sin(sl*angsl),r*sin(st*angst),r*cos(st*angst)*cos(sl*angsl));
				fprintf(op,"%f %f %f\n", cos(st*angst)*sin(sl*angsl),sin(st*angst),cos(st*angst)*cos(sl*angsl));
				fprintf(op,"%f %f\n",(float)sl/slices,0.5+(float)st/stacks);
				
				fprintf(op,"%f %f %f\n", r*cos(st*angst)*sin((sl+1)*angsl),r*sin(st*angst),r*cos(st*angst)*cos((sl+1)*angsl));
				fprintf(op,"%f %f %f\n", cos(st*angst)*sin((sl+1)*angsl),sin(st*angst),cos(st*angst)*cos((sl+1)*angsl));
				fprintf(op,"%f %f\n", (float)(sl+1)/slices,0.5+(float)st/stacks);
				
				fprintf(op,"%f %f %f\n", r*cos((st+1)*angst)*sin(sl*angsl),r*sin((st+1)*angst),r*cos((st+1)*angst)*cos(sl*angsl));
				fprintf(op,"%f %f %f\n", cos((st+1)*angst)*sin(sl*angsl),sin((st+1)*angst),cos((st+1)*angst)*cos(sl*angsl));
				fprintf(op,"%f %f\n", (float)sl/slices,0.5+(float)(st+1)/stacks);
				
				fprintf(op,"%f %f %f\n", r*cos(st*angst)*sin((sl+1)*angsl),r*sin(st*angst),r*cos(st*angst)*cos((sl+1)*angsl));
				fprintf(op,"%f %f %f\n", cos(st*angst)*sin((sl+1)*angsl),sin(st*angst),cos(st*angst)*cos((sl+1)*angsl));
				fprintf(op,"%f %f\n", (float)(sl+1)/slices,0.5+(float)st/stacks);
				
				fprintf(op,"%f %f %f\n", r*cos((st+1)*angst)*sin((sl+1)*angsl),r*sin((st+1)*angst),r*cos((st+1)*angst)*cos((sl+1)*angsl));
				fprintf(op,"%f %f %f\n", cos((st+1)*angst)*sin((sl+1)*angsl),sin((st+1)*angst),cos((st+1)*angst)*cos((sl+1)*angsl));
				fprintf(op,"%f %f\n", (float)(sl+1)/slices,0.5+(float)(st+1)/stacks);
				
				fprintf(op,"%f %f %f\n", r*cos((st+1)*angst)*sin(sl*angsl),r*sin((st+1)*angst),r*cos((st+1)*angst)*cos(sl*angsl));
				fprintf(op,"%f %f %f\n", cos((st+1)*angst)*sin(sl*angsl),sin((st+1)*angst),cos((st+1)*angst)*cos(sl*angsl));
				fprintf(op,"%f %f\n", (float)sl/slices,0.5+(float)(st+1)/stacks);
				//face inferior
				
				fprintf(op,"%f %f %f\n", r*cos((-st)*angst)*sin((sl+1)*angsl),r*sin((-st)*angst),r*cos((-st)*angst)*cos((sl+1)*angsl));
				fprintf(op,"%f %f %f\n", cos((-st)*angst)*sin((sl+1)*angsl),sin((-st)*angst),cos((-st)*angst)*cos((sl+1)*angsl));
				fprintf(op,"%f %f\n", (float)(sl+1)/slices,0.5-(float)st/stacks);
				
				fprintf(op,"%f %f %f\n", r*cos((-st)*angst)*sin(sl*angsl),r*sin((-st)*angst),r*cos((-st)*angst)*cos(sl*angsl));
				fprintf(op,"%f %f %f\n", cos((-st)*angst)*sin(sl*angsl),sin((-st)*angst),cos((-st)*angst)*cos(sl*angsl));
				fprintf(op,"%f %f\n", (float)sl/slices,0.5-(float)st/stacks);
				
				fprintf(op,"%f %f %f\n", r*cos((-st-1)*angst)*sin((sl+1)*angsl),r*sin((-st-1)*angst),r*cos((-st-1)*angst)*cos((sl+1)*angsl));
				fprintf(op,"%f %f %f\n", cos((-st-1)*angst)*sin((sl+1)*angsl),sin((-st-1)*angst),cos((-st-1)*angst)*cos((sl+1)*angsl));
				fprintf(op,"%f %f\n", (float)(sl+1)/slices,0.5-(float)(st+1)/stacks);
				
				fprintf(op,"%f %f %f\n", r*cos((-st-1)*angst)*sin(sl*angsl),r*sin((-st-1)*angst),r*cos((-st-1)*angst)*cos(sl*angsl));
				fprintf(op,"%f %f %f\n", cos((-st-1)*angst)*sin(sl*angsl),sin((-st-1)*angst),cos((-st-1)*angst)*cos(sl*angsl));
				fprintf(op,"%f %f\n", (float)sl/slices,0.5-(float)(st+1)/stacks);
				
				fprintf(op,"%f %f %f\n", r*cos((-st-1)*angst)*sin((sl+1)*angsl),r*sin((-st-1)*angst),r*cos((-st-1)*angst)*cos((sl+1)*angsl));
				fprintf(op,"%f %f %f\n", cos((-st-1)*angst)*sin((sl+1)*angsl),sin((-st-1)*angst),cos((-st-1)*angst)*cos((sl+1)*angsl));
				fprintf(op,"%f %f\n", (float)(sl+1)/slices,0.5-(float)(st+1)/stacks);

				fprintf(op,"%f %f %f\n", r*cos((-st)*angst)*sin(sl*angsl),r*sin((-st)*angst),r*cos((-st)*angst)*cos(sl*angsl));
				fprintf(op,"%f %f %f\n", cos((-st)*angst)*sin(sl*angsl),sin((-st)*angst),cos((-st)*angst)*cos(sl*angsl));
				fprintf(op,"%f %f\n", (float)sl/slices,0.5-(float)st/stacks);
			}
		}
	}
			else {
				float s = angst / 2;
				for(sl = 0; sl<slices; sl++){
					fprintf(op,"%f %f %f\n", r*cos(-s)*sin(sl*angsl),r*sin(-s),r*cos(-s)*cos(sl*angsl));
					fprintf(op,"%f %f %f\n", cos(-s)*sin(sl*angsl),sin(-s),cos(-s)*cos(sl*angsl));
					fprintf(op,"%f %f\n",    (float)sl/slices,0.5-(float)0.5/stacks);
					
					fprintf(op,"%f %f %f\n", r*cos(-s)*sin((sl+1)*angsl),r*sin(-s),r*cos(-s)*cos((sl+1)*angsl));
					fprintf(op,"%f %f %f\n", cos(-s)*sin((sl+1)*angsl),sin(-s),cos(-s)*cos((sl+1)*angsl));
					fprintf(op,"%f %f\n",   (float)(sl+1)/slices,0.5-(float)0.5/stacks);
					
					fprintf(op,"%f %f %f\n", r*cos(s)*sin(sl*angsl),r*sin(s),r*cos(s)*cos(sl*angsl));
					fprintf(op,"%f %f %f\n", cos(s)*sin(sl*angsl),sin(s),cos(s)*cos(sl*angsl));
					fprintf(op,"%f %f\n",	 (float)sl/slices,0.5+(float)0.5/stacks);
					
					fprintf(op,"%f %f %f\n", r*cos(-s)*sin((sl+1)*angsl),r*sin(-s),r*cos(-s)*cos((sl+1)*angsl));
					fprintf(op,"%f %f %f\n", cos(-s)*sin((sl+1)*angsl),sin(-s),cos(-s)*cos((sl+1)*angsl));
					fprintf(op,"%f %f\n",    (float)(sl+1)/slices,0.5-(float)0.5/stacks);
					
					fprintf(op,"%f %f %f\n", r*cos(s)*sin((sl+1)*angsl),r*sin(s),r*cos(s)*cos((sl+1)*angsl));
					fprintf(op,"%f %f %f\n", cos(s)*sin((sl+1)*angsl),sin(s),cos(s)*cos((sl+1)*angsl));
					fprintf(op,"%f %f\n",    (float)(sl+1)/slices,0.5+(float)0.5/stacks);
					
					fprintf(op,"%f %f %f\n", r*cos(s)*sin(sl*angsl),r*sin(s),r*cos(s)*cos(sl*angsl));
					fprintf(op,"%f %f %f\n", cos(s)*sin(sl*angsl),sin(s),cos(s)*cos(sl*angsl));
					fprintf(op,"%f %f\n",    (float)sl/slices,0.5+(float)0.5/stacks);
				}
				for (stf=0.5;stf<(stacks/2);stf++) {
					for(sl=0; sl<slices; sl++) {
						fprintf(op,"%f %f %f\n", r*cos(stf*angst)*sin(sl*angsl),r*sin(stf*angst),r*cos(stf*angst)*cos(sl*angsl));
						fprintf(op,"%f %f %f\n", cos(stf*angst)*sin(sl*angsl),sin(stf*angst),cos(stf*angst)*cos(sl*angsl));
						fprintf(op,"%f %f\n",    (float)sl/slices,0.5+(float)stf/stacks);
						
						fprintf(op,"%f %f %f\n", r*cos(stf*angst)*sin((sl+1)*angsl),r*sin(stf*angst),r*cos(stf*angst)*cos((sl+1)*angsl));
						fprintf(op,"%f %f %f\n", cos(stf*angst)*sin((sl+1)*angsl),sin(stf*angst),cos(stf*angst)*cos((sl+1)*angsl));
						fprintf(op,"%f %f\n",    (float)(sl+1)/slices,0.5+(float)stf/stacks);

						fprintf(op,"%f %f %f\n", r*cos((stf+1)*angst)*sin(sl*angsl),r*sin((stf+1)*angst),r*cos((stf+1)*angst)*cos(sl*angsl));
						fprintf(op,"%f %f %f\n", cos((stf+1)*angst)*sin(sl*angsl),sin((stf+1)*angst),cos((stf+1)*angst)*cos(sl*angsl));
						fprintf(op,"%f %f\n",    (float)sl/slices,0.5+(float)(stf+1)/stacks);
						
						fprintf(op,"%f %f %f\n", r*cos(stf*angst)*sin((sl+1)*angsl),r*sin(stf*angst),r*cos(stf*angst)*cos((sl+1)*angsl));
						fprintf(op,"%f %f %f\n", cos(stf*angst)*sin((sl+1)*angsl),sin(stf*angst),cos(stf*angst)*cos((sl+1)*angsl));
						fprintf(op,"%f %f\n",    (float)(sl+1)/slices,0.5+(float)stf/stacks);
						
						fprintf(op,"%f %f %f\n", r*cos((stf+1)*angst)*sin((sl+1)*angsl),r*sin((stf+1)*angst),r*cos((stf+1)*angst)*cos((sl+1)*angsl));
						fprintf(op,"%f %f %f\n", cos((stf+1)*angst)*sin((sl+1)*angsl),sin((stf+1)*angst),cos((stf+1)*angst)*cos((sl+1)*angsl));
						fprintf(op,"%f %f\n",    (float)(sl+1)/slices,0.5+(float)(stf+1)/stacks);
						
						fprintf(op,"%f %f %f\n", r*cos((stf+1)*angst)*sin(sl*angsl),r*sin((stf+1)*angst),r*cos((stf+1)*angst)*cos(sl*angsl));
						fprintf(op,"%f %f %f\n", cos((stf+1)*angst)*sin(sl*angsl),sin((stf+1)*angst),cos((stf+1)*angst)*cos(sl*angsl));
						fprintf(op,"%f %f\n",    (float)sl/slices,0.5+(float)(stf+1)/stacks);
					}
				}
				for (stf=-0.5;stf<(stacks/2);stf++) {
					for(sl=0; sl<slices; sl++) {
						//face inferior
						fprintf(op,"%f %f %f\n", r*cos((-stf)*angst)*sin((sl+1)*angsl),r*sin((-stf)*angst),r*cos((-stf)*angst)*cos((sl+1)*angsl));
						fprintf(op,"%f %f %f\n", cos((-stf)*angst)*sin((sl+1)*angsl),sin((-stf)*angst),cos((-stf)*angst)*cos((sl+1)*angsl));
						fprintf(op,"%f %f\n",    (float)(sl+1)/slices,0.5-(float)stf/stacks);
						
						fprintf(op,"%f %f %f\n", r*cos((-stf)*angst)*sin(sl*angsl),r*sin((-stf)*angst),r*cos((-stf)*angst)*cos(sl*angsl));
						fprintf(op,"%f %f %f\n", cos((-stf)*angst)*sin(sl*angsl),sin((-stf)*angst),cos((-stf)*angst)*cos(sl*angsl));
						fprintf(op,"%f %f\n",    (float)sl/slices,0.5-(float)stf/stacks);

						fprintf(op,"%f %f %f\n", r*cos((-stf-1)*angst)*sin((sl+1)*angsl),r*sin((-stf-1)*angst),r*cos((-stf-1)*angst)*cos((sl+1)*angsl));
						fprintf(op,"%f %f %f\n", cos((-stf-1)*angst)*sin((sl+1)*angsl),sin((-stf-1)*angst),cos((-stf-1)*angst)*cos((sl+1)*angsl));
						fprintf(op,"%f %f\n",    (float)(sl+1)/slices,0.5-(float)(stf+1)/stacks);
						
						fprintf(op,"%f %f %f\n", r*cos((-stf-1)*angst)*sin(sl*angsl),r*sin((-stf-1)*angst),r*cos((-stf-1)*angst)*cos(sl*angsl));
						fprintf(op,"%f %f %f\n", cos((-stf-1)*angst)*sin(sl*angsl),sin((-stf-1)*angst),cos((-stf-1)*angst)*cos(sl*angsl));
						fprintf(op,"%f %f\n",    (float)sl/slices,0.5-(float)(stf+1)/stacks);

						fprintf(op,"%f %f %f\n", r*cos((-stf-1)*angst)*sin((sl+1)*angsl),r*sin((-stf-1)*angst),r*cos((-stf-1)*angst)*cos((sl+1)*angsl));
						fprintf(op,"%f %f %f\n", cos((-stf-1)*angst)*sin((sl+1)*angsl),sin((-stf-1)*angst),cos((-stf-1)*angst)*cos((sl+1)*angsl));
						fprintf(op,"%f %f\n",    (float)(sl+1)/slices,0.5-(float)(stf+1)/stacks);

						fprintf(op,"%f %f %f\n", r*cos((-stf)*angst)*sin(sl*angsl),r*sin((-stf)*angst),r*cos((-stf)*angst)*cos(sl*angsl));
						fprintf(op,"%f %f %f\n", cos((-stf)*angst)*sin(sl*angsl),sin((-stf)*angst),cos((-stf)*angst)*cos(sl*angsl));
						fprintf(op,"%f %f\n",    (float)sl/slices,0.5-(float)(stf)/stacks);
					}
				}
			}


	fclose(op);
}



void cylinder(float radius, float height, int slices, int stacks, char *ficheiro){
	int st;
	//ângulo das slices
	float alpha = 2*M_PI / slices;

	//altura das stacks
	float h = height/stacks;
	//colocar a altura em metade para centrar em 0,0
	height=height/2;
	FILE *op;
	op = fopen(ficheiro, "w+");

	for(int sl = 0; sl < slices; sl++) {
		//faces superiores
		fprintf(op,"%f %f %f\n", radius*sin(alpha*sl), height, radius*cos(alpha*sl));
		fprintf(op,"0 1 0\n");
		fprintf(op,"0 0\n");

		fprintf(op,"%f %f %f\n", radius*sin(alpha*(sl+1)), height, radius*cos(alpha*(sl+1)));
		fprintf(op,"0 1 0\n");
		fprintf(op,"0 0\n");

		fprintf(op,"0 %f 0\n", height);
		fprintf(op,"0 1 0\n");
		fprintf(op,"0 0\n");

		//faces inferiores
		fprintf(op,"%f %f %f\n", radius*sin(alpha*sl), -height, radius*cos(alpha*sl));
		fprintf(op,"0 -1 0\n");
		fprintf(op,"0 0\n");
		
		fprintf(op,"0 %f 0\n", -height);
		fprintf(op,"0 -1 0\n");
		fprintf(op,"0 0\n");

		fprintf(op,"%f %f %f\n", radius*sin(alpha*(sl+1)), -height, radius*cos(alpha*(sl+1)));
		fprintf(op,"0 -1 0\n");
		fprintf(op,"0 0\n");

		for(st = 0; st < stacks; st++){
			//faces laterais
			fprintf(op,"%f %f %f\n", radius*sin(alpha*(sl+1)), height-(st*h), radius*cos(alpha*(sl+1)));
			fprintf(op,"%f 0 %f\n", sin(alpha*(sl+1)), cos(alpha*(sl+1)));
			fprintf(op,"%f %f\n",(float)(sl+1)/(float)stacks,(float)(st+1)/(float)stacks);
			
			fprintf(op,"%f %f %f\n", radius*sin(alpha*sl), height-(st*h), radius*cos(alpha*sl));
			fprintf(op,"%f 0 %f\n", sin(alpha*sl), cos(alpha*sl));
			fprintf(op,"%f %f\n",(float)sl/(float)slices,(float)st/(float)stacks);

			fprintf(op,"%f %f %f\n", radius*sin(alpha*sl), height-((st+1)*h), radius*cos(alpha*sl));
			fprintf(op,"%f 0 %f\n", sin(alpha*sl), cos(alpha*sl));
			fprintf(op,"%f %f\n",(float)sl/(float)slices,(float)(st+1)/(float)stacks);

			fprintf(op,"%f %f %f\n", radius*sin(alpha*sl), height-((st+1)*h), radius*cos(alpha*sl));
			fprintf(op,"%f 0 %f\n", sin(alpha*sl), cos(alpha*sl));
			fprintf(op,"%f %f\n",(float)sl/(float)slices,(float)(st+1)/(float)stacks);

			fprintf(op,"%f %f %f\n", radius*sin(alpha*(sl+1)), height-((st+1)*h), radius*cos(alpha*(sl+1)));
			fprintf(op,"%f 0 %f\n", sin(alpha*(sl+1)), cos(alpha*(sl+1)));
			fprintf(op,"%f %f\n",(float)(sl+1)/(float)slices,(float)(st+1)/(float)stacks);

			fprintf(op,"%f %f %f\n", radius*sin(alpha*(sl+1)), height-(st*h), radius*cos(alpha*(sl+1)));
			fprintf(op,"%f 0 %f\n", sin(alpha*(sl+1)), cos(alpha*(sl+1)));
			fprintf(op,"%f %f\n",(float)(sl+1)/(float)slices,(float)st/(float)stacks);
		}
	}
	fclose(op);
}

void ring (float innerRadius, float outerRadius, int sl, char *ficheiro){
	FILE *op = fopen(ficheiro, "w+");
	float alpha = 2*M_PI/sl;

	for(int i = 0; i < sl; i++){
		//Triangulo com base no inner
		fprintf(op,"%f 0 %f\n", innerRadius*sin(alpha*(i)),innerRadius*cos(alpha*(i)));
		fprintf(op,"0 1 0\n");
		fprintf(op,"%f 1\n",(float)(i)/sl);

		fprintf(op,"%f 0 %f\n", outerRadius*sin(alpha*(i)),outerRadius*cos(alpha*(i)));
		fprintf(op,"0 1 0\n");
		fprintf(op,"%f 0\n",(float)(i)/sl);

		fprintf(op,"%f 0 %f\n", innerRadius*sin(alpha*(i+1)),innerRadius*cos(alpha*(i+1)));
		fprintf(op,"0 1 0\n");
		fprintf(op,"%f 1\n",(float)(i+1)/sl);

		fprintf(op,"%f 0 %f\n", innerRadius*sin(alpha*(i+1)),innerRadius*cos(alpha*(i+1)));
		fprintf(op,"0 -1 0\n");
		fprintf(op,"%f 1\n",(float)(i+1)/sl);

		fprintf(op,"%f 0 %f\n", outerRadius*sin(alpha*(i)),outerRadius*cos(alpha*(i)));
		fprintf(op,"0 -1 0\n");
		fprintf(op,"%f 0\n",(float)(i)/sl);
		
		fprintf(op,"%f 0 %f\n", innerRadius*sin(alpha*(i)),innerRadius*cos(alpha*(i)));
		fprintf(op,"0 -1 0\n");
		fprintf(op,"%f 1\n",(float)(i)/sl);


		//Triangulo com base no outer
		fprintf(op,"%f 0 %f\n", outerRadius*sin(alpha*(i)),outerRadius*cos(alpha*(i)));
		fprintf(op,"0 1 0\n");
		fprintf(op,"%f 0\n",(float)(i)/sl);
		
		fprintf(op,"%f 0 %f\n", outerRadius*sin(alpha*(i+1)),outerRadius*cos(alpha*(i+1)));
		fprintf(op,"0 1 0\n");
		fprintf(op,"%f 0\n",(float)(i+1)/sl);
		
		fprintf(op,"%f 0 %f\n", innerRadius*sin(alpha*(i+1)),innerRadius*cos(alpha*(i+1)));
		fprintf(op,"0 1 0\n");
		fprintf(op,"%f 1\n",(float)(i+1)/sl);

		fprintf(op,"%f 0 %f\n", innerRadius*sin(alpha*(i+1)),innerRadius*cos(alpha*(i+1)));
		fprintf(op,"0 -1 0\n");
		fprintf(op,"%f 1\n",(float)(i+1)/sl);
		
		fprintf(op,"%f 0 %f\n", outerRadius*sin(alpha*(i+1)),outerRadius*cos(alpha*(i+1)));
		fprintf(op,"0 -1 0\n");
		fprintf(op,"%f 0\n",(float)(i+1)/sl);
		
		fprintf(op,"%f 0 %f\n", outerRadius*sin(alpha*(i)),outerRadius*cos(alpha*(i)));
		fprintf(op,"0 -1 0\n");
		fprintf(op,"%f 0\n",(float)(i)/sl);

		printf("%i\n",i);
	}
	fclose(op);
}

void bezier(int tesselation, char* file, char* filetoWrite) {
	FILE* op = fopen(file, "r+");
	int n = 0, np = 0, i = 0;

	fscanf(op, "%d\n", &n);
	int patches[n][16];
	for(i = 0; i < n; i++)
		fscanf(op, "%d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d\n",
		&patches[i][0], &patches[i][1], &patches[i][2], &patches[i][3],
		&patches[i][4], &patches[i][5], &patches[i][6], &patches[i][7],
		&patches[i][8], &patches[i][9], &patches[i][10],&patches[i][11],
		&patches[i][12],&patches[i][13],&patches[i][14],&patches[i][15]);

	fscanf(op, "%d\n", &np);
	float points[np][3];
	for(i = 0; i < np; i++) fscanf(op, " %g, %g, %g\n", &points[i][0], &points[i][1], &points[i][2]);

	makeFigure_bezier(tesselation, filetoWrite, patches, n, points, np);
}

void makeFigure_bezier(int tess, char *newFile, int patches[][16], int n, float points[][3], int np) {
  int * patchIndices;
  float * ma[16], mT[3][16], px[4][4], py[4][4], pz[4][4], res[3];
  float u, v, level = (float) 1 / tess;
	int patch, i, j, temp;

  FILE *file = fopen(newFile, "w+");
  for(patch = 0; patch < n; patch++) {
    temp = 0;
    //Criar matriz com de pontos
    for(i = 0; i < 16; i++) ma[i] = points[patches[patch][i]];

    //Matriz com todos os Pix, Piy, Piz
    for(i = 0; i < 4; i++) {
      for(j = 0; j < 4; j++, temp++) {
        px[i][j] = ma[temp][0];
        py[i][j] = ma[temp][1];
        pz[i][j] = ma[temp][2];
      }
    }

    //Matriz de bezier M
    float m[4][4] = { {-1.0f,  3.0f, -3.0f, 1.0f},
		                  { 3.0f, -6.0f,  3.0f, 0.0f},
		                  {-3.0f,  3.0f,  0.0f, 0.0f},
		                  { 1.0f,  0.0f,  0.0f, 0.0f} };

    for(u = 0.0f; u < 1 ; u += level) {
      for(v = 0.0f; v < 1 ; v += level) {
          res[0] = getB_Point(u, v, m, px);
          res[1] = getB_Point(u, v, m, py);
          res[2] = getB_Point(u, v, m, pz);
          fprintf(file, "%f %f %f\n", res[0], res[1], res[2]);

          res[0] = getB_Point(u + level, v + level, m, px);
          res[1] = getB_Point(u + level, v + level, m, py);
          res[2] = getB_Point(u + level, v + level, m, pz);
          fprintf(file, "%f %f %f\n", res[0], res[1], res[2]);

          res[0] = getB_Point(u + level, v, m, px);
          res[1] = getB_Point(u + level, v, m, py);
          res[2] = getB_Point(u + level, v, m, pz);
          fprintf(file, "%f %f %f\n", res[0], res[1], res[2]);

          res[0] = getB_Point(u, v, m, px);
          res[1] = getB_Point(u, v, m, py);
          res[2] = getB_Point(u, v, m, pz);
          fprintf(file, "%f %f %f\n", res[0], res[1], res[2]);

          res[0] = getB_Point(u, v + level, m, px);
          res[1] = getB_Point(u, v + level, m, py);
          res[2] = getB_Point(u, v + level, m, pz);
          fprintf(file, "%f %f %f\n", res[0], res[1], res[2]);

          res[0] = getB_Point (u + level, v + level, m, px);
          res[1] = getB_Point (u + level, v + level, m, py);
          res[2] = getB_Point (u + level, v + level, m, pz);
          fprintf(file, "%f %f %f\n", res[0], res[1], res[2]);
      }
    }
  }
}

float getB_Point(float u, float v, float m[4][4], float p[4][4]) {
  float ponto = 0;
  float temp1[4], temp2[4];
	int i;

  //Calculo  temp1i = Bi(u) * M
  for(i = 0; i < 4; i++)
    temp1[i] = ( m[0][i] * powf(u, 3.0) ) +
							 ( m[1][i] * powf(u, 2.0) ) +
							 ( m[2][i] * u) +
							 m[3][i];

  //Calculo temp2i = temp1i * Pi
  for(i = 0; i < 4; i++)
    temp2[i] = ( temp1[0] * p[0][i] ) +
							 ( temp1[1] * p[1][i] ) +
							 ( temp1[2] * p[2][i] ) +
							 ( temp1[3] * p[3][i] );

  //Calculo temp1i = temp2i * MT
  for(i = 0; i < 4; i++)
    temp1[i] = ( temp2[0] * m[0][i] ) +
							 ( temp2[1] * m[1][i] ) +
							 ( temp2[2] * m[2][i] ) +
							 ( temp2[3] * m[3][i] );

  //Calculo ponto = temp1i * Bi(v)
  ponto = temp1[0] * powf(v,3.0);
  ponto += temp1[1] * powf(v,2.0);
  ponto += temp1[2] * v;
  ponto += temp1[3];

  return ponto;
}
