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
   float Length() { return sqrt(x * x + y * y + z * z); }
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

const int screenWidth = 600;	// alkalmazás ablak felbontása
const int screenHeight = 600;

Vector points[100];
float weights[100];
int n = 0;

int minindex = -1;
int clickPart = 0;

bool spacePressed = false;
long spaceTime;

static float PI = 3.1415926f;


void drawCross(){
    glColor3f(1.0f, 1.0f, 1.0f);
    glBegin(GL_LINES);

    glVertex2f(0, 3);
    glVertex2f(6, 3);
    glVertex2f(3, 0);
    glVertex2f(3, 6);

    glEnd();
}

void drawCircles(Vector p, float r, Color c){

    glColor3f(c.r, c.g, c.b);
    int res = 100;
    //bal felso
    glBegin(GL_POLYGON);
    for(int i = 0; i <= res; i++){
        float angle = float(i) / res * 2.0f * PI;
        glVertex2f(p.x + r*cos(angle) + 1.5, p.y + r*sin(angle) + 4.5);
    }
    glEnd();
    //bal also
    glBegin(GL_POLYGON);
    for(int i = 0; i <= res; i++){
        float angle = float(i) / res * 2.0f * PI;
        glVertex2f(p.x + r*cos(angle) + 1.5, p.z + r*sin(angle) + 1.5);
    }
    glEnd();

    //jobb also
    glBegin(GL_POLYGON);
    for(int i = 0; i <= res; i++){
        float angle = float(i) / res * 2.0f * PI;
        glVertex2f(p.z + r*cos(angle) + 4.5, p.y + r*sin(angle) + 1.5);
    }
    glEnd();
}

void drawVelocities(Vector circlePlace, Vector vel){
    glColor3f(1.0f, 0, 0);

    //bal felso
    glBegin(GL_LINES);
    glVertex2f(circlePlace.x + 1.5, circlePlace.y + 4.5);
    glVertex2f(circlePlace.x + vel.x + 1.5, circlePlace.y + vel.y + 4.5);
    glEnd();

    //bal also
    glBegin(GL_LINES);
    glVertex2f(circlePlace.x + 1.5, circlePlace.z + 1.5);
    glVertex2f(circlePlace.x + vel.x + 1.5, circlePlace.z + vel.z + 1.5);
    glEnd();

    //jobb also
    glBegin(GL_LINES);
    glVertex2f(circlePlace.z + 4.5, circlePlace.y + 1.5);
    glVertex2f(circlePlace.z + vel.z + 4.5, circlePlace.y + vel.y + 1.5);
    glEnd();

}

float distance(Vector a, Vector b){
    return sqrt((a.x - b.x)*(a.x - b.x) + (a.y - b.y)*(a.y - b.y) + (a.z - b.z)*(a.z - b.z));
}

Vector normalize(Vector a){
    return a*(1/a.Length());
}

//RennerSpline Methods

Vector v(int i){
    float a = ((points[i-1] - points[i-2])%(points[i] - points[i-1])).Length() / 2.0;
    float b = ((points[i+1] - points[i])%(points[i+2] - points[i+1])).Length() / 2.0;

    Vector vel = (points[i] - points[i-1]) * a * (1 / (a + b))
                + (points[i+1] - points[i]) * b * (1 / (a + b));

    return normalize(vel);
}

Vector a0(int i){
    return points[i];
}

Vector a1(int i){
    return v(i);
}

Vector a2(int i){
    return (points[i+1] - points[i]) * 3 * (1/(pow(weights[i+1] - weights[i], 2)))
            - (v(i+1) + v(i) * 2) * (1/(weights[i+1] - weights[i]));
}

Vector a3(int i){
    return (((points[i] - points[i+1]) * 2 * (1 / (pow(weights[i+1] - weights[i], 3))))
            + (v(i+1) + v(i)) * (1 / (pow(weights[i+1] - weights[i], 2))));
}

