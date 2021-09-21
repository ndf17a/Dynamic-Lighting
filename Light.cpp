#include <windows.h>
#include <gl/Gl.h>
#include <gl/glu.h>
#include "glut.h"
#include <iostream>
#include <math.h>
using namespace std;

/*

g++ -o light.exe -Wall Light.cpp glut32.lib -lopengl32 -lglu32

*/


class Point3
{
    public:
        float x, y, z;

    Point3() {};
    Point3(float x, float y, float z) : x(x), y(y), z(z) {}

    void set(Point3 p) { x = p.x; y = p.y; z = p.z; }
};

class Vector3 {

    public:

        // -------------------- Attributes -------------------- //

        // Components of the vector
        float x, y, z;

        

        // -------------------- Methods -------------------- //

        // Constructor
        Vector3(float x = 0, float y = 0, float z = 0) : x(x), y(y), z(z) {}

        // Constructor
        Vector3(const Vector3& vector) : x(vector.x), y(vector.y), z(vector.z) {}
        //return the s vector from two points, shape, sun/eye
        Vector3(Point3 a, Point3 b) : x(b.x - a.x), y(b.y - a.y), z(b.z - a.z) {}
        // Constructor
        ~Vector3() {}


        //set vector with floats
        void set(float xx, float yy, float zz) { x = xx; y = yy; z = zz; }

        //set vector with a point3
        void set(Point3 p) { x = p.x; y = p.y; z = p.z; }

        //set vector with a vector
        void set(Vector3 v) { x = v.x; y = v.y; z = v.z; }

        //return the opposite vector of the given vector
        Vector3 negative() { return Vector3(-1*x, -1*y, -1*z); }

        // = operator
        Vector3& operator=(const Vector3& vector) {
            if (&vector != this) {
                x = vector.x;
                y = vector.y;
                z = vector.z;
            }
            return *this;
        }

        // + operator
        Vector3 operator+(const Vector3 &v) const {
            return Vector3(x + v.x, y + v.y, z + v.z);
        }

        // += operator
        Vector3& operator+=(const Vector3 &v) {
            x += v.x; y += v.y; z += v.z;
            return *this;
        }

        // - operator
        Vector3 operator-(const Vector3 &v) const {
            return Vector3(x - v.x, y - v.y, z - v.z);
        }

        // -= operator
        Vector3& operator-=(const Vector3 &v) {
            x -= v.x; y -= v.y; z -= v.z;
            return *this;
        }

        // == operator
        bool operator==(const Vector3 &v) const {
            return x == v.x && y == v.y && z == v.z;
        }

        // != operator
        bool operator!=(const Vector3 &v) const {
          return !( *this == v );
        }

        // * operator
        Vector3 operator*(float f) const {
            return Vector3(f*x, f*y, f*z);
        }

        // *= operator
        Vector3 &operator*=(float f) {
            x *= f; y *= f; z *= f;
            return *this;
        }

        // / operator
        Vector3 operator/(float f) const {
            float inv = 1.f / f;
            return Vector3(x * inv, y * inv, z * inv);
        }

        // /= operator
        Vector3 &operator/=(float f) {
            float inv = 1.f / f;
            x *= inv; y *= inv; z *= inv;
            return *this;
        }

        // - operator
        Vector3 operator-() const {
            return Vector3(-x, -y, -z);
        }


        // Cross product operator
        Vector3 cross(const Vector3 &v) const{
            return Vector3(y * v.z - z * v.y, z * v.x - x * v.z, x * v.y - y * v.x);
        }

        // Dot product operator
        float dot(const Vector3 &v) const{
            return x * v.x + y * v.y + z * v.z;
        }

        // Normalize the vector and return it
        Vector3 normalize() {
            float l = magnitude();

            x /= l;
            y /= l;
            z /= l;
            return *this;
        }

        // Return the squared length of the vector
        float lengthSquared() const { return x*x + y*y + z*z; }

        // Return the length of the vector
        float magnitude() const { return sqrt(lengthSquared()); }

};

