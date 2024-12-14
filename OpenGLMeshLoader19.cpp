#include "TextureBuilder.h"
#include "Model_3DS.h"
#include "GLTexture.h"
#include <glut.h>
#include <math.h>


#include <iostream>

#define DEG2RAD(a) (a * 0.0174532925)


int WIDTH = 1280;
int HEIGHT = 720;
int xPlayer = 0;
int yPlayer = 0;
int zPlayer = 0;
int playerAngle = 90;
bool playerView = FALSE;
bool thirdPerson = true;
bool jumping = FALSE;
int coinAngle = 0;
int lives = 3;
int score = 0;
bool hitObstacle = FALSE;
int coinNumber = 0;
bool stopGameLoss = false;
bool stopGameWin = false;
bool L1 = true;
bool L2 = false;
bool lightmode = true;
int playerSpeed = 50;






GLuint tex;
char title[] = "subway";

// 3D Projection Options
GLdouble fovy = 45.0;
GLdouble aspectRatio = (GLdouble)WIDTH / (GLdouble)HEIGHT;
GLdouble zNear = 0.1;
GLdouble zFar = 1000;

// player position and orientation





class Vector3f {

public:
	float x, y, z;

	Vector3f(float _x = 0.0f, float _y = 0.0f, float _z = 0.0f) {
		x = _x;
		y = _y;
		z = _z;
	}

	Vector3f operator+(Vector3f& v) {
		return Vector3f(x + v.x, y + v.y, z + v.z);
	}

	Vector3f operator-(Vector3f& v) {
		return Vector3f(x - v.x, y - v.y, z - v.z);
	}

	Vector3f operator*(float n) {
		return Vector3f(x * n, y * n, z * n);
	}

	Vector3f operator/(float n) {
		return Vector3f(x / n, y / n, z / n);
	}

	Vector3f unit() {
		return *this / sqrt(x * x + y * y + z * z);
	}

	Vector3f cross(Vector3f v) {
		return Vector3f(y * v.z - z * v.y, z * v.x - x * v.z, x * v.y - y * v.x);
	}
};

class Camera {
public:
	Vector3f eye, center, up;

	Camera(float eyeX = 1.0f, float eyeY = 1.0f, float eyeZ = 1.0f, float centerX = 0.0f, float centerY = 0.0f, float centerZ = 0.0f, float upX = 0.0f, float upY = 1.0f, float upZ = 0.0f) {
		eye = Vector3f(eyeX, eyeY, eyeZ);
		center = Vector3f(centerX, centerY, centerZ);
		up = Vector3f(upX, upY, upZ);
	}

	void moveX(float d) {
		Vector3f right = up.cross(center - eye).unit();
		eye = eye + right * d;
		center = center + right * d;
	}

	void moveY(float d) {
		eye = eye + up.unit() * d;
		center = center + up.unit() * d;
	}

	void moveZ(float d) {
		Vector3f view = (center - eye).unit();
		eye = eye + view * d;
		center = center + view * d;
	}

	void rotateX(float a) {
		Vector3f view = (center - eye).unit();
		Vector3f right = up.cross(view).unit();
		view = view * cos(DEG2RAD(a)) + up * sin(DEG2RAD(a));
		up = view.cross(right);
		center = eye + view;
	}

	void rotateY(float a) {
		Vector3f view = (center - eye).unit();
		Vector3f right = up.cross(view).unit();
		view = view * cos(DEG2RAD(a)) + right * sin(DEG2RAD(a));
		right = view.cross(up);
		center = eye + view;
	}

	

	void look() {
		gluLookAt(
			eye.x, eye.y, eye.z,
			center.x, center.y, center.z,
			up.x, up.y, up.z
		);
	}
};

class Vector
{
public:
	GLdouble x, y, z;
	Vector() {}
	Vector(GLdouble _x, GLdouble _y, GLdouble _z) : x(_x), y(_y), z(_z) {}
	//================================================================================================//
	// Operator Overloading; In C++ you can override the behavior of operators for you class objects. //
	// Here we are overloading the += operator to add a given value to all vector coordinates.        //
	//================================================================================================//
	void operator +=(float value)
	{
		x += value;
		y += value;
		z += value;
	}
};

