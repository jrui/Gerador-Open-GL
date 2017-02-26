#include <math.h>
#include <stdio.h>
#include <unistd.h> /* chamadas ao sistema: defs e decls essenciais */
#include <fcntl.h>  /* O_RDONLY, O_WRONLY, O_CREAT, O_* */
#include <stdlib.h>
#include <string.h>

// opcao==0, funcao é utilizada na plane, opcao==1, funcao utilizada como auxiliar de outras, vai preencher em ficheiro os vértices do plane
void plane(float x, float z, char* ficheiro){
	char buff[128];
	int r;
	int op;
	op = open(ficheiro, O_CREAT | O_TRUNC | O_RDWR, 0777);
	if(op<1){
		printf("Erro ao abrir.\n");
		exit(-1);
	}
	r =snprintf(buff,127,"0 0 0\n0 0 %f\n%f 0 %f\n%f 0 %f\n%f 0 0\n0 0 0\n", z, x, z, x, z,x);
	write(op,buff,r);
	close(op);
}

void box(float x, float y, float z, char* ficheiro){
	char buff[128];
	int r;
	int op = open(ficheiro, O_CREAT | O_TRUNC | O_RDWR, 0777);
	if(op<1){
		printf("Erro ao abrir.\n");
		exit(-1);
	}
	//face de baixo
	r =snprintf(buff,127,"0 0 0\n%f 0 %f\n0 0 %f\n%f 0 %f\n0 0 0\n%f 0 0\n", x, z, z, x, z,x);
	write(op,buff,r);
	//face da direita
	r =snprintf(buff,127,"%f 0 0\n%f %f 0\n%f 0 %f\n%f 0 %f\n%f %f 0\n%f %f %f\n", x, x, y, x, z,x,z,x,y,x,y,z);
	write(op,buff,r);
	//face de cima
	r =snprintf(buff,127,"%f %f %f\n%f %f 0\n0 %f 0\n0 %f 0\n0 %f %f\n%f %f %f\n", x,y,z,x,y,y,y,y,z,x,y,z);
	write(op,buff,r);
	//face da frente
	r =snprintf(buff,127,"%f %f %f\n0 %f %f\n0 0 %f\n0 0 %f\n%f 0 %f\n%f %f %f\n", x,y,z,y,z,z,z,x,z,x,y,z);
	write(op,buff,r);
	//face da esquerda
	r =snprintf(buff,127,"0 %f 0\n0 0 0\n0 %f %f\n0 0 0\n0 0 %f\n0 %f %f\n", y,y,z,z,y,z);
	write(op,buff,r);
	//face de traz
	r =snprintf(buff,127,"0 0 0\n0 %f 0\n%f %f 0\n0 0 0\n%f %f 0\n%f 0 0\n", y,x,y,x,y,x);
	write(op,buff,r);

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
			box(atoi(argv[2]), atoi(argv[3]), atoi(argv[4]), argv[5]);
		}
	}
	return 1;
}