class Camera {
    public:
        Point3 eye, look;
        Vector3 u, v, n, up;
        double viewAngle, aspect, nearDist, farDist; // view volume shape
        void setModelviewMatrix(); //tell openGL where the camera is

    
        Camera() {};
        void set(Point3 eye, Point3 look, Vector3 up);
        void set(float x1, float y1, float z1, float x2, float y2, float z2, float x3, float y3, float z3);
        void roll(float angle);
        void pitch(float angle);
        void yaw(float angle);
        void slide(float delU, float delV, float delN);
        void setShape(float vAng, float asp, float nearD, float farD);
        void getShapes(float &vAng, float &asp, float &nearD, float &farD);
};

void Camera::setModelviewMatrix(void)
{
    //load modelview matrix with existing camera values
    float m[16];
    Vector3 eVec(eye.x, eye.y, eye.z); //constructor of a vector version of eye

    m[0] = u.x; m[4] = u.y; m[8]  = u.z; m[12] = -eVec.dot(u);
    m[1] = v.x; m[5] = v.y; m[9]  = v.z; m[13] = -eVec.dot(v);
    m[2] = n.x; m[6] = n.y; m[10] = n.z; m[14] = -eVec.dot(n);
    m[3] = 0;   m[7] = 0;   m[11] = 0;   m[15] = 1.0;

    glMatrixMode(GL_MODELVIEW);
    glLoadMatrixf(m); //load openGL modelview matrix
}

void Camera::set(float x1, float y1, float z1, float x2, float y2, float z2, float x3, float y3, float z3)
{
    //set(Point3(x1,y1,z1), Point3(x2,y2,z2), Vector3(x3,y3,z3));

    Point3  Eye  =  Point3(x1,y1,z1);
    Point3  Look =  Point3(x2,y2,z2);
    Vector3 Up   =  Vector3(x3,y3,z3);

    //create a modelview matrix and send it to opengl
    eye.set(Eye); // store the given eye position
    
    n.set(eye.x - Look.x, eye.y - Look.y, eye.z - Look.z); //make n

    u.set(Up.cross(n)); //make u = up cross n (cross product)
    n.normalize(); 
    u.normalize(); //make them unit length
    v.set(n.cross(u));


    setModelviewMatrix(); //tell openGL

}

void Camera::setShape(float vAng, float asp, float nearD, float farD) 
{

    viewAngle = vAng;
    aspect = asp;
    nearDist = nearD;
    farDist = farD; 

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(vAng, asp, nearD, farD);

}
    

void Camera::slide(float delU, float delV, float delN)
{
    eye.x  += delU * u.x + delV * v.x + delN * n.x;
    eye.y  += delU * u.y + delV * v.y + delN * n.y;
    eye.z  += delU * u.z + delV * v.z + delN * n.z;

    look.x  += delU * u.x + delV * v.x + delN * n.x;
    look.y  += delU * u.y + delV * v.y + delN * n.y;
    look.z  += delU * u.z + delV * v.z + delN * n.z;

    setModelviewMatrix();
}

void Camera::roll(float angle)
{
    //roll the camera through angle degrees
    float pinum = 3.14159265/180 * angle;

    float cs = cos(pinum);
    float sn = sin(pinum);

    Vector3 t( u );
    u.set(cs*t.x - sn*v.x, cs*t.y - sn*v.y, cs*t.z - sn*v.z);
    v.set(sn*t.x + cs*v.x, sn*t.y + cs*v.y, sn*t.z + cs*v.z);

    setModelviewMatrix();
}

void Camera::yaw(float angle)
{
    //roll the camera through angle degrees
    float pinum = 3.14159265/180 * angle;

    float cs = cos(pinum);
    float sn = sin(pinum);

    Vector3 t( n );
    n.set(cs*t.x - sn*u.x, cs*t.y - sn*u.y, cs*t.z - sn*u.z);
    u.set(sn*t.x + cs*u.x, sn*t.y + cs*u.y, sn*t.z + cs*u.z);

    setModelviewMatrix();
}