Camera camera;

Vector Eye(-16, 5, 0);
Vector At(-1, 0, 0);
Vector Up(0, 1, 0);









// Model Variables
Model_3DS model_house;
Model_3DS model_tree;

// Textures
GLTexture tex_ground;
GLTexture tex_box;
GLTexture tex_present;

//=======================================================================
// Lighting Configuration Function
//=======================================================================
void InitLightSource()
{
	// Enable Lighting for this OpenGL Program
	glEnable(GL_LIGHTING);

	// Enable Light Source number 0
	// OpengL has 8 light sources
	glEnable(GL_LIGHT0);

	// Define Light source 0 ambient light
	GLfloat ambient[] = { 1.0f, 0.7f, 0.1f, 1.0f };
	glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);

	// Define Light source 0 diffuse light
	GLfloat diffuse[] = { 0.5f, 0.5f, 0.5f, 1.0f };
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);

	// Define Light source 0 Specular light
	GLfloat specular[] = { 1.0f, 1.0f, 0.0f, 1.0f };
	glLightfv(GL_LIGHT0, GL_SPECULAR, specular);

	// Finally, define light source 0 position in World Space
	GLfloat light_position[] = { 0.0f, 10.0f, 0.0f, 1.0f };
	glLightfv(GL_LIGHT0, GL_POSITION, light_position);
}

void setLight0() {
	glEnable(GL_LIGHTING);
	glDisable(GL_LIGHT1);
	glEnable(GL_LIGHT0);
	GLfloat lightIntensity[] = { 0.7, 0.7, 0.7, 1.0f };
	//GLfloat lightIntensity[] = { 1.0, 1.0, 0.0, 1.0f };
	GLfloat lightPosition[] = { 0.0f, 100.0f, 0.0f, 0.0f };
	glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);
	glLightfv(GL_LIGHT0, GL_AMBIENT, lightIntensity);
}

void setLight1() {
	glEnable(GL_LIGHTING);
	glDisable(GL_LIGHT0);
	glEnable(GL_LIGHT1);
	GLfloat ambient[] = { 0.1f, 0.1f, 0.1, 1.0f };
	GLfloat diffuse[] = { 1.0, 1.0, 0.1, 1.0 }; //light color: green
	GLfloat specular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	GLfloat rightSpotPosition[] = { xPlayer, 3, 0 + zPlayer, 1.0 };
	//GLfloat rightSpotPosition[] = { 0.0f, 100.0f, 0.0f, 0.0f };
	GLfloat spotsDirection[] = { 1.0, 0.0, 0.0 };
	glLightfv(GL_LIGHT1, GL_POSITION, rightSpotPosition);
	glLightfv(GL_LIGHT1, GL_AMBIENT, ambient);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, diffuse);
	glLightfv(GL_LIGHT1, GL_SPECULAR, specular);
	//glLightfv(GL_LIGHT1, GL_SPOT_DIRECTION, spotsDirection);
	glLightf(GL_LIGHT1, GL_SPOT_CUTOFF, 20);
	glLightf(GL_LIGHT1, GL_SPOT_EXPONENT, 0);
}

//=======================================================================
// Material Configuration Function
//======================================================================


//=======================================================================
// OpengGL Configuration Function
//=======================================================================


//=======================================================================
// Render Ground Function
//=======================================================================


