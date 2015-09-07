//=============================================================================================
// Szamitogepes grafika hazi feladat keret. Ervenyes 2014-tol.
// A //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// sorokon beluli reszben celszeru garazdalkodni, mert a tobbit ugyis toroljuk.
// A beadott program csak ebben a fajlban lehet, a fajl 1 byte-os ASCII karaktereket tartalmazhat.
// Tilos:
// - mast "beincludolni", illetve mas konyvtarat hasznalni
// - faljmuveleteket vegezni (printf is fajlmuvelet!)
// - new operatort hivni az onInitialization függvényt kivéve, a lefoglalt adat korrekt felszabadítása nélkül
// - felesleges programsorokat a beadott programban hagyni
// - tovabbi kommenteket a beadott programba irni a forrasmegjelolest kommentjeit kiveve
// ---------------------------------------------------------------------------------------------
// A feladatot ANSI C++ nyelvu forditoprogrammal ellenorizzuk, a Visual Studio-hoz kepesti elteresekrol
// es a leggyakoribb hibakrol (pl. ideiglenes objektumot nem lehet referencia tipusnak ertekul adni)
// a hazibeado portal ad egy osszefoglalot.
// ---------------------------------------------------------------------------------------------
// A feladatmegoldasokban csak olyan gl/glu/glut fuggvenyek hasznalhatok, amelyek
// 1. Az oran a feladatkiadasig elhangzottak ES (logikai AND muvelet)
// 2. Az alabbi listaban szerepelnek:
// Rendering pass: glBegin, glVertex[2|3]f, glColor3f, glNormal3f, glTexCoord2f, glEnd, glDrawPixels
// Transzformaciok: glViewport, glMatrixMode, glLoadIdentity, glMultMatrixf, gluOrtho2D,
// glTranslatef, glRotatef, glScalef, gluLookAt, gluPerspective, glPushMatrix, glPopMatrix,
// Illuminacio: glMaterialfv, glMaterialfv, glMaterialf, glLightfv
// Texturazas: glGenTextures, glBindTexture, glTexParameteri, glTexImage2D, glTexEnvi,
// Pipeline vezerles: glShadeModel, glEnable/Disable a kovetkezokre:
// GL_LIGHTING, GL_NORMALIZE, GL_DEPTH_TEST, GL_CULL_FACE, GL_TEXTURE_2D, GL_BLEND, GL_LIGHT[0..7]
//
// NYILATKOZAT
// ---------------------------------------------------------------------------------------------
// Nev    : Bunta Roland Arpad
// Neptun : BI5UYV
// ---------------------------------------------------------------------------------------------
// ezennel kijelentem, hogy a feladatot magam keszitettem, es ha barmilyen segitseget igenybe vettem vagy
// mas szellemi termeket felhasznaltam, akkor a forrast es az atvett reszt kommentekben egyertelmuen jeloltem.
// A forrasmegjeloles kotelme vonatkozik az eloadas foliakat es a targy oktatoi, illetve a
// grafhazi doktor tanacsait kiveve barmilyen csatornan (szoban, irasban, Interneten, stb.) erkezo minden egyeb
// informaciora (keplet, program, algoritmus, stb.). Kijelentem, hogy a forrasmegjelolessel atvett reszeket is ertem,
// azok helyessegere matematikai bizonyitast tudok adni. Tisztaban vagyok azzal, hogy az atvett reszek nem szamitanak
// a sajat kontribucioba, igy a feladat elfogadasarol a tobbi resz mennyisege es minosege alapjan szuletik dontes.
// Tudomasul veszem, hogy a forrasmegjeloles kotelmenek megsertese eseten a hazifeladatra adhato pontokat
// negativ elojellel szamoljak el es ezzel parhuzamosan eljaras is indul velem szemben.
//=============================================================================================

#define _USE_MATH_DEFINES
#include <math.h>
#include <stdlib.h>

#if defined(__APPLE__)
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#include <GLUT/glut.h>
#else
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__)
#include <windows.h>
#endif
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#endif

#include <iostream>
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Innentol modosithatod...

const float PI = 3.14159265;
const float EPSILON = 0.0001;

//--------------------------------------------------------
// 3D Vektor
//--------------------------------------------------------
struct Vector {
   float x, y, z;

   Vector( ) {
	x = y = z = 0;
   }
   Vector(float x0, float y0, float z0 = 0) {
	x = x0; y = y0; z = z0;
   }
   Vector operator*(float a) {
	return Vector(x * a, y * a, z * a);
   }
   Vector operator+(const Vector& v) {
 	return Vector(x + v.x, y + v.y, z + v.z);
   }
   Vector operator-(const Vector& v) {
 	return Vector(x - v.x, y - v.y, z - v.z);
   }
   float operator*(const Vector& v) { 	// dot product
	return (x * v.x + y * v.y + z * v.z);
   }
   Vector operator%(const Vector& v) { 	// cross product
	return Vector(y*v.z-z*v.y, z*v.x - x*v.z, x*v.y - y*v.x);
   }
   void operator=(const Vector& v) {
     x = v.x; y = v.y; z = v.z;
   }
   float Length() { return sqrt(x * x + y * y + z * z);
   }
    Vector Normalize() {
        Vector v;
        float length = this->Length();
        if(length != 0){
        v.x = x/length;
        v.y = y/length;
        v.z = z/length;
        }
        return v;
    }
    bool operator==(const Vector& v) {
     return (x <= v.x + EPSILON && x >= v.x - EPSILON
          && y <= v.y + EPSILON && y >= v.y - EPSILON
          && z <= v.z + EPSILON && z >= v.z - EPSILON);
    }
};