void Camera::pitch(float angle)
{
    //roll the camera through angle degrees
    float pinum = 3.14159265/180 * angle;

    float cs = cos(pinum);
    float sn = sin(pinum);

    Vector3 t(v);
    v.set(cs*t.x - sn*n.x, cs*t.y - sn*n.y, cs*t.z - sn*n.z);
    n.set(sn*t.x + cs*n.x, sn*t.y + cs*n.y, sn*t.z + cs*n.z);

    setModelviewMatrix();
}


Camera cam;
//These are constants that I pick I believe
//double Is = 50;
//double Pd = 50;
//double Ps = 50;
double SX = .25, SY = .25, SZ = .25;
double RX = 0.1, RY = 0, RZ = 0;
int a = 0;
int spin = 0;


//Is is the intensity of the light source
//Pd is the diffuse reflection coefficent

//lambert pg 384 ------------------------------------ 
//Returns Id
double lambert(Vector3 s, Vector3 m) {
    float top = s.dot(m);
    float bottom = s.magnitude() * m.magnitude();

    float n = top/bottom;
    float lambert = max((float)0, n);

    //Is and Pd are global
    //return Is * Pd * lambert;;
    return lambert;;
}

//Is is the intensity of the light source
//Ps is the specular reflection coefficent

//phong pg 387--------------------------------------
//Adjusted Specular term
//returns Isp
double phong(Vector3 v, Vector3 s, Vector3 m, int f) {
        
    Vector3 h = s + v;

    //Is this correctly translated? pg 387
    Vector3 left = (h/h.magnitude());
    Vector3 right = (m/m.magnitude());
    float frac = left.dot(right);

    //pick an f value 1-200
    float n = pow(frac,f);
    float phong = max((float)0, n);

    //Is and Ps are global
    //return Is * Ps * phong;
    return phong;
}

//-s + 2[(2dotm)/|m|^2]*m finding vector r... mirrior reflection direction
Vector3 getR(Vector3 s, Vector3 m)
{
    double top = s.dot(m);
    double bottom = pow(m.magnitude(),2);
    double fraction = top/bottom;
 
    return s.negative() + (m*(2*fraction));

}

//vector from shape --> sun/eye
Vector3 getSV(Point3 shape, Point3 sun)
{
    return Vector3(shape, sun);
}



double light(Vector3 s, Vector3 m, Vector3 v, double Ia, double Pa, double Id, double Pd, double Is, double Ps, int f) {
    
    return (Ia * Pa) + (Id * Pd * lambert(s,m)) + ( Is * Ps * phong(v, s, m, f));

}

void axis(double length)
{
    glPushMatrix();
        glBegin(GL_LINES);
            //x is red
            glColor3d(1,0,0);
            glVertex3d(0, 0, 0);
            glVertex3d(length,0,0);

            //y is green
            glColor3d(.5,1,.5);
            glVertex3d(0, 0, 0);
            glVertex3d(0, length, 0);

            //z is blue
            glColor3d(0,0,1);
            glVertex3d(0, 0, 0);
            glVertex3d(0, 0, length);
        glEnd();

    glPopMatrix();

    
}


