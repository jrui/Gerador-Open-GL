#ifndef TRANSFORMACAO_H
#define TRANSFORMACAO_H
;

class Transformacao {
	public:
		float x,y,z;
		Transformacao(){}
		Transformacao(float xx,float yy,float zz){ x=xx; y=yy; z=zz;}
		virtual void transformar() = 0;
};

class PopMatrix: public Transformacao {
	public:
		virtual void transformar(){
			glPopMatrix();
		}
};

class PushMatrix: public Transformacao {
	public:
		virtual void transformar(){
			glPushMatrix();
		}
};

class Rotate: public Transformacao {
	public:
		float angle;
		Rotate(float ang, float xx, float yy, float zz): angle(ang), Transformacao(xx,yy,zz) {}
		virtual void transformar(){
			glRotatef(angle,x,y,z);
		}
};

class Translate: public Transformacao {
	public:
		Translate(float x,float y,float z) : Transformacao(x,y,z) {}
		virtual void transformar(){
			glTranslatef(x, y, z);
		}
};

class Scale: public Transformacao {
	public:
		Scale(float x,float y,float z) : Transformacao(x,y,z) {}
		virtual void transformar(){
			glScalef(x, y, z);
		}
};

class Model: public Transformacao {
	public:
		Model(std::vector<float> v): vc(v) {}
		std::vector<float> vc;
		virtual void transformar(){
			glBegin(GL_TRIANGLES);
			for(int i = 0; i < vc.size(); i+=3) {
				glVertex3f(vc[i],vc[i+1],vc[i+2]);
			}
			glEnd();
		}
};

class Color: public Transformacao {
	public:
		Color(float r, float g, float b) : Transformacao(r,g,b) {}
		virtual void transformar(){
			glColor3ub(x, y, z);
		}
};


#endif
