#include <math.h>
#include <stdio.h>
#include <unistd.h> /* chamadas ao sistema: defs e decls essenciais */
#include <fcntl.h>  /* O_RDONLY, O_WRONLY, O_CREAT, O_* */
#include <stdlib.h>
#include <string.h>


void plane(float x, float z, char* ficheiro);
void box(float x, float y, float z, int stacks, int slices, char* ficheiro);
void cone (float r, float h, float sl, char *ficheiro);



int main(int argc, char **argv) {
	int err = 0;
	if(argc < 2){
		printf("Input inválido.\n");
		exit(-1);
	}

	if(strcmp(argv[1], "plane") == 0)
			plane(atof(argv[2]), atof(argv[3]), argv[4]);
	else if(strcmp(argv[1], "box") == 0)
			box(atof(argv[2]), atof(argv[3]), atof(argv[4]), atoi(argv[5]), atoi(argv[6]), argv[7]);
	else if(strcmp(argv[1], "cone") == 0)
			cone(atoi(argv[2]), atoi(argv[3]), atoi(argv[4]), argv[5]);
	else printf("Not suported yet!");

	return 1;
}



void plane(float x, float z, char* ficheiro){
	int r;
	FILE *op;
	op = fopen(ficheiro, "w+");
		fprintf(op,"0 0 0\n");
		fprintf(op, "0 0 %f\n", z);
		fprintf(op, "%f 0 %f\n", x, z);
		fprintf(op, "%f 0 %f\n", x, z);
		fprintf(op, "%f 0 0\n", x);
		fprintf(op, "0 0 0\n");
	fclose(op);
}



void box(float x, float y, float z, int stacks, int slices, char* ficheiro){
	int r,st,sl;
	float yaux0,yaux1,zaux0,zaux1;
	//altura das stacks
	float yaux = y / stacks;
	//largura das slices
	float zaux = z / slices;
	FILE *op = fopen(ficheiro, "w+");

	for(sl = 1; sl <= slices; sl++) {
		zaux0 = z - (sl-1) * zaux;
		zaux1 = z - sl * zaux;
		//face de baixo
		fprintf(op,"0 0 %f\n", zaux0);
		fprintf(op,"0 0 %f\n", zaux1);
		fprintf(op,"%f 0 %f\n", x, zaux1);
		fprintf(op,"%f 0 %f\n", x, zaux1);
		fprintf(op,"%f 0 %f\n", x, zaux0);
		fprintf(op,"0 0 %f\n", zaux0);

		//face de cima
		fprintf(op,"%f %f %f\n", x, y, zaux0);
		fprintf(op,"%f %f %f\n", x, y, zaux1);
		fprintf(op,"0 %f %f\n", y, zaux1);
		fprintf(op,"0 %f %f\n", y, zaux1);
		fprintf(op,"0 %f %f\n", y, zaux0);
		fprintf(op,"%f %f %f\n", x, y, zaux0);
	}

	for(st = 1; st <= stacks; st++) {
		yaux0 = y - (st-1) * yaux;
		yaux1 = y - st * yaux;

		for(sl = 1; sl <= slices; sl++) {
			zaux0 = z - (sl-1) * zaux;
			zaux1 = z - sl * zaux;
			//face da direita
			fprintf(op,"%f %f %f\n", x, yaux1, zaux1);
			fprintf(op,"%f %f %f\n", x, yaux0, zaux1);
			fprintf(op,"%f %f %f\n", x, yaux1, zaux0);
			fprintf(op,"%f %f %f\n", x, yaux1, zaux0);
			fprintf(op,"%f %f %f\n", x, yaux0, zaux1);
			fprintf(op,"%f %f %f\n", x, yaux0, zaux0);

			//face da esquerda
			fprintf(op,"0 %f %f\n", yaux0, zaux1);
			fprintf(op,"0 %f %f\n", yaux1, zaux1);
			fprintf(op,"0 %f %f\n", yaux1, zaux0);
			fprintf(op,"0 %f %f\n", yaux1, zaux0);
			fprintf(op,"0 %f %f\n", yaux0, zaux0);
			fprintf(op,"0 %f %f\n", yaux0, zaux1);
		}
	}

	for(st = 1; st <= stacks; st++){
		yaux0 = y - (st-1) * yaux;
		yaux1 = y - st * yaux;

		//face da frente
		fprintf(op,"%f %f %f\n", x, yaux0, z);
		fprintf(op,"0 %f %f\n", yaux0, z);
		fprintf(op,"0 %f %f\n", yaux1, z);
		fprintf(op,"0 %f %f\n", yaux1, z);
		fprintf(op,"%f %f %f\n", x, yaux1, z);
		fprintf(op,"%f %f %f\n", x, yaux0, z);

		//face de traz
		fprintf(op,"0 %f 0\n", yaux1);
		fprintf(op,"0 %f 0\n", yaux0);
		fprintf(op,"%f %f 0\n", x, yaux0);
		fprintf(op,"0 %f 0\n", yaux1);
		fprintf(op,"%f %f 0\n", x, yaux0);
		fprintf(op,"%f %f 0\n", x, yaux1);
	}

	fclose(op);
}



void cone (float r, float h, float sl, char *ficheiro) {
	FILE *op = fopen(ficheiro, "w+");
	int i;

	for(i = 1; i <= sl; i++){
		// base
		fprintf(op,"0 0 0\n");
		fprintf(op,"%f 0 %f\n", r*cos(i*2*M_PI/sl), r*sin(i*2*M_PI/sl));
		fprintf(op,"%f 0 %f\n", r*cos((i-1)*2*M_PI/sl), r*sin((i-1)*2*M_PI/sl));

		// faces
		fprintf(op,"%f 0 %f\n", r*cos(i*2*M_PI/sl), r*sin(i*2*M_PI/sl));
		fprintf(op,"0 %f 0\n", h);
		fprintf(op,"%f 0 %f\n", r*cos((i-1)*2*M_PI/sl), r*sin((i-1)*2*M_PI/sl));
	}
	fclose(op);
}