void drawNumbers()
{

    //
    glRasterPos3d(.75, 1.15, 0);
    glutBitmapCharacter(GLUT_BITMAP_8_BY_13, '1');

    //
    glRasterPos3d(-.75, 1.15, 0);
    glutBitmapCharacter(GLUT_BITMAP_8_BY_13, '2');

    //
    glRasterPos3d(.75, -1.15, 0);
    glutBitmapCharacter(GLUT_BITMAP_8_BY_13, '3');

    //
    glRasterPos3d(0, -.65, 1.25);
    glutBitmapCharacter(GLUT_BITMAP_8_BY_13, '4');

    //
    glRasterPos3d(0, .5, 1.25);
    glutBitmapCharacter(GLUT_BITMAP_8_BY_13, '5');

    //
    glRasterPos3d(0, .5, -1.25);
    glutBitmapCharacter(GLUT_BITMAP_8_BY_13, '6');

    //
    glRasterPos3d(0, -.65, -1.25);
    glutBitmapCharacter(GLUT_BITMAP_8_BY_13, '7');

    //
    glRasterPos3d(-1.20, .10, .75);
    glutBitmapCharacter(GLUT_BITMAP_8_BY_13, '8');

    //
    glRasterPos3d(-.75, -1.15, 0);
    glutBitmapCharacter(GLUT_BITMAP_8_BY_13, '9');
    
    //
    string str = "10";
    glRasterPos3d(1.20, .10, -.60);
    for (int i = 0; i < 2; i++)
        glutBitmapCharacter(GLUT_BITMAP_8_BY_13, (int)str[i]);

    //
    str = "11";
    glRasterPos3d(-1.20, .10, -.75);
    for (int i = 0; i < 2; i++)
        glutBitmapCharacter(GLUT_BITMAP_8_BY_13, (int)str[i]);
    
    str = "12";
    glRasterPos3d(1.20, .10, .60);
    for (int i = 0; i < 2; i++)
        glutBitmapCharacter(GLUT_BITMAP_8_BY_13, (int)str[i]);
    
}
  
void drawSecretShapes(void)
{

    glPushMatrix();
        glColor3d(.4,.2,.9);
        glTranslated(-1.5,-1,-1);
        glScaled(SX-.15,SY-.1,SZ-.1);
        glRotated(a, RX, RY, RZ);
        glutSolidCube(1);
        glColor3d(0,0,0);
        glutWireCube(1);
    glPopMatrix();

    glPushMatrix();
        glColor3d(.2,0,.2);
        glTranslated(-1.1,-1.3,-1.5);
        glScaled(SX-.15,SY-.1,SZ-.1);
        glRotated(a, RX, RY, RZ);
        glutSolidSphere(.60,10,50);
        glColor3d(0,0,0);
        glutWireSphere(.60,10,50);
    glPopMatrix();

    glPushMatrix();
        glColor3d(0,.56,.1);
        glTranslated(-2.7,-2.9,-2.6);
        glScaled(SX-.15,SY-.1,SZ-.1);
        glRotated(a, RX, RY, RZ);
        glutSolidCone(.30,1,10,10);
        glColor3d(0,0,0);
        glutWireCone(.30,1,10,10);
    glPopMatrix();

    glPushMatrix();
        glColor3d(.5,0,.5);
        glTranslated(1.5,1,1);
        glScaled(SX+.15,SY+.1,SZ+.1);
        glRotated(a, RX, RY, RZ);
        glutSolidCube(1);
        glColor3d(0,0,0);
        glutWireCube(1);
    glPopMatrix();

    glPushMatrix();
        glColor3d(.3,.5,.98);
        glTranslated(1.1,1.3,1.5);
        glScaled(SX+.15,SY+.1,SZ+.1);
        glRotated(a, RX, RY, RZ);
        glutSolidIcosahedron();
        glColor3d(0,0,0);
        glutWireIcosahedron();

    glPopMatrix();

    glPushMatrix();
        glColor3d(.1,.7,.2);
        glTranslated(1.5,2.2,1.4);
        glScaled(SX+.15,SY+.1,SZ+.1);
        glRotated(a, RX, RY, RZ);
        glutSolidTorus(.50,.50,5,5);
        glColor3d(0,0,0);
        glutWireTorus(.50,.50,5,5);

    glPopMatrix();
    
}
Point3 sunShine = Point3(15,20,10);
bool GS = false;