void RenderGround()
{
	glDisable(GL_LIGHTING);	// Disable lighting 

	glColor3f(0.6, 0.6, 0.6);	// Dim the ground texture a bit

	glEnable(GL_TEXTURE_2D);	// Enable 2D texturing


	glBindTexture(GL_TEXTURE_2D, tex_ground.texture[0]);	// Bind the ground texture

	glPushMatrix();
	glBegin(GL_QUADS);
	glNormal3f(0, 1, 0);	// Set quad normal direction.
	glTexCoord2f(0, 0);		// Set tex coordinates ( Using (0,0) -> (5,5) with texture wrapping set to GL_REPEAT to simulate the ground repeated grass texture).
	glVertex3f((-1000 - 1000), 0, -10);
	glTexCoord2f(5, 0);
	glVertex3f((10000 - 1000), 0, -10);
	glTexCoord2f(5, 5);
	glVertex3f((10000 - 1000), 0, 10);
	glTexCoord2f(0, 5);
	glVertex3f((-1000 - 1000), 0, 10);

	glEnd();
	glPopMatrix();

	glEnable(GL_LIGHTING);	// Enable lighting again for other entites coming throung the pipeline.

	glColor3f(1, 1, 1);	// Set material back to white instead of grey used for the ground texture.
}

//=======================================================================
// Display Function
//=======================================================================

void cube(int x, int y, int z)
{

	
	
	glPushMatrix();
	
	glColor3f(1, 1, 1);
	glTranslatef(x, y, z);
	glScalef(4, 2, 8);
	glRotatef(90, 0, 1, 0);
	glutSolidCube(1);
	glPopMatrix();


}


void cube2(int x, int y, int z)
{



	glPushMatrix();
	glColor3f(1, 1, 1);
	glTranslatef(x, y, z-0.25);
	glScalef(1, 2, 4.5);
	glRotatef(90, 0, 1, 0);
	glutSolidCube(1);
	glPopMatrix();

	glPushMatrix();
	glColor3f(1, 1, 1);
	glTranslatef(x, y-2, z+1.75);
	
	glScaled(0.5, 6, 0.5);
	glutSolidCube(1);
	glPopMatrix();

	glPushMatrix();
	glColor3f(1, 1, 1);
	glTranslatef(x, y-2, z - 2.25);
	
	glScaled(0.5, 6, 0.5);
	glutSolidCube(1);
	glPopMatrix();
}

//draw coin
void drawCoin(int x, int y, int z)
{
	glPushMatrix();
	glColor3f(1, 1, 0);
	glTranslatef(x, y, z);
	glScalef(0.5, 1, 1);
	glRotatef(coinAngle, 0, 1, 0);
	glutSolidTorus(0.1, 0.5, 10, 10);
	glPopMatrix();
}

//time function to rotate the coinAngle

void cointime(int val)
{
	if (!stopGameLoss && !stopGameWin){
		coinAngle += 22.5;
		glutPostRedisplay();
		glutTimerFunc(100, cointime, 0);
	}
}





void print(float x, float y, float z, char* string)
{
	int len, i;
	glRasterPos3f(x, y, z);
	len = (int)strlen(string);
	for (i = 0; i < len; i++)
	{
		glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, string[i]);
	}
}







void playSoundCoin()
{
	PlaySound(TEXT("coinsound2.wav"), NULL, SND_ASYNC);
}

void playSoundHit() {
	PlaySound(TEXT("hitsound.wav"), NULL, SND_ASYNC);
}

void playSoundVictory() {
	PlaySound(TEXT("victorysound.wav"), NULL, SND_ASYNC);
}

void playSoundLose() {
	PlaySound(TEXT("losesound.wav"), NULL, SND_ASYNC);
}


//setup the camera
void setupCamera()
{
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45, 1, 0.01, 1000);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(xPlayer, 3, zPlayer, xPlayer + At.x, 3, zPlayer + At.z, 0, 1, 0);
}





