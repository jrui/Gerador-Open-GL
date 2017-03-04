#include <math.h>
#include <stdio.h>
#include <unistd.h> /* chamadas ao sistema: defs e decls essenciais */
#include <fcntl.h>  /* O_RDONLY, O_WRONLY, O_CREAT, O_* */
#include <stdlib.h>
#include <string.h>



void plane(float x, float z, char *ficheiro);
void box(float x, float y, float z, int slices, char *ficheiro);
void cone (float r, float h, int sl, int st, char *ficheiro);
void sphere (float r,float sl,float st , char *ficheiro);
void cylinder(float radius, float height, int slices, int stacks, char *ficheiro);




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
	else printf("Not suported yet!");

	return 1;
}



void plane(float x, float z, char* ficheiro){
	int r;
	FILE *op;
	op = fopen(ficheiro, "w+");
	fprintf(op, "%f 0 %f\n", x/2, z/2);
	fprintf(op, "%f 0 %f\n", x/2, -z/2);
	fprintf(op, "%f 0 %f\n", -x/2, -z/2);
	fprintf(op, "%f 0 %f\n", -x/2, -z/2);
	fprintf(op, "%f 0 %f\n", -x/2, z/2);
	fprintf(op, "%f 0 %f\n", x/2, z/2);
	fclose(op);
}



void box(float x, float y, float z, int slices, char* ficheiro){
	int r,st,sl;
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
			fprintf(op,"%f %f %f\n", xaux0, y, zaux1);
			fprintf(op,"%f %f %f\n", xaux1, y, zaux1);
			//triângulo "mais negativo" em x e z
			fprintf(op,"%f %f %f\n", xaux0, y, zaux0);
			fprintf(op,"%f %f %f\n", xaux1, y, zaux1);
			fprintf(op,"%f %f %f\n", xaux1, y, zaux0);
			//face de baixo
			//triângulo "menos negativo" em x e z
			fprintf(op,"%f %f %f\n", xaux0, yn, zaux0);
			fprintf(op,"%f %f %f\n", xaux1, yn, zaux1);
			fprintf(op,"%f %f %f\n", xaux0, yn, zaux1);
			//triângulo "mais negativo" em x e z
			fprintf(op,"%f %f %f\n", xaux0, yn, zaux0);
			fprintf(op,"%f %f %f\n", xaux1, yn, zaux0);
			fprintf(op,"%f %f %f\n", xaux1, yn, zaux1);
			//face da frente
			//triangulo "menos negativo" em x e "mais negativo" em y
			fprintf(op,"%f %f %f\n", xaux0, yaux0, z);
			fprintf(op,"%f %f %f\n", xaux1, yaux0, z);
			fprintf(op,"%f %f %f\n", xaux1, yaux1, z);
			//triangulo "mais negativo" em x e "menos negativo" em y
			fprintf(op,"%f %f %f\n", xaux0, yaux0, z);
			fprintf(op,"%f %f %f\n", xaux1, yaux1, z);
			fprintf(op,"%f %f %f\n", xaux0, yaux1, z);
			//face de traz
			//triangulo "menos negativo" em x e "mais negativo" em y
			fprintf(op,"%f %f %f\n", xaux0, yaux0, zn);
			fprintf(op,"%f %f %f\n", xaux1, yaux1, zn);
			fprintf(op,"%f %f %f\n", xaux1, yaux0, zn);
			//triangulo "mais negativo" em x e "menos negativo" em y
			fprintf(op,"%f %f %f\n", xaux0, yaux0, zn);
			fprintf(op,"%f %f %f\n", xaux0, yaux1, zn);
			fprintf(op,"%f %f %f\n", xaux1, yaux1, zn);
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
			fprintf(op,"%f %f %f\n", xn, yaux0, zaux1);
			fprintf(op,"%f %f %f\n", xn, yaux1, zaux0);
			//triangulo "menos negativo" em z e y
			fprintf(op,"%f %f %f\n", xn, yaux0, zaux1);
			fprintf(op,"%f %f %f\n", xn, yaux1, zaux1);
			fprintf(op,"%f %f %f\n", xn, yaux1, zaux0);
			//face da direita
			//triangulo "mais negativo" em z e y
			fprintf(op,"%f %f %f\n", x, yaux0, zaux0);
			fprintf(op,"%f %f %f\n", x, yaux1, zaux0);
			fprintf(op,"%f %f %f\n", x, yaux0, zaux1);
			//triangulo "menos negativo" em z e y
			fprintf(op,"%f %f %f\n", x, yaux0, zaux1);
			fprintf(op,"%f %f %f\n", x, yaux1, zaux0);
			fprintf(op,"%f %f %f\n", x, yaux1, zaux1);
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
	//altura das stacks
	float haux = h/stacks;
	//tamanho que decresce o raio em cada stack
	float xaux=r/stacks;
	float y;
	FILE *op = fopen(ficheiro, "w+");
	//base
	for(sl=0;sl<slices;sl++){
		fprintf(op, "%f %f %f\n",r*cos(sl*ang), yaux, r*sin(sl*ang));
		fprintf(op, "%f %f %f\n",r*cos((sl+1)*ang), yaux, r*sin((sl+1)*ang));
		fprintf(op, "0 %f 0\n", yaux);
	}
	for(st=1;st<=stacks;st++){
		raux0=raux1;
		raux1=raux1-xaux;
		y=yaux+(st-1)*haux;
		for(sl=0;sl<slices;sl++){
			fprintf(op,"%f %f %f\n",raux0*cos(sl*ang),y,raux0*sin(sl*ang));
			fprintf(op,"%f %f %f\n",raux1*cos(sl*ang),y+haux,raux1*sin(sl*ang));
			fprintf(op,"%f %f %f\n",raux0*cos((sl+1)*ang),y,raux0*sin((sl+1)*ang));
			fprintf(op,"%f %f %f\n",raux1*cos(sl*ang),y+haux,raux1*sin(sl*ang));
			fprintf(op,"%f %f %f\n",raux1*cos((sl+1)*ang),y+haux,raux1*sin((sl+1)*ang));
			fprintf(op,"%f %f %f\n",raux0*cos((sl+1)*ang),y,raux0*sin((sl+1)*ang));
		}
	}
	fclose(op);
}