void display(void)
{

    glClear(GL_COLOR_BUFFER_BIT);
    //draw axis lines, x = red, y = green, z = blue
    glPushMatrix();
        axis(1);
    glPopMatrix();


    int f; 

    //ambience
    double Ia, Par, Pag, Pab;

    //diffuse
    double Id, Pdr, Pdg, Pdb;

    //source intensity
    double Is, Psr, Psg, Psb;
    
    if(GS)
    {
        //silver
        f = 51.2;

        Ia = .5;
        Par = .19225;
        Pag = .19225;
        Pab = .19225;

        Id = .5;
        Pdr = .50754;
        Pdg = .50754;
        Pdb = .50754;

        Is = 100;
        Psr = .508273;
        Psg = .508273;
        Psb = .508273;
    }
    else 
    {
        //brass
        f = 27.8974;

        Ia = .5;
        Par = .329412;
        Pag = .223529;
        Pab = .027451;

        Id = .5;
        Pdr = .780392;
        Pdg = .568627;
        Pdb = .113725;

        Is = 100;
        Psr = .992157;
        Psg = .941176;
        Psb = .807843;
    }


    //r g b values
    double Ir, Ig, Ib;
    //3 points for each triangle
    Point3 a, b, c, center;
    //calculate these for every vertex
    Vector3 s, m, v, r;

    //Start Triangles
    a = Point3(1,1,-1);
    b = Point3(1,-1,1);
    c = Point3(1,-1,-1);
    glBegin(GL_TRIANGLES);
        //calculate m once per triangle
        m = Vector3(a, c).cross(Vector3(b, c));

        //s and v per vertices
        s = Vector3(a, sunShine);
        v = Vector3(a, cam.eye);
        Ir = light(s,m,v,Ia,Par,Id,Pdr,Is,Psr,f);
        Ig = light(s,m,v,Ia,Pag,Id,Pdg,Is,Psg,f);
        Ib = light(s,m,v,Ia,Pab,Id,Pdb,Is,Psb,f);
        glColor3f(Ir,Ig,Ib);
        glVertex3d(a.x,a.y,a.z);

        s = Vector3(b, sunShine);
        v = Vector3(b, cam.eye);
        Ir = light(s,m,v,Ia,Par,Id,Pdr,Is,Psr,f);
        Ig = light(s,m,v,Ia,Pag,Id,Pdg,Is,Psg,f);
        Ib = light(s,m,v,Ia,Pab,Id,Pdb,Is,Psb,f);
        glColor3f(Ir,Ig,Ib);
        glVertex3d(b.x,b.y,b.z);

        s = Vector3(c, sunShine);
        v = Vector3(c, cam.eye);
        Ir = light(s,m,v,Ia,Par,Id,Pdr,Is,Psr,f);
        Ig = light(s,m,v,Ia,Pag,Id,Pdg,Is,Psg,f);
        Ib = light(s,m,v,Ia,Pab,Id,Pdb,Is,Psb,f);
        glColor3f(Ir,Ig,Ib);
        glVertex3d(c.x,c.y,c.z);
    glEnd();
    /*
    glBegin(GL_LINES); //draw plane normal
            center = Point3((a.x+b.x+c.x)/3.0, (a.y+b.y+c.y)/3.0, (a.z+b.z+c.z)/3.0);
            m = Vector3(a, b).cross(Vector3(b, c));
            glColor3f(.7,0,.7);
            glVertex3d(center.x,center.y,center.z);
            glVertex3d(m.x-2,m.y,m.z);
    glEnd();*/

    a = Point3(1,-1,1);
    b = Point3(-1,1,1);
    c = Point3(-1,-1,1);
    glBegin(GL_TRIANGLES);
        //calculate m once per triangle
        m = Vector3(a, c).cross(Vector3(b, c));

        //s and v per vertices
        s = Vector3(a, sunShine);
        v = Vector3(a, cam.eye);
        Ir = light(s,m,v,Ia,Par,Id,Pdr,Is,Psr,f);
        Ig = light(s,m,v,Ia,Pag,Id,Pdg,Is,Psg,f);
        Ib = light(s,m,v,Ia,Pab,Id,Pdb,Is,Psb,f);
        glColor3f(Ir,Ig,Ib);
        glVertex3d(a.x,a.y,a.z);

        s = Vector3(b, sunShine);
        v = Vector3(b, cam.eye);
        Ir = light(s,m,v,Ia,Par,Id,Pdr,Is,Psr,f);
        Ig = light(s,m,v,Ia,Pag,Id,Pdg,Is,Psg,f);
        Ib = light(s,m,v,Ia,Pab,Id,Pdb,Is,Psb,f);
        glColor3f(Ir,Ig,Ib);
        glVertex3d(b.x,b.y,b.z);

        s = Vector3(c, sunShine);
        v = Vector3(c, cam.eye);
        Ir = light(s,m,v,Ia,Par,Id,Pdr,Is,Psr,f);
        Ig = light(s,m,v,Ia,Pag,Id,Pdg,Is,Psg,f);
        Ib = light(s,m,v,Ia,Pab,Id,Pdb,Is,Psb,f);
        glColor3f(Ir,Ig,Ib);
        glVertex3d(c.x,c.y,c.z);
    glEnd();

    a = Point3(-1,1,1);
    b = Point3(1,1,-1);
    c = Point3(-1,1,-1);
    glBegin(GL_TRIANGLES);
        //calculate m once per triangle
        m = Vector3(a, c).cross(Vector3(b, c));

        //s and v per vertices
        s = Vector3(a, sunShine);
        v = Vector3(a, cam.eye);
        Ir = light(s,m,v,Ia,Par,Id,Pdr,Is,Psr,f);
        Ig = light(s,m,v,Ia,Pag,Id,Pdg,Is,Psg,f);
        Ib = light(s,m,v,Ia,Pab,Id,Pdb,Is,Psb,f);
        glColor3f(Ir,Ig,Ib);
        glVertex3d(a.x,a.y,a.z);

        s = Vector3(b, sunShine);
        v = Vector3(b, cam.eye);
        Ir = light(s,m,v,Ia,Par,Id,Pdr,Is,Psr,f);
        Ig = light(s,m,v,Ia,Pag,Id,Pdg,Is,Psg,f);
        Ib = light(s,m,v,Ia,Pab,Id,Pdb,Is,Psb,f);
        glColor3f(Ir,Ig,Ib);
        glVertex3d(b.x,b.y,b.z);

        s = Vector3(c, sunShine);
        v = Vector3(c, cam.eye);
        Ir = light(s,m,v,Ia,Par,Id,Pdr,Is,Psr,f);
        Ig = light(s,m,v,Ia,Pag,Id,Pdg,Is,Psg,f);
        Ib = light(s,m,v,Ia,Pab,Id,Pdb,Is,Psb,f);
        glColor3f(Ir,Ig,Ib);
        glVertex3d(c.x,c.y,c.z);
    glEnd();


    a = Point3(1,-1,1);
    b = Point3(1,1,-1);
    c = Point3(1,1,1);
    glBegin(GL_TRIANGLES);
        //calculate m once per triangle
        m = Vector3(a, c).cross(Vector3(b, c));

        //s and v per vertices
        s = Vector3(a, sunShine);
        v = Vector3(a, cam.eye);
        Ir = light(s,m,v,Ia,Par,Id,Pdr,Is,Psr,f);
        Ig = light(s,m,v,Ia,Pag,Id,Pdg,Is,Psg,f);
        Ib = light(s,m,v,Ia,Pab,Id,Pdb,Is,Psb,f);
        glColor3f(Ir,Ig,Ib);
        glVertex3d(a.x,a.y,a.z);

        s = Vector3(b, sunShine);
        v = Vector3(b, cam.eye);
        Ir = light(s,m,v,Ia,Par,Id,Pdr,Is,Psr,f);
        Ig = light(s,m,v,Ia,Pag,Id,Pdg,Is,Psg,f);
        Ib = light(s,m,v,Ia,Pab,Id,Pdb,Is,Psb,f);
        glColor3f(Ir,Ig,Ib);
        glVertex3d(b.x,b.y,b.z);

        s = Vector3(c, sunShine);
        v = Vector3(c, cam.eye);
        Ir = light(s,m,v,Ia,Par,Id,Pdr,Is,Psr,f);
        Ig = light(s,m,v,Ia,Pag,Id,Pdg,Is,Psg,f);
        Ib = light(s,m,v,Ia,Pab,Id,Pdb,Is,Psb,f);
        glColor3f(Ir,Ig,Ib);
        glVertex3d(c.x,c.y,c.z);
    glEnd();


    a = Point3(1,1,-1);
    b = Point3(-1,1,1);
    c = Point3(1,1,1);
    glBegin(GL_TRIANGLES);
        //calculate m once per triangle
        m = Vector3(a, c).cross(Vector3(b, c));

        //s and v per vertices
        s = Vector3(a, sunShine);
        v = Vector3(a, cam.eye);
        Ir = light(s,m,v,Ia,Par,Id,Pdr,Is,Psr,f);
        Ig = light(s,m,v,Ia,Pag,Id,Pdg,Is,Psg,f);
        Ib = light(s,m,v,Ia,Pab,Id,Pdb,Is,Psb,f);
        glColor3f(Ir,Ig,Ib);
        glVertex3d(a.x,a.y,a.z);

        s = Vector3(b, sunShine);
        v = Vector3(b, cam.eye);
        Ir = light(s,m,v,Ia,Par,Id,Pdr,Is,Psr,f);
        Ig = light(s,m,v,Ia,Pag,Id,Pdg,Is,Psg,f);
        Ib = light(s,m,v,Ia,Pab,Id,Pdb,Is,Psb,f);
        glColor3f(Ir,Ig,Ib);
        glVertex3d(b.x,b.y,b.z);

        s = Vector3(c, sunShine);
        v = Vector3(c, cam.eye);
        Ir = light(s,m,v,Ia,Par,Id,Pdr,Is,Psr,f);
        Ig = light(s,m,v,Ia,Pag,Id,Pdg,Is,Psg,f);
        Ib = light(s,m,v,Ia,Pab,Id,Pdb,Is,Psb,f);
        glColor3f(Ir,Ig,Ib);
        glVertex3d(c.x,c.y,c.z);
    glEnd();
    

    b = Point3(1,-1,1);
    a = Point3(-1,1,1);
    c = Point3(1,1,1);
    glBegin(GL_TRIANGLES);
        //calculate m once per triangle
        m = Vector3(a, c).cross(Vector3(b, c));

        //s and v per vertices
        s = Vector3(a, sunShine);
        v = Vector3(a, cam.eye);
        Ir = light(s,m,v,Ia,Par,Id,Pdr,Is,Psr,f);
        Ig = light(s,m,v,Ia,Pag,Id,Pdg,Is,Psg,f);
        Ib = light(s,m,v,Ia,Pab,Id,Pdb,Is,Psb,f);
        glColor3f(Ir,Ig,Ib);
        glVertex3d(a.x,a.y,a.z);

        s = Vector3(b, sunShine);
        v = Vector3(b, cam.eye);
        Ir = light(s,m,v,Ia,Par,Id,Pdr,Is,Psr,f);
        Ig = light(s,m,v,Ia,Pag,Id,Pdg,Is,Psg,f);
        Ib = light(s,m,v,Ia,Pab,Id,Pdb,Is,Psb,f);
        glColor3f(Ir,Ig,Ib);
        glVertex3d(b.x,b.y,b.z);

        s = Vector3(c, sunShine);
        v = Vector3(c, cam.eye);
        Ir = light(s,m,v,Ia,Par,Id,Pdr,Is,Psr,f);
        Ig = light(s,m,v,Ia,Pag,Id,Pdg,Is,Psg,f);
        Ib = light(s,m,v,Ia,Pab,Id,Pdb,Is,Psb,f);
        glColor3f(Ir,Ig,Ib);
        glVertex3d(c.x,c.y,c.z);
    glEnd();

    //End Triangles

    
    //draw sunshine
    glPushMatrix();
        //draw a line to the sunshine 
        glBegin(GL_LINES);
            glColor3f(1,1,.5);
            glVertex3d(1,1,1);
            glVertex3d(sunShine.x,sunShine.y,sunShine.z);   
        glEnd();

        glTranslated(sunShine.x,sunShine.y,sunShine.z);

        glColor3d(1,1,0);
        glutSolidSphere(1,20,20);

        glColor3f(0,0,0);
        string str = "Sunshine";
        glRasterPos3d(1, 1, 1);
        for (int i = 0; i < 8; i++)
            glutBitmapCharacter(GLUT_BITMAP_8_BY_13, (int)str[i]); 

        
    glPopMatrix();

    glFlush();
    glutSwapBuffers();

}

