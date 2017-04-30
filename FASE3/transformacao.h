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
		float x,y,z, angle;
		Rotate(float ang, float xx, float yy, float zz){
		 angle = ang;
		 x = xx;
		 y = yy;
		 z = zz;
		}
		virtual void transformar(){
			glRotatef(angle,x,y,z);
		}
};

class Translate: public Transformacao {
	public:
		float x,y,z;
		Translate(float xx,float yy,float zz){
			x = xx;
			y = yy;
			z = zz;
		}
		virtual void transformar(){
			glTranslatef(x, y, z);
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