Vector equation(float t, int i){
    return a3(i) * pow(t - weights[i], 3) + a2(i) * pow(t - weights[i], 2)
            + a1(i) * (t - weights[i]) + a0(i);
}

Vector derive(float t, int i){
    return a3(i) * pow(t - weights[i], 2) * 3 + a2(i) * (t - weights[i]) * 2
        + a1(i);
}


void drawSpline(){
    glColor3f(1.0f, 1.0f, 1.0f);
    glBegin(GL_LINE_STRIP);

    //Bal felső
    for(int i = 0; i < n-1; i++){
        float step = (weights[i+1] - weights[i])/ 80.0;

        for(float t = weights[i]; t < weights[i+1]; t += step){
            Vector p = equation(t, i);
            glVertex2f(p.x + 1.5, p.y + 4.5);
        }
    }
    glEnd();

    glBegin(GL_LINE_STRIP);

    //Jobb also
    for(int i = 0; i < n-1; i++){
        float step = (weights[i+1] - weights[i])/ 80.0;

        for(float t = weights[i]; t < weights[i+1]; t += step){
            Vector p = equation(t, i);
            glVertex2f(p.z + 4.5, p.y + 1.5);
        }
    }
    glEnd();

    glBegin(GL_LINE_STRIP);

    //Bal alsó
    for(int i = 0; i < n-1; i++){
        float step = (weights[i+1] - weights[i])/ 80.0;

        for(float t = weights[i]; t < weights[i+1]; t += step){
            Vector p = equation(t, i);
            glVertex2f(p.x + 1.5, p.z + 1.5);
        }
    }
    glEnd();
}

void moving(){
    float dt = weights[n-1] - weights[0];
    float fm = fmod((glutGet(GLUT_ELAPSED_TIME) - spaceTime)/1050.0, dt);
    float t = fm + weights[0];

    int i = 0;
    for(int j = 0; weights[j] < t; j++){
        i = j;
    }
    Vector circlePlace = equation(t, i);
    drawCircles(circlePlace, 0.04, Color(1.0f, 0, 0));

    Vector vel = derive(t, i);

    vel = normalize(vel);
    vel = vel * (-1);
    vel = vel * 0.8;

    drawVelocities(circlePlace, vel);

}


// Inicializacio, a program futasanak kezdeten, az OpenGL kontextus letrehozasa utan hivodik meg (ld. main() fv.)
void onInitialization( ) {
	glViewport(0, 0, screenWidth, screenHeight);


}

// Rajzolas, ha az alkalmazas ablak ervenytelenne valik, akkor ez a fuggveny hivodik meg
void onDisplay( ) {

    glClearColor(0.5f, 0.5f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0, 6, 0, 6);

    //kepernyo felosztasa 4 reszre
    drawCross();

    //korok kirajzolasa
    for(int i = 0; i < n; i++){
        drawCircles(points[i], 0.04, Color(1.0f, 1.0f, 0));
    }

    //gorbe kirajzolasa
    if(n>3){
        drawSpline();
    }
    if(spacePressed){
        moving();
    }

    glutSwapBuffers();     				// Buffercsere: rajzolas vege

}

// Billentyuzet esemenyeket lekezelo fuggveny (lenyomas)
void onKeyboard(unsigned char key, int x, int y) {
    if (key == ' '){
        spacePressed = true;
        spaceTime = glutGet(GLUT_ELAPSED_TIME);
        glutPostRedisplay( );
    }
}

// Billentyuzet esemenyeket lekezelo fuggveny (felengedes)
void onKeyboardUp(unsigned char key, int x, int y) {

}


