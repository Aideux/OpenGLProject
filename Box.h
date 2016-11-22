#pragma once
#include "GameObject.h"
#include "md2model.h"

class Box : public GameObject
{

private:
	mat4 MVP;
	mat4 ModelMatrix;
	mat4 RotationMatrix;
	mat4 OrbitMatrix;
	mat4 TranslationMatrix;
	mat4 ScaleMatrix;

public:
	void Draw(float deltaTime);
	void OnCollide(Box * box);
	virtual ObjectType getType() const { return BOX; }

	mat4 getModelMatrix() { return getOrbitMatrix()*getTranslationMatrix()* getRotationMatrix()*getScaleMatrix(); }

	mat4 getTranslationMatrix() { return translate(glm::mat4(1.0f), Position); }
	mat4 getRotationMatrix() { return RotationMatrix; }
	mat4 getScaleMatrix() { return ScaleMatrix; }
	mat4 getOrbitMatrix() { return OrbitMatrix; }
	vec3 getPosition(){ return Position; }


	void setOrbitMatrix(vec3 pos, float angle, float x, float y, float z);
	void setRotationMatrix(float angle, float x, float y, float z);
	void setScaleMatrix(vec3 scale);
	void setTranslationMatrix(vec3 pos);
	void setMVPMatrix(mat4 viewMatrix, mat4 projectionMatrix);

	bool Visible;
	void ChangeTexture();
	bool operator!=(Box b);
	float getXBuffer();
	float getYBuffer();
	float getZBuffer();

	void setPosition(vec3 position);

	Box(vec3 initialPosition, const char* imagePath, const string vertexShader, const string fragmentShader, char* modelPath, bool m, float sp, vec3 s, bool vis);

	vec3 Position;
	vec3 direction;
	vec3 scaleVector;
	float rotationAngle;
	bool Hit;
	bool master;
	float speed;
	float radius;
	
};
