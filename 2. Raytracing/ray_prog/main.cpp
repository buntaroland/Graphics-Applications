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
// Nev    : Bunta Roland Árpád
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




//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Innentol modosithatod...

//--------------------------------------------------------
// 3D Vektor
//--------------------------------------------------------
struct Vector {
   float x, y, z;

   Vector( ) {
	x = y = z = 0;
   }
   Vector(float x0){
    x = x0, y = x0, z = x0;
   }
   Vector(float x0, float y0, float z0 = 0) {
	x = x0; y = y0; z = z0;
   }
   Vector operator*(float a) {
	return Vector(x * a, y * a, z * a);
   }
   Vector operator/(float a) {
	return Vector(x / a, y / a, z / a);
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
   Color operator/(const Color& c) {
    return Color(r / c.r, g / c.g, b / c.b);
   }
   Color operator+(const Color& c) {
 	return Color(r + c.r, g + c.g, b + c.b);
   }
   Color operator-(const Color& c) {
    return Color(r - c.r, g - c.g, b - c.b);
   }
};

const int screenWidth = 600;
const int screenHeight = 600;

Color image[screenWidth][screenHeight];

Vector perlin_gradient[900];

#define EPSILON 0.001
#define PI 3.1412

float get_random_num(float minim, float maxim)
{
  return minim + (maxim - minim) * (float)rand() / (float) RAND_MAX;
}

Vector mix(Vector a, Vector b, Vector c)
{
    return a * ( Vector(1) - c ) + b * c;
}

//http://www.wikiwand.com/en/Perlin_noise
float dot_grid_gradient(int ix, int iy, Vector v)
{
  float dx = v.x - ix;
  float dy = v.y - iy;

  return dx * perlin_gradient[ix * 30 + iy].x + dy * perlin_gradient[ix * 30 + iy].y;
}

float perlin(Vector v)
{
    int x0 = (v.x > 0.0 ? v.x : v.x - 1);
    int x1 = x0 + 1;
    int y0 = (v.y > 0.0 ? v.y : v.y - 1);
    int y1 = y0 + 1;

    x0 &= 15;
    x1 &= 15;
    y0 &= 15;
    y1 &= 15;

    float sx = v.x - x0;
    float sy = v.y - y0;

    float n0, n1, ix0, ix1;
    n0 = dot_grid_gradient( x0, y0, v );
    n1 = dot_grid_gradient( x1, y0, v );
    ix0 = mix( n0, n1, sx ).x;

    n0 = dot_grid_gradient( x0, y1, v );
    n1 = dot_grid_gradient( x1, y1, v );
    ix1 = mix( n0, n1, sx ).x;

    return fabs(mix(ix0, ix1, sy).x);
}

Color procText1(Vector v){
    if(fmod(floor(v.x) + floor(v.y) + floor(v.z), 2.0) == 0)
        return Color(0.61424, 0.04136, 0.04136);
    else
        return Color(0.07568, 0.61424, 0.07568);
}

Color procText2(Vector v){
    if (fmod(floor(3.5*perlin(v) * v.x), 2) == 0)
        return Color(0, 0, 1);
    else
        return Color(0, 1, 0);

}

float lowestPos(float a, float b){
    if(a < 0 && b < 0)
        return 0;
    else if(a < 0)
        return b;
    else if (b < 0)
        return a;
    else
        return a < b ? a : b;
}
struct Ray{
    Vector p;
    Vector v;

    Ray(){}

    Ray(Vector _p, Vector _v){
        p = _p;
        v = _v.Normalize();
    }
};

struct RectH{
    Vector P;
    Vector N;
    int indexObj;
    float t;
    bool isPlain;
    RectH(){indexObj = -1; t = -1.0f; isPlain = false;}
};

struct Material{
    Color kd, ks, n, k;
    float shine;
    bool refl = false;
    Material(){}

    Material(Color _kd, Color _ks, Color _n, Color _k, float _shine, bool _refl){
        kd = _kd;
        ks = _ks;
        n = _n;
        k = _k;
        shine = _shine;
        refl = _refl;
    }

    Color Fresnel(float angle){
        Color F0 = ((n - Color(1,1,1)) * (n - Color(1,1,1)) + k * k) / ((n + Color(1,1,1)) * (n + Color(1,1,1)) + k * k);
        return F0 + (Color(1,1,1) - F0) * pow(1 - cos(angle), 5);
    }
};

struct Object{
    Material material;
    virtual RectH intersect(Ray ray) = 0;
};

struct Orb : public Object{
    Vector o;
    float r;