//--------------------------------------------------------
// Spektrum illetve szin
//--------------------------------------------------------
struct Color {
   float r, g, b;

   Color( ) {
	r = g = b = 0;
   }
   Color(float r0, float g0, float b0) {
	r = r0; g = g0; b = b0;
   }
   Color operator*(float a) {
	return Color(r * a, g * a, b * a);
   }
   Color operator*(const Color& c) {
	return Color(r * c.r, g * c.g, b * c.b);
   }
   Color operator+(const Color& c) {
 	return Color(r + c.r, g + c.g, b + c.b);
   }
};

struct Sun{
    float pos[4];

    Sun(Vector position, Color ambient, Color diffuse, Color specular){
        pos[0] = position.x;
        pos[1] = position.y;
        pos[2] = position.z;
        pos[3] = 0;
        float la[] = {ambient.r, ambient.g, ambient.b, 1};
        float ld[] = {diffuse.r, diffuse.g, diffuse.b, 1};
        float ls[] = {specular.r, specular.g, specular.b, 1};

        glLightfv(GL_LIGHT0, GL_POSITION, pos);
        glLightfv(GL_LIGHT0, GL_AMBIENT, la);
        glLightfv(GL_LIGHT0, GL_DIFFUSE, ld);
        glLightfv(GL_LIGHT0, GL_SPECULAR, ls);

        glEnable(GL_LIGHT0);
    }
};

struct Object{
    Color ambient, diffuse, specular;
    float shininess;

    void setMaterial(){
        float ka[] = {ambient.r, ambient.g, ambient.b, 1};
        float kd[] = {diffuse.r, diffuse.g, diffuse.b, 1};
        float ks[] = {specular.r, specular.g, specular.b, 1};
        float shine = shininess;

        glMaterialfv(GL_FRONT, GL_AMBIENT, ka);
        glMaterialfv(GL_FRONT, GL_DIFFUSE, kd);
        glMaterialfv(GL_FRONT, GL_SPECULAR, ks);
        glMaterialf(GL_FRONT, GL_SHININESS, shine);
    }
};

struct RegularShape : public Object{
    float resolution;
    float uMax;
    float vMax;

    virtual Vector fv(float u, float v) = 0;
    virtual Vector fvN(float u, float v) = 0;

    void draw(){
        setMaterial();
        float u, v, uE, vE;
        glBegin(GL_QUAD_STRIP);

        for(int i = 0; i < resolution; i++){
            u = i / (resolution - 1) * uMax;
            uE = uMax / (resolution - 1);

            for(int j = 0; j < (4 * resolution); j++){
                v = j / (4 * resolution - 1) * vMax;
                vE = vMax / (4 * resolution - 1);

                Vector A = fv(u, v);
                Vector AN = fvN(u, v);
                Vector B = fv(u + uE, v);
                Vector BN = fvN(u + uE, v);
                Vector C = fv(u, v + vE);
                Vector CN = fvN(u, v + vE);
                Vector D = fv(u + uE, v + vE);
                Vector DN = fvN(u + uE, v + vE);

                glNormal3f(AN.x, AN.y, AN.z);
                glVertex3f(A.x, A.y, A.z);
                glNormal3f(BN.x, BN.y, BN.z);
                glVertex3f(B.x, B.y, B.z);
                glNormal3f(CN.x, CN.y, CN.z);
                glVertex3f(C.x, C.y, C.z);
                glNormal3f(DN.x, DN.y, DN.z);
                glVertex3f(D.x, D.y, D.z);
            }
        }
        glEnd();
    }
};

struct Ellipsoid : public RegularShape{
    float a, b, c;

    Ellipsoid(){}

    Ellipsoid(Color amb, Color diff, Color spec, float shine, float res, Vector eSize){
        ambient = amb;
        diffuse = diff;
        specular = spec;
        shininess = shine;
        resolution = res;
        uMax = PI;
        vMax = PI;
        a = eSize.x;
        b = eSize.y;
        c = eSize.z;
    }

    Vector fv(float u, float v){
        Vector p;
        p.x = a * cos(u) * sin(v);
        p.y = b * sin(u) * sin(v);
        p.z = c * cos(v);
        return p;
    }
    Vector fvN(float u, float v){
        if(v == 0)
            return Vector(0, 0, 1);
        if(v > PI)
            return Vector(0, 0, -1);

        Vector ud;
        ud.x = a * sin(v) * (-1) * sin(u);
        ud.y = b * sin(v) * cos(u);
        ud.z = 0;

        Vector vd;
        vd.x = a * cos(u) * cos(v);
        vd.y = b * sin(u) * cos(v);
        vd.z = c * (-1) * sin(v);

        return (vd % ud).Normalize();
    }
};

struct Cylinder : public RegularShape{
    float radius;
    float height;

