#include "Font.h"
#include "ogro.h"
#include "Box.h"
#include "md2model.h"
#include "sstream"
#include "Camera.h"
#include "Time.h"
#include <vector>
using namespace std;

#include "vgl.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/norm.hpp>
using namespace glm;
#define SPACEBAR_KEY 32
#define ESCAPE_KEY 033

Camera* camera;
Ogro* ogro;
//Box* boxes;
Font* font;
string fontText;
vector<GameObject*> gameObjects;
vector<Box * > boxes;
//vector<Box * > boxes;

int treesTapped = 0, points = 0;


float lastTime;
bool keyDown[255];

void timer(int value)
{
	glutPostRedisplay();
	glutTimerFunc(25, timer, 0);
}

void closeApp()
{
	delete camera;
	delete font;
	for (auto it = gameObjects.begin(); it != gameObjects.end(); ++it)
	{
		delete (*it);
	}	
}

void CheckKeyboard()
{
	int o = 0;

	if (keyDown['a'])
		camera->RotateLeft();
	if (keyDown['d'])
		camera->RotateRight();
	if (keyDown['w'])
		camera->MoveForward();
	if (keyDown['s'])
		camera->MoveBackWard();
	/*if (keyDown['e'])
		camera->StrafeRight();
	if (keyDown['q'])
		camera->StrafeLeft();*/

	vec3 p = camera->getPosition();
	camera->Update();
	bool collided = false;

	for (vector<Box*>::iterator i = boxes.begin(); i != boxes.end(); ++i)
	{
		if ((*i)->getType() == ObjectType::BOX)
		{
			if (camera->getPosition().x >= ((*i)->Position.x - (*i)->getXBuffer()) && camera->getPosition().x <= ((*i)->Position.x + (*i)->getXBuffer()))
			{
				if (camera->getPosition().z >= ((*i)->Position.z - (*i)->getZBuffer()) && camera->getPosition().z <= ((*i)->Position.z + (*i)->getZBuffer()))		//change to Z axis and get camera position instead of radius
				{
					o++;
					cout << o << endl;
					collided = true;
				}
			}
		}
	}
	
	if (collided)
	{
		camera->position = p;
		camera->Update();
	}

	if (camera->position.x >= 90.0f)
	{
		camera->position.x = 90.0f;
	}
	if (camera->position.x <= -90.0f)
	{
		camera->position.x = -90.0f;
	}
	if (camera->position.z >= 90.0f)
	{
		camera->position.z = 90.0f;
	}
	if (camera->position.z <= -90.0f)
	{
		camera->position.z = -90.0f;
	}

	srand(time(NULL));

}

float getElapsedSeconds()
{
	float currentTime = float(GetTickCount()) / 1000.0f;
	float seconds = float(currentTime - lastTime);
	lastTime = currentTime;
	return seconds;
}

void keyboardUp(unsigned char key, int x, int y)
{
	keyDown[key] = false;
}

void mouseMovement(int x, int y)
{
	static bool warp = true;

	if (warp)
	{
		if (x>glutGet(GLUT_WINDOW_WIDTH) / 2)
			camera->RotateRight();
		else if (x<glutGet(GLUT_WINDOW_WIDTH) / 2)
			camera->RotateLeft();

		if (y>glutGet(GLUT_WINDOW_HEIGHT) / 2)
			camera->RotateUp();
		else if (y<glutGet(GLUT_WINDOW_HEIGHT) / 2)
			camera->RotateDown();

		glutWarpPointer(glutGet(GLUT_WINDOW_WIDTH) / 2, glutGet(GLUT_WINDOW_HEIGHT) / 2);
		warp = false;
	}
	else
		warp = true;
}

void mouseWheel(int button, int direction, int x, int y)
{

	if (button == 16)
		camera->ResetFOV();
	else if (direction > 0)
		camera->ZoomIn();
	else
		camera->ZoomOut();
}

void keyboardSpecialKeys(int key, int x, int y)
{
	float yaw;
	switch (key)
	{
	case GLUT_KEY_LEFT:
		yaw = ((Ogro*)gameObjects[1])->getYaw();
		yaw -= 5.0;
		((Ogro*)gameObjects[1])->setYaw(yaw);
		break;
	case GLUT_KEY_RIGHT:
		yaw = ((Ogro*)gameObjects[1])->getYaw();
		yaw += 5.0;
		((Ogro*)gameObjects[1])->setYaw(yaw);
		break;
	}
}