void myDisplay(void)
{

	//setupCamera();

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);



	/*GLfloat lightIntensity[] = {0.7, 0.7, 0.7, 1.0f};
	GLfloat lightPosition[] = { 0.0f, 100.0f, 0.0f, 0.0f };
	glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);
	glLightfv(GL_LIGHT0, GL_AMBIENT, lightIntensity);*/

	if (lightmode) {
		setLight0();
		
	}
	else {
		setLight1();
		
	}

	

	// Draw Ground
	RenderGround();

	// Draw Player Model
	glPushMatrix();
	glTranslatef(xPlayer, yPlayer, zPlayer);
	glRotatef(playerAngle, 0, 1, 0);
	glScalef(0.05, 0.05, 0.05);
	model_tree.Draw();
	glPopMatrix();

	
	if (L1 && !L2) {

		for (int i = 0; i < 10000; i += 20)
		{
			if (i % 40 == 20)
			{
				cube(i, 1, -5);
				cube(i, 1, 5);
				drawCoin(i, 1, 0);
			}
			else
			{
				cube(i, 1, 0);
				drawCoin(i, 1, -5);
				drawCoin(i, 1, 5);
			}
		}

	}

	else if (L2 && !L1){
		
		for (int i = 0; i < 10000; i += 20)
		{
			if (i % 40 == 20)
			{
				cube2(i, 3, -5);
				cube2(i, 3, 5);
				drawCoin(i, 1, 0);
			}
			else
			{
				cube2(i, 3, 0);
				drawCoin(i, 1, -5);
				drawCoin(i, 1, 5);
			}
		}
		
	}

	if (stopGameWin)
	{
		playSoundVictory();
	}

	
	

	glColor3f(0, 0, 0);
	
	char* livesText[10];
	sprintf((char*)livesText, "Lives: %d", lives);
	print(90 + xPlayer, 10, 0 + zPlayer - 80, (char*)livesText);

	
	char* coinText[10];
	sprintf((char*)coinText, "Coins: %d", coinNumber);
	print(90 + xPlayer, 7, 0 + zPlayer - 80, (char*)coinText);
	
	if (stopGameLoss)
	{
		print(90 + xPlayer, 5, 0 + zPlayer +25, "Game Over .. YOU LOST!!");
	}

	if (stopGameWin)
	{
		print(90 + xPlayer, 5, 0 + zPlayer + 25, "Game Over .. YOU WON!!");
	}

	if (!stopGameLoss && !stopGameWin) {
		print(90 + xPlayer, 5, 0 + zPlayer + 25, "To pass the first level");
		print(90 + xPlayer, 2.5, 0 + zPlayer + 25, "collect 20 coins");
		print(90 + xPlayer, 0, 0 + zPlayer + 25, "To win the whole game");
		print(90 + xPlayer, -2.5, 0 + zPlayer + 25, "collect 40 coins");
	}


	glPushMatrix();

	GLUquadricObj* qobj;
	qobj = gluNewQuadric();
	glColor3f(1, 1, 1);
	glTranslated(100, 0, 0);
	glRotated(90, 1, 0, 1);
	glBindTexture(GL_TEXTURE_2D, tex);
	gluQuadricTexture(qobj, true);
	gluQuadricNormals(qobj, GL_SMOOTH);
	gluSphere(qobj, 1000, 100, 100);
	gluDeleteQuadric(qobj);


	glPopMatrix();
	


	glutSwapBuffers();
}







//move camera third person
void moveCameraThirdPerson()
{
	Eye.x = xPlayer - 10 * sin(playerAngle * 180 / 180);
	Eye.z = zPlayer - 10 * cos(playerAngle * 180 / 180);
	At.x = xPlayer;
	At.z = zPlayer;
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(Eye.x-16, Eye.y+4, Eye.z-4, At.x, At.y, At.z, Up.x, Up.y, Up.z);
}

// make camera moves with a player prespective
void moveCameraFirst()
{
	Eye.x = xPlayer;
	Eye.z = zPlayer;
	At.x = xPlayer + 10 * sin(playerAngle * 180 / 180);
	At.z = zPlayer + 10 * cos(playerAngle * 180 / 180);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(Eye.x, Eye.y, Eye.z, At.x+9, At.y-2, At.z+4, Up.x, Up.y, Up.z);
	
	
}


void timerPlayerMove(int val) {
	
	if ( !stopGameLoss && !stopGameWin) {
			xPlayer += 1;
			playerAngle = 90;
			if (playerView)
			{
				moveCameraFirst();

			}
			else
			{
				moveCameraThirdPerson();
			}
			glutPostRedisplay();
			if (L2) {
				playerSpeed = 15;
			}
			else {
				playerSpeed = 25;
			}
			glutTimerFunc(playerSpeed, timerPlayerMove, 0);
		}
	       
}