    Cylinder(){}

    Cylinder(Color amb, Color diff, Color spec, float shine, float res, float hei, float rad){
        ambient = amb;
        diffuse = diff;
        specular = spec;
        shininess = shine;
        resolution = res;
        radius = rad;
        height = hei;
        uMax = 2 *PI;
        vMax = hei;
    }

    Vector fv(float u, float v){
        Vector p;
        p.x = radius * cos(u);
        p.y = v;
        p.z = radius * sin(u);
        return p;
    }
    Vector fvN(float u, float v){
        if(v == 0)
            return Vector(0, 0, 1);
        if(v > height)
            return Vector(0, 0, -1);

        Vector ud;
        ud.x = radius * (-1) * sin(u);
        ud.y = 0;
        ud.z = radius * cos(u);

        Vector vd;
        vd.x = 0;
        vd.y = 1;
        vd.z = 0;

        return (vd % ud).Normalize();
    }
};

struct Cone : public RegularShape{
    float height;
    float radius;

    Cone(){}

    Cone(Color amb, Color diff, Color spec, float shine, float res, float hei, float rad){
        ambient = amb;
        diffuse = diff;
        specular = spec;
        shininess = shine;
        resolution = res;
        height = hei;
        uMax = 2 * PI;
        vMax = 1;
        radius = rad;
    }
    Vector fv(float u, float v){
        Vector p;
        p.x = radius * (1 - v) * cos(u);
        p.y = radius * (1 - v) * sin(u);
        p.z = -v * height;
        return p;
    }
    Vector fvN(float u, float v){
        if(v == 0)
            return Vector(0, 0, 1);
        if(v > height)
            return Vector(0, 0, -1);

        Vector ud;
        ud.x = radius * (1 - v) * (-1) * sin(u);
        ud.y = radius * (1 - v) * cos(u);
        ud.z = 0;

        Vector vd;
        vd.x = radius * (-1) * cos(u);
        vd.y = radius * (-1) * sin(u);
        vd.z = -height;

        return (ud % vd).Normalize();
    }
};

void imaging(Color imagingColor[4]){
    GLuint id;
    glEnable(GL_TEXTURE_2D);
    glGenTextures(1, &id);
    glBindTexture(GL_TEXTURE_2D, id);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 2, 2, 0, GL_RGB, GL_FLOAT, imagingColor);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
}

struct Plane : public Object{

    Plane(Color amb, Color diff, Color spec, float shine){
        ambient = amb;
        diffuse = diff;
        specular = spec;
        shininess = shine;
    }

    void draw(){
        Color imagingColor[] = {Color(0.8, 0.8, 0.8),
                                Color(0.7, 0.7, 0.7),
                                Color(0.6, 0.6, 0.6),
                                Color(1, 1, 0.7)};

        imaging(imagingColor);

        setMaterial();

        glBegin(GL_QUADS);
        glNormal3f(0, 1, 0);
        glTexCoord2f(0, 0);
        glVertex3f(-50, 0, 50);
        glTexCoord2f(5, 0);
        glVertex3f(50, 0, 50);
        glTexCoord2f(0, 5);
        glVertex3f(50, 0, -50);
        glTexCoord2f(5, 5);
        glVertex3f(-50, 0, -50);
        glEnd();
        glDisable(GL_TEXTURE_2D);
    }
};

struct Face;
struct Vertex;

struct HalfEdge{
    Vertex* endPoint;
    HalfEdge* nextHE;
    HalfEdge* pairHE;
    Face* face;
};

struct Face{
    HalfEdge* edge;
};

struct Vertex{
    Vector point;
    HalfEdge* edge;
};

struct PenguinBody : public Object{
    HalfEdge halfEdge[1000];
    Face face[1000];
    Vertex vertex[1000];

    int halfEdgeCount, faceCount, vertexCount;

    float height;
    float a, b, c;

    PenguinBody(){}

    PenguinBody(Color amb, Color diff, Color spec, float shine, Vector pSize){
        ambient = amb;
        diffuse = diff;
        specular = spec;
        shininess = shine;

        a = pSize.x;
        b = pSize.y;
        c = pSize.z;

        height = pSize.y;

        Vector cuboid[8];

        cuboid[0] = Vector( a / 2, 0, -c / 2);
        cuboid[1] = Vector(-a / 2, 0, -c / 2);
        cuboid[2] = Vector(-a / 2, 0,  c / 2);
        cuboid[3] = Vector( a / 2, 0,  c / 2);

        cuboid[4] = Vector( a / 2, b, -c / 2);
        cuboid[5] = Vector(-a / 2, b, -c / 2);
        cuboid[6] = Vector(-a / 2, b,  c / 2);
        cuboid[7] = Vector( a / 2, b,  c / 2);

        halfEdgeCount = 0;
        faceCount = 0;
        vertexCount = 0;

        vertex[0].point = cuboid[2];
        vertex[1].point = cuboid[1];
        vertex[2].point = cuboid[0];
        vertex[3].point = cuboid[3];

        vertex[4].point = cuboid[1];
        vertex[5].point = cuboid[5];
        vertex[6].point = cuboid[4];
        vertex[7].point = cuboid[0];

        vertex[8].point = cuboid[2];
        vertex[9].point = cuboid[6];
        vertex[10].point = cuboid[5];
        vertex[11].point = cuboid[1];

        vertex[12].point = cuboid[3];
        vertex[13].point = cuboid[7];
        vertex[14].point = cuboid[6];
        vertex[15].point = cuboid[2];

        vertex[16].point = cuboid[5];
        vertex[17].point = cuboid[6];
        vertex[18].point = cuboid[7];
        vertex[19].point = cuboid[4];

        vertex[20].point = cuboid[0];
        vertex[21].point = cuboid[4];
        vertex[22].point = cuboid[7];
        vertex[23].point = cuboid[3];

        vertexCount = 24;

        setPoints();

        CatmullClark();
        setPoints();
        CatmullClark();
        setPoints();
}