// Eger esemenyeket lekezelo fuggveny
void onMouse(int button, int state, int x, int y) {
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN){   // A GLUT_LEFT_BUTTON / GLUT_RIGHT_BUTTON illetve GLUT_DOWN / GLUT_UP

        //pontok felvetele, a bal felso reszbe kattintva
        if(n < 100){
            if(x <= 300 && (600-y) >= 300){

                if(clickPart == 0){
                    clickPart = 1;
                }

                Vector v;
                v.x = (x - 150) / 100.0;
                v.y = ((600 - y) - 450) / 100.0;
                v.z = 0;

                if(clickPart == 1){
                    points[n] = v;
                    if(n == 0){
                        weights[n++] = 0;
                    }
                    else{
                        weights[n++] = weights[n-1] + distance(points[n], points[n-1]);
                    }
                }
                else{
                    clickPart = 100;
                    float minimum = 10000;
                    for(int i = 0; i < n; i++){
                        Vector tmp = points[i];
                        tmp.z = 0;
                        float dist = distance(v, tmp);
                        if(dist < minimum){
                            minimum = dist;
                            minindex = i;
                            }
                    }
                    points[minindex].x = v.x;
                    points[minindex].y = v.y;
                    minindex = -1;

                    for(int i = 1; i < n; i++){
                        weights[i] = weights[i-1] + distance(points[i], points[i-1]);
                    }
                }
            }


            //pontok mozgatasa a bal also reszbe kattintva
            if(x < 300 && (600-y) < 300){

                if(clickPart == 0){
                    clickPart = 2;
                }

                Vector v;
                v.x = (x - 150) / 100.0;
                v.y = 0;
                v.z = ((600 - y) - 150) / 100.0;

                if(clickPart == 2){
                    points[n] = v;
                    if(n == 0){
                        weights[n++] = 0;
                    }
                    else{
                        weights[n++] = weights[n-1] + distance(points[n], points[n-1]);
                    }
                }

                else{
                    clickPart = 100;
                    float minimum = 10000;
                    for(int i = 0; i < n; i++){
                        Vector tmp = points[i];
                        tmp.y = 0;
                        float dist = distance(v, tmp);
                        if(dist < minimum){
                            minimum = dist;
                            minindex = i;
                        }
                    }
                    points[minindex].x = v.x;
                    points[minindex].z = v.z;
                    minindex = -1;

                    for(int i = 1; i < n; i++){
                        weights[i] = weights[i-1] + distance(points[i], points[i-1]);
                    }
                }
            }

            //pontok mozgatasa a jobb also reszbe kattintva
            if(x > 300 && (600-y) < 300){

                if(clickPart == 0){
                    clickPart = 3;
                }

                Vector v;
                v.x = 0;
                v.y = ((600 - y) - 150) / 100.0;
                v.z = (x - 450) / 100.0;

                if(clickPart == 3){
                    points[n] = v;
                    if(n == 0){
                        weights[n++] = 0;
                    }
                    else{
                        weights[n++] = weights[n-1] + distance(points[n], points[n-1]);
                    }
                }

                else{
                    clickPart = 100;
                    float minimum = 10000;
                    for(int i = 0; i < n; i++){
                        Vector tmp = points[i];
                        tmp.x = 0;
                        float dist = distance(v, tmp);
                        if(dist < minimum){
                            minimum = dist;
                            minindex = i;
                        }
                    }
                    points[minindex].y = v.y;
                    points[minindex].z = v.z;
                    minindex = -1;

                    for(int i = 1; i < n; i++){
                        weights[i] = weights[i-1] + distance(points[i], points[i-1]);
                    }
                }
            }
        }

    glutPostRedisplay( );
    }
}
// Eger mozgast lekezelo fuggveny
void onMouseMotion(int x, int y)
{

}

// `Idle' esemenykezelo, jelzi, hogy az ido telik, az Idle esemenyek frekvenciajara csak a 0 a garantalt minimalis ertek
void onIdle( ) {
    long time = glutGet(GLUT_ELAPSED_TIME);		// program inditasa ota eltelt ido
     if(spacePressed){
        	glutPostRedisplay();
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