void keyboardDrawPrompt(unsigned char key, int xmouse, int ymouse) {
	switch(key)
	{
        //camera controls
        case 'a':    cam.yaw(-2.0); break; 
        case 'd':    cam.yaw(2.0); break;
        
        case 'w':    cam.pitch(-2.0); break;
        case 's':    cam.pitch(2.0); break; 
        
        case 'q':    cam.roll(-2.0); break; 
        case 'e':    cam.roll(2.0); break; 

        case '\'':   cam.slide(0.0, .2, 0); break;
        case '/':    cam.slide(0.0, -.2, 0); break;

        //sunshine controlls
        case 'u':    sunShine = Point3(sunShine.x, sunShine.y + 1, sunShine.z); break; 
        case 'j':    sunShine = Point3(sunShine.x, sunShine.y - 1, sunShine.z); break; 
        
        case 'h':    sunShine = Point3(sunShine.x - 1, sunShine.y, sunShine.z); break; 
        case 'k':    sunShine = Point3(sunShine.x + 1, sunShine.y, sunShine.z); break; 

        //color controls
        case 'c':    GS ? GS = false : GS = true; break; 

        case 27 : exit(1);
    
    }

    if(a == 360 || a == -360)
        a = 0;

    //call the redraw function
    glutPostRedisplay();

}