    Orb(Vector _o, float _r){
        o = _o;
        r = _r;
    }

    RectH intersect(Ray ray){
        float A = ray.v * ray.v;
        float B = ((ray.p - o) * ray.v) * 2.0f;
        float C = (ray.p - o) * (ray.p - o) - r * r;
        float D = B * B - 4 * A * C;

        if(D < 0)
            return RectH();

        float t1 = ((B * (-1)) + sqrtf(D)) / (2.0 * A);
        float t2 = ((B * (-1)) - sqrtf(D)) / (2.0 * A);
        float t = lowestPos(t1, t2);

        if(t > EPSILON){
            RectH res;
            res.t = t;
            res.P = ray.p + ray.v * t;
            res.N = (res.P - o).Normalize();
            return res;
        }
        return RectH();
    }
};

struct Plain : public Object{
    Vector N, P;

    Plain(Vector _N, Vector _P){
        N = _N;
        P = _P;
    }

    RectH intersect(Ray ray){
        if(N * ray.v == 0)
            return RectH();

        float t = (N * (P - ray.p)) / (N * ray.v);

        if(t > EPSILON){
            RectH res;
            res.t = t;
            res.N = N.Normalize();
            res.P = ray.p + ray.v * t;
            res.isPlain = true;

            if(   res.P.x < 5.0 + EPSILON && res.P.x > -5.0 - EPSILON
               && res.P.y < 5.0 + EPSILON && res.P.y > -5.0 - EPSILON
               && res.P.z < 5.0 + EPSILON && res.P.z > -5.0 - EPSILON)
            {
                return res;
            }
            else
                return RectH();
        }
        return RectH();
    }
};

struct Triangle : public Object{
    Vector a, b, c;
    Triangle(Vector _a, Vector _b, Vector _c){
        a = _a;
        b = _b;
        c = _c;
    }
    bool isInTriangle(Vector n, Vector hit){
        if(((b - a) % (hit - a)) * n > EPSILON &&
           ((c - b) % (hit - b)) * n > EPSILON &&
           ((a - c) % (hit - c)) * n > EPSILON)
            return true;
        else
            return false;
    }

    RectH intersect(Ray ray){
        RectH res;
        Vector n = ((b - a) % (c - a)).Normalize();

        if(n * ray.v == 0)
            return RectH();

        float t = ((a - ray.p) * n) / (ray.v * n);

        if(t < EPSILON)
            return RectH();

        Vector hit = ray.p + ray.v * t;

        if(isInTriangle(n, hit) && t > EPSILON){
            res.t = t;
            res.N = n.Normalize();
            res.P = hit;
            return res;
        }
        else
            return RectH();
    }

};

struct Torus : public Object{
    Triangle *tri[1000];
    int cntTri;
    Vector o;
    float R, r;

    Torus(Vector _o, float _R, float _r, float resolution){
        o = _o;
        R = _R;
        r = _r;
        cntTri = 0;
        float uMax = 2.0 * PI;
        float vMax = 2.0 * PI;
        float u, uE, v, vE;
        for(int i = 0; i < resolution + 1; i++){
            u = i / resolution * uMax;
            uE = uMax / resolution;
            for(int j = 0; j < resolution + 1; j++){
                v = j / resolution * vMax;
                vE = vMax / resolution;

                Vector p1 = point(u, v);
                Vector p2 = point(u + uE, v);
                Vector p3 = point(u, v + vE);
                Vector p4 = point(u + uE, v + vE);

                tri[cntTri++] = new Triangle(p3, p2, p1);
                tri[cntTri++] = new Triangle(p3, p4, p2);
            }
        }
    }
    Vector point(float u, float v){
        Vector res;
        res.x = R * cos(v) + r * cos(u) * cos(v) + o.x;
        res.y = R * sin(v) + r * cos(u) * sin(v) + o.y;
        res.z = r * sin(u) + o.z;
        return res;
    }
    RectH intersect(Ray ray){

        Orb orb = Orb(o, R + 1.2);
        if (orb.intersect(ray).t == -1.0)
            return RectH();

        RectH res, tmp;
        res.t = 10000.0;
        bool wasIntersection = false;
        for(int i = 0; i < cntTri; i++){
            tmp = tri[i]->intersect(ray);
            if(tmp.t >= EPSILON){
                if(res.t > tmp.t){
                    res = tmp;
                    wasIntersection = true;
                }
            }
        }
        if(wasIntersection)
            return res;

        else
            return RectH();
    }
};

struct Light{
    Vector pos;
    float power;
    Color color;