void keyboard(unsigned char key, int x, int y)
{
	keyDown[key] = true;
	switch (key)
	{
	case ESCAPE_KEY:
		exit(EXIT_SUCCESS);
		break;
	case 'e':
		((Box*)gameObjects[1])->ChangeTexture();
		treesTapped++;
		break;
	}
	
}

void display()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	CheckKeyboard();

	if (fontText.length() != 0)
		font->printText(fontText, 350, 550, 25);

	float dt = getElapsedSeconds();
	for (int i = 0; i <= gameObjects.size() - 1; i++)
	{
		switch (gameObjects[i]->getType())
		{
		case ObjectType::BOX:
			((Box*)gameObjects[i])->setMVPMatrix(camera->ViewMatrix, camera->ProjectionMatrix);
			gameObjects[i]->Draw(dt);
			break;
		/*case ObjectType::OGRO:
			((Ogro*)gameObjects[i])->changeInTime = dt;
			((Ogro*)gameObjects[i])->Draw(camera->ViewMatrix, camera->ProjectionMatrix);
			break;*/
		}
	}

	std::stringstream ss1;
	ss1 << "Trees tapped: " << treesTapped;
	font->printText(ss1.str().c_str(), 20, 550, 15);
	std::stringstream ss2;
	ss2 << "Points: " << points;
	font->printText(ss2.str().c_str(), 20, 520, 15);


	glutSwapBuffers();
}


void init()
{
	glClearColor(0.0f, 0.0f, 0.7f, 0.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	font = new Font();
	camera = new Camera();
	camera->setPosition(vec3(20.0f, 1.5f, 0.0f));
	glutWarpPointer(glutGet(GLUT_WINDOW_WIDTH) / 2, glutGet(GLUT_WINDOW_HEIGHT) / 2);
	std::fill(keyDown, keyDown + 255, false);

	gameObjects.push_back(new Box(vec3(0.0f, -2.0f, 0.0f), "data/images/grass-1.png", "data/shaders/Texture.vertexshader", "data/shaders/Texture.fragmentshader", "Data\\Models\\Floor.xml", true, 0.1f, vec3(100.0f, 0.1f, 100.0f), true));
	gameObjects.push_back(new Box(vec3(0.0f, -2.0f, 0.0f), "data/images/wood.bmp", "data/shaders/Texture.vertexshader", "data/shaders/Texture.fragmentshader", "Data\\Models\\Floor.xml", true, 0.1f, vec3(1.0f, 10.0f, 1.0f), true));
	gameObjects.push_back(new Box(vec3(0.0f, 6.0f, 0.0f), "data/images/leaves.jpg", "data/shaders/Texture.vertexshader", "data/shaders/Texture.fragmentshader", "Data\\Models\\Floor.xml", true, 0.1f, vec3(2.5f, 2.5f, 2.5f), true));

	/*gameObjects.push_back(new Ogro());
	((Ogro*)gameObjects[1])->Initialize();
	((Ogro*)gameObjects[1])->setPosition(vec3(0, 0, 0));
	((Ogro*)gameObjects[1])->setDirection(vec3(1, 0, 0));
	((Ogro*)gameObjects[1])->setYaw(90);

	gameObjects.push_back(new Ogro());
	((Ogro*)gameObjects[2])->Initialize();
	((Ogro*)gameObjects[2])->setYaw(-90);
	((Ogro*)gameObjects[2])->setPosition(vec3(0, 0, -3));
	((Ogro*)gameObjects[2])->setDirection(vec3(1, 0, 0));*/
}



int main(int argc, char** argv)
{
	glutInit(&argc, argv);

	glutInitDisplayMode(GLUT_RGBA | GLUT_DEPTH);
	glutInitWindowSize(1024, 768);
	glutInitContextProfile(GLUT_CORE_PROFILE);
	glutCreateWindow("Model Example1");
	

	if (glewInit())
	{
		cerr << "Unable to init glew" << endl;
		exit(EXIT_FAILURE);
	}

	init();
	glutDisplayFunc(display);
	glutKeyboardFunc(keyboard);
	glutKeyboardUpFunc(keyboardUp);
	glutSpecialFunc(keyboardSpecialKeys);
	//glutPassiveMotionFunc(mouseMovement);
	glutMouseWheelFunc(mouseWheel);
	glutTimerFunc(25, timer, 0);
	glutMainLoop();
	return 0;
}