void sphere (float r,float slices,float stacks , char *ficheiro){

	FILE *op = fopen(ficheiro, "w+");
	int sl,st;
	//ângulo das stacks
	float angst=M_PI/stacks;
	//ângulo das slices
	float angsl=2*M_PI/slices;
	for (st=0;st<stacks/2;st++){
		for(sl = 0; sl<slices; sl++){
			//face superior
			fprintf(op,"%f %f %f\n", r*cos(st*angst)*sin(sl*angsl),r*sin(st*angst),r*cos(st*angst)*cos(sl*angsl));
			fprintf(op,"%f %f %f\n", r*cos(st*angst)*sin((sl+1)*angsl),r*sin(st*angst),r*cos(st*angst)*cos((sl+1)*angsl));
			fprintf(op,"%f %f %f\n", r*cos((st+1)*angst)*sin(sl*angsl),r*sin((st+1)*angst),r*cos((st+1)*angst)*cos(sl*angsl));

			fprintf(op,"%f %f %f\n", r*cos(st*angst)*sin((sl+1)*angsl),r*sin(st*angst),r*cos(st*angst)*cos((sl+1)*angsl));
			fprintf(op,"%f %f %f\n", r*cos((st+1)*angst)*sin((sl+1)*angsl),r*sin((st+1)*angst),r*cos((st+1)*angst)*cos((sl+1)*angsl));
			fprintf(op,"%f %f %f\n", r*cos((st+1)*angst)*sin(sl*angsl),r*sin((st+1)*angst),r*cos((st+1)*angst)*cos(sl*angsl));

			//face inferior
			fprintf(op,"%f %f %f\n", r*cos((-st)*angst)*sin(sl*angsl),r*sin((-st)*angst),r*cos((-st)*angst)*cos(sl*angsl));
			fprintf(op,"%f %f %f\n", r*cos((-st-1)*angst)*sin(sl*angsl),r*sin((-st-1)*angst),r*cos((-st-1)*angst)*cos(sl*angsl));
			fprintf(op,"%f %f %f\n", r*cos((-st)*angst)*sin((sl+1)*angsl),r*sin((-st)*angst),r*cos((-st)*angst)*cos((sl+1)*angsl));

			fprintf(op,"%f %f %f\n", r*cos((-st)*angst)*sin((sl+1)*angsl),r*sin((-st)*angst),r*cos((-st)*angst)*cos((sl+1)*angsl));
			fprintf(op,"%f %f %f\n", r*cos((-st-1)*angst)*sin(sl*angsl),r*sin((-st-1)*angst),r*cos((-st-1)*angst)*cos(sl*angsl));
			fprintf(op,"%f %f %f\n", r*cos((-st-1)*angst)*sin((sl+1)*angsl),r*sin((-st-1)*angst),r*cos((-st-1)*angst)*cos((sl+1)*angsl));

		}
	}


	fclose(op);

}



void cylinder(float radius, float height, int slices, int stacks, char *ficheiro) {
	float x, y, z;
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
		fprintf(op,"%f %f %f\n", radius*sin(alpha*(sl+1)), height, radius*cos(alpha*(sl+1)));
		fprintf(op,"0 %f 0\n", height);

		//faces inferiores
		fprintf(op,"%f %f %f\n", radius*sin(alpha*sl), -height, radius*cos(alpha*sl));
		fprintf(op,"0 %f 0\n", -height);
		fprintf(op,"%f %f %f\n", radius*sin(alpha*(sl+1)), -height, radius*cos(alpha*(sl+1)));

		for(st = 0; st < stacks; st++){
			//faces laterais
			fprintf(op,"%f %f %f\n", radius*sin(alpha*(sl+1)), height-(st*h), radius*cos(alpha*(sl+1)));
			fprintf(op,"%f %f %f\n", radius*sin(alpha*sl), height-(st*h), radius*cos(alpha*sl));
			fprintf(op,"%f %f %f\n", radius*sin(alpha*sl), height-((st+1)*h), radius*cos(alpha*sl));
			fprintf(op,"%f %f %f\n", radius*sin(alpha*sl), height-((st+1)*h), radius*cos(alpha*sl));
			fprintf(op,"%f %f %f\n", radius*sin(alpha*(sl+1)), height-((st+1)*h), radius*cos(alpha*(sl+1)));
			fprintf(op,"%f %f %f\n", radius*sin(alpha*(sl+1)), height-(st*h), radius*cos(alpha*(sl+1)));
		}
	}
	fclose(op);
}