void SpecialKeys(int key, int x, int y)
{
    switch (key)
    {
        //camera controls
        case GLUT_KEY_LEFT:  if(spin) a -= 10; cam.slide(-0.2, 0, 0); break;
        case GLUT_KEY_UP:    if(spin) a += 10; cam.slide(0, 0, -0.2); break; 
        case GLUT_KEY_RIGHT: if(spin) a += 10; cam.slide(0.2, 0, 0);  break;
        case GLUT_KEY_DOWN:  if(spin) a -= 10; cam.slide(0, 0, 0.2);  break;  
    }
    glutPostRedisplay();
}


// Runs the code setting the GL functions to the appropriate from above
//<<<<<<<<<<<<<<<<<<<<<<<< main >>>>>>>>>>>>>>>>>>>>>>
int main(int argc, char **argv) {
	
	cout << "Camera tilt: 'w', 'a', 's', 'd', '/', '(single quote)'\n"; 
	cout << "Camera movement: arrow keys\n"; 
	cout << "Light movement: 'u','h','j','k'\n"; 
	cout << "Color switch: 'c'"; 
		
	glutInit(&argc, argv);          // initialize the toolkit
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB); // set the display mode
	glutInitWindowSize(680,480);     // set the window size
	glutInitWindowPosition(680, 0); // set the window position on the screen
	glutCreateWindow("Light"); // open the screen window(with its exciting title)
    glutKeyboardFunc(keyboardDrawPrompt); // register the keyboard action function
    glutSpecialFunc(SpecialKeys);
	glutDisplayFunc(display);     // register the redraw function
    glClearColor(0.5f,0.5,0.5f,0.0f);
    glColor3f(0.0f,0.0f,0.0f);
    glViewport(0,0,640,430);
    //eye, look, up
    cam.set(3,3,3,0,0,0,0,1,0);
    cam.setShape(30.0, 64.0/48.0, .5, 100.0);
	glutMainLoop(); 		     // go into a perpetual loop
	
}



