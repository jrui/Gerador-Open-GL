#ifndef TRANSFORMACAO_H
#define TRANSFORMACAO_H
;

class Transformacao {
	public:
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
		float x, y, z, angle;
		float time;
		bool usetime;
		Rotate(float ang, float xx, float yy, float zz){
		 angle = ang;
		 x = xx;
		 y = yy;
		 z = zz;
		 usetime = false;
		}
		Rotate(float tt, float xx, float yy, float zz, bool ut) {
			time = tt;
			x = xx;
			y = yy;
			z = zz;
			usetime = true;
		}
		virtual void transformar() {
			if(usetime)
				angle = fmod((glutGet(GLUT_ELAPSED_TIME) * 360.0f) / (time * 1000.0f), 360.0f);
			glRotatef(angle,x,y,z);
			glutPostRedisplay();
		}
};

class Translate: public Transformacao {
	public:
		float x,y,z;
		std::vector<float> points;
		float time;
		bool usetime;
		Translate(float xx,float yy,float zz) {
			x = xx;
			y = yy;
			z = zz;
			usetime = false;
		}
		Translate(float tt, std::vector<float> pp) {
			time = tt;
			points = pp;
			usetime = true;
		}
		virtual void transformar(){
			if(usetime) {
				// -------------------------------------------------
				// Tratar da transformacao com as curvas em "points"
				// -------------------------------------------------
			}
			else glTranslatef(x, y, z);
		}
};

class Scale: public Transformacao {
	public:
		float x,y,z;
		Scale(float xx,float yy,float zz){
			x = xx;
			y = yy;
			z = zz;
		}
		virtual void transformar(){
			glScalef(x, y, z);
		}
};

class Model: public Transformacao {
	public:
		Model(std::vector<float> v){
			vc = v;
		}
		std::vector<float> vc;
		virtual void transformar(){
			glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vc.size(), &vc[0], GL_STATIC_DRAW);
			glVertexPointer(3,GL_FLOAT,0,0);
			glDrawArrays(GL_TRIANGLES, 0, vc.size());
		}
};

class Color: public Transformacao {
	public:
		float r,g,b;
		Color(float red, float green, float blue){
			r = red;
			g = green;
			b = blue;
		}
		virtual void transformar(){
			glColor3ub(r, g, b);
		}
};

#endif