    void setPoints(){
        for(int i = 0; i < vertexCount - 3; i = i + 4){
            face[faceCount].edge = &halfEdge[halfEdgeCount];

            //1. halfedge
            halfEdge[halfEdgeCount].endPoint = &vertex[i + 1];
            halfEdge[halfEdgeCount].face = &face[faceCount];
            halfEdge[halfEdgeCount++].nextHE = &halfEdge[halfEdgeCount + 1];
            vertex[i].edge = &halfEdge[halfEdgeCount - 1];

            //2. halfedge
            halfEdge[halfEdgeCount].endPoint = &vertex[i + 2];
            halfEdge[halfEdgeCount].face = &face[faceCount];
            halfEdge[halfEdgeCount++].nextHE = &halfEdge[halfEdgeCount + 1];
            vertex[i+1].edge = &halfEdge[halfEdgeCount - 1];

            //3. halfedge
            halfEdge[halfEdgeCount].endPoint = &vertex[i + 3];
            halfEdge[halfEdgeCount].face = &face[faceCount];
            halfEdge[halfEdgeCount++].nextHE = &halfEdge[halfEdgeCount + 1];
            vertex[i+2].edge = &halfEdge[halfEdgeCount - 1];

            //4. halfedge
            halfEdge[halfEdgeCount].endPoint = &vertex[i];
            halfEdge[halfEdgeCount].face = &face[faceCount++];
            vertex[i+3].edge = &halfEdge[halfEdgeCount];
            halfEdge[halfEdgeCount++].nextHE = &halfEdge[halfEdgeCount - 3];
        }

        for(int i = 0; i < halfEdgeCount; i++)
            for(int j = 0; j < halfEdgeCount; j++)
                if(i != j && halfEdge[i].endPoint->point == halfEdge[j].nextHE->nextHE->nextHE->endPoint->point
                          && halfEdge[i].nextHE->nextHE->nextHE->endPoint->point == halfEdge[j].endPoint->point){
                    halfEdge[i].pairHE = &halfEdge[j];
                    halfEdge[j].pairHE = &halfEdge[i];
                }
    }

    Vector vertexBorderedFaceCenter(Vertex v){
        Vector result;

        for(int i = 0; i < faceCount; i++){
            Vector A = face[i].edge->endPoint->point;
            Vector B = face[i].edge->nextHE->endPoint->point;
            Vector C = face[i].edge->nextHE->nextHE->endPoint->point;
            Vector D = face[i].edge->nextHE->nextHE->nextHE->endPoint->point;

            if(v.point == A || v.point == B || v.point == C || v.point == D){

                Vector AS = face[i].edge->endPoint->point;
                Vector BS = face[i].edge->nextHE->endPoint->point;
                Vector CS = face[i].edge->nextHE->nextHE->endPoint->point;
                Vector DS = face[i].edge->nextHE->nextHE->nextHE->endPoint->point;

                result = result + (AS + BS + CS + DS) * 0.25;
            }
        }
        return result;
    }

    Vector pointBorderedFaceNormal(Vector point){
        Vector result;

        for(int i = 0; i < faceCount; i++){
            Vector A = face[i].edge->endPoint->point;
            Vector B = face[i].edge->nextHE->endPoint->point;
            Vector C = face[i].edge->nextHE->nextHE->endPoint->point;
            Vector D = face[i].edge->nextHE->nextHE->nextHE->endPoint->point;

            if(point == A || point == B || point == C || point == D)
                result = result + (face[i].edge->endPoint->point % face[i].edge->nextHE->endPoint->point).Normalize();
        }
        return result * 0.25;
    }

    int vertexBorderedFaceCount(Vertex v){
        int fCount = 0;

        for(int i = 0; i < faceCount; i++){
            Vector A = face[i].edge->endPoint->point;
            Vector B = face[i].edge->nextHE->endPoint->point;
            Vector C = face[i].edge->nextHE->nextHE->endPoint->point;
            Vector D = face[i].edge->nextHE->nextHE->nextHE->endPoint->point;

            if(v.point == A || v.point == B || v.point == C || v.point == D)
                fCount++;
        }
        return fCount;
    }

    Vector vertexBorderedEdgeHalve(Vertex v){
        Vector res;

        for(int i = 0; i < halfEdgeCount; i++){
            Vector A = halfEdge[i].pairHE->endPoint->point;
            Vector B = halfEdge[i].endPoint->point;

            if(v.point == A || v.point == B)
                res = res + (A + B) * 0.5;
        }
        return res * 0.5;
    }