    Light(){}

    Light(Color _color, float _power, Vector _pos){
        color = _color;
        power = _power;
        pos = _pos;
    }

    Color getIntensity(Vector P){
        float d = (pos - P).Length();
        return color*power*(1/powf(d, 2));
    }
};

struct Camera{
    Vector eye, lookat;
    Vector up, right;

    Camera(){}
    Camera(Vector _eye, Vector _lookat, Vector _up){
            eye = _eye;
            lookat = (_eye + (_lookat - _eye).Normalize());
            Vector dir = (lookat - eye).Normalize();
            right = (dir % _up).Normalize();
            up = (right % dir).Normalize();
    }

    Ray getRay(int x, int y){
        Vector p = lookat + right * (2.0 * (x + 0.5) / 600.0 - 1) + up * (2.0 * (y + 0.5) / 600.0 - 1);

        Ray r(p, (p - eye).Normalize());
        return r;
    }
};

struct Scene{
    Object *elements[100];
    int cnt;
    Light l1, l2, l3;
    Camera camera;

    Scene(){}

    void render(){
        for(int i = 0; i < 600; i++){
            for(int j = 0; j < 600; j++){
                Ray r = camera.getRay(j, i);
                image[i][j] = trace(r, 0);
            }
        }
    }
    RectH intersectAll(Ray ray){
        RectH res;
        RectH tmp;
        res.t = 10000;
        for(int i = 0; i < cnt; i++){
            tmp = elements[i] -> intersect(ray);
            if(tmp.t >= EPSILON){
                if(tmp.t < res.t){
                    res = tmp;
                    res.indexObj = i;
                }
            }
        }
        return res;
    }
    RectH intersectAllB(Ray ray){
        Vector blackHO = Vector(2, 2, 1);
        Ray newRay = ray;
        RectH res;
        for(float radius = 0; radius < 20; radius += 2){
            res = intersectAll(newRay);
            float Rs = 0.8;
            if((newRay.p - blackHO).Length() <= Rs){
                RectH tmp = RectH();
                tmp.indexObj == 6;
                return tmp;
            }
            float step = 1.6;
            if((res.P - newRay.p).Length() < step){
                return res;
            }
            float r = (blackHO - newRay.p).Length();
            Vector d = newRay.v * step + ((blackHO - newRay.p) / r) * 0.5 * Rs * (step * step) / (r * r);
            newRay.v = d.Normalize();
            newRay.p = newRay.p + d.Normalize() * step;
        }
        return RectH();
    }
    Color trace(Ray ray, int depth){
        Color sum = Color(0.24725, 0.1995, 0.0745);

        if(depth > 3)
            return sum;
        depth++;
        RectH hit = intersectAllB(ray);

        if(hit.indexObj < 0)
            return sum;
        if(hit.indexObj == 6)
            return Color(0,0,0);

        sum = sum + directLS(l1, hit, ray);

        if(elements[hit.indexObj] -> material.refl){
            Ray reflRay;
            reflRay.v = (ray.v - (hit.N * 2.0 * (ray.v * hit.N))).Normalize();
            reflRay.p = hit.P + reflRay.v * EPSILON;

            float d = (-1) * (ray.v * hit.N);

            sum = sum + (elements[hit.indexObj] -> material.Fresnel(d)) * trace(reflRay, depth);
        }
        return sum;
    }
    Color directLS(Light l1, RectH hit, Ray ray){
        Ray shadow;
        shadow.v = (l1.pos - hit.P).Normalize();
        shadow.p = hit.P + shadow.v * EPSILON;

        RectH s = intersectAll(shadow);
        if(hit.isPlain){
            if(hit.P.y < 0){
                Color color = procText2(hit.P);
                elements[hit.indexObj] -> material.kd = color;
                }
            else{
                Color color = procText1(hit.P);
                elements[hit.indexObj] -> material.kd = color;
            }
        }
        if((hit.P - l1.pos).Length() < (s.P - hit.P).Length() || s.indexObj < 0){
            Color kd = elements[hit.indexObj] -> material.kd;
            Color ks = elements[hit.indexObj] -> material.ks;
            float shine = elements[hit.indexObj] -> material.shine;
            Vector V = (ray.v * (-1)).Normalize();
            Vector L = shadow.v.Normalize();
            Vector H = (V + L).Normalize();
            float LN = (L * hit.N) > 0 ? (L * hit.N) : 0;
            float HN = (H * hit.N) > 0 ? (H * hit.N) : 0;

            return l1.getIntensity(hit.P) * kd * LN + ks * pow(HN, shine);
        }
        else
            return Color(0,0,0);
    }

