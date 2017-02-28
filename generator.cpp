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

void box(float x, float y, float z, int stacks, int slices, char* ficheiro){
	int r,st,sl;
	float yaux0,yaux1,zaux0,zaux1;
	//altura das stacks
	float yaux=y/stacks;
	//largura das slices
	float zaux=z/slices;
	FILE *op = fopen(ficheiro, "w+");
	for(sl=1;sl<=slices;sl++){
		zaux0=z-(sl-1)*zaux;
		zaux1=z-sl*zaux;
		//face de baixo
		fprintf(op,"0 0 %f\n0 0 %f\n%f 0 %f\n%f 0 %f\n%f 0 %f\n0 0 %f\n",zaux0, zaux1,x, zaux1, x, zaux1,x,zaux0,zaux0);
		//face de cima
		fprintf(op,"%f %f %f\n%f %f %f\n0 %f %f\n0 %f %f\n0 %f %f\n%f %f %f\n", x,y,zaux0,x,y,zaux1,y,zaux1,y,zaux1,y,zaux0,x,y,zaux0);
	}
	for(st=1;st<=stacks;st++){
		yaux0=y-(st-1)*yaux;
		yaux1=y-st*yaux;
		for(sl=1;sl<=slices;sl++){
			zaux0=z-(sl-1)*zaux;
			zaux1=z-sl*zaux;
			//face da direita
			fprintf(op,"%f %f %f\n%f %f %f\n%f %f %f\n%f %f %f\n%f %f %f\n%f %f %f\n", x,yaux1,zaux1, x, yaux0,zaux1, x,yaux1, zaux0,x,yaux1, zaux0,x,yaux0,zaux1,x,yaux0,zaux0);
			//face da esquerda
			fprintf(op,"0 %f %f\n0 %f %f\n0 %f %f\n0 %f %f\n0 %f %f\n0 %f %f\n", yaux0,zaux1,yaux1,zaux1,yaux1,zaux0,yaux1,zaux0,yaux0,zaux0,yaux0,zaux1);
		}
	}
	for(st=1;st<=stacks;st++){
		yaux0=y-(st-1)*yaux;
		yaux1=y-st*yaux;
		//face da frente
		fprintf(op,"%f %f %f\n0 %f %f\n0 %f %f\n0 %f %f\n%f %f %f\n%f %f %f\n", x,yaux0,z,yaux0,z,yaux1,z,yaux1,z,x,yaux1,z,x,yaux0,z);
		//face de traz
		fprintf(op,"0 %f 0\n0 %f 0\n%f %f 0\n0 %f 0\n%f %f 0\n%f %f 0\n", yaux1,yaux0,x,yaux0,yaux1,x,yaux0,x,yaux1);
	}
	fclose(op);
}
void cone (float r,float h,float sl,float st,char *ficheiro) {

	FILE *op = fopen(ficheiro, "w+");
	// base
	int i,j;
	float y;
	for(i=1;i<=sl;i++){
		//faces
		fprintf(op,"%f 0 %f\n0 %f 0\n%f 0 %f\n",r*cos(i*2*M_PI/sl),r*sin(i*2*M_PI/sl),h,r*cos((i-1)*2*M_PI/sl),r*sin((i-1)*2*M_PI/sl));
	}
	for (j=0;j<st;j++){ 
		y = ((h-((h/st)*j))*r)/h;
		for(i = 1; i <= sl; i++){
	
			fprintf(op,"0 %f 0\n",(h/st)*j);
			fprintf(op,"%f %f %f\n", y*cos(i*2*M_PI/sl),(h/st)*j ,y*sin(i*2*M_PI/sl));
			fprintf(op,"%f %f %f\n", y*cos((i-1)*2*M_PI/sl),(h/st)*j,y*sin((i-1)*2*M_PI/sl));
		}
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
			box(atof(argv[2]), atof(argv[3]), atof(argv[4]), atoi(argv[5]), atoi(argv[6]), argv[7]);
		}
		else {
			if(strcmp(argv[1],"cone")==0){
				cone(atoi(argv[2]), atoi(argv[3]), atoi(argv[4]), atoi(argv[5]), argv[6]);
			}
		}

	}
	return 1;
}