    Vector newVertex(Vertex y){
        Vector center = vertexBorderedFaceCenter(y);
        Vector halve = vertexBorderedEdgeHalve(y);
        float v = vertexBorderedFaceCount(y);

        return (center * (1 / v / v) + halve * (2 / v / v) + y.point * ((v - 3) / v));
    }

    Vector newHalve(Vector g){
        for(int i = 0; i < halfEdgeCount; i++)
            if((halfEdge[i].endPoint->point + halfEdge[i].pairHE->endPoint->point) * 0.5 == g){

                Vector A = halfEdge[i].face->edge->endPoint->point;
                Vector B = halfEdge[i].face->edge->nextHE->endPoint->point;
                Vector C = halfEdge[i].face->edge->nextHE->nextHE->endPoint->point;
                Vector D = halfEdge[i].face->edge->nextHE->nextHE->nextHE->endPoint->point;

                Vector AP = halfEdge[i].pairHE->face->edge->endPoint->point;
                Vector BP = halfEdge[i].pairHE->face->edge->nextHE->endPoint->point;
                Vector CP = halfEdge[i].pairHE->face->edge->nextHE->nextHE->endPoint->point;
                Vector DP = halfEdge[i].pairHE->face->edge->nextHE->nextHE->nextHE->endPoint->point;

                Vector P = (A + B + C + D) * 0.25;
                Vector PP = (AP + BP + CP + DP) * 0.25;

                return P * 0.25 + PP * 0.25 + g * 0.5;
            }
    }

    void CatmullClark(){
        Vertex newVertexes[16000];
        int newVertexCount = 0;
        Vector average;
        int planeQuarter = 0;
        for(int i = 0; i < vertexCount; i++){
            if(planeQuarter == 0){
                average = (vertex[i].point + vertex[i].edge->endPoint->point
                                           + vertex[i].edge->nextHE->endPoint->point
                                           + vertex[i].edge->nextHE->nextHE->endPoint->point) * 0.25;

                newVertexes[newVertexCount++].point = newVertex(vertex[i]);
                newVertexes[newVertexCount++].point = newHalve((vertex[i].point + vertex[i].edge->endPoint->point) * 0.5);
                newVertexes[newVertexCount++].point = average;
                newVertexes[newVertexCount++].point = newHalve((vertex[i].point + vertex[i].edge->nextHE->nextHE->endPoint->point) * 0.5);
            }

            if(planeQuarter == 1){
                newVertexes[newVertexCount++].point = newHalve((vertex[i].point + vertex[i].edge->nextHE->nextHE->endPoint->point) * 0.5);
                newVertexes[newVertexCount++].point = newVertex(vertex[i]);
                newVertexes[newVertexCount++].point = newHalve((vertex[i].point + vertex[i].edge->endPoint->point) * 0.5);
                newVertexes[newVertexCount++].point = average;
            }
            if(planeQuarter == 2){
                newVertexes[newVertexCount++].point = average;
                newVertexes[newVertexCount++].point = newHalve((vertex[i].point + vertex[i].edge->nextHE->nextHE->endPoint->point) * 0.5);
                newVertexes[newVertexCount++].point = newVertex(vertex[i]);
                newVertexes[newVertexCount++].point = newHalve((vertex[i].point + vertex[i].edge->endPoint->point) * 0.5);
            }
            if(planeQuarter == 3){
                newVertexes[newVertexCount++].point = newHalve((vertex[i].point + vertex[i].edge->endPoint->point) * 0.5);
                newVertexes[newVertexCount++].point = average;
                newVertexes[newVertexCount++].point = newHalve((vertex[i].point + vertex[i].edge->nextHE->nextHE->endPoint->point) * 0.5);
                newVertexes[newVertexCount++].point = newVertex(vertex[i]);
            }
            planeQuarter++;
            if(planeQuarter == 4)
                planeQuarter = 0;
        }
        for(int i = 0; i < newVertexCount; i++){
            vertexCount = newVertexCount;
            vertex[i] = newVertexes[i];
        }
    }

    float getCylinderU(float z, float x){
        return (1 / (2 * PI) * (atan2(z, x) + PI));
    }

    float getCylinderV(float y){
        return y / height;
    }