    void build(){
        cnt = 0;

        camera = Camera(Vector(0.0f,0.0f,10.0f), Vector(0.0f,0.0f,0.0f), Vector(0.0f,1.0f,0.0f));

        l1 = Light(Color(1,1,1), 30, Vector(-3,2,2));

        Plain pBottom = Plain(Vector(0,1,0), Vector(0, -5.0, 0));
        pBottom.material = Material(Color(1,1,0), Color(0.633, 0.727811, 0.633), Color(0,0,0), Color(0,0,0), 76.8, false);

        Plain pTop = Plain(Vector(0,-1,0), Vector(0, 5.0, 0));
        pTop.material = Material(Color(1,1,0), Color(0.633, 0.727811, 0.633), Color(0,0,0), Color(0,0,0), 76.8, false);

        Plain pRight = Plain(Vector(-1,0,0), Vector(5.0, 0, 0));
        pRight.material = Material(Color(0,1,1), Color(0.633, 0.727811, 0.633), Color(0,0,0), Color(0,0,0), 76.8, false);

        Plain pLeft = Plain(Vector(1,0,0), Vector(-5.0, 0, 0));
        pLeft.material = Material(Color(0,1,1), Color(0.633, 0.727811, 0.633), Color(0,0,0), Color(0,0,0), 76.8, false);

        Plain pBack = Plain(Vector(0,0,1), Vector(0, 0, -5.0));
        pBack.material = Material(Color(1,0,1), Color(0.633, 0.727811, 0.633), Color(0,0,0), Color(0,0,0), 76.8, false);

        Torus t1 = Torus(Vector(1,-1.5,-2), 3.0, 1.0, 6);
        t1.material = Material(Color(0,0,0), Color(0,0,0), Color(0.17,0.35,1.5), Color(3.1,2.7,1.9), 51.2, true);

        elements[cnt++] = &pBottom;
        elements[cnt++] = &pTop;
        elements[cnt++] = &pRight;
        elements[cnt++] = &pLeft;
        elements[cnt++] = &pBack;
        elements[cnt++] = &t1;

        render();
    }
};

Scene scene;

// Inicializacio, a program futasanak kezdeten, az OpenGL kontextus letrehozasa utan hivodik meg (ld. main() fv.)
void onInitialization( ) {
	glViewport(0, 0, screenWidth, screenHeight);

    for(int c = 0; c < 900; ++c){
        perlin_gradient[c] = Vector(get_random_num(0, 1), get_random_num(0, 1),  get_random_num(0, 1)).Normalize();
    }

	scene.build();

}

// Rajzolas, ha az alkalmazas ablak ervenytelenne valik, akkor ez a fuggveny hivodik meg
void onDisplay( ) {
    glClearColor(0.1f, 0.2f, 0.3f, 1.0f);		// torlesi szin beallitasa
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // kepernyo torles
    glDrawPixels(screenWidth, screenHeight, GL_RGB, GL_FLOAT, image);

    glutSwapBuffers();

}

// Billentyuzet esemenyeket lekezelo fuggveny (lenyomas)
void onKeyboard(unsigned char key, int x, int y) {
    if (key == 'd') glutPostRedisplay( ); 		// d beture rajzold ujra a kepet

}

// Billentyuzet esemenyeket lekezelo fuggveny (felengedes)
void onKeyboardUp(unsigned char key, int x, int y) {

}

// Eger esemenyeket lekezelo fuggveny
void onMouse(int button, int state, int x, int y) {
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)   // A GLUT_LEFT_BUTTON / GLUT_RIGHT_BUTTON illetve GLUT_DOWN / GLUT_UP
		glutPostRedisplay( ); 						 // Ilyenkor rajzold ujra a kepet
}

// Eger mozgast lekezelo fuggveny
void onMouseMotion(int x, int y)
{

}

// `Idle' esemenykezelo, jelzi, hogy az ido telik, az Idle esemenyek frekvenciajara csak a 0 a garantalt minimalis ertek
void onIdle( ) {
     long time = glutGet(GLUT_ELAPSED_TIME);		// program inditasa ota eltelt ido

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

