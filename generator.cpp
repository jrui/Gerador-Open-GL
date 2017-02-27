#include <math.h>
#include <stdio.h>
#include <unistd.h> /* chamadas ao sistema: defs e decls essenciais */
#include <fcntl.h>  /* O_RDONLY, O_WRONLY, O_CREAT, O_* */
#include <stdlib.h>
#include <string.h>

// opcao==0, funcao é utilizada na plane, opcao==1, funcao utilizada como auxiliar de outras, vai preencher em ficheiro os vértices do plane
void plane(float x, float z, char* ficheiro){
	int r;
	FILE *op;
	op = fopen(ficheiro, "w+");
	fprintf(op,"0 0 0\n0 0 %f\n%f 0 %f\n%f 0 %f\n%f 0 0\n0 0 0\n", z, x, z, x, z,x);
	fclose(op);
}

void box(float x, float y, float z, int stacks, char* ficheiro){
	int r,st;
	float yaux0,yaux1;
	//altura das stacks
	float yaux=y/stacks;
	FILE *op = fopen(ficheiro, "w+");
	//face de baixo
	fprintf(op,"0 0 0\n%f 0 %f\n0 0 %f\n%f 0 %f\n0 0 0\n%f 0 0\n", x, z, z, x, z,x);
	//face da direita
	fprintf(op,"%f 0 0\n%f %f 0\n%f 0 %f\n%f 0 %f\n%f %f 0\n%f %f %f\n", x, x, y, x, z,x,z,x,y,x,y,z);
	//face de cima
	fprintf(op,"%f %f %f\n%f %f 0\n0 %f 0\n0 %f 0\n0 %f %f\n%f %f %f\n", x,y,z,x,y,y,y,y,z,x,y,z);
	//face da esquerda
	fprintf(op,"0 %f 0\n0 0 0\n0 %f %f\n0 0 0\n0 0 %f\n0 %f %f\n", y,y,z,z,y,z);
	for(st=1;st<=stacks;st++){
		yaux0=y-st*(yaux-1);
		yaux1=y-st*yaux;
		//face da frente
		fprintf(op,"%f %f %f\n0 %f %f\n0 %f %f\n0 %f %f\n%f %f %f\n%f %f %f\n", x,yaux1,z,yaux1,z,yaux0,z,yaux0,z,x,yaux0,z,x,yaux1,z);
		//face de traz
		fprintf(op,"0 %f 0\n0 %f 0\n%f %f 0\n0 %f 0\n%f %f 0\n%f %f 0\n", yaux1,yaux0,x,yaux0,yaux1,x,yaux0,x,yaux0);
	}
	fclose(op);
}
void cone (float r,float h,float sl,char *ficheiro) {

	FILE *op = fopen(ficheiro, "w+");
	// base
	int i;
	for(i=1;i<=sl;i++){
		//base
		fprintf(op,"0 0 0\n%f 0 %f\n%f 0 %f\n",r*cos(i*2*M_PI/sl),r*sin(i*2*M_PI/sl), r*cos((i-1)*2*M_PI/sl), r*sin((i-1)*2*M_PI/sl));
		//faces
		fprintf(op,"%f 0 %f\n0 %f 0\n%f 0 %f\n",r*cos(i*2*M_PI/sl),r*sin(i*2*M_PI/sl),h,r*cos((i-1)*2*M_PI/sl),r*sin((i-1)*2*M_PI/sl));
	}
	fclose(op);
}

int main(int argc, char **argv) {
	char* string;
	int op;
	if(argc<2){
		printf("Input inválido.\n");
		exit(-1);
	}
	if(strcmp(argv[1],"plane")==0){
		//perguntar ao stor, dar as dimensões como argumento ou não
		plane(atof(argv[2]), atof(argv[3]), argv[4]);
	}
	else{
		if(strcmp(argv[1],"box")==0){
			box(atof(argv[2]), atof(argv[3]), atof(argv[4]), atoi(argv[5]), argv[6]);
		}
		else {
			if(strcmp(argv[1],"cone")==0){
				cone(atoi(argv[2]), atoi(argv[3]), atoi(argv[4]), argv[5]);
			}
		}

	}
	return 1;
}