    void draw(){
        setMaterial();

        Color imagingColor[] = {Color(1, 1, 1),
                                Color(1, 1, 0.9),
                                Color(0, 0, 0),
                                Color(0.1, 0.1, 0.1)};
        imaging(imagingColor);


        glBegin(GL_QUADS);

        for(int i = 0; i < faceCount; i++){
            Vector A = face[i].edge->endPoint->point;
            Vector B = face[i].edge->nextHE->endPoint->point;
            Vector C = face[i].edge->nextHE->nextHE->endPoint->point;
            Vector D = face[i].edge->nextHE->nextHE->nextHE->endPoint->point;

            Vector AT = A - Vector(0, A.y, 0);
            AT = AT.Normalize() * 10;
            AT = Vector(0, A.y, 0) + AT;

            Vector BT = B - Vector(0, B.y, 0);
            BT = BT.Normalize() * 10;
            BT = Vector(0, B.y, 0) + BT;

            Vector CT = C - Vector(0, C.y, 0);
            CT = CT.Normalize() * 10;
            CT = Vector(0, C.y, 0) + CT;

            Vector DT = D - Vector(0, D.y, 0);
            DT = DT.Normalize() * 10;
            DT = Vector(0, D.y, 0) + DT;

            Vector AN = pointBorderedFaceNormal(A);
            Vector BN = pointBorderedFaceNormal(B);
            Vector CN = pointBorderedFaceNormal(C);
            Vector DN = pointBorderedFaceNormal(D);

            glNormal3f(AN.x, AN.y, AN.z);
            glTexCoord2f(getCylinderU(AT.z, AT.x), getCylinderV(AT.y));
            glVertex3f(A.x, A.y, A.z);

            glNormal3f(BN.x, BN.y, BN.z);
            glTexCoord2f(getCylinderU(BT.z, BT.x), getCylinderV(BT.y));
            glVertex3f(B.x, B.y, B.z);

            glNormal3f(CN.x, CN.y, CN.z);
            glTexCoord2f(getCylinderU(CT.z, CT.x), getCylinderV(CT.y));
            glVertex3f(C.x, C.y, C.z);

            glNormal3f(DN.x, DN.y, DN.z);
            glTexCoord2f(getCylinderU(DT.z, DT.x), getCylinderV(DT.y));
            glVertex3f(D.x, D.y, D.z);
        }

        glEnd();
        glDisable(GL_TEXTURE_2D);
    }
};

int direction; // 0->0,0,-1; 1->1,0,0; 2->0,0,1; 3->-1,0,0
Vector position;
int angle;
int stepAngle = 15;
int p1StepAngle = 15;
int p2StepAngle = 15;
bool spacePressed = false;
long spacePressedTime;

bool p1Sitting;
bool p2Sitting;

struct Penguin{
    PenguinBody pgb;
    Cone cone;
    Cylinder leg1;
    Cylinder leg2;
    Ellipsoid shoe1;
    Ellipsoid shoe2;
    Ellipsoid wing1;
    Ellipsoid wing2;
    Ellipsoid eye1;
    Ellipsoid eye2;
    Ellipsoid egg;

    bool isHatched;
    Vector pPosition;
    float pAngle;
    bool isAI;
    int stAngle;
    int whichAI;

    Penguin(PenguinBody _pgb, Cone _cone, Cylinder _leg1, Cylinder _leg2, Ellipsoid _shoe1, Ellipsoid _shoe2,
            Ellipsoid _wing1, Ellipsoid _wing2, Ellipsoid _eye1, Ellipsoid _eye2, Ellipsoid _egg,
            bool hatched, Vector pos, float ang, bool ai, int stA, int whAI){
        pgb = _pgb;
        cone = _cone;
        leg1 = _leg1;
        leg2 = _leg2;
        shoe1 = _shoe1;
        shoe2 = _shoe2;
        wing1 = _wing1;
        wing2 = _wing2;
        eye1 = _eye1;
        eye2 = _eye2;
        egg = _egg;

        isHatched = hatched;
        pPosition = pos;
        pAngle = ang;
        isAI = ai;
        stAngle = stA;
        whichAI = whAI;

    }

    void draw(){

        if(isHatched){
            glPushMatrix();
                glTranslatef(pPosition.x, pPosition.y, pPosition.z);
                if(isAI)
                    glScalef(0.5, 0.5, 0.5);
                glRotatef(pAngle, 0, 1, 0);

                if((!isAI && spacePressed) || (isAI && p1Sitting && (whichAI == 1)) || (isAI && p2Sitting && (whichAI == 2))){
                    glPushMatrix();
                        glTranslatef(0, -0.5, 0);
                    }

                    glPushMatrix();
                        glTranslatef(-0.7, 6, -1.85);
                        eye2.draw();
                    glPopMatrix();

                    glPushMatrix();
                        glTranslatef(0.7, 6, -1.85);
                        eye1.draw();
                    glPopMatrix();

                    glPushMatrix();
                        glTranslatef(-2, 3.5, 0);
                        glRotatef(-20, 0, 0, 1);
                        wing2.draw();
                    glPopMatrix();

                    glPushMatrix();
                        glTranslatef(2, 3.5, 0);
                        glRotatef(20, 0, 0, 1);
                        wing1.draw();
                    glPopMatrix();

                    glPushMatrix();
                        glTranslatef(0, 5.5, -2);
                        glRotatef(-5, 1, 0, 0);
                        cone.draw();
                    glPopMatrix();

                    glPushMatrix();
                        glTranslatef(0, 0, 0);
                        pgb.draw();
                    glPopMatrix();

                if(!isAI && spacePressed || (isAI && p1Sitting && (whichAI == 1)) || (isAI && p2Sitting && (whichAI == 2)))
                    glPopMatrix();

                glPushMatrix();
                    glTranslatef(-0.7, 0.4, -0.3);
                    glRotatef(stAngle, 1, 0, 0);
                    shoe2.draw();
                glPopMatrix();

                glPushMatrix();
                    glTranslatef(0.7, 0.4, -0.3);
                    glRotatef(-stAngle, 1, 0, 0);
                    shoe1.draw();
                glPopMatrix();

                glPushMatrix();
                    glTranslatef(-0.7, 0.4, 0);
                    glRotatef(stAngle, 1, 0, 0);
                    leg2.draw();
                glPopMatrix();

                glPushMatrix();
                    glTranslatef(0.7, 0.4, 0);
                    glRotatef(-stAngle, 1, 0, 0);
                    leg1.draw();
                glPopMatrix();

            glPopMatrix();
        }
        else{
            glPushMatrix();
                glTranslatef(pPosition.x, pPosition.y, pPosition.z);
                egg.draw();
            glPopMatrix();
        }

    }
};