void close()
{
	exit(0);
}



void myKeyboard(unsigned char button, int x, int y)
{
	float d = 0.5;
	
		switch (button)
		{

			if (button == 'u')
				camera.moveY(d);
			if (button == 'j')
				camera.moveY(-d);
			if (button == 'h')
				camera.moveX(d);
			if (button == 'k')
				camera.moveX(-d);
			if (button == 'y')
				camera.moveZ(d);
			if (button == 'i')
				camera.moveZ(-d);

		case 's':
			if (L2) {
				if (!stopGameLoss && !stopGameWin) {
					yPlayer -= 2;
					if (yPlayer < -3)
					{
						yPlayer = 0;
					}

				}
			}
			
			else if (L1) {
				if (!stopGameLoss && !stopGameWin) {
					yPlayer += 2;
					if (yPlayer > 3)
					{
						yPlayer = 0;
					}

				}
			}
			break;

		case 'l':
			lightmode = !lightmode;
			break;


		case 'd':
			if (!stopGameLoss && !stopGameWin) {
				zPlayer += 5;
				if (zPlayer > 9)
				{
					zPlayer = 5;
				}
			}


			break;
		case 'a':
			if (!stopGameLoss && !stopGameWin) {
				zPlayer -= 5;
				if (zPlayer < -6)
				{
					zPlayer = -5;
				}

			}


			break;
		case 'g':

			if (!playerView)

			{
				playerView = true;
				moveCameraFirst();
			}
			else
			{
				playerView = false;
				moveCameraThirdPerson();
			}

			break;

		default:
			break;
		}
	

	glutPostRedisplay();
}

void myKeyboardUp(unsigned char button, int x, int y)
{

	switch (button)
	{

	case 's':
		if (L2) {
			if (!stopGameLoss && !stopGameWin) {
				yPlayer += 2;
			}
		}
		else if (L1) {
			if (!stopGameLoss && !stopGameWin) {
				yPlayer -= 2;
			}

		}

		break;
		
		
	default:
		break;
	}

}


void Special(int key, int x, int y) {
	float a = 1.0;


	if (key == GLUT_KEY_UP)
		camera.rotateX(a);
	if (key == GLUT_KEY_DOWN)
		camera.rotateX(-a);
	if (key == GLUT_KEY_LEFT)
		camera.rotateY(a);
	if (key == GLUT_KEY_RIGHT)
		camera.rotateY(-a);

	glutPostRedisplay();
}




//=======================================================================
// Motion Function
//=======================================================================
/*

void myMotion(int x, int y)
{
	y = HEIGHT - y;

	if (cameraZoom - y > 0)
	{
		Eye.x += -0.1;
		Eye.z += -0.1;
	}
	else
	{
		Eye.x += 0.1;
		Eye.z += 0.1;
	}

	cameraZoom = y;

	glLoadIdentity();	//Clear Model_View Matrix

	gluLookAt(Eye.x, Eye.y, Eye.z, At.x, At.y, At.z, Up.x, Up.y, Up.z);	//Setup Camera with modified paramters 

	GLfloat light_position[] = { 0.0f, 10.0f, 0.0f, 1.0f };
	glLightfv(GL_LIGHT0, GL_POSITION, light_position);

	glutPostRedisplay();	//Re-draw scene 
}

//=======================================================================
// Mouse Function
//=======================================================================
void myMouse(int button, int state, int x, int y)
{
	y = HEIGHT - y;

	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
	{
		cameraZoom = y;
	}

	glutPostRedisplay();
}
*/

//=======================================================================
// Reshape Function
//=======================================================================


void myReshape(int w, int h)
{
	if (h == 0) {
		h = 1;
	}

	WIDTH = w;
	HEIGHT = h;

	// set the drawable region of the window
	glViewport(0, 0, w, h);

	// set up the projection matrix 
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(fovy, (GLdouble)WIDTH / (GLdouble)HEIGHT, zNear, zFar);

	// go back to modelview matrix so we can move the objects about
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(Eye.x, Eye.y, Eye.z, At.x, At.y, At.z, Up.x, Up.y, Up.z);
}

