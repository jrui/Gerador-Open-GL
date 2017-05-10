#ifndef TRANSFORMACAO_H
#define TRANSFORMACAO_H
;

class Transformacao {
	public:
		virtual void transformar() = 0;
};

class Light: public Transformacao{
	public:
		GLfloat pos[4];
		char* type;
		Light(float xx, float yy, float zz, char* ty){
			pos[0]=xx;
			pos[1]=yy;
			pos[2]=zz;
			pos[3]=0.0;
			type=strdup(ty);
		}
		virtual void transformar(){
			/*GLfloat amb[4] = {0.2, 0.2, 0.2, 1.0};
			GLfloat diff[4] = {1.0, 1.0, 1.0, 1.0};
			glLightfv(GL_LIGHT0, GL_AMBIENT, amb);
			glLightfv(GL_LIGHT0, GL_DIFFUSE, diff);
			glLightfv(GL_LIGHT0, GL_POSITION, pos);*/
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
		Translate(float tt, std::vector<float> pp) {
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
		Model(std::vector< float> v, std::vector<float> n, std::vector<float> tex, GLuint t){
			texID = t;
			vc = v;
			normal = n;
			text = tex;
		}

		Model(std::vector<float> v, std::vector<float> n, std::vector<float> t, float dr, float dg, float db, float mr, float mg, float mb){
			text = t;
			texID = -1;
			diff[0]=dr;
			diff[1]=dg;
			diff[2]=db;
			diff[3]=1;
			mat[0]=mr;
			mat[1]=mg;
			mat[2]=mb;
			mat[3]=1;
			vc = v;
			normal = n;
			printf("%f %f %f\n", mat[0], mat[1], mat[2]);
		}
		GLuint buffers[3];
		std::vector <float> vc, normal, text;
		GLfloat diff[4], mat[4];
		GLuint texID;
		virtual void transformar(){
			glBindTexture(GL_TEXTURE_2D,texID);
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
			if(texID==-1){
				glBindTexture(GL_TEXTURE_2D,0);
				GLfloat reset[4] = {0,0,0,0};
				if(diff[0] || diff[1] || diff[2]) glMaterialfv(GL_FRONT, GL_DIFFUSE, diff);
				if(mat[0] || mat[1] || mat[2]) glMaterialfv(GL_FRONT, GL_EMISSION, mat);
				glMaterialf(GL_FRONT,GL_SHININESS,127); 
				if(diff[0] || diff[1] || diff[2]) glMaterialfv(GL_FRONT, GL_DIFFUSE, reset);
				if(mat[0] || mat[1] || mat[2]) glMaterialfv(GL_FRONT, GL_EMISSION, reset);
			}
			else{
				float white[4] = { 1,1,1,1 };
				glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, white);
				glMaterialfv(GL_FRONT, GL_EMISSION, white);
			}
			glDrawArrays(GL_TRIANGLES, 0, vc.size());
			if(texID!=-1) glBindTexture(GL_TEXTURE_2D,0);
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