Vector p1Position = Vector(8, 2, -6);
Vector p2Position = Vector(-2, 2, -7);

bool p1Hatched = false;
bool p2Hatched = false;
float p1Angle = 90;
float p2Angle = 270;
Vector p1Direction = Vector(-1, 0, 0);
Vector p2Direction = Vector(1, 0, 0);

void onInitialization( ) {

    glEnable(GL_LIGHTING);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_NORMALIZE);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(60, 1, 2, 500);
}

void onDisplay( ) {
    glClearColor(0.4f, 0.5f, 0.6f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    Vector eye = Vector(-6, 15, -20);
    Vector focus = Vector(position.x, position.y, position.z);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(eye.x, eye.y, eye.z, focus.x, focus.y, focus.z, 0.0, 1.0, 0.0);

    glEnable(GL_LIGHTING);

    Sun sun = Sun(Vector(5, 10, -50), Color(0.2, 0.2, 0.2), Color(0.8, 0.8, 0.8), Color(0.5, 0.5, 0.5));

    Plane pl1 = Plane(Color(1, 1, 1), Color(1, 1, 1), Color(1, 1, 1), 0);
    pl1.draw();

    PenguinBody _pgb = PenguinBody(Color(0.8, 0.8, 0.8), Color(1, 1, 0.8), Color(1, 0.8, 0.6), 10, Vector(4, 8, 6));
    Cone _cone = Cone(Color(1, 0.6, 0), Color(1, 0.6, 0), Color(1, 0.6, 0), 20, 50, 2, 0.5);
    Cylinder _leg1 = Cylinder(Color(1, 0.6, 0), Color(1, 0.6, 0), Color(1, 0.6, 0), 20, 50, 1.2, 0.35);
    Cylinder _leg2 = Cylinder(Color(1, 0.6, 0), Color(1, 0.6, 0), Color(1, 0.6, 0), 20, 50, 1.2, 0.35);
    Ellipsoid _shoe1 = Ellipsoid(Color(1, 0.6, 0), Color(1, 0.6, 0), Color(1, 0.6, 0), 20, 50, Vector(0.6, 0.2, 0.8));
    Ellipsoid _shoe2 = Ellipsoid(Color(1, 0.6, 0), Color(1, 0.6, 0), Color(1, 0.6, 0), 20, 50, Vector(0.6, 0.2, 0.8));
    Ellipsoid _wing1 = Ellipsoid(Color(0.1, 0.1, 0.1), Color(0.2, 0.2, 0.2), Color(0.1, 0.1, 0.1), 10, 50, Vector(0.1, 2, 1));
    Ellipsoid _wing2 = Ellipsoid(Color(0.1, 0.1, 0.1), Color(0.2, 0.2, 0.2), Color(0.1, 0.1, 0.1), 10, 50, Vector(0.1, 2, 1));
    Ellipsoid _eye1 = Ellipsoid(Color(0.4, 0.4, 0.4), Color(0.5, 0.5, 0.5), Color(0.6, 0.6, 0.6), 20, 50, Vector(0.15, 0.15, 0.15));
    Ellipsoid _eye2 = Ellipsoid(Color(0.4, 0.4, 0.4), Color(0.5, 0.5, 0.5), Color(0.6, 0.6, 0.6), 20, 50, Vector(0.15, 0.15, 0.15));
    Ellipsoid _egg = Ellipsoid(Color(1, 0.96, 0.764), Color(0.8, 0.8, 0.8), Color(0.6, 0.6, 0.6), 40, 50, Vector(1, 2, 1));

    Penguin avatarPenguin = Penguin(_pgb, _cone, _leg1, _leg2, _shoe1, _shoe2, _wing1, _wing2, _eye1, _eye2, _egg, true, position, angle, false, stepAngle, -1);
    Penguin penguin1 = Penguin(_pgb, _cone, _leg1, _leg2, _shoe1, _shoe2, _wing1, _wing2, _eye1, _eye2, _egg, p1Hatched, p1Position, p1Angle, true, p1StepAngle, 1);
    Penguin penguin2 = Penguin(_pgb, _cone, _leg1, _leg2, _shoe1, _shoe2, _wing1, _wing2, _eye1, _eye2, _egg, p2Hatched, p2Position, p2Angle, true, p2StepAngle, 2);

    penguin1.draw();
    penguin2.draw();
    avatarPenguin.draw();

    float tmp1 = (-1) * sun.pos[0] / sun.pos[1];
    float tmp2 = (-1) * sun.pos[2] / sun.pos[1];

    float shadowMatrix[4][4] = {   1,    0,    0,    0,
                                tmp1,    0, tmp2,    0,
                                   0,    0,    1,    0,
                                   0, 0.1,    0,    1};

    glMultMatrixf(&shadowMatrix[0][0]);

    glDisable(GL_LIGHTING);
    glColor3f(0, 0, 0);

    penguin1.draw();
    penguin2.draw();
    avatarPenguin.draw();

    glutSwapBuffers();

}

void onKeyboard(unsigned char key, int x, int y) {
    if (key == ' '){
        spacePressedTime = glutGet(GLUT_ELAPSED_TIME);
        spacePressed = true;
    }

    if(!spacePressed){
        if (key == 'e'){
            stepAngle = -stepAngle;

            if(direction == 0)
                position = position + Vector(0, 0, -3);
            if(direction == 1)
                position = position + Vector(3, 0, 0);
            if(direction == 2)
                position = position + Vector(0, 0, 3);
            if(direction == 3)
                position = position + Vector(-3, 0, 0);
        }
        if (key == 'w'){
            stepAngle = -stepAngle;
            switch(direction){
                case 0: direction = 1;
                        angle = 270;
                        break;
                case 1: direction = 2;
                        angle = 180;
                        break;
                case 2: direction = 3;
                        angle = 90;
                        break;
                case 3: direction = 0;
                        angle = 0;
                        break;
            }
        }
        if (key == 'r'){
            stepAngle = -stepAngle;
            switch(direction){
                case 0: direction = 3;
                        angle = 90;
                        break;
                case 1: direction = 0;
                        angle = 0;
                        break;
                case 2: direction = 1;
                        angle = 270;
                        break;
                case 3: direction = 2;
                        angle = 180;
                        break;
            }
        }
    }
    glutPostRedisplay();
}

void onKeyboardUp(unsigned char key, int x, int y) {

}

void onMouse(int button, int state, int x, int y) {
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
		glutPostRedisplay( );
}

void onMouseMotion(int x, int y)
{

}

long p1Time;
long p2Time;
int countTo3 = 0;

void onIdle( ) {
    long time = glutGet(GLUT_ELAPSED_TIME);

    if(p1Hatched && (time - p1Time) >= 1000){
        if((p1Position - p2Position).Length() < 3 && !p2Hatched){
            p1Direction = Vector(0, 0, 0);
            p1Sitting = true;
            countTo3++;
            if(countTo3 == 3){
                p1Sitting = false;
                p2Hatched = true;
                p2Position = Vector(-2, 0, -7);
                countTo3 = 0;
            }
        }
        else
            p1Direction = Vector(-1, 0, 0);
        p1Position = p1Direction + p1Position;
        p1StepAngle = -p1StepAngle;
        p1Time = time;
        glutPostRedisplay();
    }

    if(p2Hatched && (time - p2Time) >= 1000){
        if((p2Position - p1Position).Length() < 3 && !p1Hatched){
            p2Direction = Vector(0,0,0);
            p2Sitting = true;
            countTo3++;
            if(countTo3 == 3){
                p2Sitting = false;
                p1Hatched = true;
                p1Position = Vector(8, 0, -6);
                countTo3 = 0;
            }
        }
        else
            p2Direction = Vector(1, 0, 0);
        p2Position = p2Direction + p2Position;
        p1StepAngle = -p2StepAngle;
        p2Time = time;
        glutPostRedisplay();
    }

    if(spacePressed && (time - spacePressedTime) >= 3000){
        if((p1Position - position).Length() < 3 && !p1Hatched){
            p1Hatched = true;
            p1Position = Vector(8, 0, -6);
            p1Time = time;
            }

        else if((p2Position - position).Length() < 3 && !p2Hatched){
            p2Hatched = true;
            p2Position = Vector(-2, 0, -7);
            p2Time = time;
        }
        spacePressed = false;
     }
}

// ...Idaig modosithatod
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

// A C++ program belepesi pontja, a main fuggvenyt mar nem szabad bantani
int main(int argc, char **argv) {
    glutInit(&argc, argv); 				// GLUT inicializalasa
    glutInitWindowSize(600, 600);			// Alkalmazas ablak kezdeti merete 600x600 pixel
    glutInitWindowPosition(100, 100);			// Az elozo alkalmazas ablakhoz kepest hol tunik fel
    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);	// 8 bites R,G,B,A + dupla buffer + melyseg buffer

    glutCreateWindow("Grafika hazi feladat");		// Alkalmazas ablak megszuletik es megjelenik a kepernyon

    glMatrixMode(GL_MODELVIEW);				// A MODELVIEW transzformaciot egysegmatrixra inicializaljuk
    glLoadIdentity();
    glMatrixMode(GL_PROJECTION);			// A PROJECTION transzformaciot egysegmatrixra inicializaljuk
    glLoadIdentity();

    onInitialization();					// Az altalad irt inicializalast lefuttatjuk

    glutDisplayFunc(onDisplay);				// Esemenykezelok regisztralasa
    glutMouseFunc(onMouse);
    glutIdleFunc(onIdle);
    glutKeyboardFunc(onKeyboard);
    glutKeyboardUpFunc(onKeyboardUp);
    glutMotionFunc(onMouseMotion);

    glutMainLoop();					// Esemenykezelo hurok

    return 0;
}

