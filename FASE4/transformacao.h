#ifndef TRANSFORMACAO_H
#define TRANSFORMACAO_H
;
#include <math.h>

class Transformacao {
	public:
		virtual void transformar() = 0;
};

class Light: public Transformacao{
	public:
		GLfloat pos[4], diff[4], amb[4], spec[4], dir[4];
		float exp, cut;
		GLenum light;
		char* type;
		Light(GLfloat* p,GLfloat* d, GLfloat *a, GLfloat *s, GLfloat *di, float e, float c, int luz, char* ty){
			switch(luz%8){
				case 0:
					light = GL_LIGHT0;
					break;
				case 01:
					light = GL_LIGHT1;
					break;
				case 2:
					light = GL_LIGHT2;
					break;
				case 3:
					light = GL_LIGHT3;
					break;
				case 4:
					light = GL_LIGHT4;
					break;
				case 5:
					light = GL_LIGHT5;
					break;	
				case 6:
					light = GL_LIGHT6;
					break;
				case 7:
					light = GL_LIGHT7;
					break;
			}
			glEnable(light);
			pos[0]=p[0];
			pos[1]=p[1];
			pos[2]=p[2];
			if(!strcmp(ty, "DIRECIONAL")) pos[3]=0;
			else{ pos[3] = 1; }
			diff[0] = d[0];
			diff[1] = d[1];
			diff[2] = d[2];
			diff[3] = 1;
			dir[0] = di[0];
			dir[1] = di[1];
			dir[2] = di[2];
			dir[3] = 1;
			amb[0] = a[0];
			amb[1] = a[1];
			amb[2] = a[2];
			amb[3] = 1;
			spec[0] = s[0];
			spec[1] = s[1];
			spec[2] = s[2];
			spec[3] = 1;
			exp = e;
			cut = c;
			type=strdup(ty);
		}
		virtual void transformar(){
			GLfloat resAmb[4] = {0, 0, 0, 1.0};
			GLfloat resDiff[4] = {1.0, 1.0, 1.0, 1.0};
			if(amb[0] || amb[1] || amb[2]) glLightfv(light, GL_AMBIENT, amb);
			else{ glLightfv(light, GL_AMBIENT, resAmb); }
			if(diff[0] || diff[1] || diff[2]) glLightfv(light, GL_DIFFUSE, diff);
			else{ glLightfv(light, GL_DIFFUSE, resDiff); }
			if( spec[0] || spec[1] || spec[2]) glLightfv(light, GL_SPECULAR, spec);
			else{ glLightfv(light, GL_SPECULAR, resDiff); }
			if(!strcmp(type, "SPOTLIGHT")){
				glLightfv(light, GL_POSITION, pos);
				glLightfv(light, GL_SPOT_DIRECTION, dir);
				glLightf(light, GL_SPOT_EXPONENT, exp);
				glLightf(light, GL_SPOT_CUTOFF, cut);
			}
			else{
				glLightfv(light, GL_POSITION, pos);
			}
		}
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
		float **p;
		float t;
		Translate(float xx,float yy,float zz) {
			x = xx;
			y = yy;
			z = zz;
			t = 0.0f;
			usetime = false;
		}
		Translate(float tt, std::vector<float>& pp) {
			time = tt;
			points = pp;
			t = 0.0f;
			usetime = true;
			p = (float**) malloc(sizeof(float) * points.size() * 3);
			for(int i = 0; i < points.size() / 3; i++) {
				p[i] = (float*) malloc(sizeof(float) * 3);
				for(int j = 0; j < 3; j++)
					p[i][j] = points.at(i*3 + j);
			}
		}
		virtual void transformar(){
			if(usetime) {
				float res[3];
				float der[3];
				t += 0.001f / time * 6;
				glBegin(GL_LINE_LOOP);
				for(int i = 0; i < 1000 / 3; i++ ){
					getGlobalCatmullRomPoint((float)i/1000 + t,res,der);
					glVertex3f(res[0], res[1], res[2]);
				}
				glEnd();
				glTranslatef(res[0], res[1], res[2]);
				glutPostRedisplay();
			}
			else glTranslatef(x, y, z);
		}
		void buildRotMatrix(float *x, float *y, float *z, float *m) {
			m[0] = x[0];
			m[1] = x[1];
			m[2] = x[2];
			m[3] = 0;
			m[4] = y[0];
			m[5] = y[1];
			m[6] = y[2];
			m[7] = 0;
			m[8] = z[0];
			m[9] = z[1];
			m[10] = z[2];
			m[11] = 0;
			m[12] = 0;
			m[13] = 0;
			m[14] = 0;
			m[15] = 1;
		}
		void cross(float *a, float *b, float *res) {
			res[0] = a[1]*b[2] - a[2]*b[1];
			res[1] = a[2]*b[0] - a[0]*b[2];
			res[2] = a[0]*b[1] - a[1]*b[0];
		}
		void normalize(float *a) {
			float l = sqrt(a[0]*a[0] + a[1] * a[1] + a[2] * a[2]);
			a[0] = a[0] / l;
			a[1] = a[1] / l;
			a[2] = a[2] / l;
		}
		float length(float *v) {
			float res = sqrt(v[0]*v[0] + v[1]*v[1] + v[2]*v[2]);
			return res;
		}
		void multMatrixVector(float *m, float *v, float *res) {
			for (int j = 0; j < 4; ++j) {
				res[j] = 0;
				for (int k = 0; k < 4; ++k) res[j] += v[k] * m[j * 4 + k];
			}
		}
		void getCatmullRomPoint(float t, float *p0, float *p1, float *p2, float *p3, float *res, float *deriv) {
			float px[4], py[4], pz[4], ax[4], ay[4], az[4];
			px[0] = p0[0]; px[1] = p1[0]; px[2] = p2[0]; px[3] = p3[0];
			py[0] = p0[1]; py[1] = p1[1]; py[2] = p2[1]; py[3] = p3[1];
			pz[0] = p0[2]; pz[1] = p1[2]; pz[2] = p2[2]; pz[3] = p3[2];
			float tGrande[4] = {t*t*t, t*t, t, 1};
			float tDer[4] = {3*t*t, 2*t, 1, 0};
			// catmull-rom matrix
			float m[4][4] = {	{-0.5f,  1.5f, -1.5f,  0.5f},
												{ 1.0f, -2.5f,  2.0f, -0.5f},
												{-0.5f,  0.0f,  0.5f,  0.0f},
												{ 0.0f,  1.0f,  0.0f,  0.0f} };
			// reset res and deriv
			res[0] = 0.0; res[1] = 0.0; res[2] = 0.0;
			deriv[0] = 0.0; deriv[1] = 0.0; deriv[2] = 0.0;
			// Compute A = M * P
			multMatrixVector((float*) m,px,ax);
			multMatrixVector((float*) m,py,ay);
			multMatrixVector((float*) m,pz,az);
			// Compute point res = T *A
			res[0]= tGrande[0] * ax[0] + tGrande[1] * ax[1] + tGrande[2] * ax[2] + tGrande[3] * ax[3];
			res[1]= tGrande[0] * ay[0] + tGrande[1] * ay[1] + tGrande[2] * ay[2] + tGrande[3] * ay[3];
			res[2]= tGrande[0] * az[0] + tGrande[1] * az[1] + tGrande[2] * az[2] + tGrande[3] * az[3];
			// compute deriv = T' * A
			deriv[0]= tDer[0] * ax[0] + tDer[1] * ax[1] + tDer[2] * ax[2] + tDer[3] * ax[3];
			deriv[1]= tDer[0] * ay[0] + tDer[1] * ay[1] + tDer[2] * ay[2] + tDer[3] * ay[3];
			deriv[2]= tDer[0] * az[0] + tDer[1] * az[1] + tDer[2] * az[2] + tDer[3] * az[3];
		}
		void getGlobalCatmullRomPoint(float gt, float *res, float *deriv) {
			float t = gt * points.size(); // this is the real global t
			int index = floor(t);  // which segment
			t = t - index; // where within  the segment
			// indices store the points
			int indices[4];
			indices[0] = (index + (points.size()/3)-1)%(points.size()/3);
			indices[1] = (indices[0]+1)%(points.size()/3);
			indices[2] = (indices[1]+1)%(points.size()/3);
			indices[3] = (indices[2]+1)%(points.size()/3);

			getCatmullRomPoint(t, p[indices[0]], p[indices[1]], p[indices[2]], p[indices[3]], res, deriv);
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
		Model(std::vector< float>& v, std::vector<float>& n, std::vector<float>& tex, GLuint t, int num, float rMi, float rMa, float xMi, float xMa, float yMi, float yMa, float zMi, float zMa){
			texID = t;
			vc = v;
			normal = n;
			text = tex;
			numero = num;
			rMin = rMi;
			rMax = rMa;
			xMin = xMi;
			xMax = xMa;
			yMin = yMi;
			yMax = yMa;
			zMin = zMi;
			zMax = zMa;
		}
		GLuint buffers[3];
		std::vector <float> vc, normal, text;
		GLuint texID;
		int numero;
		float rMin, rMax, xMin, xMax, yMin, yMax, zMin, zMax;
		virtual void transformar(){
			GLfloat resAmb[4] = {0.2, 0.2, 0.2, 1.0}, resDiff[4] = {0.8, 0.8, 0.8, 1.0}, resSpec[4] = {0.0, 0.0, 0.0, 1.0};;
			srand(123456789);
			glGenBuffers(3, buffers);
			glBindBuffer(GL_ARRAY_BUFFER,buffers[0]);
			glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vc.size(), &vc[0], GL_STATIC_DRAW);
			glVertexPointer(3,GL_FLOAT,0,0);
			glBindBuffer(GL_ARRAY_BUFFER,buffers[1]);
			glBufferData(GL_ARRAY_BUFFER, sizeof(float) * normal.size(), &normal[0],GL_STATIC_DRAW);
			glNormalPointer(GL_FLOAT, 0, 0);
			glBindBuffer(GL_ARRAY_BUFFER,buffers[2]);
			glBufferData(GL_ARRAY_BUFFER, sizeof(float) * text.size(), &text[0], GL_STATIC_DRAW);
			glTexCoordPointer(2,GL_FLOAT,0,0);
			if(texID!=-1) glBindTexture(GL_TEXTURE_2D,texID);
			if(numero!=0){
				for(int i = 0; i < numero; i++){
					glPushMatrix();
					float tx, tz,alpha;
						tx = (float) rand()/(float)RAND_MAX*(rMax - rMin) + rMin; 
						tz = (float) rand()/(float)RAND_MAX*(rMax - rMin) + rMin;
						alpha = (float) rand() / (float)RAND_MAX * 2*M_PI;
			
					float sx = (float) rand()/(float)RAND_MAX*(xMax-xMin)+xMin; 
					float sy = (float) rand()/(float)RAND_MAX*(yMax-yMin)+yMin;
					float sz = (float) rand()/(float)RAND_MAX*(zMax-zMin)+zMin;
					glTranslatef(0.75*tx*cos(alpha),0,1.25*tz*sin(alpha));
					glScalef(sy, sy, sy);
					glDrawArrays(GL_TRIANGLES, 0, vc.size());
					glPopMatrix();
				}
			}
			else{

			 glDrawArrays(GL_TRIANGLES, 0, vc.size()); }
			if(texID!=-1){ 
				glBindTexture(GL_TEXTURE_2D,0);
				//glDeleteTextures(1, &texID);
			}
			glDeleteBuffers(3, buffers);
			glMaterialfv(GL_FRONT, GL_EMISSION, resSpec);
			glMaterialfv(GL_FRONT, GL_SPECULAR, resSpec);
			glMaterialfv(GL_FRONT, GL_AMBIENT, resAmb);
			glMaterialfv(GL_FRONT, GL_DIFFUSE, resDiff);
			glMaterialf(GL_FRONT, GL_SHININESS, 0);

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




class Material: public Transformacao {
	public:
		float diff[4], amb[4], spec[4], emi[4];
		GLfloat shininess;
		Material(float drr, float dgg, float dbb, float arr, float agg, float abb, float srr, float sgg, float sbb, float err, float egg, float ebb, GLfloat shi){
			diff[0] = drr;
			diff[1] = dgg;
			diff[2] = dbb;
			diff[3] = 1;
			amb[0] = arr;
			amb[1] = agg;
			amb[2] = abb;
			amb[3] = 1;
			spec[0] = srr;
			spec[1] = sgg;
			spec[2] = sbb;
			spec[3] = 1;
			emi[0] = err;
			emi[1] = egg;
			emi[2] = ebb;
			emi[3] = 1;
			shininess = shi;
		}
		virtual void transformar(){
			if(diff[0] || diff[1] || diff[2]) glMaterialfv(GL_FRONT, GL_DIFFUSE, diff);
			if(amb[0] || amb[1] || amb[2]) glMaterialfv(GL_FRONT, GL_AMBIENT, amb);
			if(spec[0] || spec[1] || spec[2]) glMaterialfv(GL_FRONT, GL_SPECULAR, spec);
			if(emi[0] || emi[1] || emi[2]) glMaterialfv(GL_FRONT, GL_EMISSION, emi);
			if(shininess) glMaterialf(GL_FRONT, GL_SHININESS, shininess);
		}
};

#endif