//=======================================================================
// Assets Loading Function
//=======================================================================


void LoadAssets()
{
	// Loading Model files
	model_house.Load("Models/house/house.3DS");
	model_tree.Load("Models/player/player.3ds");

	// Loading texture files
	
	if (L1) {
		tex_ground.Load("Textures/ground.bmp");
	}
	

	
	loadBMP(&tex, "Textures/blu-sky-3.bmp", true);
}


//=======================================================================
// Main Function
//=======================================================================

void rotate()
{
	playerAngle += 22.5;
}

void Anim()
{
	
	rotate();
	
	if (xPlayer % 40 == 20 && (zPlayer == 5 || zPlayer == -5))
	{
		if (lives > 0) {
			if (L2 && yPlayer+3 >= 3) {
				if (!stopGameLoss) {

					playSoundHit();
				}
				lives -= 1;
				if (lives <= 0)
				{
					stopGameLoss = true;
					playSoundLose();

				}
			}
			else if (L1 && yPlayer  < 2) {
				if (!stopGameLoss) {

					playSoundHit();
				}
				lives -= 1;
				if (lives <= 0)
				{
					stopGameLoss = true;
					playSoundLose();

				}
			}
		}
		
		
	}
	else if (xPlayer % 40 == 0 && zPlayer == 0)
	{

		if (lives > 0) {
			if (L2 && yPlayer+3 >= 3 ) {
				
				if (!stopGameLoss) {

					playSoundHit();
				}
				lives -= 1;
				if (lives <= 0)
				{
					stopGameLoss = true;
					playSoundLose();

				}
			}
			else if (L1 && yPlayer < 2) {
				if (!stopGameLoss) {

					playSoundHit();
				}
				lives -= 1;
				if (lives <= 0)
				{
					stopGameLoss = true;
					playSoundLose();

				}
			}
		}
	}
	
	else if (xPlayer % 40 == 20 && zPlayer == 0)
	{
		coinNumber +=1;
		if (coinNumber >= 20) {
			L2 = true;
			L1 = false;
		}
		if (!stopGameWin) {
			playSoundCoin();
		}

		if (coinNumber >= 40)
		{
			stopGameWin = true;
			playSoundVictory();
			return;
			
		}
		
		
	}

	else if (xPlayer % 40 == 0 && (zPlayer == 5 || zPlayer == -5))
	{
		coinNumber += 1;
		if (coinNumber >= 20) {
			L2 = true;
			L1 = false;
		}
		if (!stopGameWin) {
			playSoundCoin();
		}

		if (coinNumber >= 40)
		{
			stopGameWin = true;
			playSoundVictory();
			return;
			
		}

		
	}

	if (L2)
	{
		tex_ground.Load("Textures/wall.tga");
		lightmode = false;
	}

	


	glutPostRedisplay();
}


void main(int argc, char** argv)
{
	glutInit(&argc, argv);

	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);

	glutInitWindowSize(WIDTH, HEIGHT);

	glutInitWindowPosition(100, 150);

	glutCreateWindow(title);

	glutDisplayFunc(myDisplay);

	glutKeyboardFunc(myKeyboard);

	glutKeyboardUpFunc(myKeyboardUp);

	glutSpecialFunc(Special);

	//glutMotionFunc(myMotion);

	//glutMouseFunc(myMouse);

	glutReshapeFunc(myReshape);

	
	glutTimerFunc(0, cointime, 0);

	glutTimerFunc(0, timerPlayerMove, 0);

	glutIdleFunc(Anim);

	//myInit();

	LoadAssets();
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_NORMALIZE);
	glEnable(GL_COLOR_MATERIAL);

	glShadeModel(GL_SMOOTH);

	if (stopGameWin)
	{
		playSoundVictory();
	}
	else if (stopGameLoss)
	{
		playSoundLose();
	}
	
	

	glutMainLoop();
}

